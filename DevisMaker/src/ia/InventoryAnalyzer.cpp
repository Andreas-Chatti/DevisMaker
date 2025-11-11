#include "InventoryAnalyzer.h"

InventoryAnalyzer::InventoryAnalyzer(QObject* parent)
    : QObject(parent)
{
    connect(m_aiService, &AIService::error, this, &InventoryAnalyzer::error);

    loadVolumeReference();
}

void InventoryAnalyzer::loadVolumeReference()
{
    QFile file(SettingsConstants::FileSettings::DATA_FILE_PATH + REFERENCE_FILE_NAME);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error("Error loading volume reference file.");
        return;
    }

    QByteArray data{ file.readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    m_volumeReference = doc.object();
    file.close();
}

void InventoryAnalyzer::cleanList(QString rawInventory)
{
    if (m_aiService->getAPI_Key().isEmpty())
    {
        emit analysisError("API key not found or invalid.\n Check your config file.");
        return;
    }

    if (m_rawInventory != rawInventory)
        m_rawInventory = rawInventory;

    QNetworkRequest request;
    try
    {
        request = m_aiService->buildRequest(rawInventory, AIService::RequestType::CleanName);
    }
    catch(std::invalid_argument ex)
    {
        emit error(ex.what());
        return;
    }

    QByteArray jsonData{ request.attribute(QNetworkRequest::User).toByteArray() };
    request.setAttribute(QNetworkRequest::User, QVariant());

    QNetworkReply* reply{ m_networkManager->post(request, jsonData) };
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleCleanNameResponse(reply); });
}

void InventoryAnalyzer::handleCleanNameResponse(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        emit analysisError("Erreur API Grok: " + reply->errorString());

        if (tryNextModelOrAbort())
            cleanList(m_rawInventory);

        reply->deleteLater();
        return;
    }

    QByteArray data{ reply->readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonObject response{ doc.object() };

    if (!response.contains("choices") || !response["choices"].isArray())
    {
        if (tryNextModelOrAbort())
            cleanList(m_rawInventory);

        reply->deleteLater();
        return;
    }

    QJsonArray choices{ response["choices"].toArray() };
    if (choices.isEmpty())
    {
        if (tryNextModelOrAbort())
            cleanList(m_rawInventory);

        reply->deleteLater();
        return;
    }

    QJsonObject firstChoice{ choices[0].toObject() };
    QString cleanInventoryList{ firstChoice["message"].toObject()["content"].toString() };
    m_aiService->resetModelIndex();
    analyzeInventory(cleanInventoryList);

    reply->deleteLater();
}

void InventoryAnalyzer::analyzeInventory(const QString& cleanInventory)
{
    if (m_cleanInventory != cleanInventory)
        m_cleanInventory = cleanInventory;

    auto [networkRequest, jsonData]{ createRequest(cleanInventory) };
    QNetworkReply* reply{ m_networkManager->post(networkRequest, jsonData) };
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleAnalyseInventoryResponse(reply); });
}


std::pair<QNetworkRequest, QByteArray> InventoryAnalyzer::createRequest(const QString& inventoryText)
{
    QJsonDocument refDoc(m_volumeReference);
    QString jsonReference{ refDoc.toJson(QJsonDocument::Compact) };

    QNetworkRequest request{ m_aiService->buildRequest(inventoryText, AIService::RequestType::AnalyseInventory, &jsonReference) };
    QByteArray jsonData{ request.attribute(QNetworkRequest::User).toByteArray() };

    request.setAttribute(QNetworkRequest::User, QVariant());
 
    return { request, jsonData };
}


void InventoryAnalyzer::handleAnalyseInventoryResponse(QNetworkReply* reply)
{
   if (reply->error() != QNetworkReply::NoError)
   {
       emit analysisError("API Grok error: " + reply->errorString());
       reply->deleteLater();

       if (tryNextModelOrAbort())
           analyzeInventory(m_cleanInventory);

       return;
   }

   QVector<MovingObject> objectList{ extractReplyInfos(reply) };
   if (objectList.isEmpty())
   {
       emit analysisError("Error: AI has returned an empty list !");
       reply->deleteLater();

       if (tryNextModelOrAbort())
           analyzeInventory(m_cleanInventory);

       return;
   }

   else
   {
       double listTotalVolume{};
       for (const MovingObject& object : objectList)
           listTotalVolume += object.getTotalVolume();

       m_aiService->resetModelIndex();
       emit analysisComplete(listTotalVolume, objectList);
   }
    reply->deleteLater();
}

QVector<MovingObject> InventoryAnalyzer::extractReplyInfos(QNetworkReply* reply)
{
    QByteArray data{ reply->readAll() };

    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonObject response{ doc.object() };

    // Extraire la réponse de Grok
    if (!response.contains("choices") || !response["choices"].isArray())
        return QVector<MovingObject>{};

    QJsonArray choices{ response["choices"].toArray() };

    if (choices.isEmpty())
        return QVector<MovingObject>{};

    QJsonObject firstChoice{ choices[0].toObject() };
    QString responseText{ firstChoice["message"].toObject()["content"].toString() };

    std::optional<QString> jsonText{ textToJsonFormat(std::move(responseText)) };
    if(!jsonText)
        return QVector<MovingObject>{};

    QJsonParseError parseError{};
    QJsonDocument itemsDoc{ QJsonDocument::fromJson(jsonText.value().toUtf8(), &parseError) };
    if (parseError.error != QJsonParseError::NoError)
    {
        emit error("JSON parsing error: " + parseError.errorString() + " at offset " + QString::number(parseError.offset));
        return QVector<MovingObject>{};
    }
    else if (itemsDoc.isNull() || itemsDoc.isEmpty())
    {
        emit error("JSON document is null or empty");
        return QVector<MovingObject>{};
    }

    QJsonObject itemsObj{ itemsDoc.object() };
    if (!itemsObj.contains("items"))
    {
        QStringList keys{};
        for (const QString& key : itemsObj.keys())
            keys.append(key);

        emit error("JSON object does not contain 'items' key. Available keys: " + keys.join(", "));
        return QVector<MovingObject>{};
    }

    QJsonArray items{ itemsObj["items"].toArray() };
    return getObjectListFromReply(items);
}

bool InventoryAnalyzer::tryNextModelOrAbort()
{
    if (m_aiService->advanceToNextModel() >= m_aiService->getAIModelList()->size())
    {
        emit error("All AI models failed. Analysis aborted.");
        m_aiService->resetModelIndex();
        return false;
    }
    return true;
}

bool InventoryAnalyzer::isJSONFormatValid(const QString& jsonText)
{
    int jsonStart{ static_cast<int>(jsonText.indexOf("{")) };
    int jsonEnd{ static_cast<int>(jsonText.lastIndexOf("}") + 1) };

    return jsonStart >= 0 && jsonEnd > jsonStart;
}

std::optional<QString> InventoryAnalyzer::textToJsonFormat(QString text)
{
    if (!isJSONFormatValid(text))
        return std::nullopt;

    int jsonStart{ static_cast<int>(text.indexOf("{")) };
    int jsonEnd{ static_cast<int>(text.lastIndexOf("}") + 1) };

    return std::make_optional<QString>( text.mid(jsonStart, jsonEnd - jsonStart) );
}

QVector<MovingObject> InventoryAnalyzer::getObjectListFromReply(QJsonArray items)
{
    QVector<MovingObject> objectList{};
    for (const QJsonValue& item : items)
    {
        QJsonObject itemObj{ item.toObject() };
        QString name{ itemObj["name"].toString() };
        if (name.contains('_'))
            name.replace('_', ' ');
        int quantity{ itemObj["quantity"].toInt() };
        double unitaryVolume{ itemObj["unitaryVolume"].toDouble() };
        bool disassembly{ itemObj["disassembly"].toBool() };
        bool assembly{ itemObj["assembly"].toBool() };
        bool heavy{ itemObj["heavy"].toBool() };
        QString areaKey{ itemObj["areaKey"].toString() };
        if (areaKey.contains('_'))
            areaKey.replace('_', ' ');

        MovingObject movingObject{ std::move(name), unitaryVolume, std::move(areaKey), quantity, disassembly,
            assembly, heavy };

        objectList.emplace_back(std::move(movingObject));
    }
    return objectList;
}

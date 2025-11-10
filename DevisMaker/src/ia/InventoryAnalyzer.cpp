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
        return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    m_volumeReference = doc.object();
    file.close();
}

void InventoryAnalyzer::removeModelFromBuffer(const AIModel* aiModelToRemove)
{
    auto isSameModel{ [this, aiModelToRemove](const AIModel& model) { return model.getModelName() == aiModelToRemove->getModelName(); } };
    auto it{ std::find_if(m_aiModelBuffer.begin(), m_aiModelBuffer.end(), isSameModel) };
    if (it != m_aiModelBuffer.end())
        m_aiModelBuffer.erase(it); 
}

void InventoryAnalyzer::resetAIModelBuffer()
{
    const QVector<AIModel>* aiModelList{ m_aiService->getAIModelList() };
    if (aiModelList && !aiModelList->isEmpty())
        m_aiModelBuffer = *aiModelList;

    else
    {
        m_aiModelBuffer.clear();
        m_aiModelBuffer.emplaceBack(AIModel::makeDefaultModel(this));
    }
}

void InventoryAnalyzer::cleanList(QString rawText)
{
    resetAIModelBuffer();
    m_aiService->setCurrentAIModel(m_aiModelBuffer.back());
    removeModelFromBuffer(&m_aiModelBuffer.back());

    if (m_aiService->getAPI_Key().isEmpty())
    {
        emit analysisError("API key not found. Analyse aborted.");
        return;
    }

    QNetworkRequest request;
    try
    {
        request = m_aiService->buildRequest(rawText, AIService::RequestType::CleanName);
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
        reply->deleteLater();
        return;
    }

    QByteArray data{ reply->readAll() };

    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonObject response{ doc.object() };

    if (response.contains("choices") && response["choices"].isArray())
    {
        QJsonArray choices{ response["choices"].toArray() };

        if (!choices.isEmpty())
        {
            QJsonObject firstChoice{ choices[0].toObject() };
            QString cleanInventoryList{ firstChoice["message"].toObject()["content"].toString() };
            analyzeInventory(cleanInventoryList);
        }
    }

    reply->deleteLater();
}

void InventoryAnalyzer::analyzeInventory(const QString& inventoryText)
{

    if (m_aiService->getAPI_Key().isEmpty())
    {
        emit analysisError("API key not found in ia_config.json");
        return;
    }

    bool isNewInventoryText{ inventoryText != m_userInventoryInput };
    if (isNewInventoryText)
        m_userInventoryInput = inventoryText;

    m_request = createRequest(inventoryText);

    QNetworkReply* reply{ m_networkManager->post(m_request.request, m_request.jsonData) };
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleAnalyseInventoryResponse(reply); });
}


InventoryAnalyzer::Request InventoryAnalyzer::createRequest(const QString& inventoryText)
{
    // Convertir la référence JSON en string
    QJsonDocument refDoc(m_volumeReference);
    QString jsonReference = refDoc.toJson(QJsonDocument::Compact);

    // Utiliser la classe IA pour construire la requête
    QNetworkRequest request{ m_aiService->buildRequest(inventoryText, AIService::RequestType::AnalyseInventory, &jsonReference) };
    QByteArray jsonData{ request.attribute(QNetworkRequest::User).toByteArray() };

    // Supprimer l'attribut temporaire
    request.setAttribute(QNetworkRequest::User, QVariant());
 
    return { request, jsonData };
}


void InventoryAnalyzer::handleAnalyseInventoryResponse(QNetworkReply* reply)
{
   if (reply->error() != QNetworkReply::NoError)
   {
       emit analysisError("API Grok error: " + reply->errorString());
       reply->deleteLater();

       m_aiService->setCurrentAIModel(m_aiModelBuffer.back());
       removeModelFromBuffer(&m_aiModelBuffer.back());
       analyzeInventory(m_userInventoryInput);
       return;
   }

   QVector<MovingObject> objectList{ extractReplyInfos(reply) };
   if (objectList.isEmpty())
   {
       emit analysisError("Error: AI has returned an empty list !");
       reply->deleteLater();

       m_aiService->setCurrentAIModel(m_aiModelBuffer.back());
       removeModelFromBuffer(&m_aiModelBuffer.back());
       analyzeInventory(m_userInventoryInput);
       return;
   }

   else
   {
       double listTotalVolume{};
       for (const MovingObject& object : objectList)
           listTotalVolume += object.getTotalVolume();

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
    if (response.contains("choices") && response["choices"].isArray())
    {
        QJsonArray choices{ response["choices"].toArray() };

        if (!choices.isEmpty())
        {
            QJsonObject firstChoice{ choices[0].toObject() };
            QString responseText{ firstChoice["message"].toObject()["content"].toString() };

            int jsonStart{ static_cast<int>(responseText.indexOf("{")) };
            int jsonEnd{ static_cast<int>(responseText.lastIndexOf("}") + 1) };

            if (jsonStart >= 0 && jsonEnd > jsonStart)
            {
                QString jsonText{ responseText.mid(jsonStart, jsonEnd - jsonStart) };

                // Parser le JSON avec vérification d'erreur
                QJsonParseError parseError{};
                QJsonDocument itemsDoc{ QJsonDocument::fromJson(jsonText.toUtf8(), &parseError) };

                // Vérifier si le parsing a réussi
                if (parseError.error != QJsonParseError::NoError)
                {
                    emit error("JSON parsing error: " + parseError.errorString() + " at offset " + QString::number(parseError.offset));
                    return QVector<MovingObject>{};
                }

                // Vérifier si le document est valide
                if (itemsDoc.isNull() || itemsDoc.isEmpty())
                {
                    emit error("JSON document is null or empty");
                    return QVector<MovingObject>{};
                }

                QJsonObject itemsObj{ itemsDoc.object() };
                QVector<MovingObject> objectList{};

                if (itemsObj.contains("items"))
                {
                    QJsonArray items{ itemsObj["items"].toArray() };
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
                }
                else
                {
                    // Déboguer : afficher les clés présentes dans l'objet JSON
                    QStringList keys{};
                    for (const QString& key : itemsObj.keys())
                    {
                        keys.append(key);
                    }
                    emit error("JSON object does not contain 'items' key. Available keys: " + keys.join(", "));
                }
                return std::move(objectList);
            }
        }
    }
    return QVector<MovingObject>{};
}
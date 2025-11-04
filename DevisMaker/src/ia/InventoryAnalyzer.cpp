#include "InventoryAnalyzer.h"

InventoryAnalyzer::InventoryAnalyzer(QObject* parent)
    : QObject(parent)
{
    connect(m_ia, &IA::error, this, &InventoryAnalyzer::error);

    loadVolumeReference();
}


void InventoryAnalyzer::cleanList(QString rawText)
{
    if (m_ia->getAPI_Key().isEmpty())
    {
        emit analysisError("API key not found in ia_config.json");
        return;
    }

    // Utiliser la classe IA pour construire la requête
    QNetworkRequest request{ m_ia->buildCleanTextRequest(rawText) };
    QByteArray jsonData{ request.attribute(QNetworkRequest::User).toByteArray() };

    // Supprimer l'attribut temporaire
    request.setAttribute(QNetworkRequest::User, QVariant());

    QNetworkReply* reply{ m_networkManager->post(request, jsonData) };
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleCleanNameResponse(reply);
        });
}

void InventoryAnalyzer::analyzeInventory(const QString& inventoryText)
{

    if (m_ia->getAPI_Key().isEmpty())
    {
        emit analysisError("API key not found in ia_config.json");
        return;
    }

    bool isNewInventoryText{ inventoryText != m_userInventoryInput };
    if (isNewInventoryText)
        m_userInventoryInput = inventoryText;

    m_request = createRequest(inventoryText);

    QNetworkReply* reply{ m_networkManager->post(m_request.request, m_request.jsonData) };
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleGrokResponse(reply); });
}


InventoryAnalyzer::Request InventoryAnalyzer::createRequest(const QString& inventoryText)
{
    // Convertir la référence JSON en string
    QJsonDocument refDoc(m_volumeReference);
    QString jsonReference = refDoc.toJson(QJsonDocument::Compact);

    // Utiliser la classe IA pour construire la requête
    QNetworkRequest request{ m_ia->buildRequest(inventoryText, jsonReference) };
    QByteArray jsonData{ request.attribute(QNetworkRequest::User).toByteArray() };

    // Supprimer l'attribut temporaire
    request.setAttribute(QNetworkRequest::User, QVariant());
 
    return { request, jsonData };
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


void InventoryAnalyzer::handleGrokResponse(QNetworkReply* reply)
{
   if (reply->error() != QNetworkReply::NoError)
   {
       emit analysisError("Erreur API Grok: " + reply->errorString());
       reply->deleteLater();
       return;
   }

   QVector<MovingObject> objectList{ extractReplyInfos(reply) };
   if (objectList.isEmpty())
   {
       emit analysisError("Format de reponse Grok inattendu");

       m_ia->setCurrentModel(IA::fallback);
       analyzeInventory(m_userInventoryInput);
   }

   else
   {
       double listTotalVolume{};
       for (const MovingObject& object : objectList)
           listTotalVolume += object.getTotalVolume();

       addFallbackResult(listTotalVolume);
       addFallbackAttempt();

       bool hasReachedMaxAttempts{ getFallbackAttempts() >= m_ia->getMaxFallbackAttempts() };
       if (hasReachedMaxAttempts)
       {
           double averageVolume{ calculateAverageVolume(getFallbackResults()) };
           emit analysisComplete(averageVolume, objectList);
           
           if (m_ia->getCurrentModelString() == m_ia->getFallbackModel())
               m_ia->setCurrentModel(IA::primary);

           clearFallbackAttempts();
           clearFallbackResults();
       }

       else
       {
           std::function<void()> sendRequest{ [this]() {
               QNetworkReply* reply{m_networkManager->post(m_request.request, m_request.jsonData)};
               connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleGrokResponse(reply); });
           } };

           QTimer::singleShot(NETWORK_REQUEST_DELAY, this, sendRequest);
       }
   }

    reply->deleteLater();
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
            QString responseText{ firstChoice["message"].toObject()["content"].toString() };
            analyzeInventory(responseText);
        }
    }

    reply->deleteLater();
}

double InventoryAnalyzer::calculateAverageVolume(const QVector<double>& results)
{
    double averageVolume{};
    double resultsNumber{ static_cast<double>(results.size()) };

    for (double volume : results)
        averageVolume += volume;

    return averageVolume /= resultsNumber;
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
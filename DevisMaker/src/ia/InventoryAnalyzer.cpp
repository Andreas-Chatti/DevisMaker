#include "InventoryAnalyzer.h"

InventoryAnalyzer::InventoryAnalyzer(QObject* parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);

    m_ia = new IA{};

    loadVolumeReference();

    connect(m_networkManager, &QNetworkAccessManager::finished, this, &InventoryAnalyzer::handleGrokResponse);

    connect(this, &InventoryAnalyzer::resultsAnalysis, this, &InventoryAnalyzer::calculateAverageVolume);
}


void InventoryAnalyzer::analyzeInventory(const QString& inventoryText)
{
    m_request = createRequest(inventoryText);

    // Envoyer la requête
    m_networkManager->post(m_request.request, m_request.jsonData);
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


std::optional<InventoryAnalyzer::ReplyInfos> InventoryAnalyzer::extractReplyInfos(QNetworkReply* reply)
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

            // Extraire le JSON de la réponse
            // Chercher le début et la fin du JSON dans la réponse
            int jsonStart = responseText.indexOf("{");
            int jsonEnd = responseText.lastIndexOf("}") + 1;


            if (jsonStart >= 0 && jsonEnd > jsonStart)
            {
                QString jsonText = responseText.mid(jsonStart, jsonEnd - jsonStart);

                QJsonDocument itemsDoc = QJsonDocument::fromJson(jsonText.toUtf8());
                QJsonObject itemsObj = itemsDoc.object();

                if (itemsObj.contains("items") && itemsObj.contains("totalVolume"))
                {
                    // Extraire le volume total
                    double totalVolume = itemsObj["totalVolume"].toDouble();

                    // Extraire les éléments structurés
                    QStringList structuredItems;
                    QJsonArray items = itemsObj["items"].toArray();

                    for (const QJsonValue& item : items)
                    {
                        QJsonObject itemObj = item.toObject();
                        QString name = itemObj["name"].toString();
                        double volume = itemObj["volume"].toDouble();
                        structuredItems.append(QString("%1 - %2 m\u00B3").arg(name).arg(volume));
                    }

                    return std::make_optional<ReplyInfos>({ totalVolume, structuredItems });
                }
            }
        }
    }
    return std::nullopt;
}


void InventoryAnalyzer::handleGrokResponse(QNetworkReply* reply)
{
   if (reply->error() != QNetworkReply::NoError)
   {
       emit analysisError("Erreur API Grok: " + reply->errorString());
       reply->deleteLater();
       return;
   }

   auto replyInfos{ extractReplyInfos(reply) };
   if (!replyInfos)
   {
       emit analysisError("Format de reponse Grok inattendu");

       m_ia->setCurrentModel(IA::fallback);
       analyzeInventory(m_request.request.attribute(QNetworkRequest::User).toJsonObject().value("content").toString());
   }

   else
   {
       double volume{ replyInfos.value().volume };
       QStringList structuredItems{ replyInfos.value().structuredItems };

       addFallbackResult(volume);
       addFallbackAttempt();

       bool hasReachedMaxAttempts{ getFallbackAttempts() >= m_ia->getMaxFallbackAttempts() };
       if (hasReachedMaxAttempts)
           emit resultsAnalysis(getFallbackResults(), structuredItems);


       else
       {
           std::function<void()> sendRequest{[this]() {
               m_networkManager->post(m_request.request, m_request.jsonData); }};

           QTimer::singleShot(NETWORK_REQUEST_DELAY, this, sendRequest);
       }
   }

    reply->deleteLater();
}


void InventoryAnalyzer::calculateAverageVolume(QVector<double> results, const QStringList& structuredItems)
{
    double finalVolume{};
    double resultsNumber{ static_cast<double>(results.size()) };

    for (double value : results)
        finalVolume += value;

    finalVolume /= resultsNumber;

    emit analysisComplete(finalVolume, structuredItems);

    clearFallbackAttempts();
    clearFallbackResults();
}


void InventoryAnalyzer::loadVolumeReference() 
{
    QFile file(SettingsConstants::FileSettings::DATA_FILE_PATH + "/volumes_reference.json");

    if (!file.open(QIODevice::ReadOnly)) 
    {
        qDebug() << "Impossible d'ouvrir le fichier de reference";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    m_volumeReference = doc.object();
    file.close();

    qDebug() << "Reference de volumes chargee avec succes";
}
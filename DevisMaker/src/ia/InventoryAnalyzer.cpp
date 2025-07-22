// InventoryAnalyzer.cpp
#include "InventoryAnalyzer.h"


InventoryAnalyzer::InventoryAnalyzer(QObject* parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);

    m_ia = new IA{};

    loadVolumeReference();

    connect(m_networkManager, &QNetworkAccessManager::finished, this, &InventoryAnalyzer::handleGrokResponse);
}


void InventoryAnalyzer::analyzeInventory(const QString& inventoryText)
{
    // Convertir la référence JSON en string
    QJsonDocument refDoc(m_volumeReference);
    QString jsonReference = refDoc.toJson(QJsonDocument::Compact);

    // Utiliser la classe IA pour construire la requête
    QNetworkRequest request{ m_ia->buildRequest(inventoryText, jsonReference) };
    QByteArray jsonData{ request.attribute(QNetworkRequest::User).toByteArray() };

    // Supprimer l'attribut temporaire
    request.setAttribute(QNetworkRequest::User, QVariant());

    // Envoyer la requête
    qDebug() << "Envoi de la requête avec modèle:" << m_ia->getCurrentModel();
    m_networkManager->post(request, jsonData);
}


void InventoryAnalyzer::handleGrokResponse(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) 
    {
        QByteArray data = reply->readAll();
        qDebug() << "Reponse Grok recue:" << data;

        QJsonDocument doc{ QJsonDocument::fromJson(data) };
        QJsonObject response{ doc.object() };

        // Extraire la réponse de Grok
        if (response.contains("choices") && response["choices"].isArray()) 
        {
            QJsonArray choices = response["choices"].toArray();

            if (!choices.isEmpty()) 
            {
                QJsonObject firstChoice{ choices[0].toObject() };
                QString responseText{ firstChoice["message"].toObject()["content"].toString() };

                qDebug() << "Contenu de la reponse: " << responseText;

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

                        for (const QJsonValue& item : items) {
                            QJsonObject itemObj = item.toObject();
                            QString name = itemObj["name"].toString();
                            double volume = itemObj["volume"].toDouble();
                            structuredItems.append(QString("%1 - %2 m\u00B3").arg(name).arg(volume));
                        }

                        // Émettre le signal de complétion
                        emit analysisComplete(totalVolume, structuredItems);

                        qDebug() << "Analyse Grok reussie. Volume total:" << totalVolume;
                        reply->deleteLater();
                        return;
                    }
                }
            }
        }

        // Si nous arrivons ici, le format n'était pas correct
        emit analysisError("Format de reponse Grok inattendu: " + response["error"].toObject()["message"].toString());
    }

    else
        emit analysisError("Erreur API Grok: " + reply->errorString());

    reply->deleteLater();
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
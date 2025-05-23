// InventoryAnalyzer.cpp
#include "InventoryAnalyzer.h"


void InventoryAnalyzer::analyzeInventory(const QString& inventoryText)
{
    // Convertir la référence JSON en string
    QJsonDocument refDoc(m_volumeReference);  // ← Renommé en refDoc
    QString jsonReference = refDoc.toJson(QJsonDocument::Compact);


    QString prompt = QString(R"(
Tu dois analyser cet inventaire de déménagement et calculer les volumes avec cette référence : %1

RÈGLES IMPORTANTES:
- Lis chaque ligne attentivement
- Si tu vois "matelas ET sommiers" = ce sont 2 objets différents à lister séparément
- Si tu vois "2 matelas et 2 sommiers" = 4 objets au total (2+2)
- INTERDIT d'inventer des volumes ! Utilise OBLIGATOIREMENT les valeurs exactes de la référence fournie
- Trouve l'objet le plus proche dans la référence si pas de correspondance exacte
- Calcule: quantité × volume_unitaire_référence = volume_total

RÉPONSE OBLIGATOIRE: JSON pur uniquement, sans texte avant ou après.
Commence par { et finis par }

Format exact:
{
  "items": [
    {"name": "2 matelas 1 place", "volume": 1.0},
    {"name": "2 sommiers", "volume": 1.0}
  ],
  "totalVolume": 2.0
}

INVENTAIRE À ANALYSER:
%2
)").arg(jsonReference, inventoryText);


    // Construire la requête pour l'API Grok
    QUrl url("https://api.groq.com/openai/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    // Corps de la requête JSON pour Grok
    QJsonObject jsonBody;
    jsonBody["model"] = "gemma2-9b-it";
    jsonBody["max_tokens"] = 4000;
    jsonBody["temperature"] = 0.1; // Faible température pour plus de cohérence

    QJsonArray messages;
    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = prompt;
    messages.append(userMessage);

    jsonBody["messages"] = messages;

    QJsonDocument doc(jsonBody);  // ← Celui-ci garde le nom "doc"
    QByteArray jsonData = doc.toJson();

    // Envoyer la requête
    qDebug() << "Envoi de la requête à Grok...";
    m_networkManager->post(request, jsonData);
}


void InventoryAnalyzer::handleGrokResponse(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) 
    {
        QByteArray data = reply->readAll();
        qDebug() << "Reponse Grok recue:" << data;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject response = doc.object();

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

                        qDebug() << "Analyse Grok réussie. Volume total:" << totalVolume;
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
    QFile file("volumes_reference.json");

    if (!file.open(QIODevice::ReadOnly)) 
    {
        qDebug() << "Impossible d'ouvrir le fichier de référence";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    m_volumeReference = doc.object();
    file.close();

    qDebug() << "Référence de volumes chargée avec succès";
}
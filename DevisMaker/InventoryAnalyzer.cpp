// InventoryAnalyzer.cpp
#include "InventoryAnalyzer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


void InventoryAnalyzer::analyzeInventory(const QString& inventoryText)
{
    // Prompt optimisé pour Grok
    QString prompt = QString(R"(
Analyse cette liste d'objets de déménagement et calcule le volume total.
Pour chaque objet, détermine son volume approximatif en m³.

RÈGLES:
- Canapé 2-3 places: 2 m³
- Lit simple: 1 m³, Lit double: 1.5 m³
- Armoire 2 portes: 2 m³, 3 portes: 3 m³
- Table à manger: 1 m³
- Chaise: 0.3 m³
- Réfrigérateur: 1.5 m³
- Lave-linge/lave-vaisselle: 1 m³
- Télévision: 0.5 m³
- Commode: 1 m³
- Carton standard: 0.1 m³
- Carton livre: 0.05 m³

IMPORTANT: Les valeurs de volume doivent être des nombres purs, pas des expressions mathématiques. N'utilise pas de caractère d'échappement comme "/n".
RÉPONDS UNIQUEMENT EN JSON PUR, SANS TEXTE EXPLICATIF AVANT OU APRÈS.
RÉPONDS ABSOLUMENT UNIQUEMENT AU FORMAT JSON.
{
  "items": [
    {"name": "Canapé 3 places", "volume": 2.0},
    {"name": "Table à manger", "volume": 1.0}
  ],
  "totalVolume": 3.0
}

LISTE À ANALYSER:
%1
)").arg(inventoryText);


    // Construire la requête pour l'API Grok
    QUrl url("https://api.groq.com/openai/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    // Corps de la requête JSON pour Grok
    QJsonObject jsonBody;
    jsonBody["model"] = "llama-3.1-8b-instant";
    jsonBody["max_tokens"] = 2000;
    jsonBody["temperature"] = 0.1; // Faible température pour plus de cohérence

    QJsonArray messages;
    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = prompt;
    messages.append(userMessage);

    jsonBody["messages"] = messages;

    QJsonDocument doc(jsonBody);
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
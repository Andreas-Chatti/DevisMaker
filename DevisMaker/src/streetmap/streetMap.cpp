#include "streetMap.h"

QNetworkRequest OpenStreetMap::createRequest(const QString& qItem)
{
    QUrl url(URL_STREET_MAP);
    QUrlQuery query;
    query.addQueryItem("format", "json");
    query.addQueryItem("q", qItem);
    query.addQueryItem("limit", "1");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker");

    return request;
}


void OpenStreetMap::calculateDistance(const QString& adresseDepart, const QString& adresseArrivee)
{
    if (adresseDepart.isEmpty() || adresseArrivee.isEmpty()) 
    {
        qDebug() << "[streetMap] At least one address is empty ! Cannot calculate distance.";
        return;
    }

    QNetworkRequest request{ createRequest(adresseDepart) };

    QNetworkReply* reply{ m_networkManager->get(request) };
    reply->setProperty("adresseArrivee", adresseArrivee);
    reply->setProperty("requestType", RequestType::geocodeDepart);
}


void OpenStreetMap::handleDistanceReply(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) 
    {
        qDebug() << "[streetMap] Network error: " + reply->errorString();
        return;
    }

    RequestType requestType{ static_cast<RequestType>(reply->property("requestType").toInt()) };
    QByteArray data{ reply->readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonArray array{ doc.array() };

    if (array.isEmpty()) 
    {
        qDebug() << "[streetMap] Array is empty ! Cannot calculate distance.";
        return;
    }

    switch (requestType)
    {
    case OpenStreetMap::geocodeDepart:
    {
        QJsonObject obj{ array.first().toObject() };
        QString lat{ obj["lat"].toString() };
        QString lon{ obj["lon"].toString() };

        QString adresseArrivee{ reply->property("adresseArrivee").toString() };

        QNetworkRequest request{ createRequest(adresseArrivee) };

        QNetworkReply* newReply{ m_networkManager->get(request) };
        newReply->setProperty("departLat", lat);
        newReply->setProperty("departLon", lon);
        newReply->setProperty("requestType", RequestType::geocodeArrivee);
        break;
    }
    case OpenStreetMap::geocodeArrivee:
    {
        QJsonObject obj{ array.first().toObject() };
        QString arriveeLat{ obj["lat"].toString() };
        QString arriveeLon{ obj["lon"].toString() };

        QString departLat{ reply->property("departLat").toString() };
        QString departLon{ reply->property("departLon").toString() };

        QString startCoords{ departLon + "," + departLat };
        QString endCoords{ arriveeLon + "," + arriveeLat };

        requestRouteDistance(startCoords, endCoords);
    }
    }
}


void OpenStreetMap::requestRouteDistance(const QString& startCoords, const QString& endCoords)
{
    QUrl osrmUrl{ URL_OSRM.arg(startCoords, endCoords) };

    QNetworkRequest request(osrmUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker");

    QNetworkReply* reply{ m_networkManager->get(request) };
    reply->setProperty("requestType", RequestType::routeCalculation);
}

void OpenStreetMap::handleRouteResponse(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "[streetMap] Error calculating route: " + reply->errorString();
        return;
    }

    QByteArray data{ reply->readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonObject response{ doc.object() };

    if (response.contains("routes") && response["routes"].isArray())
    {
        QJsonArray routes{ response["routes"].toArray() };

        if (!routes.isEmpty())
        {
            QJsonObject route{ routes.first().toObject()};
            double distanceMeters{ route["distance"].toDouble() };
            double distanceKm{ distanceMeters / 1000.0 };

            emit distanceCalculated(distanceKm);
            return;
        }
    }

    qDebug() << "[streetMap] Route calculation Failed.";
}
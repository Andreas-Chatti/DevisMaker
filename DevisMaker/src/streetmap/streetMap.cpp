#include "streetMap.h"


void OpenStreetMap::calculateDistance(const QString& adresseDepart, const QString& adresseArrivee)
{
    if (adresseDepart.isEmpty() || adresseArrivee.isEmpty()) 
    {
        emit calculationError("Adresses vides");
        return;
    }

    qDebug() << "Calcul de distance entre:" << adresseDepart << " et " << adresseArrivee;

    // Première requête: géocoder l'adresse de départ
    QUrl url(m_baseUrl);
    QUrlQuery query;
    query.addQueryItem("format", "json");
    query.addQueryItem("q", adresseDepart);
    query.addQueryItem("limit", "1");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker");

    QNetworkReply* reply{ m_networkManager->get(request) };
    reply->setProperty("adresseArrivee", adresseArrivee);
    reply->setProperty("requestType", "geocodeDepart");
}


void OpenStreetMap::handleDistanceReply(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) 
    {
        emit calculationError("Erreur réseau: " + reply->errorString());
        return;
    }

    QString requestType{ reply->property("requestType").toString() };
    QByteArray data{ reply->readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonArray array{ doc.array() };

    if (array.isEmpty()) 
    {
        emit calculationError("Adresse non trouvee");
        return;
    }

    if (requestType == "geocodeDepart") 
    {
        // Obtenir les coordonnées de l'adresse de départ
        QJsonObject obj = array.at(0).toObject();
        QString lat = obj["lat"].toString();
        QString lon = obj["lon"].toString();

        QString adresseArrivee = reply->property("adresseArrivee").toString();

        QUrl url(m_baseUrl);
        QUrlQuery query;
        query.addQueryItem("format", "json");
        query.addQueryItem("q", adresseArrivee);
        query.addQueryItem("limit", "1");
        url.setQuery(query);

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");

        QNetworkReply* newReply = m_networkManager->get(request);
        newReply->setProperty("departLat", lat);
        newReply->setProperty("departLon", lon);
        newReply->setProperty("requestType", "geocodeArrivee");
    }

    else if (requestType == "geocodeArrivee") 
    {
        // Obtenir les coordonnées de l'adresse d'arrivée
        QJsonObject obj{ array.at(0).toObject() };
        QString arriveeLat{ obj["lat"].toString() };
        QString arriveeLon{ obj["lon"].toString() };

        // Récupérer les coordonnées de départ
        QString departLat = reply->property("departLat").toString();
        QString departLon = reply->property("departLon").toString();

        // Calculer la distance
        QString startCoords{ departLon + "," + departLat };  // Format OSRM : lon,lat
        QString endCoords{ arriveeLon + "," + arriveeLat };
        qDebug() << "Appel OSRM avec coordonnées:" << startCoords << "vers" << endCoords;
        requestRouteDistance(startCoords, endCoords);
    }
}


void OpenStreetMap::requestRouteDistance(const QString& startCoords, const QString& endCoords)
{
    QString osrmUrl{ QString("http://router.project-osrm.org/route/v1/driving/%1;%2?overview=false&geometries=geojson").arg(startCoords, endCoords) };

    QNetworkRequest request(osrmUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker");

    QNetworkReply* reply{ m_networkManager->get(request) };
    reply->setProperty("requestType", "routeCalculation");

    qDebug() << "Requete OSRM: " << osrmUrl;
}

void OpenStreetMap::handleRouteResponse(QNetworkReply* reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        emit calculationError("Erreur calcul itinéraire: " + reply->errorString());
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
            QJsonObject route{ routes[0].toObject() };
            double distanceMeters{ route["distance"].toDouble() };
            double distanceKm{ distanceMeters / 1000.0 };

            qDebug() << "Distance routiere OSRM: " << distanceKm << " km";
            emit distanceCalculated(distanceKm);
            return;
        }
    }

    emit calculationError("Impossible de calculer l'itineraire");
}
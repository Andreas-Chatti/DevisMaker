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
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");

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

        // Géocoder l'adresse d'arrivée
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
        double distance{ calculateHaversineDistance(departLat.toDouble(), departLon.toDouble(), arriveeLat.toDouble(), arriveeLon.toDouble()) };

        qDebug() << "Distance calculée:" << distance << "km";

        // ÉMETTRE LE SIGNAL au lieu de modifier directement l'UI
        emit distanceCalculated(distance);
    }
}

double OpenStreetMap::calculateHaversineDistance(double lat1, double lon1, double lat2, double lon2)
{
    constexpr double R{ 6371.0 };
    double dLat{ (lat2 - lat1) * M_PI / 180.0 };
    double dLon{ (lon2 - lon1) * M_PI / 180.0 };

    double a{ sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
        sin(dLon / 2) * sin(dLon / 2) };

    double c{ 2 * atan2(sqrt(a), sqrt(1 - a)) };
    return R * c;
}
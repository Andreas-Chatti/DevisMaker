#include "streetMap.h"

void OpenStreetMap::calculateDistance(const QString& adresseDepart, const QString& adresseArrivee)
{

    if (adresseDepart.isEmpty() || adresseArrivee.isEmpty())
        return;

    qDebug() << "Calcul de distance entre: " << adresseDepart << " et " << adresseArrivee;

    // Initialiser si nécessaire
    if (!m_networkManager)
    {
        m_networkManager = new QNetworkAccessManager(this);
        connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenStreetMap::handleDistanceReply);
    }

    // Première requête: géocoder l'adresse de départ
    QUrl url("https://nominatim.openstreetmap.org/search");
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


double OpenStreetMap::calculateHaversineDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Rayon de la Terre en km
    constexpr double R{ 6371.0 };

    // Convertir en radians
    double dLat{ (lat2 - lat1) * M_PI / 180.0 };
    double dLon{ (lon2 - lon1) * M_PI / 180.0 };

    // Formule haversine
    double a{ sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
        sin(dLon / 2) * sin(dLon / 2) };

    double c{ 2 * atan2(sqrt(a), sqrt(1 - a)) };


    return R * c;
}


void OpenStreetMap::handleDistanceReply(QNetworkReply* reply, Ui::MainWindowClass ui, Client client)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::warning(this, "Erreur", "Erreur réseau: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QString requestType{ reply->property("requestType").toString() };
    QByteArray data{ reply->readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonArray array{ doc.array() };

    if (array.isEmpty())
    {
        QMessageBox::warning(this, "Adress not found", "Cannot find specified adress");
        reply->deleteLater();
        return;
    }

    if (requestType == "geocodeDepart")
    {
        // Obtenir les coordonnées de l'adresse de départ
        QJsonObject obj = array.at(0).toObject();
        QString lat = obj["lat"].toString();
        QString lon = obj["lon"].toString();

        // Géocoder l'adresse d'arrivée
        QString adresseArrivee{ reply->property("adresseArrivee").toString() };

        QUrl url("https://nominatim.openstreetmap.org/search");
        QUrlQuery query;
        query.addQueryItem("format", "json");
        query.addQueryItem("q", adresseArrivee);
        query.addQueryItem("limit", "1");

        url.setQuery(query);

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");

        QNetworkReply* newReply{ m_networkManager->get(request) };
        newReply->setProperty("departLat", lat);
        newReply->setProperty("departLon", lon);
        newReply->setProperty("requestType", "geocodeArrivee");
    }

    else if (requestType == "geocodeArrivee")
    {
        // Obtenir les coordonnées de l'adresse d'arrivée
        QJsonObject obj = array.at(0).toObject();
        QString arriveeLat = obj["lat"].toString();
        QString arriveeLon = obj["lon"].toString();

        // Récupérer les coordonnées de départ
        QString departLat = reply->property("departLat").toString();
        QString departLon = reply->property("departLon").toString();

        // Calculer la distance
        double distance = calculateHaversineDistance(departLat.toDouble(), departLon.toDouble(), arriveeLat.toDouble(), arriveeLon.toDouble());

        // Afficher la distance calculée
        ui.distanceLineEdit->setText(QString::number(distance, 'f', 1));

        // Mettre à jour le modèle client
        client.setDistance(distance);

        qDebug() << "Distance calculée: " << distance << " km";
    }

    reply->deleteLater();
}
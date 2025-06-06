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
        connect(m_networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleDistanceReply);
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
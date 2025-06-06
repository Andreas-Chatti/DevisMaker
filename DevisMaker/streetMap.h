#pragma once

#include <QUrlQuery>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class OpenStreetMap
{
public:

	OpenStreetMap()
	{
		m_networkManager = new QNetworkAccessManager(this);
	}

	void calculateDistance(const QString& adresseChargement, const QString& adresseLivraison);

	double calculateHaversineDistance(double lat1, double lon1, double lat2, double lon2);

private:

	const QUrl m_url{ "https://nominatim.openstreetmap.org/search" };

	void handleDistanceReply(QNetworkReply* reply);

	QNetworkAccessManager* m_networkManager;
};

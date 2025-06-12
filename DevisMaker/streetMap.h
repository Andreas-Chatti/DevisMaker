#pragma once

#include <QUrlQuery>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class OpenStreetMap : public QObject
{
	Q_OBJECT

public:

	OpenStreetMap(QObject* parent)
		: QObject{ parent }
	{
		m_networkManager = new QNetworkAccessManager(this);
	}

	void calculateDistance(const QString& adresseChargement, const QString& adresseLivraison);

	double calculateHaversineDistance(double lat1, double lon1, double lat2, double lon2);

private:

	const QUrl m_url{ "https://nominatim.openstreetmap.org/search" };

	void handleDistanceReply(QNetworkReply* reply, Ui::MainWindowClass ui, Client client);

	QNetworkAccessManager* m_networkManager;
};

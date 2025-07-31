#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class OpenStreetMap : public QObject
{
    Q_OBJECT

public:

    explicit OpenStreetMap(QObject* parent = nullptr)
        : QObject(parent)
    {
        m_networkManager = new QNetworkAccessManager(this);

        connect(m_networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {

                QString requestType{ reply->property("requestType").toString() };

                if (requestType == "routeCalculation")
                    handleRouteResponse(reply);

                else
                    handleDistanceReply(reply);
            });
    }

    const QUrl& getUrl() { return m_baseUrl; }

signals:

    // OpenStreetMap émet ces signaux, MainWindow les reçoit
    void distanceCalculated(double distance);
    void calculationError(const QString& errorMessage);


public slots:


    void calculateDistance(const QString& adresseChargement, const QString& adresseLivraison);


private slots:

    // OpenStreetMap reçoit le signal de QNetworkReply
    void handleDistanceReply(QNetworkReply* reply);


private:

    void requestRouteDistance(const QString& startCoords, const QString& endCoords);
    void handleRouteResponse(QNetworkReply* reply);

    QNetworkAccessManager* m_networkManager;
    const QUrl m_baseUrl{ "https://nominatim.openstreetmap.org/search" };
};
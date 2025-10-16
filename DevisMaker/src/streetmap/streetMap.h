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

    enum RequestType
    {
        geocodeDepart,
        geocodeArrivee,
        routeCalculation
    };

    explicit OpenStreetMap(QObject* parent = nullptr)
        : QObject(parent)
    {
        const auto handleRequestType{ [this](QNetworkReply* reply) {

            RequestType requestType{ static_cast<RequestType>(reply->property("requestType").toInt()) };

            if (requestType == RequestType::routeCalculation)
                handleRouteResponse(reply);

            else
                handleDistanceReply(reply);
        }};

        connect(m_networkManager, &QNetworkAccessManager::finished, this, handleRequestType);
    }

    static inline const QUrl URL_STREET_MAP{ "https://nominatim.openstreetmap.org/search" };

    /*
        Type QString needed for arguments %1 and %2
    */
    static inline const QString URL_OSRM{ "http://router.project-osrm.org/route/v1/driving/%1;%2?overview=false&geometries=geojson" };

signals:

    void distanceCalculated(double distance);
    void calculationError(const QString& errorMessage);

public slots:

    void calculateDistance(const QString& adresseChargement, const QString& adresseLivraison);

private slots:

    void handleDistanceReply(QNetworkReply* reply);

private:

    void requestRouteDistance(const QString& startCoords, const QString& endCoords);
    void handleRouteResponse(QNetworkReply* reply);
    QNetworkRequest createRequest(const QString& qItem);


    QNetworkAccessManager* m_networkManager{ new QNetworkAccessManager{this} };
};
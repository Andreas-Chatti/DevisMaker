#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <qtimer.h>
#include <optional>
#include "ia/IA.h"
#include "utils/constants.h"
#include "inventory/movingObject.h"

class InventoryAnalyzer : public QObject
{
    Q_OBJECT

public:

    struct Request
    {
        QNetworkRequest request;
        QByteArray jsonData;
    };

    struct ReplyInfos
    {
        double volume;
        QStringList structuredItems;
    };

    InventoryAnalyzer(QObject* parent = nullptr);

    void cleanList(QString text);
    void analyzeInventory(const QString& inventoryText);

signals:

    void analysisComplete(double totalVolume, QVector<MovingObject>& objectList);
    void analysisError(const QString& errorMessage);
    void error(const QString& errorMessage);

private slots:

    void handleGrokResponse(QNetworkReply* reply);
    void handleCleanNameResponse(QNetworkReply* reply);
    double calculateAverageVolume(const QVector<double>& results);

private:

    void loadVolumeReference();

    Request createRequest(const QString& inventoryText);

    QVector<MovingObject> extractReplyInfos(QNetworkReply* reply);

    void addFallbackResult(double result) { m_fallbackResults.emplace_back(result); }
    void addFallbackAttempt() { m_fallbackAttempts++; }
    void clearFallbackAttempts() { m_fallbackAttempts = 0; }
    void clearFallbackResults() { m_fallbackResults.clear(); };
    const QVector<double>& getFallbackResults() { return m_fallbackResults; }
    int getFallbackAttempts() { return m_fallbackAttempts; }

    QVector<double> m_fallbackResults;
    int m_fallbackAttempts;
    static constexpr int NETWORK_REQUEST_DELAY{ 1500 };
    const QString REFERENCE_FILE_NAME{ "/volumes_reference.json" };

    QNetworkAccessManager* m_networkManager{ new QNetworkAccessManager{this} };
    QJsonObject m_volumeReference{};
    IA* m_ia{ new IA{this} };
    Request m_request{};
    QString m_userInventoryInput{""};
};
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

    void analyzeInventory(const QString& inventoryText);

signals:

    void analysisComplete(double totalVolume, const QStringList& structuredItems);
    void analysisError(const QString& errorMessage);
    void resultsAnalysis(QVector<double> results, const QStringList& structuredItems);

private slots:

    void handleGrokResponse(QNetworkReply* reply);
    void calculateAverageVolume(QVector<double> results, const QStringList& structuredItems);

private:

    void loadVolumeReference();

    Request createRequest(const QString& inventoryText);

    std::optional<ReplyInfos> extractReplyInfos(QNetworkReply* reply);

    void addFallbackResult(double result) { m_fallbackResults.emplace_back(result); }
    void addFallbackAttempt() { m_fallbackAttempts++; }
    void clearFallbackAttempts() { m_fallbackAttempts = 0; }
    void clearFallbackResults() { m_fallbackResults.clear(); };
    const QVector<double>& getFallbackResults() { return m_fallbackResults; }
    int getFallbackAttempts() { return m_fallbackAttempts; }

    QVector<double> m_fallbackResults;
    int m_fallbackAttempts;
    const int NETWORK_REQUEST_DELAY{ 1500 };

    QNetworkAccessManager* m_networkManager;
    QJsonObject m_volumeReference;
    IA* m_ia;
    Request m_request;
    QString m_userInventoryInput;
};
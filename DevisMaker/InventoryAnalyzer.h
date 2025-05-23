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

class InventoryAnalyzer : public QObject
{
    Q_OBJECT

public:

    InventoryAnalyzer(QObject* parent)
        : QObject(parent)
    {
        m_networkManager = new QNetworkAccessManager(this);

        loadVolumeReference();

        connect(m_networkManager, &QNetworkAccessManager::finished, this, &InventoryAnalyzer::handleGrokResponse);
    }

    void analyzeInventory(const QString& inventoryText);

signals:

    void analysisComplete(double totalVolume, const QStringList& structuredItems);
    void analysisError(const QString& errorMessage);

private slots:

    void handleGrokResponse(QNetworkReply* reply);

private:

    void loadVolumeReference();

    QNetworkAccessManager* m_networkManager;

    QJsonObject m_volumeReference;

    // Clé API directement dans le code pour simplicité (usage personnel)
    const QString m_apiKey = "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";
};
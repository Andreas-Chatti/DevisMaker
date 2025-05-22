#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class InventoryAnalyzer : public QObject
{
    Q_OBJECT

public:

    InventoryAnalyzer(QObject* parent)
        : QObject(parent)
    {
        m_networkManager = new QNetworkAccessManager(this);

        connect(m_networkManager, &QNetworkAccessManager::finished, this, &InventoryAnalyzer::handleGrokResponse);
    }

    void analyzeInventory(const QString& inventoryText);

signals:

    void analysisComplete(double totalVolume, const QStringList& structuredItems);
    void analysisError(const QString& errorMessage);

private slots:

    void handleGrokResponse(QNetworkReply* reply);

private:

    QNetworkAccessManager* m_networkManager;

    // Clé API directement dans le code pour simplicité (usage personnel)
    const QString m_apiKey = "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";
};
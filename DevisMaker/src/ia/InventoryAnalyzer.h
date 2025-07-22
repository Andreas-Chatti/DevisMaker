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
#include "ia/IA.h"
#include "utils/constants.h"

class InventoryAnalyzer : public QObject
{
    Q_OBJECT

public:

    InventoryAnalyzer(QObject* parent = nullptr);

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

    IA* m_ia;
};
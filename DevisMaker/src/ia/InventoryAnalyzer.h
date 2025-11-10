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
#include "ia/AIService.h"
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

    void handleAnalyseInventoryResponse(QNetworkReply* reply);
    void handleCleanNameResponse(QNetworkReply* reply);

private:

    void loadVolumeReference();

    Request createRequest(const QString& inventoryText);

    QVector<MovingObject> extractReplyInfos(QNetworkReply* reply);

    static constexpr int NETWORK_REQUEST_DELAY{ 1500 };
    const QString REFERENCE_FILE_NAME{ "/volumes_reference.json" };

    QNetworkAccessManager* m_networkManager{ new QNetworkAccessManager{this} };
    QJsonObject m_volumeReference{};
    AIService* m_aiService{ new AIService{this} };
    Request m_request{};
    QString m_cleanInventory{""};
    QString m_rawInventory{ "" };
};
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
    bool tryNextModelOrAbort();
    bool isJSONFormatValid(const QString& jsonText);
    std::optional<QString> textToJsonFormat(QString text);
    QVector<MovingObject> getObjectListFromReply(QJsonArray items);

    std::pair<QNetworkRequest, QByteArray> createRequest(const QString& inventoryText);

    QVector<MovingObject> extractReplyInfos(QNetworkReply* reply);

    static constexpr int NETWORK_REQUEST_DELAY{ 1500 };
    const QString REFERENCE_FILE_NAME{ "/volumes_reference.json" };

    QNetworkAccessManager* m_networkManager{ new QNetworkAccessManager{this} };
    QJsonObject m_volumeReference{};
    AIService* m_aiService{ new AIService{this} };
    QString m_cleanInventory{""};
    QString m_rawInventory{ "" };
};
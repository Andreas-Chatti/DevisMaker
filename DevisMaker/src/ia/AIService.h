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
#include "utils/constants.h"
#include "AIModel.h"


class AIService : public QObject
{
    Q_OBJECT

public:

    enum class RequestType
    {
        CleanName,
        AnalyseInventory,
    };

    explicit AIService(QObject* parent = nullptr)
        : QObject(parent)
    {
        initializePrompt();
        loadModelConfigFile();
    }

    ~AIService() = default;
    AIService(const AIService& aiService) = delete;
    AIService& operator=(const AIService& aiService) = delete;
    AIService(AIService&& aiService) = delete;
    AIService& operator=(AIService&& aiService) = delete;

    const AIModel* getCurrentAIModel() { return m_currentAIModel; }
    const QString& getPrompt() { return m_currentPrompt; }
    const QString& getAPI_Key() { return m_apiKey; }
    const QString& getCurrentPrompt() const { return m_currentPrompt; }

    void reloadPrompt();
    bool savePrompt(const QString& promptContent);
    QNetworkRequest buildRequest(const QString& inventoryText, RequestType requestType, const QString* jsonReference = nullptr);

signals:

    void error(const QString& errorMessage);

private:

    AIModel* m_currentAIModel;
    std::unique_ptr <QVector<AIModel>> m_AIModelList{ std::make_unique<QVector<AIModel>>() };
    QString m_currentPrompt;
    QString m_apiKey{""};
    static constexpr int MAX_FALLBACK_ATTEMPTS{ 3 };
    static inline const QString PROMPT_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/prompt_template.txt" };
    static inline const QString IA_MODEL_CONFIG_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/config_model_" };


    void initializePrompt();
    QString loadPrompt();
    static QString getDefaultPrompt();
    static QString getDefaultCleanListPrompt();

    AIModel loadModelParametersFromConfig(QJsonObject& jsonBody);
    bool loadModelConfigFile(int loadAttempts = 0, QString errorMessage = "");
    void saveModelToConfig(QJsonObject& jsonBody, const AIModel* aiModel);
    bool createModelConfigFile(const AIModel* aiModel);
};
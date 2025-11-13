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
#include <QDir>
#include "utils/constants.h"
#include "utils/FileManager.h"
#include "AIModel.h"

using AIModelList = std::unique_ptr <QVector<AIModel>>;

class AIService : public QObject
{
    Q_OBJECT

public:

    enum class RequestType
    {
        CleanName,
        AnalyseInventory,
        max_type,
    };

    explicit AIService(QObject* parent = nullptr)
        : QObject(parent)
    {
        initializePrompts();

        if (!loadAllAIModels())
            qCritical() << "[AIService::AIService] Loading AI Models FAILED";

        if(!loadAIMainConfig())
            qCritical() << "[AIService::AIService] Loading AI Main Config FAILED";
    }

    ~AIService() = default;
    AIService(const AIService& aiService) = delete;
    AIService& operator=(const AIService& aiService) = delete;
    AIService(AIService&& aiService) = delete;
    AIService& operator=(AIService&& aiService) = delete;

    const QVector<AIModel>* getAIModelList() const { return m_AIModelList.get(); }
    const QString& getAPI_Key() const { return m_apiKey; }
    const QString& getCleanListPrompt() const { return m_cleanListPrompt; }
    const QString& getAnalysePrompt() const { return m_analysePrompt; }
    qsizetype getCurrentAiModelIndex() const { return m_currentAiModelIndex; }

    bool reloadPrompt(const QString& path, RequestType type);
    bool savePrompt(const QString& promptContent, const QString& path);
    QNetworkRequest buildRequest(const QString& inventoryText, RequestType requestType, const QString* jsonReference = nullptr);

    qsizetype advanceToNextModel() { return m_currentAiModelIndex >= m_AIModelList->size() ? m_currentAiModelIndex : ++m_currentAiModelIndex; }
    qsizetype resetModelIndex() { return m_currentAiModelIndex = 0; }

signals:

    void error(const QString& errorMessage);

private:

    AIModelList m_AIModelList{ std::make_unique<QVector<AIModel>>() };
    qsizetype m_currentAiModelIndex{ 0 };
    QString m_analysePrompt;
    QString m_cleanListPrompt;
    QString m_apiKey{""};
    const QString ANALYSE_PROMPT_FILE_PATH{ FileManager::getPromptsPath() + "/analyse_prompt.txt"};
    const QString CLEAN_LIST_PROMPT_FILE_PATH{ FileManager::getPromptsPath() + "/cleanList_prompt.txt"};
    const QString IA_MODEL_CONFIG_FILE_PATH{ FileManager::getModelsPath() + "/config_model_"};
    const QString GLOBAL_AI_CONFIG_FILE_PATH{ FileManager::getDataPath() + "/ai_service_config.json" };

    void initializePrompts();
    QString loadPrompt(const QString& path, RequestType requestType);
    static QString getAnalyseDefaultPrompt();
    static QString getCleanListDefaultPrompt();

    bool loadAIMainConfig();
    bool createAIMainConfigFile();
    void saveAIMainConfigFile(QJsonObject& jsonBody);
    bool loadAllAIModels(int loadAttempts = 0, QString errorMessage = "");
    std::optional<AIModel> loadAIModelConfig(const QString& path);
    bool saveModelToConfig(QJsonObject& jsonBody, const AIModel* aiModel);
    bool createModelConfigFile(const AIModel* aiModel);
    bool addModelToList(AIModel model);
};
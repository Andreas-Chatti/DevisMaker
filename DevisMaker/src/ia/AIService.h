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
        loadAllAIModels();
        loadAIMainConfig();

        if (!m_AIModelList.get()->isEmpty())
            m_currentAIModel = new AIModel{ m_AIModelList.get()->back() };
    }

    ~AIService() = default;
    AIService(const AIService& aiService) = delete;
    AIService& operator=(const AIService& aiService) = delete;
    AIService(AIService&& aiService) = delete;
    AIService& operator=(AIService&& aiService) = delete;

    const AIModel* getCurrentAIModel() const { return m_currentAIModel; }
    const QVector<AIModel>* getAIModelList() const { return m_AIModelList.get(); }
    const QString& getAPI_Key() const { return m_apiKey; }
    const QString& getCleanListPrompt() const { return m_cleanListPrompt; }
    const QString& getAnalysePrompt() const { return m_analysePrompt; }

    void setCurrentAIModel(AIModel model);

    bool reloadPrompt(const QString& path, RequestType type);
    bool savePrompt(const QString& promptContent, const QString& path);
    QNetworkRequest buildRequest(const QString& inventoryText, RequestType requestType, const QString* jsonReference = nullptr);

signals:

    void error(const QString& errorMessage);

private:

    AIModel* m_currentAIModel;
    AIModelList m_AIModelList{ std::make_unique<QVector<AIModel>>() };
    QString m_analysePrompt;
    QString m_cleanListPrompt;
    QString m_apiKey{""};
    static constexpr int MAX_FALLBACK_ATTEMPTS{ 3 };
    static inline const QString ANALYSE_PROMPT_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/analyse_prompt.txt" };
    static inline const QString CLEAN_LIST_PROMPT_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/cleanList_prompt.txt" };
    static inline const QString IA_MODEL_CONFIG_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/config_model_" };


    void initializePrompts();
    QString loadPrompt(const QString& path, RequestType requestType);
    static QString getAnalyseDefaultPrompt();
    static QString getCleanListDefaultPrompt();

    bool loadAIMainConfig();
    bool createAIMainConfigFile();
    void saveAIMainConfigFile(QJsonObject& jsonBody);
    bool loadAllAIModels(int loadAttempts = 0, QString errorMessage = "");
    std::optional<AIModel> loadAIModelConfig(const QString& path);
    void saveModelToConfig(QJsonObject& jsonBody, const AIModel* aiModel);
    bool createModelConfigFile(const AIModel* aiModel);
    bool addModelToList(AIModel model);
};
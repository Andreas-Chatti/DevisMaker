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


class IA : public QObject
{
    Q_OBJECT

public:

    enum ModelType
    {
        primary,
        fallback,
        max_model_types,
    };

    explicit IA(QObject* parent = nullptr)
        : QObject(parent)
    {
        initializePrompt();
        loadConfigFile();
    }

    ~IA() = default;
    IA(const IA& ia) = delete;
    IA& operator=(const IA& ia) = delete;
    IA(IA&& ia) = delete;
    IA& operator=(IA&& ia) = delete;

    const QString& getCurrentModelString() { return m_currentModel; }
    const QString& getPrimaryModel() { return m_primaryModel; }
    const QString& getFallbackModel() { return m_fallbackModel; }
    int getMaxFallbackAttempts() { return m_maxFallbackAttempts; }
    int getmaxTokens() { return m_maxTokens; }
    double getTemperature() { return m_temperature; }
    const QString& getPrompt() { return m_currentPrompt; }
    const QUrl& getUrl() { return m_url; }
    const QString& getAPI_Key() { return m_apiKey; }
    const QString& getCurrentPrompt() const { return m_currentPrompt; }

    void setCurrentModel(ModelType modelType);

    void reloadPrompt();
    bool savePrompt(const QString& promptContent);
    QNetworkRequest buildRequest(const QString& inventoryText, const QString& jsonReference);
    QNetworkRequest buildCleanTextRequest(const QString& rawText);

signals:

    void error(const QString& errorMessage);

private:

    QString m_primaryModel{ "llama-3.3-70b-versatile" };
    QString m_fallbackModel{ "gemma2-9b-it" };
    QString m_currentModel{ m_primaryModel };
    int m_maxFallbackAttempts{ 3 };
    int m_maxTokens{ 4000 };
    double m_temperature{ 0.1 };
    QString m_currentPrompt;
    QUrl m_url{ "https://api.groq.com/openai/v1/chat/completions" };
    QString m_apiKey{""};
    static inline const QString PROMPT_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/prompt_template.txt" };
    static inline const QString IA_CONFIG_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/ia_config.json" };


    void initializePrompt();
    QString loadPrompt();
    QString getDefaultPrompt();

    void createDefaultConfigFile();
    void loadConfigFile(int loadAttempts = 0, QString errorMessage = "");
};
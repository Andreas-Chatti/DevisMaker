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
#include "utils/constants.h"


class IA
{
public:

    enum ModelType
    {
        primary,
        fallback,
        max_model_types,
    };

    IA()
    {
        initializePrompt();

        if (!doesConfigFileExist())
            createDefaultConfigFile();

        else
            loadConfigFile();

        m_currentModel = m_primaryModel;
    }

    const QString& getCurrentModel() { return m_currentModel; }
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

    void reloadPrompt();  // Si l'utilisateur modifie le fichier externement
    bool savePrompt(const QString& promptContent);  // Pour sauvegarder un nouveau prompt
    QNetworkRequest buildRequest(const QString& inventoryText, const QString& jsonReference);


private:

    const QString PROMPT_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/prompt_template.txt" };
    const QString IA_CONFIG_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/ia_config.json" };

    QString m_primaryModel;
    QString m_fallbackModel;
    QString m_currentModel;
    int m_maxFallbackAttempts;
    int m_maxTokens;
    double m_temperature;
    QString m_currentPrompt;
    QUrl m_url;
    QString m_apiKey;


    void initializePrompt();
    QString loadPrompt();
    QString getDefaultPrompt();

    void createDefaultConfigFile();
    bool doesConfigFileExist();
    void loadConfigFile();
};
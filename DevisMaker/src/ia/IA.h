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

    IA()
        : m_currentModel{ m_primaryModel }
    {
        initializePrompt();

        if (!doesConfigFileExist())
            createDefaultConfigFile();

        else
            loadConfigFile();
    }

    const QString& getCurrentModel() { return m_currentModel; }
    const QString& getPrimaryModel() { return m_primaryModel; }
    const QString& getFallbackModel() { return m_fallbackModel; }
    const QVector<double>& getFallbackResults() { return m_fallbackResults; }
    int getFallbackAttempts() { return m_fallbackAttempts; }
    int getMaxFallbackAttempts() { return m_maxFallbackAttempts; }
    int getmaxTokens() { return m_maxTokens; }
    double getTemperature() { return m_temperature; }
    const QString& getPrompt() { return m_currentPrompt; }
    const QUrl& getUrl() { return m_url; }
    const QString& getAPI_Key() { return m_apiKey; }


    void setCurrentModel(QString model) { m_currentModel = model; }
    void addFallbackResult(double result) { m_fallbackResults.emplace_back(result); }
    void addFallbackAttempt() { m_fallbackAttempts++; }
    void clearFallbackAttempts() { m_fallbackAttempts = 0; }


    const QString& getCurrentPrompt() const { return m_currentPrompt; }
    void reloadPrompt();  // Si l'utilisateur modifie le fichier externement
    bool savePrompt(const QString& promptContent);  // Pour sauvegarder un nouveau prompt
    QNetworkRequest buildRequest(const QString& inventoryText, const QString& jsonReference);


private:

    const QString PROMPT_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/prompt_template.txt" };
    const QString IA_CONFIG_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/ia_config.json" };

    QString m_primaryModel;
    QString m_fallbackModel;
    QString m_currentModel;
    QVector<double> m_fallbackResults;
    int m_fallbackAttempts{};
    int m_maxFallbackAttempts;
    int m_maxTokens;
    double m_temperature;
    QString m_currentPrompt;
    QUrl m_url;
    QString m_apiKey;


    void initializePrompt();   // Appelée uniquement dans le constructeur
    QString loadPrompt();      // Utilisée uniquement en interne
    QString getDefaultPrompt(); // Template par défaut, usage interne uniquement

    void createDefaultConfigFile();
    bool doesConfigFileExist();
    void loadConfigFile();
};
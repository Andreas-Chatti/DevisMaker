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

    QString m_promptFilePath{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/prompt_template.txt" };
    const QString m_primaryModel{ "llama-3.3-70b-versatile" };
    const QString m_fallbackModel{ "gemma2-9b-it" };
    QString m_currentModel;
    QVector<double> m_fallbackResults;
    int m_fallbackAttempts{};
    const int m_maxFallbackAttempts{ 3 };
    int m_maxTokens{ 4000 };
    double m_temperature{ 0.1 };
    QString m_currentPrompt;
    const QUrl m_url{ "https://api.groq.com/openai/v1/chat/completions" };
    const QString m_apiKey{ "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" };


    void initializePrompt();   // Appelée uniquement dans le constructeur
    QString loadPrompt();      // Utilisée uniquement en interne
    QString getDefaultPrompt(); // Template par défaut, usage interne uniquement
};

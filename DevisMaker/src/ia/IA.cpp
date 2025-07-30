#include "IA.h"

void IA::initializePrompt() 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.exists()) 
    {
        if (!savePrompt(getDefaultPrompt()))
            m_currentPrompt = getDefaultPrompt();
    }

    m_currentPrompt = loadPrompt();
}

QString IA::loadPrompt() 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QTimer::singleShot(500, this, [this]() { emit error("Can't open prompt file.\n Loading default prompt.");});
        return getDefaultPrompt();
    }

    QTextStream in(&promptFile);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    promptFile.close();

    return content;
}

bool IA::savePrompt(const QString& promptContent) 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.open(QIODevice::WriteOnly | QIODevice::Text)) 
    {
        QTimer::singleShot(500, this, [this]() { emit error("Couldn't create prompt_template.txt !");});
        return false;
    }

    QTextStream out(&promptFile);
    out.setEncoding(QStringConverter::Utf8);
    out << promptContent;
    promptFile.close();

    return true;
}

QString IA::getDefaultPrompt() 
{
    return R"(Tu dois analyser cet inventaire de déménagement et calculer les volumes avec cette référence : %1

RÈGLES IMPORTANTES:
- Lis chaque ligne attentivement
- Si tu vois "matelas ET sommiers" = ce sont 2 objets différents à lister séparément
- Si tu vois "2 matelas et 2 sommiers" = 4 objets au total (2+2)
- Utilise UNIQUEMENT les volumes de la référence fournie
- CALCULE le résultat final : 6 valises = 6 × 0.3 = 1.8 (pas "6 * 0.3" !)
- Les volumes doivent être des NOMBRES PURS uniquement

RÉPONSE OBLIGATOIRE: JSON pur uniquement, sans texte avant ou après.
INTERDICTION ABSOLUE d'expressions mathématiques dans les valeurs !

Format exact:
{
  "items": [
    {"name": "6 valises", "volume": 1.8 }
  ],
  "totalVolume": 1.8
}

INVENTAIRE À ANALYSER:
%2)";
}


void IA::reloadPrompt() 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.exists()) 
    {
        QTimer::singleShot(500, this, [this]() { emit error("Couldn't reload prompt.\n Cannot find prompt file's path !");});
        return;
    }

    QString oldPrompt{ m_currentPrompt };
    QString newPrompt{ loadPrompt() };

    if (newPrompt != oldPrompt) 
        m_currentPrompt = newPrompt;
}


QNetworkRequest IA::buildRequest(const QString& inventoryText, const QString& jsonReference) 
{
    // Construire le prompt avec les paramètres
    QString prompt{ m_currentPrompt.arg(jsonReference, inventoryText) };

    // Créer la requête
    QNetworkRequest request(m_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    // Corps de la requête JSON
    QJsonObject jsonBody;
    jsonBody["model"] = m_currentModel;
    jsonBody["max_tokens"] = m_maxTokens;
    jsonBody["temperature"] = m_temperature;

    QJsonArray messages;
    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = prompt;
    messages.append(userMessage);
    jsonBody["messages"] = messages;

    // Stocker le JSON dans la requête (via un attribut custom)
    QJsonDocument doc(jsonBody);
    request.setAttribute(QNetworkRequest::User, doc.toJson());

    return request;
}


void IA::createDefaultConfigFile()
{
    QJsonObject jsonBody;
 
    jsonBody["primary_model"] = DEFAULT_PRIMARY_MODEL;
    jsonBody["fallback_model"] = DEFAULT_FALLBACK_MODEL;
    jsonBody["url"] = DEFAULT_API_URL;
    jsonBody["max_tokens"] = DEFAULT_MAX_TOKENS;
    jsonBody["temperature"] = DEFAULT_TEMPERATURE;
    jsonBody["api_key"] = "";
    jsonBody["fallback_max_attempts"] = DEFAULT_MAX_FALLBACK_ATTEMPTS;

    QJsonDocument jsonDocument{ jsonBody };

    QFile jsonFile{ IA_CONFIG_FILE_PATH };

    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        QTimer::singleShot(500, this, [this]() { emit error("Cannot create ia_config.json");});
        return;
    }

    jsonFile.write(jsonDocument.toJson(QJsonDocument::Indented));
}


void IA::loadConfigFile(int loadAttempts)
{
    if (loadAttempts >= 3)
        return;

    QFile jsonFile{ IA_CONFIG_FILE_PATH };

    if (!jsonFile.exists())
        createDefaultConfigFile();

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        QTimer::singleShot(500, this, [this]() { emit error("Cannot open or access ia_config.json.");});
        createDefaultConfigFile();
        loadConfigFile(++loadAttempts);
        return;
    }

    QByteArray fileRawData{ jsonFile.readAll() };

    QJsonParseError error;
    QJsonDocument jsonDocument{ QJsonDocument::fromJson(fileRawData, &error) };

    if (error.error != QJsonParseError::NoError)
    {
        QTimer::singleShot(500, this, [this, error]() { emit IA::error("Error parsing ia_config.json: " + error.errorString());});
        jsonFile.close();

        createDefaultConfigFile();
        loadConfigFile(++loadAttempts);
        return;
    }

    QJsonObject jsonBody{ jsonDocument.object() };

    m_primaryModel = jsonBody["primary_model"].toString();
    m_fallbackModel = jsonBody["fallback_model"].toString();
    m_url = jsonBody["url"].toString();
    m_maxTokens = jsonBody["max_tokens"].toInt();
    m_temperature = jsonBody["temperature"].toDouble();
    m_apiKey = jsonBody["api_key"].toString();
    m_maxFallbackAttempts = jsonBody["fallback_max_attempts"].toInt();
    m_currentModel = m_primaryModel;

    jsonFile.close();
}


void IA::setCurrentModel(ModelType modelType)
{
    switch (modelType)
    {
    case IA::primary: m_currentModel = m_primaryModel;
        break;
    case IA::fallback: m_currentModel = m_fallbackModel;
    }
}
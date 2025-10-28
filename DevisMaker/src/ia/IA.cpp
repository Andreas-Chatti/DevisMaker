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
    return R"(Tu dois analyser cet inventaire de d�m�nagement et calculer les volumes avec cette r�f�rence : %1

R�GLES IMPORTANTES:
- Lis chaque ligne attentivement
- Si tu vois "matelas ET sommiers" = ce sont 2 objets diff�rents � lister s�par�ment
- Si tu vois "2 matelas et 2 sommiers" = 4 objets au total (2+2)
- Utilise UNIQUEMENT les volumes de la r�f�rence fournie
- CALCULE le r�sultat final : 6 valises = 6 � 0.3 = 1.8 (pas "6 * 0.3" !)
- Les volumes doivent �tre des NOMBRES PURS uniquement

D�TECTION DES PARAM�TRES SUPPL�MENTAIRES (par d�faut = false):
- Si tu vois "D" ou "d�montage" ou "d�monter" ou "� d�monter" � mets "disassembly": true
- Si tu vois "R" ou "remontage" ou "remonter" ou "� remonter" � mets "assembly": true
- Si tu vois "L" ou "lourd" ou "tr�s lourd" ou "heavy" � mets "heavy": true
- Si tu d�tectes une pi�ce (salon, cuisine, chambre, bureau, cave, grenier, garage, etc.) � mets "areaKey": "nom_de_la_piece"
- Par d�faut: disassembly=false, assembly=false, heavy=false, areaKey="divers"

R�PONSE OBLIGATOIRE: JSON pur uniquement, sans texte avant ou apr�s.
INTERDICTION ABSOLUE d'expressions math�matiques dans les valeurs !

Format exact:
{
  "items": [
    {
      "name": "6 valises",
      "volume": 1.8,
      "disassembly": false,
      "assembly": false,
      "heavy": false,
      "areaKey": "divers"
    }
  ],
  "totalVolume": 1.8
}

Exemple avec param�tres:
"1 armoire (D, R, L) cuisine" devient:
{
  "name": "armoire",
  "volume": 2.5,
  "disassembly": true,
  "assembly": true,
  "heavy": true,
  "areaKey": "cuisine"
}

INVENTAIRE � ANALYSER:
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
    // Construire le prompt avec les param�tres
    QString prompt{ m_currentPrompt.arg(jsonReference, inventoryText) };

    // Cr�er la requ�te
    QNetworkRequest request(m_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    // Corps de la requ�te JSON
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

    // Stocker le JSON dans la requ�te (via un attribut custom)
    QJsonDocument doc(jsonBody);
    request.setAttribute(QNetworkRequest::User, doc.toJson());

    return request;
}


void IA::createDefaultConfigFile()
{
    QJsonObject jsonBody;
 
    jsonBody["primary_model"] = m_primaryModel;
    jsonBody["fallback_model"] = m_fallbackModel;
    jsonBody["url"] = m_url.toString();
    jsonBody["max_tokens"] = m_maxTokens;
    jsonBody["temperature"] = m_temperature;
    jsonBody["api_key"] = "";
    jsonBody["fallback_max_attempts"] = m_maxFallbackAttempts;

    QJsonDocument jsonDocument{ jsonBody };

    QFile jsonFile{ IA_CONFIG_FILE_PATH };

    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        QTimer::singleShot(500, this, [this]() { emit error("Cannot create ia_config.json");});
        return;
    }

    jsonFile.write(jsonDocument.toJson(QJsonDocument::Indented));
}


void IA::loadConfigFile(int loadAttempts, QString errorMessage)
{
    if (loadAttempts >= 3)
    {
        QTimer::singleShot(500, this, [this, errorMessage]() { emit error("Config file not loaded. Logs:\n\n" + errorMessage);});
        return;
    }

    QFile jsonFile{ IA_CONFIG_FILE_PATH };

    if (!jsonFile.exists())
        createDefaultConfigFile();

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        createDefaultConfigFile();
        loadConfigFile(++loadAttempts, errorMessage += QString{ "Attempt #%1: Cannot open or access ia_config.json.\n" }.arg(loadAttempts));
        return;
    }

    QByteArray fileRawData{ jsonFile.readAll() };
    QJsonParseError error;
    QJsonDocument jsonDocument{ QJsonDocument::fromJson(fileRawData, &error) };

    if (error.error != QJsonParseError::NoError)
    {
        createDefaultConfigFile();
        loadConfigFile(++loadAttempts, errorMessage += ("Attempt #%1: Error parsing ia_config.json: " + error.errorString() + "\n"));
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
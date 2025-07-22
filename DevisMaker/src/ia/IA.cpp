#include "IA.h"

void IA::initializePrompt() 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.exists()) 
    {
        qDebug() << "Fichier prompt introuvable, creation avec template par defaut...";

        // Créer le fichier avec le prompt par défaut
        if (savePrompt(getDefaultPrompt())) 
            qDebug() << "Fichier prompt cree :" << PROMPT_FILE_PATH;

        else 
        {
            qDebug() << "Erreur lors de la creation du fichier prompt !";
            m_currentPrompt = getDefaultPrompt(); // Fallback en mémoire
            return;
        }
    }

    // Charger le prompt depuis le fichier
    m_currentPrompt = loadPrompt();
    qDebug() << "Prompt charge depuis :" << PROMPT_FILE_PATH;
}

QString IA::loadPrompt() 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        qDebug() << "Impossible d'ouvrir le fichier prompt !";
        return getDefaultPrompt(); // Fallback
    }

    QTextStream in(&promptFile);
    in.setEncoding(QStringConverter::Utf8); // Support des caractères français
    QString content = in.readAll();
    promptFile.close();

    return content;
}

bool IA::savePrompt(const QString& promptContent) 
{
    QFile promptFile(PROMPT_FILE_PATH);

    if (!promptFile.open(QIODevice::WriteOnly | QIODevice::Text)) 
    {
        qDebug() << "Impossible de creer/ecrire le fichier prompt !";
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
        qDebug() << "Attention: Fichier prompt introuvable !" << PROMPT_FILE_PATH;
        qDebug() << "Utilisation du prompt actuel en memoire.";
        return;
    }

    QString oldPrompt{ m_currentPrompt };
    QString newPrompt{ loadPrompt() };

    // Vérifier si le chargement a réussi (loadPrompt retourne le default si erreur)
    if (newPrompt == getDefaultPrompt() && oldPrompt != getDefaultPrompt()) 
    {
        qDebug() << "Erreur lors du rechargement - prompt par defaut utilise";
        qDebug() << "Verifiez que le fichier " << PROMPT_FILE_PATH << " est accessible\n";
    }

    if (newPrompt != oldPrompt) 
    {
        m_currentPrompt = newPrompt;
        qDebug() << "Prompt recharge depuis : " << PROMPT_FILE_PATH;
        qDebug() << "Taille: " << newPrompt.length() << " caracteres";
    }

    else 
        qDebug() << "Prompt recharge - aucun changement detecte";
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
 
    jsonBody["primary_model"] = "llama-3.3-70b-versatile";
    jsonBody["fallback_model"] = "gemma2-9b-it";
    jsonBody["url"] = "https://api.groq.com/openai/v1/chat/completions";
    jsonBody["max_tokens"] = 4000;
    jsonBody["temperature"] = 0.1;
    jsonBody["api_key"] = "";
    jsonBody["fallback_max_attempts"] = 3;

    QJsonDocument jsonDocument{ jsonBody };

    QFile jsonFile{ IA_CONFIG_FILE_PATH };

    if (!jsonFile.open(QIODevice::WriteOnly)) 
    {
        qDebug() << "Erreur : impossible de creer le fichier config.json";
        return;
    }

    jsonFile.write(jsonDocument.toJson(QJsonDocument::Indented));

    qDebug() << "ia_config.json creer avec succes !";
}


bool IA::doesConfigFileExist()
{
    QFile jsonFile{ IA_CONFIG_FILE_PATH };
    return jsonFile.exists();
}


void IA::loadConfigFile()
{
    QFile jsonFile{ IA_CONFIG_FILE_PATH };

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Impossible d'ouvrir ia_config.json";
        return;
    }

    QByteArray fileRawData{ jsonFile.readAll() };

    QJsonParseError error;
    QJsonDocument jsonDocument{ QJsonDocument::fromJson(fileRawData, &error) };

    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Erreur parsing Json: " + error.errorString();
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
}
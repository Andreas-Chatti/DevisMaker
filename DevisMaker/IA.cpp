#include "IA.h"

void IA::initializePrompt() 
{
    QFile promptFile(m_promptFilePath);

    if (!promptFile.exists()) 
    {
        qDebug() << "Fichier prompt introuvable, creation avec template par defaut...";

        // Créer le fichier avec le prompt par défaut
        if (savePrompt(getDefaultPrompt())) 
            qDebug() << "Fichier prompt cree :" << m_promptFilePath;

        else 
        {
            qDebug() << "Erreur lors de la creation du fichier prompt !";
            m_currentPrompt = getDefaultPrompt(); // Fallback en mémoire
            return;
        }
    }

    // Charger le prompt depuis le fichier
    m_currentPrompt = loadPrompt();
    qDebug() << "Prompt charge depuis :" << m_promptFilePath;
}

QString IA::loadPrompt() 
{
    QFile promptFile(m_promptFilePath);

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
    QFile promptFile(m_promptFilePath);

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

QString IA::getDefaultPrompt() {
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
    {"name": "6 valises", "volume": 1.8}
  ],
  "totalVolume": 1.8
}

INVENTAIRE À ANALYSER:
%2)";
}


void IA::reloadPrompt() 
{
    QFile promptFile(m_promptFilePath);

    if (!promptFile.exists()) 
    {
        qDebug() << "Attention: Fichier prompt introuvable !" << m_promptFilePath;
        qDebug() << "Utilisation du prompt actuel en memoire.";
        return;
    }

    QString oldPrompt{ m_currentPrompt };
    QString newPrompt{ loadPrompt() };

    // Vérifier si le chargement a réussi (loadPrompt retourne le default si erreur)
    if (newPrompt == getDefaultPrompt() && oldPrompt != getDefaultPrompt()) 
    {
        qDebug() << "Erreur lors du rechargement - prompt par defaut utilise";
        qDebug() << "Verifiez que le fichier " << m_promptFilePath << " est accessible\n";
    }

    if (newPrompt != oldPrompt) 
    {
        m_currentPrompt = newPrompt;
        qDebug() << "Prompt recharge depuis : " << m_promptFilePath;
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
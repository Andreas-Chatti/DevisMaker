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

REGLES IMPORTANTES:
- Lis chaque ligne attentivement
- Si tu vois "matelas ET sommiers" = ce sont 2 objets différents à lister séparément
- Si tu vois "2 matelas et 2 sommiers" = 4 objets au total (2+2)
- Utilise UNIQUEMENT les volumes de la référence fournie
- Si tu vois que l'objet est un nom composé avec des espaces, exemple: "buffet haut et bas" ne fais pas "name": "buffet_haut_et_bas" mais comme ça: "name:": "buffet haut et bas"
- IDENTIFIE la quantité et le volume unitaire : "6 valises" = quantity:6, unitaryVolume:0.3
- Les volumes et quantités doivent être des NOMBRES PURS uniquement

DETECTION DES PARAMETRES SUPPLEMENTAIRES (par défaut = false):
- Si tu vois "D" ou "démontage" ou "démonter" ou "à démonter" mets "disassembly": true
- Si tu vois "R" ou "remontage" ou "remonter" ou "à remonter" mets "assembly": true
- Si tu vois "L" ou "lourd" ou "très lourd" mets "heavy": true
- Si tu détectes une pièce (salon, cuisine, chambre, bureau, cave, grenier, garage, etc.) mets "areaKey": "nom_de_la_piece" sans les '_'
- Par défaut: disassembly=false, assembly=false, heavy=false, areaKey="divers"

REPONSE OBLIGATOIRE: JSON pur uniquement, sans texte avant ou après.
INTERDICTION ABSOLUE d'expressions mathématiques dans les valeurs !

Format exact:
{
  "items": [
    {
      "name": "valise",
      "quantity": 6,
      "unitaryVolume": 0.3,
      "disassembly": false,
      "assembly": false,
      "heavy": false,
      "areaKey": "divers"
    }
  ]
}

Exemple avec paramètres:
"1 armoire (D, R, L) cuisine" devient:
{
  "name": "armoire",
  "quantity": 1,
  "unitaryVolume": 2.5,
  "disassembly": true,
  "assembly": true,
  "heavy": true,
  "areaKey": "cuisine"
}

INVENTAIRE A ANALYSER:
%2)";
}

QString IA::getDefaultCleanListPrompt()
{
    return R"(

Voici une liste d'inventaire : %1

Trie et formate cette liste d'inventaire de déménagement selon les règles ci-dessous.

FORMAT DE SORTIE :
quantité - nom de l'objet (nom de la pièce) [D/R/L]

RÈGLES DE FORMATAGE :
1. Réponds UNIQUEMENT avec la liste formatée, sans texte d'introduction ni commentaire
2. Chaque élément sur une nouvelle ligne
3. Nom de l'objet toujours au SINGULIER

QUANTITÉ :
- Si mentionnée : utilise la quantité indiquée
- Si NON mentionnée ET objet au singulier : quantité = 1
- Si NON mentionnée ET objet au pluriel : quantité = 2

RÈGLE DE REGROUPEMENT DES QUANTITÉS :
- Si plusieurs objets IDENTIQUES sont mentionnés dans la MÊME PIÈCE, 
  REGROUPE-les en UNE SEULE ligne avec la quantité totale
- Si des objets identiques sont dans des PIÈCES DIFFÉRENTES, 
  garde-les sur des lignes SÉPARÉES

Exemples :
✅ "3 chaises dans la cuisine, 2 chaises dans la cuisine" 
   → 5 - chaise (cuisine)

✅ "un carton livre au salon, 4 cartons livres au salon"
   → 5 - carton livre (salon)

❌ "2 chaises dans la cuisine, 3 chaises dans le salon"
   → 2 - chaise (cuisine)
   → 3 - chaise (salon)
   [NE PAS regrouper car pièces différentes !]

✅ "table basse normale au salon, table basse en marbre au salon"
   → 1 - table basse (salon)
   → 1 - table basse en marbre (salon) [L]
   [NE PAS regrouper car descriptions/tags différents !]

ATTENTION : Regroupe UNIQUEMENT si :
1. Même nom d'objet exact
2. Même pièce
3. Mêmes tags [D], [R], [L] ou absence de tags
4. Même description spécifique (ex: "IKEA", "en marbre", etc.)

NOM DE LA PIÈCE :
- Si mentionnée : indique entre parenthèses
- Si NON mentionnée : ne rien mettre

TYPES DE CARTONS (importante distinction) :
- "carton standard" → garder tel quel
- "carton livre" → garder tel quel  
- "carton" (sans précision) → renommer en "carton standard"

MENTIONS [D/R/L] :
N'ajoute ces lettres QUE si explicitement mentionné OU exception ci-dessous :

D = Démontage
- Ajouter si mention "à démonter"

R = Remontage  
- Ajouter si mention "à remonter"

L = Lourd
- Ajouter si mention "lourd"
RÈGLE [L] AUTOMATIQUE - EXCEPTIONS IMPORTANTES :
- Piano, coffre-fort, moto → TOUJOURS [L]
- SAUF SI explicitement décrit comme "léger", "pas lourd", "numérique léger"
- Si le texte dit "piano numérique léger" → PAS de [L] !
- Les mentions "léger" ou "pas lourd" ANNULENT le tag [L] automatique

EXTRACTION COMPLÈTE :
- Analyse TOUT le texte du début à la fin
- N'ignore AUCUN objet mentionné, même en fin de phrase
- Vérifie bien les mentions tardives comme "ah et puis des tableaux j'en ai 3"

CONSERVATION DES DÉTAILS :
- Si un objet a une description spécifique (ex: "piano à queue", "lit king size"),
  CONSERVE cette description exacte dans le nom
- Exemple : "piano à queue" et NON juste "piano"

EXEMPLES :
- "3 chaises de cuisine" → 3 - chaise (cuisine)
- "table à démonter salon" → 1 - table (salon) [D]
- "piano droit" → 1 - piano droit [L]
- "carton" → 1 - carton standard
- "2 cartons livre bureau" → 2 - carton livre (bureau)
)";
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

    // Cr�er la requête
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

QNetworkRequest IA::buildCleanTextRequest(const QString& rawText)
{
    // Construire le prompt avec les paramètres
    QString prompt{ getDefaultCleanListPrompt().arg(rawText) };

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
#include "AIService.h"

void AIService::initializePrompts() 
{
    QVector<RequestType> requestTypeList{ RequestType::AnalyseInventory, RequestType::CleanName };

    for (auto requestType : requestTypeList)
    {
        QString path;
        QString defaultPrompt;
        switch (requestType)
        {
        case AIService::RequestType::CleanName: 
            path = CLEAN_LIST_PROMPT_FILE_PATH;
            defaultPrompt = getCleanListDefaultPrompt();
            break;
        case AIService::RequestType::AnalyseInventory: 
            path = ANALYSE_PROMPT_FILE_PATH;
            defaultPrompt = getAnalyseDefaultPrompt();
            break;
        }

        QFile promptFile(path);
        if (!promptFile.exists() && !savePrompt(defaultPrompt, path))
        {
            switch (requestType)
            {
            case AIService::RequestType::CleanName: m_cleanListPrompt = getCleanListDefaultPrompt();
                break;
            case AIService::RequestType::AnalyseInventory: m_analysePrompt = getAnalyseDefaultPrompt();
            }
        }

        else
        { 
            switch (requestType)
            {
            case AIService::RequestType::CleanName: m_cleanListPrompt = loadPrompt(path, requestType);
                break;
            case AIService::RequestType::AnalyseInventory: m_analysePrompt = loadPrompt(path, requestType);
            }
        }
    }
}

QString AIService::loadPrompt(const QString& path, RequestType requestType)
{
    QFile promptFile(path);

    if (!promptFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QTimer::singleShot(500, this, [this]() { emit error("Can't open prompt file.\n Loading default prompt.");});
        return requestType == RequestType::CleanName ? getCleanListDefaultPrompt() : getAnalyseDefaultPrompt();
    }

    QTextStream in(&promptFile);
    in.setEncoding(QStringConverter::Utf8);
    QString prompt = in.readAll();
    promptFile.close();

    return prompt;
}

bool AIService::savePrompt(const QString& promptContent, const QString& path)
{
    QFile promptFile(path);

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

QString AIService::getAnalyseDefaultPrompt()
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

QString AIService::getCleanListDefaultPrompt()
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

bool AIService::reloadPrompt(const QString& path, RequestType type)
{
    QFile promptFile(path);

    if (!promptFile.exists()) 
    {
        QTimer::singleShot(500, this, [this]() { emit error("Couldn't reload prompt.\n Cannot find prompt file's path !");});
        return false;
    }

    QString newPrompt{ loadPrompt(path, type) };
    QString currentPrompt{ type == RequestType::AnalyseInventory ? m_analysePrompt : m_cleanListPrompt };

    if (newPrompt != currentPrompt)
    {
        switch (type)
        {
        case AIService::RequestType::CleanName: m_cleanListPrompt = std::move(newPrompt);
            break;
        case AIService::RequestType::AnalyseInventory: m_analysePrompt = std::move(newPrompt);
        }
    }

    else
        return false;
}


QNetworkRequest AIService::buildRequest(const QString& inventoryText, RequestType requestType, const QString* jsonReference)
{
    QNetworkRequest request(m_currentAIModel->getUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QJsonObject jsonBody;
    jsonBody["model"] = m_currentAIModel->getModelName();
    jsonBody["max_tokens"] = m_currentAIModel->getMaxOutputTokens();
    jsonBody["temperature"] = m_currentAIModel->getTemperature();

    QJsonArray messages;
    QJsonObject userMessage;
    userMessage["role"] = "user";

    QString prompt;
    switch (requestType)
    {
    case AIService::RequestType::CleanName: prompt = m_cleanListPrompt.arg(inventoryText);
        break;
    case AIService::RequestType::AnalyseInventory: 
        if (!jsonReference)
        {
            emit error("Error: jsonReference is null.");
            return;
            // throw ?
        }

        prompt = m_analysePrompt.arg(*jsonReference, inventoryText);
        break;
    }
    userMessage["content"] = prompt;
    messages.append(userMessage);
    jsonBody["messages"] = messages;

    QJsonDocument doc(jsonBody);
    request.setAttribute(QNetworkRequest::User, doc.toJson());

    return request;
}

AIModel AIService::loadModelParametersFromConfig(QJsonObject& jsonBody)
{
    QString modelName{ jsonBody["modelName"].toString() };
    int maxOutputTokens{ jsonBody["maxOutputTokens"].toInt() };
    QUrl url{ jsonBody["url"].toString() };
    double temperature{ jsonBody["temperature"].toDouble() };

    return AIModel{ modelName, maxOutputTokens, temperature, url, this };
}

bool AIService::loadModelConfigFile(int loadAttempts, QString errorMessage)
{

    if (loadAttempts >= MAX_FALLBACK_ATTEMPTS)
    {
        QTimer::singleShot(500, this, [this, errorMessage]() { emit error("Config file not loaded. Logs:\n\n" + errorMessage);});
        return false;
    }

    AIModel aiModel{ "llama-3.1-8b-instant", 16000, 0.1, QUrl{"https://api.groq.com/openai/v1/chat/completions"}, this }; // Default model
    QFile jsonFile{ IA_MODEL_CONFIG_FILE_PATH + aiModel.getModelName() + ".json" };

    if (!jsonFile.exists())
        createModelConfigFile(&aiModel);

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        createModelConfigFile(&aiModel);
        loadModelConfigFile(++loadAttempts, errorMessage += QString{ "Attempt #%1: Cannot open or access ia_config.json.\n" }.arg(loadAttempts));
        return false;
    }

    QByteArray fileRawData{ jsonFile.readAll() };
    QJsonParseError error;
    QJsonDocument jsonDocument{ QJsonDocument::fromJson(fileRawData, &error) };

    if (error.error != QJsonParseError::NoError)
    {
        createModelConfigFile(&aiModel);
        loadModelConfigFile(++loadAttempts, errorMessage += ("Attempt #%1: Error parsing ia_config.json: " + error.errorString() + "\n"));
        return false;
    }

    QJsonObject jsonBody{ jsonDocument.object() };
    aiModel = loadModelParametersFromConfig(jsonBody);
    m_AIModelList->emplaceBack(std::move(aiModel));
}

void AIService::saveModelToConfig(QJsonObject& jsonBody, const AIModel* aiModel)
{
    if (!aiModel)
        return;

    jsonBody["modelName"] = aiModel->getModelName();
    jsonBody["maxOutputTokens"] = aiModel->getMaxOutputTokens();
    jsonBody["url"] = aiModel->getUrl().toString();
    jsonBody["temperature"] = aiModel->getTemperature();
}

bool AIService::createModelConfigFile(const AIModel* aiModel)
{
    if (!aiModel)
    {
        emit error("aiModel is null or invalid");
        return false;
    }

    QJsonObject jsonBody;
    saveModelToConfig(jsonBody, aiModel);

    QJsonDocument jsonDocument{ jsonBody };

    QFile jsonFile{ IA_MODEL_CONFIG_FILE_PATH + aiModel->getModelName() + ".json" };

    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        emit error("Cannot create model config file");
        return false;
    }

    jsonFile.write(jsonDocument.toJson(QJsonDocument::Indented));
}
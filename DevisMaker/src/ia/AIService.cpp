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
    return R"(Voici une liste d'inventaire : %1

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
        emit error("Couldn't reload prompt.\n Cannot find prompt file's path !");
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

    return true;
}


QNetworkRequest AIService::buildRequest(const QString& inventoryText, RequestType requestType, const QString* jsonReference)
{
    if (m_AIModelList->isEmpty() || m_currentAiModelIndex >= m_AIModelList->size())
        throw std::out_of_range("No valid AI model available at current index");

    const AIModel* aiModel{ &(*m_AIModelList)[m_currentAiModelIndex] };
    if(!aiModel)
        throw std::invalid_argument("aiModel is null. Analyse aborted.");

    QNetworkRequest request(aiModel->getUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QJsonObject jsonBody;
    jsonBody["model"] = aiModel->getModelName();
    jsonBody["max_tokens"] = aiModel->getMaxOutputTokens();
    jsonBody["temperature"] = aiModel->getTemperature();

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
            throw std::invalid_argument("jsonReference is null. Analyse aborted.");

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

bool AIService::loadAIMainConfig()
{
    QFile configFile{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/ai_service_config.json"};

    if (!configFile.exists() || !configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (!createAIMainConfigFile())
        {
            emit error("Error loading main ai config file. Aborted.");
            return false;
        }
    }

    QByteArray data{ configFile.readAll() };
    configFile.close();

    QJsonParseError parseError;
    QJsonDocument doc{ QJsonDocument::fromJson(data, &parseError) };
    if (doc.isNull() || !doc.isObject())
        return false;

    QJsonObject jsonObject{ doc.object() };

    m_apiKey = jsonObject["apiKey"].toString();

    return true;
}

bool AIService::createAIMainConfigFile()
{
    QJsonObject jsonBody;
    saveAIMainConfigFile(jsonBody);

    QJsonDocument jsonDocument{ jsonBody };

    QFile jsonFile{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/ai_service_config.json" };

    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        emit error("Cannot create model config file");
        return false;
    }

    jsonFile.write(jsonDocument.toJson(QJsonDocument::Indented));

    return true;
}

void AIService::saveAIMainConfigFile(QJsonObject& jsonBody)
{
    jsonBody["apiKey"] = m_apiKey;
}

bool AIService::loadAllAIModels(int loadAttempts, QString errorMessage)
{
    if (loadAttempts >= 3 && m_AIModelList->isEmpty())
    {
        emit error("There was an error loading AI models.");
        return false;
    }

    QDir configDir(SettingsConstants::FileSettings::DATA_FILE_PATH);

    QStringList filters;
    filters << "config_model_*.json";
    configDir.setNameFilters(filters);

    QFileInfoList configFiles{ configDir.entryInfoList(QDir::Files) };

    if (configFiles.isEmpty())
    {
        AIModel defaultModel{ AIModel::makeDefaultModel(this) };
        createModelConfigFile(&defaultModel);
        return loadAllAIModels(++loadAttempts, errorMessage);
    }

    for (const QFileInfo& fileInfo : configFiles)
    {
        QString filePath{ fileInfo.absoluteFilePath() };
        std::optional<AIModel> model{ loadAIModelConfig(filePath) };

        if (!model)
            continue;

        addModelToList(std::move(*model));
    }

    return m_AIModelList.get()->count() > 0;
}

std::optional<AIModel> AIService::loadAIModelConfig(const QString& path)
{
    QFile config{ path };
    if (!config.exists() || !config.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::nullopt;

    QByteArray data{ config.readAll() };
    config.close();

    QJsonParseError parseError;
    QJsonDocument doc{ QJsonDocument::fromJson(data, &parseError) };
    if (doc.isNull() || !doc.isObject())
        return std::nullopt;

    QJsonObject jsonObject{ doc.object() };

    QString modelName{ jsonObject["modelName"].toString() };
    int maxOutputTokens{ jsonObject["maxOutputTokens"].toInt() };
    double temperature{ jsonObject["temperature"].toDouble() };
    QUrl url{ jsonObject["url"].toString() };

    return AIModel{ std::move(modelName), maxOutputTokens, temperature, std::move(url), this };
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

    return true;
}

bool AIService::addModelToList(AIModel modelToAdd)
{
    auto isSameModel{ [modelToAdd](const AIModel& aiModel) { return aiModel.getModelName() == modelToAdd.getModelName(); } };
    auto it{ std::find_if(m_AIModelList.get()->begin(), m_AIModelList.get()->end(), isSameModel) };
        
    if (it != m_AIModelList.get()->end())
    {
        qDebug("Model already exist in the list ! Aborted.");
        return false;
    }

    m_AIModelList.get()->emplace_back(std::move(modelToAdd));
    return true;
}

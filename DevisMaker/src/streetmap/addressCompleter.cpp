#include "addressCompleter.h"

AddressCompleter::AddressCompleter(QLineEdit* lineEditChargement, QLineEdit* lineEditLivraison, QObject* parent)
    : QObject(parent)
    , m_lineEditChargement(lineEditChargement)
    , m_lineEditLivraison(lineEditLivraison)
{
    m_streetMap = new OpenStreetMap{ this };

    // Créer le modèle pour les suggestions
    m_model = new QStringListModel(this);

    setupCompleter();

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &AddressCompleter::handleNetworkReply);

    connect(m_lineEditChargement, &QLineEdit::textEdited, this, &AddressCompleter::startTimer);
    connect(m_lineEditChargement, &QLineEdit::textEdited, this, &AddressCompleter::setCurrentModifiedLineEdit);

    connect(m_lineEditLivraison, &QLineEdit::textEdited, this, &AddressCompleter::startTimer);
    connect(m_lineEditLivraison, &QLineEdit::textEdited, this, &AddressCompleter::setCurrentModifiedLineEdit);

    setupDebounceTimer();
    connect(m_debounceTimer, &QTimer::timeout, this, &AddressCompleter::onTextChanged);

    // Utiliser textEdited au lieu de textChanged pour n'intercepter que les saisies utilisateur


    // Calculer la distance après modification du champ d'adresse départ
    connect(m_lineEditChargement, &QLineEdit::editingFinished, this, &AddressCompleter::onEditingFinished);
    connect(m_lineEditLivraison, &QLineEdit::editingFinished, this, &AddressCompleter::onEditingFinished);
}


void AddressCompleter::onTextChanged()
{
    QString text{ m_currentModifiedLineEdit == LineEditType::chargement ? m_lineEditChargement->text() : m_lineEditLivraison->text() };

    if (text.length() < MIN_TEXT_LENGTH_TRIGGER)
        return;

    // Vider la liste des suggestions en attente
    m_pendingSuggestions.clear();

    // 1. Créer la version avec virgule si nécessaire
    QString textWithComma{ text };
    QRegularExpression regexNumero("^(\\d+)\\s+(.+)$");
    QRegularExpressionMatch match = regexNumero.match(text);

    if (match.hasMatch()) 
    {
        QString numero = match.captured(1);
        QString rue = match.captured(2);
        textWithComma = numero + ", " + rue;
    }

    // 2. Envoyer la première requête (avec virgule)
    QUrl url1(m_streetMap->getUrl());
    QUrlQuery query1;
    query1.addQueryItem("format", "json");
    query1.addQueryItem("q", textWithComma);
    query1.addQueryItem("limit", "7");  // Réduire pour éviter trop de résultats
    query1.addQueryItem("countrycodes", "fr");
    query1.addQueryItem("addressdetails", "1");

    url1.setQuery(query1);

    QNetworkRequest request1(url1);
    request1.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");
    request1.setRawHeader("QueryType", "with_comma");

    // 3. Envoyer la deuxième requête (texte original)
    QUrl url2("https://nominatim.openstreetmap.org/search");
    QUrlQuery query2;
    query2.addQueryItem("format", "json");
    query2.addQueryItem("q", text);  // Texte original
    query2.addQueryItem("limit", "7");
    query2.addQueryItem("countrycodes", "fr");
    query2.addQueryItem("addressdetails", "1");

    url2.setQuery(query2);

    QNetworkRequest request2(url2);
    request2.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");
    request2.setRawHeader("QueryType", "original");

    // 4. Envoyer les deux requêtes
    m_networkManager->get(request1);

    // Important : ajouter un petit délai entre les requêtes pour respecter les limites d'API
    QTimer::singleShot(200, this, [this, request2]() 
        {
        m_networkManager->get(request2);
        });

    qDebug() << "Requetes envoyees pour: " << text << " et " << textWithComma;
}


void AddressCompleter::handleNetworkReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) 
    {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray array = doc.array();

        // Traiter les résultats comme avant
        QStringList suggestions;
        for (const QJsonValue& value : array) 
        {
            QJsonObject obj = value.toObject();

            // Format simplifié
            QString formattedAddress;

            if (obj.contains("address")) 
            {
                QJsonObject address{ obj["address"].toObject() };

                // Numéro, rue, code postal, ville
                QString houseNumber{ address.contains("house_number") ? address["house_number"].toString() : "" };

                QString road{ address.contains("road") ? address["road"].toString() : "" };

                QString postcode{ address.contains("postcode") ? address["postcode"].toString() : "" };

                QString city;
                if (address.contains("city"))
                    city = address["city"].toString();
                else if (address.contains("town"))
                    city = address["town"].toString();
                else if (address.contains("village"))
                    city = address["village"].toString();

                // Construction du format simplifié
                if (!houseNumber.isEmpty())
                    formattedAddress += houseNumber + ", ";

                if (!road.isEmpty())
                    formattedAddress += road;

                if (!formattedAddress.isEmpty() && (!postcode.isEmpty() || !city.isEmpty()))
                    formattedAddress += ", ";

                if (!postcode.isEmpty())
                    formattedAddress += postcode + " ";

                if (!city.isEmpty())
                    formattedAddress += city;

                if (!formattedAddress.isEmpty()) 
                {
                    suggestions.append(formattedAddress);
                    continue;
                }
            }

            // Si le format personnalisé échoue, simplifier l'adresse complète
            QString fullAddress = obj["display_name"].toString();
            // [code de simplification d'adresse comme avant]
            suggestions.append(fullAddress);
        }

        // Ajouter ces suggestions à celles en attente
        for (const QString& suggestion : suggestions) 
        {
            if (!m_pendingSuggestions.contains(suggestion)) 
                m_pendingSuggestions.append(suggestion);
        }

        // Mettre à jour le modèle avec toutes les suggestions accumulées
        m_model->setStringList(m_pendingSuggestions);

        // Forcer l'affichage du popup
        m_completer->complete();
    }

    reply->deleteLater();
}


void AddressCompleter::startTimer(const QString& lineEditText)
{
    if (lineEditText.length() >= MIN_TEXT_LENGTH_TRIGGER)
        m_debounceTimer->start();
}


void AddressCompleter::onEditingFinished()
{
    QString depart{ m_lineEditChargement->text() };
    QString arrivee{ m_lineEditLivraison->text() };

    if (!depart.isEmpty() && !arrivee.isEmpty())
        m_streetMap->calculateDistance(depart, arrivee);
}


void AddressCompleter::setupCompleter()
{
    // Créer le completer et l'attacher au champ avec une configuration améliorée
    m_completer = new QCompleter(m_model, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::PopupCompletion); // Forcer l'affichage du popup
    m_completer->setMaxVisibleItems(MAX_VISIBLE_ITEMS); // Afficher plus de suggestions
    m_completer->setFilterMode(Qt::MatchContains); // Correspondance partielle n'importe où

    for (auto* lineEdit : QVector<QLineEdit*>{ m_lineEditChargement, m_lineEditLivraison })
    {
        lineEdit->setCompleter(m_completer);
    }
}


void AddressCompleter::setupDebounceTimer()
{
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(TIMER_DELAY);
}


void AddressCompleter::setCurrentModifiedLineEdit(const QString& lineEditText)
{
    m_currentModifiedLineEdit = (lineEditText == m_lineEditChargement->text() ? LineEditType::chargement : LineEditType::livraison);
}
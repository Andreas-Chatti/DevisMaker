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

    QUrl url(m_streetMap->getUrl());
    QUrlQuery query;
    query.addQueryItem("format", "jsonv2");
    query.addQueryItem("q", text);
    query.addQueryItem("limit", "10");
    query.addQueryItem("countrycodes", "fr");
    query.addQueryItem("addressdetails", "1");
    query.addQueryItem("accept-language", "fr,fr-FR;q=0.9,en;q=0.8");
    query.addQueryItem("dedupe", "1");
    query.addQueryItem("layer", "address");

    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker");

    m_networkManager->get(request);
}


void AddressCompleter::handleNetworkReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) 
    {
        QByteArray data{ reply->readAll() };
        QJsonDocument doc{ QJsonDocument::fromJson(data) };
        QJsonArray array{ doc.array() };


        QStringList suggestions;
        for (const QJsonValue& value : array) 
        {
            QJsonObject obj{ value.toObject() };

            QString formattedAddress;

            if (obj.contains("address")) 
            {
                QJsonObject address{ obj["address"].toObject() };

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
            QString fullAddress{ obj["display_name"].toString() };
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
        m_completer->setCompletionPrefix("");
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
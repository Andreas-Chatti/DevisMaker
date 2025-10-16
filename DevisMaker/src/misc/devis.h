#pragma once
#include "models/Client.h"
#include "utils/resultatDevis.h"
#include <qstring.h>

class Devis : public QObject
{
    Q_OBJECT
public:

    explicit Devis(const std::shared_ptr<Client>& client, ResultatsDevis results, QObject* parent = nullptr)
        : QObject{ parent }
        , m_client{ client }
        , m_results{ results }
    {
    }

    ~Devis() = default;

    Devis(const Devis& devis)
        : m_client{ devis.m_client }
        , m_results{ devis.m_results }
        , m_numero{ devis.m_numero }
        , m_date{ devis.m_date }
    {
    }

    Devis& operator=(const Devis& devis)
    {
        if (&devis == this)
            return *this;

        m_client = devis.m_client;
        m_results = devis.m_results;
        m_numero = devis.m_numero;
        m_date = devis.m_date;

        return *this;
    }

    Devis(Devis&& devis) noexcept
        : m_client{std::move(devis.m_client)}
        , m_results{std::move(devis.m_results)}
        , m_numero{std::move(devis.m_numero)}
        , m_date{std::move(devis.m_date)}
    {
    }

    Devis& operator=(Devis&& devis) noexcept
    {
        if (&devis == this)
            return *this;

        m_client = std::move(devis.m_client);
        m_results = std::move(devis.m_results);
        m_numero = std::move(devis.m_numero);
        m_date = std::move(devis.m_date);

        return *this;
    }

private:

    std::weak_ptr<const Client> m_client;
    ResultatsDevis m_results{};
    QString m_numero{""};                  // Numéro unique du devis
    QDate m_date{ QDate::currentDate() }; // Date de création
};
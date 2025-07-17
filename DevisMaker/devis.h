#pragma once

#include "Client.h"
#include "resultatDevis.h"
#include <qstring.h>

class Devis 
{

public:

    Devis(const Client& client, const ResultatsDevis& results)
        : m_client(client)
        , m_totalVolume(client.getVolume())
        , m_totalHT(results.prixTotalHT)
        , m_TotalcoutMainOeuvre{}
    {
    }


private:

    const Client& m_client;

    double m_totalVolume;
    double m_totalHT;              // Montant total HT
    double m_TotalcoutMainOeuvre;
    double m_TotalcoutCamion;
    double m_TotalcoutStationnement;
    double m_TotalfraisRoute;
    double m_TotalcoutAssurance;
    double m_TotalfraisMonteMeubles;
    double m_TotalprixSuppAdresse;
    double m_TotalprixKilometrage;
    double m_TotalprixTotalHT;
    double m_Totalarrhes;

    QString m_numero;          // Numéro unique du devis
    QString m_date;            // Date de création
};
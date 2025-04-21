#include "Tarification.h"
#include <cmath>

Tarification::Tarification()
    : m_tauxHoraireMO(220.0),         // 220€ HT par personne 
    m_coutKilometrique(1.5),        // 1.5€ HT par km
    m_prixForfaitaireEmballage(50.0), // 50€ HT pour l'emballage
    m_volumeHeurePersonne(2.0)      // 2m³ par heure et par personne en moyenne
{
}

double Tarification::calculerHeuresNecessaires(double volume) const {
    if (volume <= 0.0) return 0.0;

    // Calcul du nombre d'heures en fonction du volume
    double heures = volume / m_volumeHeurePersonne;

    // Arrondir à l'heure supérieure
    return std::ceil(heures);
}

double Tarification::calculerCoutMainOeuvre(double volume) const {
    double heures = calculerHeuresNecessaires(volume);
    return heures * m_tauxHoraireMO;
}

double Tarification::calculerCoutTransport(double distance) const {
    return distance * m_coutKilometrique;
}
#include "calculateurDevis.h"

const ResultatsDevis& CalculateurDevis::calculateDevis(const PricePreset& preset)
{
    double volumeParPersonne{ calculerVolumeParPersonne() };
    int nombreCamion{ calculerNombreCamion() };
    int nombreMO{ calculerNombreMO(nombreCamion) };

    double coutMOTotal{ calculerCoutMainOeuvreTotal() };

    double coutCamionTotal{ calculerCoutCamionTotal() };

    double coutAutStatTotal{ calculerPrixStationnement() };

    double fraisRouteTotal{ calculerFraisRouteTotal() };

    double coutAssurance{ calculerCoutAssurance() };

    double fraisMMeubles{ calculerSupplementMM() };

    double prixDechetterie{ m_client.getIsDE() ? m_tarification.getPrixDechetterie() : 0 }; // A enlever plus tard

    double prixSuppAdresse{ calculerSuppAdresseTotal() };

    double prixKilometrage{ calculerCoutKilometrageTotal() };

    double prixTotalHT{ calculerCoutTotalHT() };

    double arrhes{ calculerArrhes() };

    double prixMetreCube{ calculerPrixMetreCube(preset) };

    m_lastResults = { volumeParPersonne, nombreCamion, nombreMO, coutMOTotal, coutCamionTotal,
        coutAutStatTotal, fraisRouteTotal, coutAssurance, fraisMMeubles, prixDechetterie, prixSuppAdresse, prixKilometrage, prixTotalHT, arrhes, prixMetreCube };


    return m_lastResults;
}


double CalculateurDevis::calculerCoutAssurance() const
{
    const double taux{ m_client.getTypeAssurance() == TypeAssurance::contractuelle ? 0.2 : 0.5};

    return (m_client.getValeurAssurance() * taux) / 100;
}


double CalculateurDevis::calculerPrixStationnement() const
{
    double fraisStationnement{};
    bool autStatChargement{ m_client.getAdresseDepart().m_autStationnement };
    bool autStatLivraison{ m_client.getAdresseArrivee().m_autStationnement };
    double coutFraisStationnement_Unitaire{ m_tarification.getCoutFraisStationnement() };

    for (const auto autStat : std::vector<bool>{ autStatChargement, autStatLivraison })
    {
        if (autStat)
            fraisStationnement += coutFraisStationnement_Unitaire;
    }

    return fraisStationnement;
}


double CalculateurDevis::calculerSupplementMM() const
{
    double supplement{};

    const Adresse& aChargement{ m_client.getAdresseDepart() };
    const Adresse& aLivraison{ m_client.getAdresseArrivee() };
    double prixMM_Unitaire{ m_tarification.getCoutMonteMeubles() };

    for (const auto& adresse : std::vector<const Adresse>{ aChargement, aLivraison })
    {
        if (adresse.m_monteMeubles || (!adresse.m_ascenseur && !adresse.m_monteMeubles && adresse.m_etage >= 3))
            supplement += prixMM_Unitaire

    }

    return supplement;
}


double CalculateurDevis::calculerVolumeParPersonne() const
{
    double volumeParPersonne{ 8.0 };

    // Volume par personne selon le type de prestation
    switch (m_client.getPrestation())
    {
    case Prestation::luxe: return volumeParPersonne = 6.0;  // 6m³ par personne en LUXE

    case Prestation::standard: return volumeParPersonne = 8.0;  // 8m³ par personne en STANDARD

    case Prestation::eco: [[fallthrough]];

    case Prestation::ecoPlus: return volumeParPersonne = 10.0; // 10m³ par personne en ECO/ECO+
    }
}


int CalculateurDevis::calculerNombreCamion(bool accesComplexe = false, bool montageImportant = false) const
{
    int nombreJours{ 1 }; // Par défaut un jour minimum

    const Nature& nature{ m_client.getNature() };
    const Prestation& prestation{ m_client.getPrestation() };
    double distance{ m_client.getDistance() };
    double volume{ m_client.getVolume() };


    // 2. Calcul selon le type de déménagement (urbain ou route)
    // Calcul pour déménagement urbain
    if (nature == Nature::urbain)
    {
        if (volume > 60.0)
            nombreJours++; // +1 jour pour grand volume

        // Complexité supplémentaire pour démontage/remontage en urbain
        if (montageImportant && (prestation == Prestation::luxe || prestation == Prestation::standard || prestation == Prestation::ecoPlus))
            nombreJours++;

        // Accès difficile peut ajouter un jour
        if (accesComplexe && volume > 40.0)
            nombreJours++;
    }


    // Calcul pour déménagement national (route)
    else if (nature == Nature::special)
    {
        bool petitVolume{ volume <= 20.0 };

        if (distance >= 150 && distance <= 400)
            nombreJours = petitVolume ? 2 : 3;

        else if (distance > 400 && distance <= 600)
            nombreJours = 3;

        else if (distance > 600 && distance <= 760)
            nombreJours = 4;

        else if (distance > 760 && distance <= 900)
            nombreJours = 5;

        else if (distance > 900)
            nombreJours = 5 + static_cast<int>((distance - 900) / 300); // +1 jour par 300km supplémentaires
    }


    else if (nature == Nature::groupage)
        nombreJours = 5;


    // Jour de préparation pour prestation LUXE
    if (prestation == Prestation::luxe)
        nombreJours += 1; // Jour supplémentaire pour emballage


    return nombreJours;
}


int CalculateurDevis::calculerNombreMO(int nombreCamions) const
{
    double volume{ m_client.getVolume() };
    const Prestation& prestation{ m_client.getPrestation() };
    const Nature& nature{ m_client.getNature() };
    double distance{ m_client.getDistance() };


    // 1. Utiliser la méthode existante pour déterminer le volume par personne
    double volumeParPersonne{ calculerVolumeParPersonne() };

    // 2. Calculer le nombre de base de personnes pour le volume total
    int nombrePersonnesBase{ static_cast<int>(std::ceil(volume / volumeParPersonne)) };

    // 3. Ajuster selon le type de déménagement et le nombre de jours
    int nombrePersonnesTotal{ nombrePersonnesBase };


    if (nature == Nature::special)
    {
        // Pour la route, on a besoin du nombre de base pour le premier jour
        // et ensuite 2 personnes par jour supplémentaire
        if (nombreCamions > 1)
        {
            // Premier jour: nombre de base (mais au moins 2 personnes)
            // Jours suivants: 2 personnes par camion
            nombrePersonnesTotal = std::max(2, nombrePersonnesBase); // Premier jour
            nombrePersonnesTotal += 2 * (nombreCamions - 1);         // Jours suivants
        }

        else
            // Si un seul jour, prendre le maximum entre le nombre calculé et 2
            nombrePersonnesTotal = std::max(2, nombrePersonnesBase);
    }


    else if (nature == Nature::urbain)
    {
        // Pour l'urbain, répartir le personnel sur le nombre de jours
        // avec un minimum de 2 personnes par jour
        nombrePersonnesTotal = std::max(2, static_cast<int>(std::ceil(static_cast<double>(nombrePersonnesBase) / nombreCamions)));

        // Multiplier par le nombre de jours pour avoir le total
        nombrePersonnesTotal *= nombreCamions;
    }


    else if (nature == Nature::groupage)
    {
        // Pour le groupage, généralement 2 personnes suffisent
        nombrePersonnesTotal = 2 * nombreCamions;
    }


    return nombrePersonnesTotal;
}


double CalculateurDevis::calculerPrixMetreCube(PricePreset preset) const
{
    const Prestation& prestation{ m_client.getPrestation() };
    const Nature& nature{ m_client.getNature() };
    double distance{ m_client.getDistance() };

    if (nature == Nature::urbain)
    {
        switch (prestation)
        {
        case Prestation::eco:
            return preset == PricePreset::BasseSaison ? BasseSaison::prixM3_Eco_Default : HauteSaison::prixM3_Eco_Default;
        case Prestation::ecoPlus:
            return preset == PricePreset::BasseSaison ? BasseSaison::prixM3_EcoPlus_Default : HauteSaison::prixM3_EcoPlus_Default;
        case Prestation::standard:
            return preset == PricePreset::BasseSaison ? BasseSaison::prixM3_Standard_Default : HauteSaison::prixM3_Standard_Default;
        case Prestation::luxe:
            return preset == PricePreset::BasseSaison ? BasseSaison::prixM3_Luxe_Default : HauteSaison::prixM3_Luxe_Default;
        }
    }

    else if (nature == Nature::special || nature == Nature::groupage)
    {
        if (distance >= 150 && distance <= 400)
            return 65.0;

        else if (distance > 400 && distance <= 600)
            return 75.0;

        else if (distance > 600 && distance <= 760)
            return 80.0;

        else if (distance > 760 && distance <= 900)
            return 100.0;

        else if (distance > 900)
            return 120.0;
    }
}
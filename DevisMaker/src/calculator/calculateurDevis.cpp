#include "calculateurDevis.h"

ResultatsDevis CalculateurDevis::calculateDevis(PricePreset preset, Tarification::PriceCalculation calculationMethod)
{
    double coutAutStatTotal{ calculerPrixStationnement() };
    double fraisRouteTotal{ calculerFraisRouteTotal() };
    double coutAssurance{ calculerCoutAssurance() };
    double fraisMMeubles{ calculerSupplementMM() };
    double prixSuppAdresse{ calculerSuppAdresseTotal() };

    ResultatsDevis_CinqPostes resultatsCinqPostes{};
    if (calculationMethod == Tarification::PriceCalculation::postes)
        resultatsCinqPostes = calculateDevis_Postes();

    double prixMetreCube{ calculerPrixMetreCube(preset) };
    double prixTotalHT{ calculerCoutTotalHT(calculationMethod) };
    double arrhes{ calculerArrhes(prixTotalHT) };

    m_lastResults = { coutAutStatTotal, fraisRouteTotal, coutAssurance, fraisMMeubles, prixSuppAdresse, prixMetreCube, prixTotalHT, arrhes, resultatsCinqPostes };

    return m_lastResults;
}


double CalculateurDevis::calculerFraisRouteTotal() const
{
    const Nature& nature{ m_client->getNature() };
    double fraisRouteTotal{ (m_tarification->getFraisRoute() * (calculerNombreCamion() - 1)) * 2 };

    return nature != Nature::urbain ? fraisRouteTotal : 0;
}

double CalculateurDevis::calculerCoutAssurance() const
{
    const double taux{ m_client->getTypeAssurance() == TypeAssurance::contractuelle ? SettingsConstants::CONTRACTUELLE_INSURANCE_RATE : SettingsConstants::DOMMAGE_INSURANCE_RATE };

    return (m_client->getValeurAssurance() * taux) / 100;
}


double CalculateurDevis::calculerPrixStationnement() const
{
    double fraisStationnement{};
    bool autStatChargement{ m_client->getAdresseDepart().m_autStationnement };
    bool autStatLivraison{ m_client->getAdresseArrivee().m_autStationnement };
    double coutFraisStationnement_Unitaire{ m_tarification->getCoutFraisStationnement() };

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

    const Adresse& aChargement{ m_client->getAdresseDepart() };
    const Adresse& aLivraison{ m_client->getAdresseArrivee() };
    double prixMM_Unitaire{ m_tarification->getCoutMonteMeubles() };

    for (const auto& adresse : std::vector<Adresse>{ aChargement, aLivraison })
    {
        if (adresse.m_monteMeubles || (!adresse.m_ascenseur && !adresse.m_monteMeubles && adresse.m_etage >= 3))
            supplement += prixMM_Unitaire;

    }

    return supplement;
}


double CalculateurDevis::calculerVolumeParPersonne() const
{
    // Volume par personne selon le type de prestation
    switch (m_client->getPrestation())
    {
    case Prestation::luxe: return SettingsConstants::Worker::M3_PER_WORKER_LUXE;  // 6m³ par personne en LUXE

    case Prestation::standard: return SettingsConstants::Worker::M3_PER_WORKER_STANDARD;  // 8m³ par personne en STANDARD

    case Prestation::eco: return SettingsConstants::Worker::M3_PER_WORKER_ECOPLUS; // 9m³ par personne en ECO

    case Prestation::ecoPlus: return SettingsConstants::Worker::M3_PER_WORKER_ECO; // 10m³ par personne en ECO
    }
}


int CalculateurDevis::calculerNombreCamion(bool accesComplexe, bool montageImportant) const
{
    int nombreJours{ SettingsConstants::MIN_WORKING_DAY }; // Par défaut un jour minimum

    const Nature& nature{ m_client->getNature() };
    const Prestation& prestation{ m_client->getPrestation() };
    double distance{ m_client->getDistance() };
    double volume{ m_client->getVolume() };


    // 2. Calcul selon le type de déménagement (urbain ou route)
    // Calcul pour déménagement urbain
    if (nature == Nature::urbain)
    {
        if (volume > SettingsConstants::MAX_M3_PER_TRUCK)
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

        if (distance >= SettingsConstants::Distances::URBAN_DISTANCE_LIMIT && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_1)
            nombreJours = petitVolume ? 2 : 3;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_1 && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_2)
            nombreJours = 3;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_2 && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_3)
            nombreJours = 4;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_3 && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_4)
            nombreJours = 5;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_4)
            nombreJours = 5 + static_cast<int>((distance - SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_4) / 300); // +1 jour par 300km supplémentaires
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
    double volume{ m_client->getVolume() };
    const Prestation& prestation{ m_client->getPrestation() };
    const Nature& nature{ m_client->getNature() };
    double distance{ m_client->getDistance() };


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
        if (nombreCamions > SettingsConstants::MIN_WORKING_DAY)
        {
            // Premier jour: nombre de base (mais au moins 2 personnes)
            // Jours suivants: 2 personnes par camion
            nombrePersonnesTotal = std::max(SettingsConstants::Worker::MIN_WORKERS, nombrePersonnesBase); // Premier jour
            nombrePersonnesTotal += SettingsConstants::Worker::MIN_WORKERS * (nombreCamions - 1);         // Jours suivants
        }

        else
            // Si un seul jour, prendre le maximum entre le nombre calculé et 2
            nombrePersonnesTotal = std::max(SettingsConstants::Worker::MIN_WORKERS, nombrePersonnesBase);
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
        nombrePersonnesTotal = SettingsConstants::Worker::MIN_WORKERS * nombreCamions;
    }


    return nombrePersonnesTotal;
}


double CalculateurDevis::calculerPrixMetreCube(PricePreset preset) const
{
    const Prestation& prestation{ m_client->getPrestation() };
    const Nature& nature{ m_client->getNature() };
    double distance{ m_client->getDistance() };
    bool basseSaison{ preset == PricePreset::BasseSaison };

    if (nature == Nature::urbain)
    {
        switch (prestation)
        {
        case Prestation::eco: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::ECO : M3_DefaultPrices::Urbain::HauteSaison::ECO;
        case Prestation::ecoPlus: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::ECOPLUS : M3_DefaultPrices::Urbain::HauteSaison::ECOPLUS;
        case Prestation::standard: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::STANDARD : M3_DefaultPrices::Urbain::HauteSaison::STANDARD;
        case Prestation::luxe: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::LUXE : M3_DefaultPrices::Urbain::HauteSaison::LUXE;
        }
    }

    else if (nature == Nature::special || nature == Nature::groupage)
    {
        if (distance >= SettingsConstants::Distances::URBAN_DISTANCE_LIMIT && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_1)
            return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_150_400 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_150_400;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_1 && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_2)
            return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_401_600 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_401_600;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_2 && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_3)
            return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_601_760 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_601_760;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_3 && distance <= SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_4)
            return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_761_900 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_761_900;

        else if (distance > SettingsConstants::Distances::ROUTE_DISTANCE_LIMIT_4)
            return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_901PLUS : M3_DefaultPrices::Route::HauteSaison::DISTANCE_901PLUS;
    }
}


double CalculateurDevis::calculerCoutTotalHT(Tarification::PriceCalculation calculationMethod) const
{
    double prixM3{ m_tarification->getPrixMetreCube() };
    double volumeTotal{ m_client->getVolume() };

    double prixTotalAssurance{ calculerCoutAssurance() };
    double prixTotalStationnement{ calculerPrixStationnement() };
    double prixTotalMonteMeubles{ calculerSupplementMM() };
    double prixTotalFraisRoute{ calculerFraisRouteTotal() };
    double prixTotalSuppAdresse{ calculerSuppAdresseTotal() };
    double prixTotalSupplements{ prixTotalAssurance + prixTotalStationnement + prixTotalMonteMeubles + prixTotalFraisRoute + prixTotalSuppAdresse };

    switch (calculationMethod)
    {
    case Tarification::PriceCalculation::m3:
        return volumeTotal * prixM3 + prixTotalSupplements;

    case Tarification::PriceCalculation::postes:
        double coutCamion{ calculerCoutCamionTotal() };
        double coutKilometrique{ calculerCoutKilometrageTotal() };
        double coutLocMateriel{ calculerCoutLocMaterielTotal() };
        double coutMainOeuvre{ calculerCoutMainOeuvreTotal() };
        double coutEmballage{ calculerCoutEmballageTotal() };

        return coutCamion + coutKilometrique + coutLocMateriel + coutMainOeuvre + coutEmballage + prixTotalSupplements;
    }
}

double CalculateurDevis::calculerCoutMainOeuvreTotal() const
{
    int nombreCamion{ calculerNombreCamion() };
    int nombreMO{ calculerNombreMO(nombreCamion) };

    return nombreMO * m_tarification->getCoutMO();
}


ResultatsDevis_CinqPostes CalculateurDevis::calculateDevis_Postes()
{
    double volumeParPersonne{ calculerVolumeParPersonne() };
    int nombreCamion{ calculerNombreCamion() };
    int nombreMO{ calculerNombreMO(nombreCamion) };
    double coutMOTotal{ calculerCoutMainOeuvreTotal() };
    double coutCamionTotal{ calculerCoutCamionTotal() };
    double prixKilometrage{ calculerCoutKilometrageTotal() };
    double prixEmballage{ calculerCoutEmballageTotal() };
    double prixLocMateriel{ calculerCoutLocMaterielTotal() };

    return { volumeParPersonne, nombreCamion, nombreMO, coutMOTotal, coutCamionTotal, prixKilometrage, prixEmballage, prixLocMateriel };
}
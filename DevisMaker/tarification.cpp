#include "Tarification.h"

void Tarification::loadSettings(PricePreset preset)
{
    // Définir le chemin du fichier de configuration
    QString settingsPath{ QDir::homePath() + "/DevisMaker/config.ini" };
    QFileInfo checkFile(settingsPath);

    // Créer le dossier s'il n'existe pas
    QDir().mkpath(QDir::homePath() + "/DevisMaker");

    // Initialiser QSettings
    QSettings settings(settingsPath, QSettings::IniFormat);

    // Déterminer le nom de la section selon le preset
    QString sectionName;
    if (preset == PricePreset::BasseSaison)
        sectionName = "TarificationBasseSaison";
 
    else
        sectionName = "TarificationHauteSaison";
  

    // Si le fichier existe, charger les paramètres du preset demandé
    if (checkFile.exists() && checkFile.isFile())
    {
        settings.beginGroup(sectionName);
        m_coutCamion = settings.value("CoutCamion", getDefaultValue("CoutCamion", preset)).toDouble();
        m_coutKilometrique = settings.value("CoutKilometrique", getDefaultValue("CoutKilometrique", preset)).toDouble();
        m_coutEmballage = settings.value("CoutEmballage", getDefaultValue("CoutEmballage", preset)).toDouble();
        m_prixLocMateriel = settings.value("PrixLocMateriel", getDefaultValue("PrixLocMateriel", preset)).toDouble();
        m_fraisRoute = settings.value("FraisRoute", getDefaultValue("FraisRoute", preset)).toDouble();
        m_coutMO = settings.value("CoutMO", getDefaultValue("CoutMO", preset)).toDouble();
        m_fraisStationnement = settings.value("FraisStationnement", getDefaultValue("FraisStationnement", preset)).toDouble();
        m_prixMonteMeubles = settings.value("PrixMM", getDefaultValue("PrixMM", preset)).toDouble();
        m_prixDechetterie = settings.value("PrixDechetterie", getDefaultValue("PrixDechetterie", preset)).toDouble();
        m_prixSuppAdresse = settings.value("PrixSuppAdresse", getDefaultValue("PrixSuppAdresse", preset)).toDouble();
        settings.endGroup();
    }

    else
    {
        // Fichier n'existe pas, charger les valeurs par défaut et sauvegarder les deux presets
        loadDefaultValues(PricePreset::BasseSaison);
        saveSettings(PricePreset::BasseSaison);

        loadDefaultValues(PricePreset::HauteSaison);
        saveSettings(PricePreset::HauteSaison);
    }
}


void Tarification::saveSettings(PricePreset preset) const
{
    // Définir le chemin du fichier de configuration
    QString settingsPath{ QDir::homePath() + "/DevisMaker/config.ini" };

    // Initialiser QSettings
    QSettings settings{ settingsPath, QSettings::IniFormat };

    // Déterminer le nom de la section selon le preset
    QString sectionName;
    if (preset == PricePreset::BasseSaison)
        sectionName = "TarificationBasseSaison";

    else
        sectionName = "TarificationHauteSaison";
    

    // Sauvegarder les paramètres dans la section correspondante
    settings.beginGroup(sectionName);
    settings.setValue("CoutCamion", m_coutCamion);
    settings.setValue("CoutKilometrique", m_coutKilometrique);
    settings.setValue("CoutEmballage", m_coutEmballage);
    settings.setValue("PrixLocMateriel", m_prixLocMateriel);
    settings.setValue("FraisRoute", m_fraisRoute);
    settings.setValue("CoutMO", m_coutMO);
    settings.setValue("FraisStationnement", m_fraisStationnement);
    settings.setValue("PrixMM", m_prixMonteMeubles);
    settings.setValue("PrixDechetterie", m_prixDechetterie);
    settings.setValue("PrixSuppAdresse", m_prixSuppAdresse);
    settings.endGroup();
}


double Tarification::calculerVolumeParPersonne(double volume, Prestation prestation) const
{
    double volumeParPersonne{ 8.0 };

    // Volume par personne selon le type de prestation
    switch (prestation)
    {
    case Prestation::luxe: return volumeParPersonne = 6.0;  // 6m³ par personne en LUXE

    case Prestation::standard: return volumeParPersonne = 8.0;  // 8m³ par personne en STANDARD

    case Prestation::eco: [[fallthrough]];

    case Prestation::ecoPlus: return volumeParPersonne = 10.0; // 10m³ par personne en ECO/ECO+
    }
}


int Tarification::calculerNombreCamion(double volume, Prestation prestation, Nature nature, double distance, bool accesComplexe, bool montageImportant) const 
{
    int nombreJours{ 1 }; // Par défaut un jour minimum


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


void Tarification::setPrixMetreCube(Prestation prestation, Nature nature, double distance)
{
    if (nature == Nature::urbain)
    {
        switch (prestation)
        {
        case Prestation::eco:
            m_prixMetreCube = 30.0;
            return;
        case Prestation::ecoPlus:
            m_prixMetreCube = 35.0;
            return;
        case Prestation::standard:
            m_prixMetreCube = 40.0;
            return;
        case Prestation::luxe:
            m_prixMetreCube = 50.0;
            return;
        }
    }

    else if (nature == Nature::special || nature == Nature::groupage)
    {
        if (distance >= 150 && distance <= 400)
            m_prixMetreCube = 65.0;

        else if (distance > 400 && distance <= 600)
            m_prixMetreCube = 75.0;

        else if (distance > 600 && distance <= 760)
            m_prixMetreCube = 80.0;

        else if (distance > 760 && distance <= 900)
            m_prixMetreCube = 100.0;

        else if (distance > 900)
            m_prixMetreCube = 120.0;
    }
}


int Tarification::calculerNombreMO(double volume, Prestation prestation, Nature nature, int nombreCamions, bool monteMeuble, bool ascenseur, double distance) const 
{
    // 1. Utiliser la méthode existante pour déterminer le volume par personne
    double volumeParPersonne{ calculerVolumeParPersonne(volume, prestation) };

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


double Tarification::calculerCoutAssurance(int valeurMobilier, TypeAssurance assurance) const
{
    const double taux{ assurance == TypeAssurance::contractuelle ? 0.2 : 0.5 };

    return (valeurMobilier * taux) / 100;
}


double Tarification::calculerPrixStationnement(bool autStatChargement, bool autStatLivraison) const
{
    double fraisStationnement{};

    for (const auto autStat : std::array<bool, 2>{ autStatChargement, autStatLivraison })
    {
        if (autStat)
            fraisStationnement += m_fraisStationnement;
    }

    return fraisStationnement;
}


double Tarification::calculerSupplementMM(const Adresse& aChargement, const Adresse& aLivraison) const
{
    double supplement{};

    for (const auto& adresse : std::array<const Adresse, 2>{ aChargement, aLivraison })
    {
        if (adresse.m_monteMeubles || (!adresse.m_ascenseur && !adresse.m_monteMeubles && adresse.m_etage >= 3))
            supplement += m_prixMonteMeubles;

    }

    return supplement;
}


double Tarification::getDefaultValue(const QString& key, PricePreset preset) const
{

    switch (preset)
    {
    case PricePreset::BasseSaison: return BasseSaison::pricesMap.at(key);

    case PricePreset::HauteSaison: return HauteSaison::pricesMap.at(key);
    }

    return 0.0;
}


void Tarification::loadDefaultValues(PricePreset preset)
{
    m_coutCamion = getDefaultValue("CoutCamion", preset);
    m_coutKilometrique = getDefaultValue("CoutKilometrique", preset);
    m_coutEmballage = getDefaultValue("CoutEmballage", preset);
    m_prixLocMateriel = getDefaultValue("PrixLocMateriel", preset);
    m_fraisRoute = getDefaultValue("FraisRoute", preset);
    m_coutMO = getDefaultValue("CoutMO", preset);
    m_fraisStationnement = getDefaultValue("FraisStationnement", preset);
    m_prixMonteMeubles = getDefaultValue("PrixMM", preset);
    m_prixDechetterie = getDefaultValue("PrixDechetterie", preset);
    m_prixSuppAdresse = getDefaultValue("PrixSuppAdresse", preset);
}
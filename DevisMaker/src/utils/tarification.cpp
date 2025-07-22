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
    QString sectionName{ preset == PricePreset::BasseSaison ? "Tarification_Basse_Saison" : "Tarification_Haute_Saison" };
  

    // Si le fichier existe, charger les paramètres du preset demandé
    if (checkFile.exists() && checkFile.isFile())
    {
        settings.beginGroup(sectionName);
        m_coutCamion = settings.value("Camion", getDefaultPrices_5Postes(CoutCamion, preset)).toDouble();
        m_coutKilometrique = settings.value("Kilometrage", getDefaultPrices_5Postes(CoutKilometrique, preset)).toDouble();
        m_coutEmballage = settings.value("Emballage", getDefaultPrices_5Postes(CoutEmballage, preset)).toDouble();
        m_prixLocMateriel = settings.value("Location_Materiel", getDefaultPrices_5Postes(CoutCamion, preset)).toDouble();
        m_fraisRoute = settings.value("Frais_Route", getDefaultPrices_5Postes(CoutFraisRoute, preset)).toDouble();
        m_coutMO = settings.value("Main_Oeuvre", getDefaultPrices_5Postes(CoutMainOeuvre, preset)).toDouble();
        m_fraisStationnement = settings.value("Stationnement", getDefaultPrices_5Postes(CoutFraisStationnement, preset)).toDouble();
        m_prixMonteMeubles = settings.value("Monte_Meubles", getDefaultPrices_5Postes(CoutMonteMeubles, preset)).toDouble();
        m_prixSuppAdresse = settings.value("Supplement_Adresse", getDefaultPrices_5Postes(CoutFraisStationnement, preset)).toDouble();
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
    QString sectionName{ preset == PricePreset::BasseSaison ? "Tarification_Basse_Saison" : "Tarification_Haute_Saison" };
    

    // Sauvegarder les paramètres dans la section correspondante
    settings.beginGroup(sectionName);
    settings.setValue("Camion", m_coutCamion);
    settings.setValue("Kilometrage", m_coutKilometrique);
    settings.setValue("Emballage", m_coutEmballage);
    settings.setValue("Location_Materiel", m_prixLocMateriel);
    settings.setValue("Frais_Route", m_fraisRoute);
    settings.setValue("Main_Oeuvre", m_coutMO);
    settings.setValue("Stationnement", m_fraisStationnement);
    settings.setValue("Monte_Meubles", m_prixMonteMeubles);
    settings.setValue("Supplement_Adresse", m_prixSuppAdresse);
    settings.endGroup();
}

void Tarification::loadDefaultValues(PricePreset preset)
{
    m_coutCamion = getDefaultPrices_5Postes(CoutCamion, preset);
    m_coutKilometrique = getDefaultPrices_5Postes(CoutKilometrique, preset);
    m_coutEmballage = getDefaultPrices_5Postes(CoutEmballage, preset);
    m_prixLocMateriel = getDefaultPrices_5Postes(CoutLocMateriel, preset);
    m_fraisRoute = getDefaultPrices_5Postes(CoutFraisRoute, preset);
    m_coutMO = getDefaultPrices_5Postes(CoutMainOeuvre, preset);
    m_fraisStationnement = getDefaultPrices_5Postes(CoutFraisStationnement, preset);
    m_prixMonteMeubles = getDefaultPrices_5Postes(CoutMonteMeubles, preset);
    m_prixSuppAdresse = getDefaultPrices_5Postes(CoutSupplementAdresse, preset);
}


double Tarification::getDefaultPrices_5Postes(PriceKey key, PricePreset preset) const
{
    bool basseSaison{ preset == PricePreset::BasseSaison };

    switch (key)
    {
    case Tarification::CoutCamion: return basseSaison ? Postes_DefaultPrices::BasseSaison::CAMION : Postes_DefaultPrices::HauteSaison::CAMION;
    case Tarification::CoutKilometrique: return basseSaison ? Postes_DefaultPrices::BasseSaison::KILOMETRAGE : Postes_DefaultPrices::HauteSaison::KILOMETRAGE;
    case Tarification::CoutEmballage: return basseSaison ? Postes_DefaultPrices::BasseSaison::EMBALLAGE : Postes_DefaultPrices::HauteSaison::EMBALLAGE;
    case Tarification::CoutLocMateriel: return basseSaison ? Postes_DefaultPrices::BasseSaison::LOC_MATERIEL : Postes_DefaultPrices::HauteSaison::LOC_MATERIEL;
    case Tarification::CoutFraisRoute: return basseSaison ? Postes_DefaultPrices::BasseSaison::FRAIS_ROUTE : Postes_DefaultPrices::HauteSaison::FRAIS_ROUTE;
    case Tarification::CoutMainOeuvre: return basseSaison ? Postes_DefaultPrices::BasseSaison::MAIN_OEUVRE : Postes_DefaultPrices::HauteSaison::MAIN_OEUVRE;
    case Tarification::CoutFraisStationnement: return basseSaison ? Postes_DefaultPrices::BasseSaison::STATIONNEMENT : Postes_DefaultPrices::HauteSaison::STATIONNEMENT;
    case Tarification::CoutMonteMeubles: return basseSaison ? Postes_DefaultPrices::BasseSaison::MONTE_MEUBLES : Postes_DefaultPrices::HauteSaison::MONTE_MEUBLES;
    case Tarification::CoutSupplementAdresse: return basseSaison ? Postes_DefaultPrices::BasseSaison::SUPP_ADRESSE : Postes_DefaultPrices::HauteSaison::SUPP_ADRESSE;
    }
}
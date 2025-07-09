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
        m_coutCamion = settings.value("Camion", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutCamion, preset)).toDouble();
        m_coutKilometrique = settings.value("Kilometrage", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutKilometrique, preset)).toDouble();
        m_coutEmballage = settings.value("Emballage", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutEmballage, preset)).toDouble();
        m_prixLocMateriel = settings.value("Location_Materiel", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutCamion, preset)).toDouble();
        m_fraisRoute = settings.value("Frais_Route", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutFraisRoute, preset)).toDouble();
        m_coutMO = settings.value("Main_Oeuvre", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutMainOeuvre, preset)).toDouble();
        m_fraisStationnement = settings.value("Stationnement", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutFraisStationnement, preset)).toDouble();
        m_prixMonteMeubles = settings.value("Monte_Meubles", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutMonteMeubles, preset)).toDouble();
        m_prixSuppAdresse = settings.value("Supplement_Adresse", SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutFraisStationnement, preset)).toDouble();
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
    m_coutCamion = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutCamion, preset);
    m_coutKilometrique = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutKilometrique, preset);
    m_coutEmballage = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutEmballage, preset);
    m_prixLocMateriel = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutLocMateriel, preset);
    m_fraisRoute = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutFraisRoute, preset);
    m_coutMO = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutMainOeuvre, preset);
    m_fraisStationnement = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutFraisStationnement, preset);
    m_prixMonteMeubles = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutMonteMeubles, preset);
    m_prixSuppAdresse = SettingsConstants::M3PriceUrban::getDefaultPrices(SettingsConstants::M3PriceUrban::PriceKey::CoutSupplementAdresse, preset);
}
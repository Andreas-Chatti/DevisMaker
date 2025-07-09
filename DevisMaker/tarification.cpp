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
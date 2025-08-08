#include "Tarification.h"

void Tarification::loadSettings(PricePreset preset)
{
    QFileInfo configFileInfos(CONFIG_FILE_PATH);

    QDir dataFile{ SettingsConstants::FileSettings::DATA_FILE_PATH };
    if (!dataFile.exists())
        QDir().mkpath(SettingsConstants::FileSettings::DATA_FILE_PATH);

    QSettings settings(CONFIG_FILE_PATH, QSettings::IniFormat);

    if (configFileInfos.exists() && configFileInfos.isFile())
    {
       loadSettings_5Postes(settings, preset);
       loadSettings_M3(settings, preset, Nature::urbain);
       loadSettings_M3(settings, preset, Nature::special);
    }

    else
        for (const auto& pricePreset : QVector<PricePreset>{ PricePreset::BasseSaison, PricePreset::HauteSaison })
        {
            for (const auto& priceCalculationMethod : QVector<PriceCalculation>{ PriceCalculation::m3, PriceCalculation::postes })
            {
                loadDefaultValues(pricePreset, priceCalculationMethod);
                saveSettings(pricePreset, priceCalculationMethod);
            }
        }
}


void Tarification::saveSettings(PricePreset preset, PriceCalculation priceCalculation) const
{
    QSettings settings{ CONFIG_FILE_PATH, QSettings::IniFormat };

    QString sectionNameSuffix{ preset == PricePreset::BasseSaison ? CONFIG_SECTION_BASSE_SAISON_SUFFIX : CONFIG_SECTION_HAUTE_SAISON_SUFFIX };

    switch (priceCalculation)
    {
    case Tarification::PriceCalculation::postes:
        settings.beginGroup(CONFIG_SECTION_POSTES_PREFIX + "_" + sectionNameSuffix);
        settings.setValue(enumToString(CoutCamion), m_coutCamion);
        settings.setValue(enumToString(CoutKilometrique), m_coutKilometrique);
        settings.setValue(enumToString(CoutEmballage), m_coutEmballage);
        settings.setValue(enumToString(CoutLocMateriel), m_prixLocMateriel);
        settings.setValue(enumToString(CoutFraisRoute), m_fraisRoute);
        settings.setValue(enumToString(CoutMainOeuvre), m_coutMO);
        settings.setValue(enumToString(CoutFraisStationnement), m_fraisStationnement);
        settings.setValue(enumToString(CoutMonteMeubles), m_prixMonteMeubles);
        settings.setValue(enumToString(CoutSupplementAdresse), m_prixSuppAdresse);
        settings.endGroup();
        break;

    case Tarification::PriceCalculation::m3:
        settings.beginGroup(CONFIG_SECTION_M3_PREFIX + "_" + sectionNameSuffix);
        settings.setValue(enumToString(distance150_400), m_prixM3_150_400);
        settings.setValue(enumToString(distance401_600), m_prixM3_401_600);
        settings.setValue(enumToString(distance601_760), m_prixM3_601_760);
        settings.setValue(enumToString(distance761_900), m_prixM3_761_900);
        settings.setValue(enumToString(distance901PLUS), m_prixM3_901PLUS);

        settings.setValue(enumToString(m3Eco), m_prixM3_eco);
        settings.setValue(enumToString(m3EcoPlus), m_prixM3_ecoPlus);
        settings.setValue(enumToString(m3Standard), m_prixM3_standard);
        settings.setValue(enumToString(m3Luxe), m_prixM3_luxe);
        settings.endGroup();
    }
}

void Tarification::loadDefaultValues(PricePreset preset, PriceCalculation priceCalculation)
{
    switch (priceCalculation)
    {
    case Tarification::PriceCalculation::postes:
        m_coutCamion = getDefaultPrice_5Postes(CoutCamion, preset);
        m_coutKilometrique = getDefaultPrice_5Postes(CoutKilometrique, preset);
        m_coutEmballage = getDefaultPrice_5Postes(CoutEmballage, preset);
        m_prixLocMateriel = getDefaultPrice_5Postes(CoutLocMateriel, preset);
        m_fraisRoute = getDefaultPrice_5Postes(CoutFraisRoute, preset);
        m_coutMO = getDefaultPrice_5Postes(CoutMainOeuvre, preset);
        m_fraisStationnement = getDefaultPrice_5Postes(CoutFraisStationnement, preset);
        m_prixMonteMeubles = getDefaultPrice_5Postes(CoutMonteMeubles, preset);
        m_prixSuppAdresse = getDefaultPrice_5Postes(CoutSupplementAdresse, preset);
        break;

    case Tarification::PriceCalculation::m3:
        m_prixM3_150_400 = getDefaultPrice_M3(distance150_400, preset, Nature::special);
        m_prixM3_401_600 = getDefaultPrice_M3(distance401_600, preset, Nature::special);
        m_prixM3_601_760 = getDefaultPrice_M3(distance601_760, preset, Nature::special);
        m_prixM3_761_900 = getDefaultPrice_M3(distance761_900, preset, Nature::special);
        m_prixM3_901PLUS = getDefaultPrice_M3(distance901PLUS, preset, Nature::special);

        m_prixM3_eco = getDefaultPrice_M3(m3Eco, preset, Nature::urbain, Prestation::eco);
        m_prixM3_ecoPlus = getDefaultPrice_M3(m3EcoPlus, preset, Nature::urbain, Prestation::ecoPlus);
        m_prixM3_standard = getDefaultPrice_M3(m3Standard, preset, Nature::urbain, Prestation::standard);
        m_prixM3_luxe = getDefaultPrice_M3(m3Luxe, preset, Nature::urbain, Prestation::luxe);
    }
}


double Tarification::getDefaultPrice_5Postes(PriceKey key, PricePreset preset) const
{
    bool basseSaison{ preset == PricePreset::BasseSaison };

    switch (key)
    {
    case CoutCamion: return basseSaison ? Postes_DefaultPrices::BasseSaison::CAMION : Postes_DefaultPrices::HauteSaison::CAMION;
    case CoutKilometrique: return basseSaison ? Postes_DefaultPrices::BasseSaison::KILOMETRAGE : Postes_DefaultPrices::HauteSaison::KILOMETRAGE;
    case CoutEmballage: return basseSaison ? Postes_DefaultPrices::BasseSaison::EMBALLAGE : Postes_DefaultPrices::HauteSaison::EMBALLAGE;
    case CoutLocMateriel: return basseSaison ? Postes_DefaultPrices::BasseSaison::LOC_MATERIEL : Postes_DefaultPrices::HauteSaison::LOC_MATERIEL;
    case CoutFraisRoute: return basseSaison ? Postes_DefaultPrices::BasseSaison::FRAIS_ROUTE : Postes_DefaultPrices::HauteSaison::FRAIS_ROUTE;
    case CoutMainOeuvre: return basseSaison ? Postes_DefaultPrices::BasseSaison::MAIN_OEUVRE : Postes_DefaultPrices::HauteSaison::MAIN_OEUVRE;
    case CoutFraisStationnement: return basseSaison ? Postes_DefaultPrices::BasseSaison::STATIONNEMENT : Postes_DefaultPrices::HauteSaison::STATIONNEMENT;
    case CoutMonteMeubles: return basseSaison ? Postes_DefaultPrices::BasseSaison::MONTE_MEUBLES : Postes_DefaultPrices::HauteSaison::MONTE_MEUBLES;
    case CoutSupplementAdresse: return basseSaison ? Postes_DefaultPrices::BasseSaison::SUPP_ADRESSE : Postes_DefaultPrices::HauteSaison::SUPP_ADRESSE;
    }
}


double Tarification::getDefaultPrice_M3(PriceKey key, PricePreset preset, Nature&& nature, Prestation&& prestation) const
{
    bool basseSaison{ preset == PricePreset::BasseSaison };

    if (nature == Nature::special)
        switch (key)
        {
        case distance150_400: return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_150_400 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_150_400;
        case distance401_600: return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_401_600 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_401_600;
        case distance601_760: return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_601_760 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_601_760;
        case distance761_900: return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_761_900 : M3_DefaultPrices::Route::HauteSaison::DISTANCE_761_900;
        case distance901PLUS: return basseSaison ? M3_DefaultPrices::Route::BasseSaison::DISTANCE_901PLUS : M3_DefaultPrices::Route::HauteSaison::DISTANCE_901PLUS;
        }

    else if (nature == Nature::urbain)
        switch (prestation)
        {
        case Prestation::eco: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::ECO : M3_DefaultPrices::Urbain::HauteSaison::ECO;
        case Prestation::ecoPlus: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::ECOPLUS : M3_DefaultPrices::Urbain::HauteSaison::ECOPLUS;
        case Prestation::standard: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::STANDARD : M3_DefaultPrices::Urbain::HauteSaison::STANDARD;
        case Prestation::luxe: return basseSaison ? M3_DefaultPrices::Urbain::BasseSaison::LUXE : M3_DefaultPrices::Urbain::HauteSaison::LUXE;
        }
}


void Tarification::loadSettings_5Postes(QSettings& settings, PricePreset preset)
{
    QString sectionNameSuffix{ preset == PricePreset::BasseSaison ? CONFIG_SECTION_BASSE_SAISON_SUFFIX : CONFIG_SECTION_HAUTE_SAISON_SUFFIX };

    settings.beginGroup(CONFIG_SECTION_POSTES_PREFIX + "_" + sectionNameSuffix);
    m_coutCamion = settings.value(enumToString(CoutCamion), getDefaultPrice_5Postes(CoutCamion, preset)).toDouble();
    m_coutKilometrique = settings.value(enumToString(CoutKilometrique), getDefaultPrice_5Postes(CoutKilometrique, preset)).toDouble();
    m_coutEmballage = settings.value(enumToString(CoutEmballage), getDefaultPrice_5Postes(CoutEmballage, preset)).toDouble();
    m_prixLocMateriel = settings.value(enumToString(CoutLocMateriel), getDefaultPrice_5Postes(CoutLocMateriel, preset)).toDouble();
    m_fraisRoute = settings.value(enumToString(CoutFraisRoute), getDefaultPrice_5Postes(CoutFraisRoute, preset)).toDouble();
    m_coutMO = settings.value(enumToString(CoutMainOeuvre), getDefaultPrice_5Postes(CoutMainOeuvre, preset)).toDouble();
    m_fraisStationnement = settings.value(enumToString(CoutFraisStationnement), getDefaultPrice_5Postes(CoutFraisStationnement, preset)).toDouble();
    m_prixMonteMeubles = settings.value(enumToString(CoutMonteMeubles), getDefaultPrice_5Postes(CoutMonteMeubles, preset)).toDouble();
    m_prixSuppAdresse = settings.value(enumToString(CoutSupplementAdresse), getDefaultPrice_5Postes(CoutSupplementAdresse, preset)).toDouble();
    settings.endGroup();
}

void Tarification::loadSettings_M3(QSettings& settings, PricePreset preset, Nature&& nature)
{
    QString sectionNameSuffix{ preset == PricePreset::BasseSaison ? CONFIG_SECTION_BASSE_SAISON_SUFFIX : CONFIG_SECTION_HAUTE_SAISON_SUFFIX };

    settings.beginGroup(CONFIG_SECTION_M3_PREFIX + "_" + sectionNameSuffix);
    switch (nature)
    {
    case Nature::urbain:
        m_prixM3_eco = settings.value(enumToString(m3Eco), getDefaultPrice_M3(m3Eco, preset, Nature::urbain, Prestation::eco)).toDouble();
        m_prixM3_ecoPlus = settings.value(enumToString(m3EcoPlus), getDefaultPrice_M3(m3EcoPlus, preset, Nature::urbain, Prestation::ecoPlus)).toDouble();
        m_prixM3_standard = settings.value(enumToString(m3Standard), getDefaultPrice_M3(m3Standard, preset, Nature::urbain, Prestation::standard)).toDouble();
        m_prixM3_luxe = settings.value(enumToString(m3Luxe), getDefaultPrice_M3(m3Luxe, preset, Nature::urbain, Prestation::luxe)).toDouble();
        break;
    case Nature::groupage: [[Fallthrough]]
    case Nature::special:
        m_prixM3_150_400 = settings.value(enumToString(distance150_400), getDefaultPrice_M3(distance150_400, preset, Nature::special)).toDouble();
        m_prixM3_401_600 = settings.value(enumToString(distance401_600), getDefaultPrice_M3(distance401_600, preset, Nature::special)).toDouble();
        m_prixM3_601_760 = settings.value(enumToString(distance601_760), getDefaultPrice_M3(distance601_760, preset, Nature::special)).toDouble();
        m_prixM3_761_900 = settings.value(enumToString(distance761_900), getDefaultPrice_M3(distance761_900, preset, Nature::special)).toDouble();
        m_prixM3_901PLUS = settings.value(enumToString(distance901PLUS), getDefaultPrice_M3(distance901PLUS, preset, Nature::special)).toDouble();
    }
    settings.endGroup();
}


double Tarification::getPrixM3Route(PriceKey&& key)
{
    switch (key)
    {
    case distance150_400: return m_prixM3_150_400;
    case distance401_600: return m_prixM3_401_600;
    case distance601_760: return m_prixM3_601_760;
    case distance761_900: return m_prixM3_761_900;
    case distance901PLUS: return m_prixM3_901PLUS;
    }
}


double Tarification::getPrixM3Urbain(Prestation&& prestation)
{
    switch (prestation)
    {
    case Prestation::eco: return m_prixM3_eco;
    case Prestation::ecoPlus: return m_prixM3_ecoPlus;
    case Prestation::standard: return m_prixM3_standard;
    case Prestation::luxe: return m_prixM3_luxe;
    }
}


void Tarification::setPrixM3Route(PriceKey&& key, double price)
{
    switch (key)
    {
    case distance150_400: m_prixM3_150_400 = price;
        break;
    case distance401_600: m_prixM3_401_600 = price;
        break;
    case distance601_760: m_prixM3_601_760 = price;
        break;
    case distance761_900: m_prixM3_761_900 = price;
        break;
    case distance901PLUS: m_prixM3_901PLUS = price;
    }
}


void Tarification::setPrixM3Urbain(Prestation&& prestation, double price)
{
    switch (prestation)
    {
    case Prestation::eco: m_prixM3_eco = price;
        break;
    case Prestation::ecoPlus: m_prixM3_ecoPlus = price;
        break;
    case Prestation::standard: m_prixM3_standard = price;
        break;
    case Prestation::luxe: m_prixM3_luxe = price;
    }
}
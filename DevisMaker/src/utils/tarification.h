#ifndef TARIFICATION_H
#define TARIFICATION_H
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <array>
#include <cmath>
#include <QMetaEnum>
#include "utils/common.h"
#include "utils/constants.h"
#include "utils/FileManager.h"

namespace Postes_DefaultPrices
{
    namespace BasseSaison
    {
        constexpr double CAMION{ 75.0 };
        constexpr double KILOMETRAGE{ 1.3 };
        constexpr double EMBALLAGE{ 5.0 };
        constexpr double LOC_MATERIEL{ 0.5 };
        constexpr double FRAIS_ROUTE{ 65.0 };
        constexpr double MAIN_OEUVRE{ 220.0 };
        constexpr double STATIONNEMENT{ 50.0 };
        constexpr double MONTE_MEUBLES{ 250.0 };
        constexpr double SUPP_ADRESSE{ 75.0 };
    };

    namespace HauteSaison
    {
        constexpr double CAMION{ 100.0 };
        constexpr double KILOMETRAGE{ 1.3 };
        constexpr double EMBALLAGE{ 5.0 };
        constexpr double LOC_MATERIEL{ 0.5 };
        constexpr double FRAIS_ROUTE{ 65.0 };
        constexpr double MAIN_OEUVRE{ 300.0 };
        constexpr double STATIONNEMENT{ 50.0 };
        constexpr double MONTE_MEUBLES{ 300.0 };
        constexpr double SUPP_ADRESSE{ 100.0 };
    };
};


namespace M3_DefaultPrices
{
    namespace Route
    {
        namespace BasseSaison
        {
            constexpr double DISTANCE_150_400{ 65.0 }; // Prix M3 entre 150 et 400 kms
            constexpr double DISTANCE_401_600{ 75.0 }; // Prix M3 supérieur à 400 kms et inférieur ou égale à 600 kms
            constexpr double DISTANCE_601_760{ 80.0 }; // Prix M3 supérieur à 600 kms et inférieur ou égale à 760 kms
            constexpr double DISTANCE_761_900{ 100.0 }; // Prix M3 supérieur à 760 kms et inférieur ou égale à 900 kms
            constexpr double DISTANCE_901PLUS{ 120.0 }; // Prix M3 supérieur à 900 kms
        };

        namespace HauteSaison // +15% par rapport au prix basse-saison
        {
            constexpr double DISTANCE_150_400{ 74.75 };
            constexpr double DISTANCE_401_600{ 86.25 };
            constexpr double DISTANCE_601_760{ 97.75 };
            constexpr double DISTANCE_761_900{ 126.5 };
            constexpr double DISTANCE_901PLUS{ 149.5 };
        };
    };

    namespace Urbain
    {
        namespace BasseSaison
        {
            constexpr double ECO{ 30.0 };
            constexpr double ECOPLUS{ 35.0 };
            constexpr double STANDARD{ 40.0 };
            constexpr double LUXE{ 50.0 };
        };

        namespace HauteSaison // +20% par rapport au prix basse-saison
        {
            constexpr double ECO{ 36.0 };
            constexpr double ECOPLUS{ 42.0 };
            constexpr double STANDARD{ 48.0 };
            constexpr double LUXE{ 60.0 };
        };
    };
};

class Tarification : public QObject
{
    Q_OBJECT

public:

    Tarification(QObject* parent = nullptr)
        : QObject{parent}
    {
        loadSettings(PricePreset::BasseSaison);
    }

    enum class PriceCalculation
    {
        m3,
        postes,
    };

    enum PriceKey
    {
        CoutCamion,
        CoutKilometrique,
        CoutEmballage,
        CoutLocMateriel,
        CoutFraisRoute,
        CoutMainOeuvre,
        CoutFraisStationnement,
        CoutMonteMeubles,
        CoutSupplementAdresse,

        distance150_400,
        distance401_600,
        distance601_760,
        distance761_900,
        distance901PLUS,

        m3Eco,
        m3EcoPlus,
        m3Standard,
        m3Luxe,

        max_PriceKey
    };
    Q_ENUM(PriceKey)
    
    double getCoutCamion() const { return m_coutCamion; }
    double getCoutKilometrique() const { return m_coutKilometrique; }
    double getCoutEmballage() const { return m_coutEmballage; }
    double getPrixLocMateriel() const { return m_prixLocMateriel; }
    double getFraisRoute() const { return m_fraisRoute; }
    double getCoutMO() const { return m_coutMO; }
    double getCoutFraisStationnement() const { return m_fraisStationnement; }
    double getPrixMetreCube() const { return m_prixMetreCube; }
    double getCoutMonteMeubles() const { return m_prixMonteMeubles; }
    double getPrixSuppAdresse() const { return m_prixSuppAdresse; }
    double getPrixM3Route(PriceKey key);
    double getPrixM3Urbain(Prestation prestation);

    
    void setCoutCamion(double coutCamion) { m_coutCamion = coutCamion; }
    void setCoutKilometrique(double coutKilometrique) { m_coutKilometrique = coutKilometrique; }
    void setCoutEmballage(double coutEmballage) { m_coutEmballage = coutEmballage; }
    void setPrixLocMateriel(double prixLocMateriel) { m_prixLocMateriel = prixLocMateriel; }
    void setFraisRoute(double fraisRoute) { m_fraisRoute = fraisRoute; }
    void setCoutMO(double coutMO) { m_coutMO = coutMO; }
    void setCoutFraisStationnement(double fraisStationnement) { m_fraisStationnement = fraisStationnement; }
    void setPrixMetreCube(double prixM3) { m_prixMetreCube = prixM3; }
    void setCoutMonteMeubles(double prixMM) { m_prixMonteMeubles = prixMM; }
    void setPrixSuppAdresse(double prixSuppAdresse) { m_prixSuppAdresse = prixSuppAdresse; }

    void setPrixM3Route(PriceKey key, double price);
    void setPrixM3Urbain(Prestation prestation, double price);

    /*
    Charger les paramètres depuis le fichier.ini
    OU
    Créer un nouveau fichier .ini avec les paramètres par défaut si celui-ci n'existe pas
    */
    void loadSettings(PricePreset preset);


    // Sauvegarde des paramètres de tarification dans un fichier .ini
    void saveSettings(PricePreset preset, PriceCalculation priceCalculation) const;


private:

    double m_coutCamion{Postes_DefaultPrices::BasseSaison::CAMION}; // Coût unitaire par camion H.T.
    double m_coutKilometrique{ Postes_DefaultPrices::BasseSaison::KILOMETRAGE }; // Coût unitaire par kilomètre H.T.
    double m_coutEmballage{ Postes_DefaultPrices::BasseSaison::EMBALLAGE }; // Coût unitaire emballage par m3 H.T.
    double m_prixLocMateriel{ Postes_DefaultPrices::BasseSaison::LOC_MATERIEL}; // Coût unitaire par m3 H.T.
    double m_fraisRoute{ Postes_DefaultPrices::BasseSaison::FRAIS_ROUTE}; // Coût frais de route par personnel H.T.
    double m_coutMO{ Postes_DefaultPrices::BasseSaison::MAIN_OEUVRE}; // Coût unitaire main d'oeuvre H.T.
    double m_fraisStationnement{ Postes_DefaultPrices::BasseSaison::STATIONNEMENT}; // Coût frais stationnement par adresse H.T.
    double m_prixMetreCube{}; // Prix du m3
    double m_prixMonteMeubles{ Postes_DefaultPrices::BasseSaison::MONTE_MEUBLES }; // Prix d'un monte-meuble en demi-journée PAR adresse
    double m_prixSuppAdresse{ Postes_DefaultPrices::BasseSaison::SUPP_ADRESSE }; // Prix supplément PAR adresse

    double m_prixM3_150_400{ M3_DefaultPrices::Route::BasseSaison::DISTANCE_150_400 };
    double m_prixM3_401_600{ M3_DefaultPrices::Route::BasseSaison::DISTANCE_401_600 };
    double m_prixM3_601_760{ M3_DefaultPrices::Route::BasseSaison::DISTANCE_601_760 };
    double m_prixM3_761_900{ M3_DefaultPrices::Route::BasseSaison::DISTANCE_761_900 };
    double m_prixM3_901PLUS{ M3_DefaultPrices::Route::BasseSaison::DISTANCE_901PLUS };

    double m_prixM3_eco{ M3_DefaultPrices::Urbain::BasseSaison::ECO };
    double m_prixM3_ecoPlus{ M3_DefaultPrices::Urbain::BasseSaison::ECOPLUS };
    double m_prixM3_standard{ M3_DefaultPrices::Urbain::BasseSaison::STANDARD };
    double m_prixM3_luxe{ M3_DefaultPrices::Urbain::BasseSaison::LUXE };

    const QString CONFIG_FILE_PATH{ FileManager::getDataPath() + "/PricePresets_config.ini"};
    const QString CONFIG_SECTION_M3_PREFIX{ "m3" };
    const QString CONFIG_SECTION_POSTES_PREFIX{ "POSTES" };
    const QString CONFIG_SECTION_BASSE_SAISON_SUFFIX{ "Basse_Saison" };
    const QString CONFIG_SECTION_HAUTE_SAISON_SUFFIX{ "Haute_Saison" };

    void loadDefaultValues(PricePreset preset, PriceCalculation priceCalculation);

    double getDefaultPrice_5Postes(PriceKey key, PricePreset preset) const;
    double getDefaultPrice_M3(PriceKey key, PricePreset preset, Nature nature, Prestation prestation = Prestation::eco) const;

    void loadSettings_5Postes(QSettings& settings, PricePreset preset);
    void loadSettings_M3(QSettings& settings, PricePreset preset, Nature nature);

    QString enumToString(const PriceKey& key) const { return QMetaEnum::fromType<PriceKey>().valueToKey(key); }
};

#endif
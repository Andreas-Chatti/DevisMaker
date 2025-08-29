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


class Tarification : public QObject
{
    Q_OBJECT

public:

    Tarification(QObject* parent = nullptr)
        : QObject{parent}
        , m_coutCamion{}
        , m_coutKilometrique{}
        , m_coutEmballage{}
        , m_prixLocMateriel{}
        , m_fraisRoute{}
        , m_coutMO{}
        , m_fraisStationnement{}
        , m_prixMetreCube{}
        , m_prixMonteMeubles{}
        , m_prixSuppAdresse{}
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

    struct Postes_DefaultPrices
    {
        struct BasseSaison
        {
            static constexpr double CAMION{ 75.0 };
            static constexpr double KILOMETRAGE{ 1.3 };
            static constexpr double EMBALLAGE{ 5.0 };
            static constexpr double LOC_MATERIEL{ 0.5 };
            static constexpr double FRAIS_ROUTE{ 65.0 };
            static constexpr double MAIN_OEUVRE{ 220.0 };
            static constexpr double STATIONNEMENT{ 50.0 };
            static constexpr double MONTE_MEUBLES{ 250.0 };
            static constexpr double SUPP_ADRESSE{ 75.0 };
        };

        struct HauteSaison
        {
            static constexpr double CAMION{ 100.0 };
            static constexpr double KILOMETRAGE{ 1.3 };
            static constexpr double EMBALLAGE{ 5.0 };
            static constexpr double LOC_MATERIEL{ 0.5 };
            static constexpr double FRAIS_ROUTE{ 65.0 };
            static constexpr double MAIN_OEUVRE{ 300.0 };
            static constexpr double STATIONNEMENT{ 50.0 };
            static constexpr double MONTE_MEUBLES{ 300.0 };
            static constexpr double SUPP_ADRESSE{ 100.0 };
        };
    };


    struct M3_DefaultPrices
    {
        struct Route
        {
            struct BasseSaison
            {
                static constexpr double DISTANCE_150_400{ 65.0 }; // Prix M3 entre 150 et 400 kms
                static constexpr double DISTANCE_401_600{ 75.0 }; // Prix M3 supérieur à 400 kms et inférieur ou égale à 600 kms
                static constexpr double DISTANCE_601_760{ 80.0 }; // Prix M3 supérieur à 600 kms et inférieur ou égale à 760 kms
                static constexpr double DISTANCE_761_900{ 100.0 }; // Prix M3 supérieur à 760 kms et inférieur ou égale à 900 kms
                static constexpr double DISTANCE_901PLUS{ 120.0 }; // Prix M3 supérieur à 900 kms
            };

            struct HauteSaison // +15% par rapport au prix basse-saison
            {
                static constexpr double DISTANCE_150_400{ 74.75 };
                static constexpr double DISTANCE_401_600{ 86.25 };
                static constexpr double DISTANCE_601_760{ 97.75 };
                static constexpr double DISTANCE_761_900{ 126.5 };
                static constexpr double DISTANCE_901PLUS{ 149.5 };
            };
        };

        struct Urbain
        {
            struct BasseSaison
            {
                static constexpr double ECO{ 30.0 };
                static constexpr double ECOPLUS{ 35.0 };
                static constexpr double STANDARD{ 40.0 };
                static constexpr double LUXE{ 50.0 };
            };

            struct HauteSaison // +20% par rapport au prix basse-saison
            {
                static constexpr double ECO{ 36.0 };
                static constexpr double ECOPLUS{ 42.0 };
                static constexpr double STANDARD{ 48.0 };
                static constexpr double LUXE{ 60.0 };
            };
        };
    };

    static constexpr double ASSURANCE_VALUE_PER_M3{ 500.0 };

    
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

    double getPrixM3Route(PriceKey&& key);
    double getPrixM3Urbain(Prestation&& prestation);

    
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

    void setPrixM3Route(PriceKey&& key, double price);
    void setPrixM3Urbain(Prestation&& prestation, double price);

    /*
    Charger les paramètres depuis le fichier.ini
    OU
    Créer un nouveau fichier .ini avec les paramètres par défaut si celui-ci n'existe pas
    */
    void loadSettings(PricePreset preset);


    // Sauvegarde des paramètres de tarification dans un fichier .ini
    void saveSettings(PricePreset preset, PriceCalculation priceCalculation) const;


private:

    double m_coutCamion; // Coût unitaire par camion H.T.
    double m_coutKilometrique; // Coût unitaire par kilomètre H.T.
    double m_coutEmballage; // Coût unitaire emballage par m3 H.T.
    double m_prixLocMateriel; // Coût unitaire par m3 H.T.
    double m_fraisRoute; // Coût frais de route par personnel H.T.
    double m_coutMO; // Coût unitaire main d'oeuvre H.T.
    double m_fraisStationnement; // Coût frais stationnement par adresse H.T.
    double m_prixMetreCube; // Prix du m3
    double m_prixMonteMeubles; // Prix d'un monte-meuble en demi-journée PAR adresse
    double m_prixSuppAdresse; // Prix supplément PAR adresse

    double m_prixM3_150_400;
    double m_prixM3_401_600;
    double m_prixM3_601_760;
    double m_prixM3_761_900;
    double m_prixM3_901PLUS;

    double m_prixM3_eco;
    double m_prixM3_ecoPlus;
    double m_prixM3_standard;
    double m_prixM3_luxe;

    const QString CONFIG_FILE_PATH{ SettingsConstants::FileSettings::DATA_FILE_PATH + "/PricePresets_config.ini" };
    const QString CONFIG_SECTION_M3_PREFIX{ "m3" };
    const QString CONFIG_SECTION_POSTES_PREFIX{ "POSTES" };
    const QString CONFIG_SECTION_BASSE_SAISON_SUFFIX{ "Basse_Saison" };
    const QString CONFIG_SECTION_HAUTE_SAISON_SUFFIX{ "Haute_Saison" };


    void loadDefaultValues(PricePreset preset, PriceCalculation priceCalculation);

    double getDefaultPrice_5Postes(PriceKey key, PricePreset preset) const;
    double getDefaultPrice_M3(PriceKey key, PricePreset preset, Nature&& nature, Prestation&& prestation = Prestation::eco) const;

    void loadSettings_5Postes(QSettings& settings, PricePreset preset);
    void loadSettings_M3(QSettings& settings, PricePreset preset, Nature&& nature);

    QString enumToString(const PriceKey& key) const { return QMetaEnum::fromType<PriceKey>().valueToKey(key); }
};

#endif
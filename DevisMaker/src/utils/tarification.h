#ifndef TARIFICATION_H
#define TARIFICATION_H
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <array>
#include <cmath>
#include "utils/common.h"
#include "utils/constants.h"


class Tarification 
{

public:

    Tarification()
        : m_coutCamion{}
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



        max_PriceKey
    };


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

            struct HauteSaison
            {
                static constexpr double DISTANCE_150_400{ 70.0 };
                static constexpr double DISTANCE_401_600{ 80.0 };
                static constexpr double DISTANCE_601_760{ 85.0 };
                static constexpr double DISTANCE_761_900{ 110.0 };
                static constexpr double DISTANCE_901PLUS{ 130.0 };
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

            struct HauteSaison
            {
                static constexpr double ECO{ 40.0 };
                static constexpr double ECOPLUS{ 50.0 };
                static constexpr double STANDARD{ 60.0 };
                static constexpr double LUXE{ 65.0 };
            };
        };
    };

    
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


    /*
    Charger les paramètres depuis le fichier.ini
    OU
    Créer un nouveau fichier .ini avec les paramètres par défaut si celui-ci n'existe pas
    */
    void loadSettings(PricePreset preset);


    // Sauvegarde des paramètres de tarification dans un fichier .ini
    void saveSettings(PricePreset preset) const;


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


    void loadDefaultValues(PricePreset preset);


    double getDefaultPrices_5Postes(PriceKey key, PricePreset preset) const;
};

#endif
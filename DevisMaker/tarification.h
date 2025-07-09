#ifndef TARIFICATION_H
#define TARIFICATION_H
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <array>
#include <cmath>
#include "common.h"
#include "presetsPrices.h"


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
        , m_prixDechetterie{}
        , m_prixSuppAdresse{}
    {
        loadSettings(PricePreset::BasseSaison);
    }

    
    double getCoutCamion() const { return m_coutCamion; }
    double getCoutKilometrique() const { return m_coutKilometrique; }
    double getCoutEmballage() const { return m_coutEmballage; }
    double getPrixLocMateriel() const { return m_prixLocMateriel; }
    double getFraisRoute() const { return m_fraisRoute; }
    double getCoutMO() const { return m_coutMO; }
    double getCoutFraisStationnement() const { return m_fraisStationnement; }
    double getPrixMetreCube() const { return m_prixMetreCube; }
    double getCoutMonteMeubles() const { return m_prixMonteMeubles; }
    double getPrixDechetterie() const { return m_prixDechetterie; }
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
    void setPrixDechetterie(double prixDechetterie) { m_prixDechetterie = prixDechetterie; }
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
    double m_prixDechetterie; // Prix mise en déchetterie
    double m_prixSuppAdresse; // Prix supplément PAR adresse


    double getDefaultValue(const QString& key, PricePreset preset) const;


    void loadDefaultValues(PricePreset preset);

};

#endif
#ifndef TARIFICATION_H
#define TARIFICATION_H
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <array>
#include <cmath>
#include "common.h"


class Tarification 
{

public:

    Tarification()
        : m_coutCamion{ 75.0 }
        , m_coutKilometrique{ 1.3 }
        , m_coutEmballage{ 5.0 }
        , m_prixLocMateriel{ 0.5 }
        , m_fraisRoute{ 65.0 }
        , m_coutMO{ 220.0 }
        , m_fraisStationnement{ 50.0 }
        , m_prixMetreCube{}
        , m_prixMonteMeubles{ 250.0 }
        , m_prixDechetterie{ 200.0 }
        , m_prixSuppAdresse{ 75.0 }
    {
        loadSettings();
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
    void setPrixMetreCube(Prestation prestation, Nature nature, double distance);
    void setCoutMonteMeubles(double prixMM) { m_prixMonteMeubles = prixMM; }
    void setPrixDechetterie(double prixDechetterie) { m_prixDechetterie = prixDechetterie; }
    void setPrixSuppAdresse(double prixSuppAdresse) { m_prixSuppAdresse = prixSuppAdresse; }


    // Méthodes de calcul individuelles

    double calculerVolumeParPersonne(double volume, Prestation prestation) const;

    int calculerNombreCamion(double volume, Prestation prestation, Nature nature, double distance, bool accesComplexe = false, bool montageImportant = false) const;

    int calculerNombreMO(double volume, Prestation prestation, Nature nature, int nombreCamions, bool monteMeuble, bool ascenseur, double distance) const;

    double calculerCoutTotalHT(double volume, double coutAssurance, double fraisStationnement, double fraisMonteMeubles, double prixDechetterie, double fraisRoute, double suppAdresse) const 
    { return volume * m_prixMetreCube + coutAssurance + fraisStationnement + fraisMonteMeubles + prixDechetterie + fraisRoute + suppAdresse; }

    double calculerCoutCamionTotal(int nombreCamion) const { return m_coutCamion * nombreCamion; }

    double calculerCoutKilometrageTotal(double distance) const { return distance * m_coutKilometrique; }

    double calculerCoutMainOeuvreTotal(int nombreMO) const { return nombreMO * m_coutMO; }

    double calculerCoutLocMaterielTotal(double volume) const { return volume * m_prixLocMateriel; }

    double calculerCoutFraisRouteTotal(int nombreCamion) const { return (m_fraisRoute * (nombreCamion - 1)) * 2; }

    double calculerArrhes(double coutTotal) const { return (coutTotal * 1.2) * 0.3; }

    double calculerCoutAssurance(int valeurMobilier, TypeAssurance assurance) const;

    double calculerPrixStationnement(bool autStatChargement, bool autStatLivraison) const;

    double calculerSupplementMM(const Adresse& aChargement, const Adresse& aLivraison) const;

    double calculerSuppAdresse(double nbAdresse) { return m_prixSuppAdresse * nbAdresse; }


    // Sauvegarde des paramètres de tarification dans un fichier .ini
    void saveSettings() const;


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


    /*
    Charger les paramètres depuis le fichier.ini
    OU
    Créer un nouveau fichier .ini avec les paramètres par défaut si celui-ci n'existe pas
    */
    void loadSettings();
};

#endif
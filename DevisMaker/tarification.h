#ifndef TARIFICATION_H
#define TARIFICATION_H
#include "common.h"

class Tarification 
{

public:

    Tarification()
        : m_coutCamion{ 75.0 }
        , m_coutKilometrique{ 1.3 }
        , m_coutEmballage{ 5.0 }
        , m_prixTraction{ 100.0 }
        , m_prixLocMateriel{ 0.5 }
        , m_fraisRoute{ 65.0 }
        , m_coutMO{ 220.0 }
        , m_fraisStationnement{ 50.0 }
        , m_prixMetreCube{}
    {
    }

    
    double getCoutCamion() const { return m_coutCamion; }
    double getCoutKilometrique() const { return m_coutKilometrique; }
    double getCoutEmballage() const { return m_coutEmballage; }
    double getPrixTraction() const { return m_prixTraction; }
    double getPrixLocMateriel() const { return m_prixLocMateriel; }
    double getFraisRoute() const { return m_fraisRoute; }
    double getCoutMO() const { return m_coutMO; }
    double getCoutFraisStationnement() const { return m_fraisStationnement; }
    double getPrixMetreCube() const { return m_prixMetreCube; }

    
    void setCoutCamion(double coutCamion) { m_coutCamion = coutCamion; }
    void setCoutKilometrique(double coutKilometrique) { m_coutKilometrique = coutKilometrique; }
    void setCoutEmballage(double coutEmballage) { m_coutEmballage = coutEmballage; }
    void setPrixTraction(double traction) { m_prixTraction = traction; }
    void setPrixLocMateriel(double prixLocMateriel) { m_prixLocMateriel = prixLocMateriel; }
    void setFraisRoute(double fraisRoute) { m_fraisRoute = fraisRoute; }
    void setCoutMO(double coutMO) { m_coutMO = coutMO; }
    void setCoutFraisStationnement(double fraisStationnement) { m_fraisStationnement = fraisStationnement; }
    void setPrixMetreCube(Prestation prestation, Nature nature, double distance);


    double calculerVolumeParPersonne(double volume, Prestation prestation) const;

    int calculerNombreCamion(double volume, Prestation prestation, Nature nature, double distance, bool accesComplexe = false, bool montageImportant = false) const;

    int calculerNombreMO(double volume, Prestation prestation, Nature nature, int nombreCamions, bool monteMeuble, bool ascenseur, double distance) const;

    double calculerCoutTotalHT(double volume, double coutAssurance, double fraisStationnement) const { return volume * m_prixMetreCube + coutAssurance + fraisStationnement; }

    double calculerCoutCamionTotal(int nombreCamion) const { return m_coutCamion * nombreCamion; }

    double calculerCoutKilometrageTotal(double distance) const { return distance * m_coutKilometrique; }

    double calculerCoutMainOeuvreTotal(int nombreMO) const { return nombreMO * m_coutMO; }

    double calculerCoutLocMaterielTotal(double volume) const { return volume * m_prixLocMateriel; }

    double calculerCoutFraisRouteTotal(int nombreMO, int nombreCamion) const { return (nombreMO * m_fraisRoute) * (nombreCamion - 1); }

    double calculerArrhes(double coutTotal) const { return (coutTotal * 1.2) * 0.3; }

    double calculerCoutAssurance(int valeurMobilier, TypeAssurance assurance) const 
    { 
        const double taux{ assurance == TypeAssurance::contractuelle ? 0.2 : 0.5 };

        return (valeurMobilier * taux) / 100;
    }

    double calculerPrixStationnement(bool autStatChargement, bool autStatLivraison) const
    {
        if (autStatChargement && autStatLivraison)
            return m_fraisStationnement * 2;

        else if (autStatChargement || autStatLivraison)
            return m_fraisStationnement;

        return 0.0;
    }

private:

    double m_coutCamion; // Coût unitaire par camion H.T.
    double m_coutKilometrique; // Coût unitaire par kilomètre H.T.
    double m_coutEmballage; // Coût unitaire emballage par m3 H.T.
    double m_prixTraction; // Prix traction H.T.
    double m_prixLocMateriel; // Coût unitaire par m3 H.T.
    double m_fraisRoute; // Coût frais de route par personnel H.T.
    double m_coutMO; // Coût unitaire main d'oeuvre H.T.
    double m_fraisStationnement; // Coût frais stationnement par adresse H.T.
    double m_prixMetreCube; // Prix du m3
};

#endif
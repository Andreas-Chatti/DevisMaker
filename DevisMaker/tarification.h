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

    
    void setCoutCamion(double coutCamion) { m_coutCamion = coutCamion; }
    void setCoutKilometrique(double coutKilometrique) { m_coutKilometrique = coutKilometrique; }
    void setCoutEmballage(double coutEmballage) { m_coutEmballage = coutEmballage; }
    void setPrixTraction(double traction) { m_prixTraction = traction; }
    void setPrixLocMateriel(double prixLocMateriel) { m_prixLocMateriel = prixLocMateriel; }
    void setFraisRoute(double fraisRoute) { m_fraisRoute = fraisRoute; }
    void setCoutMO(double coutMO) { m_coutMO = coutMO; }
    void setCoutFraisStationnement(double fraisStationnement) { m_fraisStationnement = fraisStationnement; }

    
    double calculerHeuresNecessaires(double volume) const;
    double calculerCoutMainOeuvre(double volume) const;
    double calculerCoutTransport(double distance) const;


    double calculerVolumeParPersonne(double volume, Prestation prestation) const;


    // Calcule le nombre de jours nécessaires pour le déménagement
    int calculerNombreCamion(double volume, Prestation prestation, Nature nature, double distance, bool accesComplexe = false, bool montageImportant = false) const;

private:

    double m_coutCamion;
    double m_coutKilometrique;
    double m_coutEmballage;
    double m_prixTraction;
    double m_prixLocMateriel;
    double m_fraisRoute;
    double m_coutMO;
    double m_fraisStationnement;
};

#endif
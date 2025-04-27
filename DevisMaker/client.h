#pragma once
#include <string>
#include "common.h"

class Client 
{

public:

    Client() = default;

   
    std::string getNom() const { return m_nom; }
    const Adresse& getAdresseDepart() const { return m_depart; }
    const Adresse& getAdresseArrivee() const { return m_livraison; }
    double getDistance() const { return m_distance; }

 
    void setNom(const std::string& nom) { m_nom = nom; }
    void setPrenom(const std::string& prenom) { m_prenom = prenom; }
    void setAdresseDepart(const Adresse& adresse) { m_depart = adresse; }
    void setAdresseArrivee(const Adresse& adresse) { m_livraison = adresse; }
    void setDistance(double distance) { m_distance = distance; }
    void setPrestation(Prestation prestation) { m_prestation = prestation; }
    void setNature(Nature nature) { m_nature = nature; }

    
    //void calculerDistance();

private:

    std::string m_nom;
    std::string m_prenom;
    Adresse m_depart;
    Adresse m_livraison;
    Prestation m_prestation;
    Nature m_nature;
    double m_distance;
};
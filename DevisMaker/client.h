#pragma once
#include <string>
#include "common.h"

class Client 
{

public:

    Client() = default;

    // Getters
    std::string getNom() const { return m_nom; }
    const Adresse& getAdresseDepart() const { return m_depart; }
    const Adresse& getAdresseArrivee() const { return m_livraison; }
    double getDistance() const { return m_distance; }

    // Setters
    void setNom(const std::string& nom) { m_nom = nom; }
    void setPrenom(std::string_view prenom) { m_prenom = prenom; }
    void setAdresseDepart(const std::string& adresse) { m_depart.m_rue = adresse; }
    void setAdresseArrivee(const std::string& adresse) { m_livraison.m_rue = adresse; }

    // Méthodes
    //void calculerDistance();

private:

    std::string m_nom;
    std::string m_prenom;
    Adresse m_depart;
    Adresse m_livraison;
    double m_distance;
};
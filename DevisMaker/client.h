#pragma once

#include <string>

class Client 
{

public:

    Client() = default;

    // Getters
    std::string getNom() const { return m_nom; }
    std::string getAdresseDepart() const { return m_adresseDepart; }
    std::string getAdresseArrivee() const { return m_adresseArrivee; }
    double getDistance() const { return m_distance; }

    // Setters
    void setNom(const std::string& nom) { m_nom = nom; }
    void setAdresseDepart(const std::string& adresse) { m_adresseDepart = adresse; }
    void setAdresseArrivee(const std::string& adresse) { m_adresseArrivee = adresse; }

    // Méthodes
    void calculerDistance();

private:

    std::string m_nom;
    std::string m_adresseDepart;
    std::string m_adresseArrivee;
    double m_distance;
};
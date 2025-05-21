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
    const Prestation& getPrestation() const { return m_prestation; }
    const Nature& getNature() const { return m_nature; }
    const TypeAssurance& getTypeAssurance() const { return m_typeAssurance; }
    double getDistance() const { return m_distance; }
    double getVolume() const { return m_volume; }
    double getValeurAssurance() const { return m_valeurAssurance; }
    bool getIsDE() const { return m_isDE; }

 
    void setNom(const std::string& nom) { m_nom = nom; }
    void setPrenom(const std::string& prenom) { m_prenom = prenom; }
    void setAdresseDepart(const Adresse& adresse) { m_depart = adresse; }
    void setAdresseArrivee(const Adresse& adresse) { m_livraison = adresse; }
    void setDistance(double distance) { m_distance = distance; }
    void setPrestation(Prestation prestation) { m_prestation = prestation; }
    void setTypeAssurance(TypeAssurance type) { m_typeAssurance = type; }
    void setValeurAssurance(double valeur) { m_valeurAssurance = valeur; }
    void setNature(Nature nature) { m_nature = nature; }
    void setVolume(double volume) { m_volume = volume; }
    void setIsDE(bool isDE) { m_isDE = isDE; }

    
    //void calculerDistance();

private:

    std::string m_nom;
    std::string m_prenom;
    Adresse m_depart;
    Adresse m_livraison;
    Prestation m_prestation;
    Nature m_nature;
    TypeAssurance m_typeAssurance;
    double m_valeurAssurance;
    double m_distance;
    double m_volume;
    bool m_isDE;
};
#pragma once
#include <QString>
#include <string>
#include "common.h"

class Client 
{

public:

    Client() = default;

   
    const QString& getNom() const { return m_nom; }
    const Adresse& getAdresseDepart() const { return m_depart; }
    const Adresse& getAdresseArrivee() const { return m_livraison; }
    const Prestation& getPrestation() const { return m_prestation; }
    const Nature& getNature() const { return m_nature; }
    const TypeAssurance& getTypeAssurance() const { return m_typeAssurance; }
    double getDistance() const { return m_distance; }
    double getVolume() const { return m_volume; }
    double getValeurAssurance() const { return m_valeurAssurance; }
    const QString& getNumTel() const { return m_tel; }
    bool getIsDE() const { return m_isDE; }
    int getNbAdresseSupp() const { return m_nbAdresseSupp; }

 
    void setNom(const QString& nom) { m_nom = nom; }
    void setPrenom(const QString& prenom) { m_prenom = prenom; }
    void setAdresseDepart(const Adresse& adresse) { m_depart = adresse; }
    void setAdresseArrivee(const Adresse& adresse) { m_livraison = adresse; }
    void setDistance(double distance) { m_distance = distance; }
    void setPrestation(Prestation prestation) { m_prestation = prestation; }
    void setTypeAssurance(TypeAssurance type) { m_typeAssurance = type; }
    void setValeurAssurance(double valeur) { m_valeurAssurance = valeur; }
    void setNature(Nature nature) { m_nature = nature; }
    void setVolume(double volume) { m_volume = volume; }
    void setIsDE(bool isDE) { m_isDE = isDE; }
    void setNumTel(QString tel) { m_tel = tel; }
    void setNbAdresseSupp(int nbAdresseSupp) { m_nbAdresseSupp = nbAdresseSupp; }


private:

    QString m_nom;
    QString m_prenom;
    Adresse m_depart;
    Adresse m_livraison;
    Prestation m_prestation;
    Nature m_nature;
    TypeAssurance m_typeAssurance;
    double m_valeurAssurance;
    double m_distance;
    double m_volume;
    bool m_isDE;
    int m_nbAdresseSupp;
    QString m_tel;
};
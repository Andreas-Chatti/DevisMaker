#pragma once
#include <QString>
#include <string>
#include "utils/common.h"

class Client 
{

public:

    Client()
        : m_nom{}
        , m_prenom{}
        , m_depart{}
        , m_livraison{}
        , m_prestation{}
        , m_nature{}
        , m_typeAssurance{}
        , m_valeurAssurance{}
        , m_distance{}
        , m_volume{}
        , m_nbAdresseSupp{}
        , m_tel{}
    {
    }

   
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
    int m_nbAdresseSupp;
    QString m_tel;
};
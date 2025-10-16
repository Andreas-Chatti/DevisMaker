#pragma once
#include <QString>
#include <QObject>
#include "utils/common.h"
#include "inventory/inventaire.h"

class Client : public QObject
{
    Q_OBJECT

public:

    Client(QObject* parent = nullptr)
        : QObject{ parent }
    {
    }

    ~Client() = default;
   
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
    const Inventory* const getInventory() const { return m_inventory; }

 
    void setNom(QString nom) { m_nom = std::move(nom); }
    void setPrenom(QString prenom) { m_prenom = std::move(prenom); }
    void setAdresseDepart(Adresse adresse) { m_depart = std::move(adresse); }
    void setAdresseArrivee(Adresse adresse) { m_livraison = std::move(adresse); }
    void setDistance(double distance) { m_distance = distance; }
    void setPrestation(Prestation prestation) { m_prestation = prestation; }
    void setTypeAssurance(TypeAssurance type) { m_typeAssurance = type; }
    void setValeurAssurance(double valeur) { m_valeurAssurance = valeur; }
    void setNature(Nature nature) { m_nature = nature; }
    void setVolume(double volume) { m_volume = volume; }
    void setNumTel(QString tel) { m_tel = std::move(tel); }
    void setNbAdresseSupp(int nbAdresseSupp) { m_nbAdresseSupp = nbAdresseSupp; }


private:

    QString m_nom{"UNDEFINED"};
    QString m_prenom{"UNDEFINED"};
    Adresse m_depart{};
    Adresse m_livraison{};
    Prestation m_prestation{ Prestation::eco };
    Nature m_nature{ Nature::urbain };
    TypeAssurance m_typeAssurance{ TypeAssurance::contractuelle };
    double m_valeurAssurance{ 0.0 };
    double m_distance{ 0.0 };
    double m_volume{ 0.0 };
    int m_nbAdresseSupp{ 0 };
    QString m_tel{ "UNDEFINED" };
    Inventory* m_inventory{ new Inventory{this} };
};
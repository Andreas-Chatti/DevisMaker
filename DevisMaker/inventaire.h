#pragma once
#include <string>
#include <map>
#include <vector>


struct ObjetDemenagement 
{
    std::string nom;
    double volumeUnitaire;
};


class Inventaire 
{

public:

    Inventaire() = default;

    double analyser(const std::string& texteInventaire);

private:

    std::map<std::string, ObjetDemenagement> m_dictionnaire;
    std::vector<ObjetDemenagement> m_objets;
    double m_volumeTotal;


    void ajouterObjet(const std::string& nom, int quantite = 1);
    void supprimerObjet(const std::string& nom, int quantite = 1);
    void viderInventaire();
    double getVolumeTotal() const { return m_volumeTotal; }

    //void chargerDictionnaire();  // Charge le dictionnaire d'objets standard
};
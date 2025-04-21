#pragma once
#include <string>
#include <map>
#include <vector>

// Structure pour définir un objet de déménagement
struct ObjetDemenagement {
    std::string nom;
    double volumeUnitaire;  // en m³
};

class Inventaire {
private:
    std::map<std::string, ObjetDemenagement> m_dictionnaire;
    std::vector<ObjetDemenagement> m_objets;
    double m_volumeTotal;

public:
    Inventaire();

    // Getters
    double getVolumeTotal() const { return m_volumeTotal; }

    // Méthodes
    void ajouterObjet(const std::string& nom, int quantite = 1);
    void supprimerObjet(const std::string& nom);
    void viderInventaire();
    double analyser(const std::string& texteInventaire);

private:
    void chargerDictionnaire();  // Charge le dictionnaire d'objets standard
};
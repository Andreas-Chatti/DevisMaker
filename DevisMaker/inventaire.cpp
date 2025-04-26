#include "Inventaire.h"
#include <sstream>
#include <algorithm>
#include <cctype>


/*void Inventaire::chargerDictionnaire() {
    // Ici, on chargera normalement depuis un fichier, mais pour simplifier
    // on ajoute quelques objets couramment déménagés directement dans le code

    m_dictionnaire["table"] = { "Table", 0.5 };
    m_dictionnaire["chaise"] = { "Chaise", 0.1 };
    m_dictionnaire["canapé"] = { "Canapé", 1.5 };
    m_dictionnaire["lit"] = { "Lit", 1.0 };
    m_dictionnaire["armoire"] = { "Armoire", 1.8 };
    m_dictionnaire["television"] = { "Television", 0.2 };
    m_dictionnaire["refrigerateur"] = { "Refrigerateur", 1.0 };
    m_dictionnaire["machine a laver"] = { "Machine a laver", 0.6 };

    // Etc...
}*/

void Inventaire::ajouterObjet(const std::string& nom, int quantite) {
    auto it = m_dictionnaire.find(nom);
    if (it != m_dictionnaire.end()) {
        for (int i = 0; i < quantite; i++) {
            m_objets.push_back(it->second);
            m_volumeTotal += it->second.volumeUnitaire;
        }
    }
}

void Inventaire::supprimerObjet(const std::string& nom) {
    auto it = std::find_if(m_objets.begin(), m_objets.end(),
        [&nom](const ObjetDemenagement& obj) { return obj.nom == nom; });

    if (it != m_objets.end()) {
        m_volumeTotal -= it->volumeUnitaire;
        m_objets.erase(it);
    }
}

void Inventaire::viderInventaire() {
    m_objets.clear();
    m_volumeTotal = 0.0;
}

double Inventaire::analyser(const std::string& texteInventaire) {
    viderInventaire();

    // Traitement basique du texte d'inventaire
    std::istringstream iss(texteInventaire);
    std::string ligne;

    while (std::getline(iss, ligne)) {
        // Analyse basique: on cherche "X nom_objet"
        std::istringstream ligneSs(ligne);
        int quantite = 1;
        std::string nomObjet;

        // Vérifier si la ligne commence par un nombre
        if (isdigit(ligne[0])) {
            ligneSs >> quantite;
        }

        // Lire le reste comme nom d'objet
        std::getline(ligneSs, nomObjet);
        // Supprimer les espaces de début
        nomObjet.erase(0, nomObjet.find_first_not_of(" \t"));

        // Convertir en minuscules pour la recherche dans le dictionnaire
        std::string nomObjetLower = nomObjet;
        std::transform(nomObjetLower.begin(), nomObjetLower.end(), nomObjetLower.begin(),
            [](unsigned char c) { return std::tolower(c); });

        ajouterObjet(nomObjetLower, quantite);
    }

    return m_volumeTotal;
}
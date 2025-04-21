#pragma once

#include "Client.h"
#include "Inventaire.h"
#include "Tarification.h"
#include <string>
#include <vector>

// Structure pour stocker les lignes du devis
struct LigneDevis {
    std::string description;
    double montantHT;
};

class Devis {
private:
    Client m_client;
    Inventaire m_inventaire;
    Tarification m_tarification;

    std::string m_numero;          // Numéro unique du devis
    std::string m_date;            // Date de création
    std::vector<LigneDevis> m_lignes; // Lignes du devis
    double m_totalHT;              // Montant total HT
    double m_tauxTVA;              // Taux de TVA (en %)
    double m_totalTTC;             // Montant total TTC

public:
    Devis(const Client& client, const Inventaire& inventaire, const Tarification& tarif);

    // Getters
    std::string getNumero() const { return m_numero; }
    std::string getDate() const { return m_date; }
    double getTotalHT() const { return m_totalHT; }
    double getTotalTTC() const { return m_totalTTC; }

    // Méthodes
    void calculer();
    bool genererPDF(const std::string& cheminFichier);
    std::string toString() const;

private:
    void genererNumero();      // Génère un numéro unique pour le devis
    std::string dateActuelle(); // Retourne la date actuelle formatée
};
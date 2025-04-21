#include "Devis.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

Devis::Devis(const Client& client, const Inventaire& inventaire, const Tarification& tarif)
    : m_client(client), m_inventaire(inventaire), m_tarification(tarif),
    m_totalHT(0.0), m_tauxTVA(20.0), m_totalTTC(0.0)
{
    genererNumero();
    m_date = dateActuelle();
}

void Devis::calculer() {
    m_lignes.clear();
    m_totalHT = 0.0;

    // Ajouter ligne pour main d'œuvre
    double volume = m_inventaire.getVolumeTotal();
    double coutMO = m_tarification.calculerCoutMainOeuvre(volume);
    m_lignes.push_back({ "Main d'œuvre", coutMO });

    // Ajouter ligne pour transport
    double distance = m_client.getDistance();
    double coutTransport = m_tarification.calculerCoutTransport(distance);
    m_lignes.push_back({ "Transport", coutTransport });

    // Ajouter ligne pour emballage
    double coutEmballage = m_tarification.getPrixForfaitaireEmballage();
    m_lignes.push_back({ "Emballage", coutEmballage });

    // Calculer total HT
    for (const auto& ligne : m_lignes) {
        m_totalHT += ligne.montantHT;
    }

    // Calculer total TTC
    m_totalTTC = m_totalHT * (1 + m_tauxTVA / 100.0);
}

bool Devis::genererPDF(const std::string& cheminFichier) {
    // À implémenter ultérieurement, par exemple avec la bibliothèque PoDoFo
    // Pour l'instant, on simule juste un succès
    std::cout << "Génération du PDF: " << cheminFichier << std::endl;
    return true;
}

std::string Devis::toString() const {
    std::ostringstream oss;

    oss << "DEVIS N° " << m_numero << "\n";
    oss << "Date: " << m_date << "\n\n";

    oss << "CLIENT:\n";
    oss << m_client.getNom() << "\n";
    oss << "De: " << m_client.getAdresseDepart() << "\n";
    oss << "À: " << m_client.getAdresseArrivee() << "\n";
    oss << "Distance: " << m_client.getDistance() << " km\n\n";

    oss << "PRESTATIONS:\n";
    for (const auto& ligne : m_lignes) {
        oss << ligne.description << ": " << ligne.montantHT << " € HT\n";
    }

    oss << "\nTotal HT: " << m_totalHT << " €\n";
    oss << "TVA (" << m_tauxTVA << "%): " << (m_totalTTC - m_totalHT) << " €\n";
    oss << "Total TTC: " << m_totalTTC << " €\n";

    return oss.str();
}

void Devis::genererNumero() {
    // Génère un numéro simple basé sur la date et l'heure
    auto now = std::time(nullptr);
    std::tm timeInfo;
    localtime_s(&timeInfo, &now);
    auto tm = timeInfo;

    std::ostringstream oss;
    oss << "DEV-" << std::put_time(&tm, "%Y%m%d-%H%M%S");
    m_numero = oss.str();
}

std::string Devis::dateActuelle() {
    auto now = std::time(nullptr);
    std::tm timeInfo;
    localtime_s(&timeInfo, &now);
    auto tm = timeInfo;

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d/%m/%Y");
    return oss.str();
}
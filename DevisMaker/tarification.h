#pragma once

class Tarification {
private:
    double m_tauxHoraireMO;     // Taux horaire de main d'œuvre en € HT
    double m_coutKilometrique;   // Coût par km en € HT
    double m_prixForfaitaireEmballage;  // Prix forfaitaire pour l'emballage en € HT
    double m_volumeHeurePersonne;  // Volume traité par heure et par personne en m³

public:
    Tarification();

    // Getters
    double getTauxHoraireMO() const { return m_tauxHoraireMO; }
    double getCoutKilometrique() const { return m_coutKilometrique; }
    double getPrixForfaitaireEmballage() const { return m_prixForfaitaireEmballage; }
    double getVolumeHeurePersonne() const { return m_volumeHeurePersonne; }

    // Setters
    void setTauxHoraireMO(double taux) { m_tauxHoraireMO = taux; }
    void setCoutKilometrique(double cout) { m_coutKilometrique = cout; }
    void setPrixForfaitaireEmballage(double prix) { m_prixForfaitaireEmballage = prix; }
    void setVolumeHeurePersonne(double volume) { m_volumeHeurePersonne = volume; }

    // Méthodes de calcul
    double calculerHeuresNecessaires(double volume) const;
    double calculerCoutMainOeuvre(double volume) const;
    double calculerCoutTransport(double distance) const;
};
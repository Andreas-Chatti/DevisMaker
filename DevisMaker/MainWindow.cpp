#include "MainWindow.h"
#include <QMessageBox>

void MainWindow::on_generateDevisButton_clicked()
{
    // 1. Mettre à jour toutes les variables de l'onglet CLIENT depuis les champs rentrés par l'utilisateur


    // Adresse Chargement
    std::string rueChargement{ ui.adresseDepartLineEdit->text().toStdString() };
    int etageChargement{ ui.etageDepartSpinBox->value() };
    bool ascChargement{ ui.ascDepartCheckBox->isChecked() };
    bool mmChargement{ ui.mmDepartCheckBox->isChecked() };
    bool autStatChargement{ ui.asDepartCheckBox->isChecked() };

    const Adresse A_Chargement{ rueChargement, etageChargement, ascChargement, mmChargement, autStatChargement };
    m_client.setAdresseDepart(A_Chargement);


    // Adresse Livraison
    std::string rueLivraison{ ui.adresseLivraisonLineEdit->text().toStdString() };
    int etageLivraison{ ui.etageLivraisonSpinBox->value() };
    bool ascLivraison{ ui.ascLivraisonCheckBox->isChecked() };
    bool mmLivraison{ ui.mmLivraisonCheckBox->isChecked() };
    bool autStatLivraison{ ui.asLivraisonCheckBox->isChecked() };

    const Adresse A_Livraison{ rueLivraison, etageLivraison, ascLivraison, mmLivraison, autStatLivraison };
    m_client.setAdresseArrivee(A_Livraison);


    // Récupérer la distance saisie
    double distance{ ui.distanceLineEdit->text().toDouble() };
    m_client.setDistance(distance);


    // Récupérer le volume saisie
    double volume{ ui.volumelineEdit->text().toDouble() };
    m_client.setVolume(volume);


    // Récupérer la prestation sélectionnée
    Prestation prestation{ ui.prestationComboBox->currentIndex() };
    m_client.setPrestation(prestation);


    // Récupérer la nature sélectionnée
    Nature nature{ ui.natureComboBox->currentIndex() };
    m_client.setNature(nature);


    // Récupérer la condition Déchetterie
    bool dechetterie{ ui.deCheckBox->isChecked() };
    m_client.setIsDE(dechetterie);


    // 1bis. Mettre à jour toutes les variables de l'onglet PARAMETRES depuis les champs rentrés par l'utilisateur


    double prixCamion{ ui.prixCamionLineEdit->text().toDouble() };
    m_tarification.setCoutCamion(prixCamion);


    double prixKilometrage{ ui.coutKmLineEdit->text().toDouble() };
    m_tarification.setCoutKilometrique(prixKilometrage);


    double prixEmballage{ ui.prixEmballageLineEdit->text().toDouble() };
    m_tarification.setCoutEmballage(prixEmballage);


    double prixTraction{ ui.prixTractionLineEdit->text().toDouble() };
    m_tarification.setPrixTraction(prixTraction);


    double prixLocMateriel{ ui.prixLocMatLineEdit->text().toDouble() };
    m_tarification.setPrixLocMateriel(prixLocMateriel);


    double prixFraisRoute{ ui.coutFraisRouteLineEdit->text().toDouble() };
    m_tarification.setFraisRoute(prixFraisRoute);


    double prixMO{ ui.moLineEdit->text().toDouble() };
    m_tarification.setCoutMO(prixMO);


    double prixFraisStationnement{ ui.fraisStatLineEdit->text().toDouble() };
    m_tarification.setCoutFraisStationnement(prixFraisStationnement);


    // 2. Faire les calculs de tarification avec les variables qu'on a récupéré


    double volumeParPersonne{ m_tarification.calculerVolumeParPersonne(volume, prestation) };
    int nombreCamion{ m_tarification.calculerNombreCamion(volume, prestation, nature, distance) };
    int nombreMO{ m_tarification.calculerNombreMO(volume, prestation, nature, nombreCamion, mmChargement || mmLivraison, ascChargement || ascLivraison, distance) };

    double coutMOTotal{ m_tarification.calculerCoutMainOeuvreTotal(nombreMO) };

    double coutCamionTotal{ m_tarification.calculerCoutCamionTotal(nombreCamion) };

    double coutAutStatTotal{ m_tarification.calculerPrixStationnement(autStatChargement, autStatLivraison) };

    double fraisRouteTotal{ m_tarification.calculerCoutFraisRouteTotal(nombreMO, nombreCamion) };

    double coutAssurance{ m_tarification.calculerCoutAssurance(30000, TypeAssurance::contractuelle) }; // A réviser plus tard (mettre en place les champs de type d'assurance voulue ainsi que champ pour la valeur mobilier à mettre)

    m_tarification.setPrixMetreCube(prestation, nature, distance);

    double prixTotalHT{ m_tarification.calculerCoutTotalHT(volume, coutAssurance, coutAutStatTotal) };

    double arrhes{ m_tarification.calculerArrhes(prixTotalHT) };


    // 3. Afficher les résultats dans l'onglet "Résultats et Devis"

    // Format pour le volume (m³)
    ui.totalVolumeTextBrowser->setText(QString::number(volume, 'f', 2) + " m\u00B3");

    // Format pour les prix H.T. (2 décimales + symbole euro + H.T.)
    ui.coutMOTextBrowser->setText(QString::number(coutMOTotal, 'f', 2) + " \u20AC H.T.");
    ui.tractionTextBrowser->setText(QString::number(prixTraction, 'f', 2) + " \u20AC H.T.");
    ui.coutCamionTextBrowser->setText(QString::number(coutCamionTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutASTextBrowser->setText(QString::number(coutAutStatTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutFraisRouteTextBrowser->setText(QString::number(fraisRouteTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutAssuranceTextBrowser->setText(QString::number(coutAssurance, 'f', 2) + " \u20AC H.T.");

    // Format pour le prix total H.T.
    ui.totalPriceTextBrowser->setText(QString::number(prixTotalHT, 'f', 2) + " \u20AC H.T.");

    // Format pour les arrhes (T.T.C.)
    ui.arrhesTextBrowser->setText(QString::number(arrhes, 'f', 2) + " \u20AC T.T.C.");
}
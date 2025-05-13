#include "MainWindow.h"
#include <QMessageBox>

void MainWindow::setupValidators()
{
    // Créer un validateur pour nombres décimaux (min, max, précision)
    const auto doubleValidator{ new QDoubleValidator(0, 500, 2, this) };
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);

    const auto intValidator{ new QIntValidator(0, 45000, this) };

    // Utiliser la locale française pour la virgule comme séparateur décimal
    doubleValidator->setLocale(QLocale::French);
    intValidator->setLocale(QLocale::French);

    // Appliquer aux champs qui demandent des nombres décimaux
    ui.distanceLineEdit->setValidator(intValidator);
    ui.volumelineEdit->setValidator(doubleValidator);
    ui.valeurAssuranceLineEdit->setValidator(intValidator);
    ui.prixCamionLineEdit->setValidator(doubleValidator);
    ui.coutKmLineEdit->setValidator(doubleValidator);
    ui.prixEmballageLineEdit->setValidator(doubleValidator);
    ui.prixLocMatLineEdit->setValidator(doubleValidator);
    ui.coutFraisRouteLineEdit->setValidator(doubleValidator);
    ui.moLineEdit->setValidator(doubleValidator);
    ui.fraisStatLineEdit->setValidator(doubleValidator);
}


void MainWindow::setupSettings()
{
    ui.prixCamionLineEdit->setText(QString::number(m_tarification.getCoutCamion()));
    ui.coutKmLineEdit->setText(QString::number(m_tarification.getCoutKilometrique()));
    ui.prixEmballageLineEdit->setText(QString::number(m_tarification.getCoutEmballage()));
    ui.prixLocMatLineEdit->setText(QString::number(m_tarification.getPrixLocMateriel()));
    ui.coutFraisRouteLineEdit->setText(QString::number(m_tarification.getFraisRoute()));
    ui.moLineEdit->setText(QString::number(m_tarification.getCoutMO()));
    ui.fraisStatLineEdit->setText(QString::number(m_tarification.getCoutFraisStationnement()));
}


void MainWindow::on_generateDevisButton_clicked()
{
    // 0. Vérifier si tous les champs importants sont remplis avant de continuer


    std::vector<QLineEdit*> champs{
      ui.distanceLineEdit,
      ui.volumelineEdit,
      ui.valeurAssuranceLineEdit,
      ui.prixCamionLineEdit,
      ui.coutKmLineEdit,
      ui.prixEmballageLineEdit,
      ui.prixLocMatLineEdit,
      ui.coutFraisRouteLineEdit,
      ui.moLineEdit,
      ui.fraisStatLineEdit,
    };


    for (const auto& e : champs)
        if (e->text().isEmpty())
        {
            QMessageBox::warning(this, "Champs manquants ou invalides", "Veuillez remplir correctement les champs avant de g\u00E9n\u00E9rer le devis");
            return;
        }


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


    // Récupérer le type d'assurance sélectionné
    TypeAssurance typeAssurance{ ui.typeAssuranceComboBox->currentIndex() };
    m_client.setTypeAssurance(typeAssurance);


    // Récupérer la valeur déclarée
    double valeurAssurance{ ui.valeurAssuranceLineEdit->text().toDouble() };
    m_client.setValeurAssurance(valeurAssurance);


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

    double coutAssurance{ m_tarification.calculerCoutAssurance(valeurAssurance, typeAssurance) };

    m_tarification.setPrixMetreCube(prestation, nature, distance);

    double prixTotalHT{ m_tarification.calculerCoutTotalHT(volume, coutAssurance, coutAutStatTotal) };

    double arrhes{ m_tarification.calculerArrhes(prixTotalHT) };


    // 3. Afficher les résultats dans l'onglet "Résultats et Devis"

    // Format pour le volume (m³)
    ui.totalVolumeTextBrowser->setText(QString::number(volume, 'f', 2) + " m\u00B3");

    // Format pour les prix H.T. (2 décimales + symbole euro + H.T.)
    ui.coutMOTextBrowser->setText(QString::number(coutMOTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutCamionTextBrowser->setText(QString::number(coutCamionTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutASTextBrowser->setText(QString::number(coutAutStatTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutFraisRouteTextBrowser->setText(QString::number(fraisRouteTotal, 'f', 2) + " \u20AC H.T.");
    ui.coutAssuranceTextBrowser->setText(QString::number(coutAssurance, 'f', 2) + " \u20AC H.T.");

    // Format pour le prix total H.T.
    ui.totalPriceTextBrowser->setText(QString::number(prixTotalHT, 'f', 2) + " \u20AC H.T.");

    // Format pour les arrhes (T.T.C.)
    ui.arrhesTextBrowser->setText(QString::number(arrhes, 'f', 2) + " \u20AC T.T.C.");

    // Afficher le nombre de déménageurs et de camion(s)
    ui.nbPersonnelsTextBrowser->setText(QString::number(nombreMO) + " d\u00E9m\u00E9nageurs");
    ui.nbCamionTextBrowser->setText(QString::number(nombreCamion) + " camion" + (nombreCamion > 1 ? "s" : ""));



    m_tarification.saveSettings();
}
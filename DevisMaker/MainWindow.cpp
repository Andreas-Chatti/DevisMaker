#include "MainWindow.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_departCompleter = new AddressCompleter(ui.adresseDepartLineEdit, this);
    m_arriveeCompleter = new AddressCompleter(ui.adresseLivraisonLineEdit, this);

    m_calculateurDevis = new CalculateurDevis(m_client, m_tarification);

    m_openStreetMap = new OpenStreetMap(this);

    m_PDFGenerator = new PDFGenerator();


    // Calculer la distance après modification du champ d'adresse départ
    connect(ui.adresseDepartLineEdit, &QLineEdit::editingFinished, [this]() {

        QString depart{ ui.adresseDepartLineEdit->text() };
        QString arrivee{ ui.adresseLivraisonLineEdit->text() };

        if (!depart.isEmpty() && !arrivee.isEmpty())
            m_openStreetMap->calculateDistance(depart, arrivee);
        });


    // Calculer la distance après modification du champ d'adresse d'arrivée
    connect(ui.adresseLivraisonLineEdit, &QLineEdit::editingFinished, [this]() {

        QString depart{ ui.adresseDepartLineEdit->text() };
        QString arrivee{ ui.adresseLivraisonLineEdit->text() };

        if (!depart.isEmpty() && !arrivee.isEmpty())
            m_openStreetMap->calculateDistance(depart, arrivee);
        });

    connect(m_openStreetMap, &OpenStreetMap::distanceCalculated, this, &MainWindow::onDistanceCalculated);
    connect(m_openStreetMap, &OpenStreetMap::calculationError, this, &MainWindow::onDistanceError);


    // Initialiser l'analyseur IA
    m_inventoryAnalyzer = new InventoryAnalyzer(this);
    connect(m_inventoryAnalyzer, &InventoryAnalyzer::analysisComplete, this, &MainWindow::handleInventoryAnalysis);
    connect(m_inventoryAnalyzer, &InventoryAnalyzer::analysisError, this, &MainWindow::handleInventoryAnalysisError);


    setupValidators();

    setupSettings();

    ui.typeSaisonLabel->setText(""); // Cacher le texte si l'utilisateur n'a pas encore mis de dates
}


void MainWindow::setupValidators()
{
    const auto doubleRegexValidator{ new QRegularExpressionValidator(QRegularExpression("^\\d{0,6}(\\.\\d{0,2})?$"), this) };

    QList<QLineEdit*> doubleFields{
        ui.volumelineEdit,
        ui.prixCamionLineEdit,
        ui.coutKmLineEdit,
        ui.prixEmballageLineEdit,
        ui.prixLocMatLineEdit,
        ui.coutFraisRouteLineEdit,
        ui.moLineEdit,
        ui.fraisStatLineEdit,
        ui.MMeublesLineEdit,
        ui.deLineEdit,
        ui.suppAdresseLineEdit,
    };

    for (auto* field : doubleFields)
    {
        field->setValidator(doubleRegexValidator);
    }


    const auto intValidator{ new QIntValidator(0, 45000, this) };

    QList<QLineEdit*> intFields{
        ui.distanceLineEdit,
        ui.valeurAssuranceLineEdit,
    };

    for (auto* field : intFields)
    {
        field->setValidator(intValidator);
    }


    QRegularExpressionValidator* phoneValidator = new QRegularExpressionValidator(QRegularExpression("^[0-9]{0,10}$"), this);
    ui.numTelLineEdit->setValidator(phoneValidator);
    ui.numTelLineEdit->setMaxLength(14);
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
    ui.MMeublesLineEdit->setText(QString::number(m_tarification.getCoutMonteMeubles()));
    ui.deLineEdit->setText(QString::number(m_tarification.getPrixDechetterie()));
    ui.suppAdresseLineEdit->setText(QString::number(m_tarification.getPrixSuppAdresse()));
}


void MainWindow::on_generateDevisButton_clicked()
{
    // 0. Vérifier si tous les champs importants sont remplis avant de continuer


    if (!areAllFieldCompleted())
    {
        QMessageBox::warning(this, "Champs manquants ou invalides", "Veuillez remplir correctement les champs avant de g\u00E9n\u00E9rer le devis");
        return;
    }


    // 1. Mettre à jour toutes les variables de l'onglet CLIENT depuis les champs rentrés par l'utilisateur


    updateClientVariables();


    /* 
        * 2. Sélection du preset de tarifs HAUTE ou BASSE saison en fonction de la date
        * Les variables sont mises à jour correctement dans m_tarification
        * Puis ensuite mise à jour de l'affichage des valeurs dans les champs des PARAMETRES
    */

    const PricePreset presetToUse{ determinePresetFromDates(ui.departDateEdit->date(), ui.livraisonDateEdit->date()) };
    m_tarification.loadSettings(presetToUse);
    setupSettings();
    ui.pricePresetComboBox->setCurrentIndex(static_cast<int>(presetToUse));


    // 3. Afficher les résultats dans l'onglet "Résultats et Devis"


    displayingResults();


    if (!ui.generatePdfButton->isEnabled())
        ui.generatePdfButton->setEnabled(true);
}


void MainWindow::on_volumelineEdit_textChanged(const QString& text)
{
    constexpr double maxValeurAssurance{ 45000 };

    // Vérifier si le texte est un nombre valide
    const double volume{ text.toDouble() };

    if (volume >= 0) 
    {
        // Calculer la valeur d'assurance
        double valeurAssurance{ m_calculateurDevis->calculerValeurAssurance(volume)};

        if (valeurAssurance > maxValeurAssurance)
            ui.typeAssuranceComboBox->setCurrentIndex(static_cast<int>(TypeAssurance::dommage));

        // Mettre à jour le champ de valeur d'assurance
        ui.valeurAssuranceLineEdit->setText(QString::number(valeurAssurance, 'f', 0));
    }

    else 
        // Si le volume n'est pas valide, vider le champ d'assurance
        ui.valeurAssuranceLineEdit->clear();
}


void MainWindow::on_distanceLineEdit_textChanged(const QString& text)
{
    constexpr double urbainMaxDistance{ 150.0 };

    const double distance{ text.toDouble() };

    if (distance <= urbainMaxDistance)
        ui.natureComboBox->setCurrentIndex(static_cast<int>(Nature::urbain));

    else if (distance > urbainMaxDistance)
        ui.natureComboBox->setCurrentIndex(static_cast<int>(Nature::special));
}


bool MainWindow::areAllFieldCompleted()
{
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
            return false;

    return true;
}


/*
Récupère les valeurs de chaque champ dans l'onglet "CLIENT"
Puis met à jour les variables de la classe Client
*/
void MainWindow::updateClientVariables()
{
    // Nom, prénom, tél du client
    m_client.setNom(ui.nomLineEdit->text());
    m_client.setPrenom(ui.prenomLineEdit->text());
    m_client.setNumTel(ui.numTelLineEdit->text());

    // Adresse Chargement
    std::string rueChargement{ ui.adresseDepartLineEdit->text().toStdString() };
    int etageChargement{ ui.etageDepartSpinBox->value() };
    bool ascChargement{ ui.ascDepartCheckBox->isChecked() };
    bool mmChargement{ ui.mmDepartCheckBox->isChecked() };
    bool autStatChargement{ ui.asDepartCheckBox->isChecked() };
    QDate dateChargement{ ui.departDateEdit->date() };

    const Adresse A_Chargement{ rueChargement, etageChargement, ascChargement, mmChargement, autStatChargement, dateChargement };
    m_client.setAdresseDepart(A_Chargement);


    // Adresse Livraison
    std::string rueLivraison{ ui.adresseLivraisonLineEdit->text().toStdString() };
    int etageLivraison{ ui.etageLivraisonSpinBox->value() };
    bool ascLivraison{ ui.ascLivraisonCheckBox->isChecked() };
    bool mmLivraison{ ui.mmLivraisonCheckBox->isChecked() };
    bool autStatLivraison{ ui.asLivraisonCheckBox->isChecked() };
    QDate dateLivraison{ ui.livraisonDateEdit->date() };

    const Adresse A_Livraison{ rueLivraison, etageLivraison, ascLivraison, mmLivraison, autStatLivraison, dateLivraison };
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


    // Récupérer le nombre d'adresses supplémentaires (si checkbox == true)
    int nbAdresseSupp{ ui.suppAdresseCheckBox->isChecked() ? ui.suppAdresseSpinBox->value() : 0 };
    m_client.setNbAdresseSupp(nbAdresseSupp);


    // Déterminer le prix du mètre cube en fonctions des paramètres actuels
    PricePreset presetToUse{ determinePresetFromDates(ui.departDateEdit->date(), ui.livraisonDateEdit->date()) };
    double prixM3{ m_calculateurDevis->calculerPrixMetreCube(presetToUse) };
    m_tarification.setPrixMetreCube(prixM3);
}


/*
Récupère les valeurs de chaque champ dans l'onglet "PARAMETRES"
Puis met à jour les variables dans la classe Tarification
*/
void MainWindow::updateSettingsVariables()
{
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


    double prixMonteMeubles{ ui.MMeublesLineEdit->text().toDouble() };
    m_tarification.setCoutMonteMeubles(prixMonteMeubles);


    double prixDechetterie{ ui.deLineEdit->text().toDouble() };
    m_tarification.setPrixDechetterie(prixDechetterie);


    double prixSuppAdresse{ ui.suppAdresseLineEdit->text().toDouble() };
    m_tarification.setPrixSuppAdresse(prixSuppAdresse);
}


/* 
Affiche les résultats de chaques ligne du devis
*/
void MainWindow::displayingResults()
{
    setupDevisTable();

    const PricePreset preset{ determinePresetFromDates(ui.departDateEdit->date(), ui.livraisonDateEdit->date()) };
    ResultatsDevis result{ m_calculateurDevis->calculateDevis(preset) };

    populateDevisTable(result);
}


void MainWindow::on_AnalyseInventoryPushButton_clicked()
{
    // Récupérer le texte d'inventaire
    QString inventoryText{ ui.inventaireTextEdit->toPlainText() };

    if (inventoryText.isEmpty()) 
    {
        QMessageBox::warning(this, "Inventaire vide", "Veuillez saisir une liste d'objets avant d'analyser.");
        return;
    }

    // Changer le texte du bouton pour indiquer le chargement
    ui.AnalyseInventoryPushButton->setText("Analyse en cours...");
    ui.AnalyseInventoryPushButton->setEnabled(false);

    // Lancer l'analyse avec l'IA
    m_inventoryAnalyzer->analyzeInventory(inventoryText);

    qDebug() << "Lancement de l'analyse IA pour:" << inventoryText.left(50) + "...";
}


// Traitement du résultat d'analyse
void MainWindow::handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems)
{
    // Mettre à jour le champ de volume
    ui.volumelineEdit->setText(QString::number(totalVolume, 'f', 2));

    // Mettre à jour le tableau des éléments détectés
    ui.tableWidget->setRowCount(structuredItems.size());
    ui.tableWidget->setColumnCount(3);


    ui.tableWidget->setHorizontalHeaderLabels({
        "Quantite",
        "Objet",
        "Volume - Total: " + QString::number(totalVolume, 'f', 2) + " m\u00B3"
        });

    for (int i{}; i < structuredItems.size(); ++i)
    {
        QString item{ structuredItems[i] };
        QStringList parts{ item.split(" - ") };

        if (parts.size() == 2)
        {
            QString fullName{ parts[0] };     // Ex: "2 matelas 1 place"
            QString volumeText{ parts[1] };   // Ex: "1.0 m³"


            QStringList words{ fullName.split(" ") };
            QString quantity{ "1" };  // Par défaut
            QString cleanName{ fullName };

            if (!words.isEmpty())
            {
                bool ok{};
                int qty{ words.first().toInt(&ok) };
                if (ok && qty > 0)
                {
                    quantity = QString::number(qty);
                    words.removeFirst();
                    cleanName = words.join(" ");
                }
            }

            // Remplir les 3 colonnes (même logique que parts[0] et parts[1])
            ui.tableWidget->setItem(i, 0, new QTableWidgetItem(quantity));
            ui.tableWidget->setItem(i, 1, new QTableWidgetItem(cleanName));
            ui.tableWidget->setItem(i, 2, new QTableWidgetItem(volumeText));
        }
    }

    QHeaderView* header = ui.tableWidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Quantitée : contenu
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);  // Objet : contenu
    header->setSectionResizeMode(2, QHeaderView::Stretch);           // Volume : étirement

    // Restaurer le bouton
    ui.AnalyseInventoryPushButton->setText("Analyser inventaire");
    ui.AnalyseInventoryPushButton->setEnabled(true);

    // Message de succès
    QString titre = "Analyse terminee";
    QString message = QString("Volume total calcule par l'IA: %1 m3, %2 objets detectes").arg(totalVolume).arg(structuredItems.size());
    QMessageBox::information(this, titre, message);

    qDebug() << "Analyse IA terminée avec succès. Volume:" << totalVolume;
}

// Gestion des erreurs
void MainWindow::handleInventoryAnalysisError(const QString& errorMessage)
{
    // Restaurer le bouton
    ui.AnalyseInventoryPushButton->setText("Analyser inventaire");
    ui.AnalyseInventoryPushButton->setEnabled(true);

    // Afficher l'erreur
    QString titre = "Erreur d'analyse IA";
    QString message = QString("L'analyse de l'inventaire a echoue: %1").arg(errorMessage);
    QMessageBox::critical(this, titre, message);

    qDebug() << "Erreur lors de l'analyse IA:" << errorMessage;
}


void MainWindow::onDistanceCalculated(double distance) 
{
    ui.distanceLineEdit->setText(QString::number(distance, 'f', 1));
    m_client.setDistance(distance);
}


void MainWindow::onDistanceError(const QString& errorMessage) 
{
    QMessageBox::warning(this, "Erreur", errorMessage);
}


void MainWindow::on_suppAdresseCheckBox_toggled(bool isChecked)
{
    ui.suppAdresseSpinBox->setEnabled(isChecked);
}


void MainWindow::setupDevisTable()
{
    // Configuration du tableau (une seule fois)
    ui.devisTableWidget->setRowCount(0);  // Vider
    ui.devisTableWidget->setColumnCount(2);

    // En-têtes de colonnes
    ui.devisTableWidget->setHorizontalHeaderLabels({ "Poste", "Montant" });

    // CONFIGURATION DU REDIMENSIONNEMENT DES COLONNES
    QHeaderView* header{ ui.devisTableWidget->horizontalHeader() };
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);  // Colonne "Poste" → s'adapte au contenu
    header->setSectionResizeMode(1, QHeaderView::Stretch);           // Colonne "Montant" → prend le reste

    // Configuration esthétique
    ui.devisTableWidget->setAlternatingRowColors(true);
    ui.devisTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.devisTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MainWindow::populateDevisTable(ResultatsDevis resultat)
{
    QVector<QPair<QString, QString>> devisItems{
        // ═══ SECTION INFORMATIONS GÉNÉRALES ═══
        {"Volume total", QString::number(m_client.getVolume(), 'f', 2) + " m³"},
        {"Personnel affecté", QString::number(resultat.nombreMO) + " déménageur" + (resultat.nombreMO > 1 ? "s" : "")},
        {"Nombre camion(s)", QString::number(resultat.nombreCamion) + " camion" + (resultat.nombreCamion > 1 ? "s" : "")},

        // ═══ SECTION COÛTS PRINCIPAUX ═══
        {"Main d'œuvre", QString::number(resultat.coutMainOeuvre, 'f', 2) + " € H.T."},
        {"Camion(s)", QString::number(resultat.coutCamion, 'f', 2) + " € H.T."},
        {"Assurance mobilier", QString::number(resultat.coutAssurance, 'f', 2) + " € H.T."}
    };

    // AJOUTER conditionnellement (SANS séparateurs vides)
    if (resultat.fraisRoute > 0)
        devisItems.push_back({ "Frais de route", QString::number(resultat.fraisRoute, 'f', 2) + " € H.T." });

    // SECTION SUPPLÉMENTS
    if (resultat.coutStationnement > 0)
        devisItems.push_back({ "Autorisation de stationnement", QString::number(resultat.coutStationnement, 'f', 2) + " € H.T." });

    if (resultat.fraisMonteMeubles > 0)
        devisItems.push_back({ "Monte-meubles", QString::number(resultat.fraisMonteMeubles, 'f', 2) + " € H.T." });

    if (resultat.prixDechetterie > 0)
        devisItems.push_back({ "Déchetterie", QString::number(resultat.prixDechetterie, 'f', 2) + " € H.T." });

    if (resultat.prixSuppAdresse > 0)
    {
        QString nbAdresses{ QString::number(ui.suppAdresseSpinBox->value()) };
        devisItems.push_back({ "Supplément adresse (" + nbAdresses + ")", QString::number(resultat.prixSuppAdresse, 'f', 2) + " € H.T." });
    }

    // SECTION TOTAUX
    devisItems.push_back({ "Arrhes (30%)", QString::number(resultat.arrhes, 'f', 2) + " € T.T.C." });
    devisItems.push_back({ "TOTAL H.T.", QString::number(resultat.prixTotalHT, 'f', 2) + " € H.T." });

    // ═══ REMPLISSAGE DU TABLEAU ═══
    ui.devisTableWidget->setRowCount(devisItems.size());

    for (int i{}; i < devisItems.size(); ++i)
    {
        // Colonne "Poste"
        QTableWidgetItem* posteItem{ new QTableWidgetItem(devisItems[i].first) };
        ui.devisTableWidget->setItem(i, 0, posteItem);

        // Colonne "Montant"
        QTableWidgetItem* montantItem{ new QTableWidgetItem(devisItems[i].second) };
        montantItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui.devisTableWidget->setItem(i, 1, montantItem);

        // ═══ STYLES CONDITIONNELS (SANS gestion lignes vides) ═══

        // Total principal (gras + fond vert) - MAINTENANT EN DERNIER
        if (devisItems[i].first.contains("TOTAL H.T."))
        {
            QFont boldFont{};
            boldFont.setBold(true);
            boldFont.setPointSize(11);  // Un peu plus gros
            posteItem->setFont(boldFont);
            montantItem->setFont(boldFont);
            posteItem->setBackground(QColor(200, 255, 200));  // Vert clair
            montantItem->setBackground(QColor(200, 255, 200));
        }

        // Arrhes (important) - MAINTENANT AVANT LE TOTAL
        else if (devisItems[i].first.contains("Arrhes"))
        {
            QFont boldFont{};
            boldFont.setBold(true);
            posteItem->setFont(boldFont);
            montantItem->setFont(boldFont);
            posteItem->setBackground(QColor(255, 255, 200));  // Jaune clair
            montantItem->setBackground(QColor(255, 255, 200));
        }

        // Informations générales (en-tête)
        else if (devisItems[i].first.contains("Volume total") ||
            devisItems[i].first.contains("Personnel") ||
            devisItems[i].first.contains("Matériel"))
        {
            QFont headerFont{};
            headerFont.setBold(true);
            posteItem->setFont(headerFont);
            montantItem->setFont(headerFont);
            posteItem->setBackground(QColor(240, 240, 240));  // Gris clair
            montantItem->setBackground(QColor(240, 240, 240));
        }
    }
}


void MainWindow::on_generatePdfButton_clicked()
{
    const auto& results{ m_calculateurDevis->getLastResults() };

    // DEMANDER à l'utilisateur où sauvegarder
    QString filePath{ QFileDialog::getSaveFileName(
        this,
        "Sauvegarder le devis PDF",
        QString("DevisChatti_%1_%2_%3.pdf")
            .arg(m_client.getNom())
            .arg(m_PDFGenerator->getPrestationString(m_client.getPrestation()))
            .arg(QDate::currentDate().toString("yyyyMMdd")),
        "Fichiers PDF (*.pdf)"
    ) };

    if (filePath.isEmpty())
        return;  // Utilisateur a annulé

    // PASSER le chemin choisi
    m_PDFGenerator->generateDevisPDF(m_client, results, filePath);
}


void MainWindow::on_numTelLineEdit_editingFinished()
{
    QString text{ ui.numTelLineEdit->text() };

    text.remove(QRegularExpression("[^0-9]"));

    if (text.isEmpty()) 
        return; // Champ vide, on ne fait rien

    if (text.length() == 10 && text.startsWith("0")) 
    {
        // Numéro valide - formatage
        QString formatted{ QString("%1 %2 %3 %4 %5")
            .arg(text.mid(0, 2))
            .arg(text.mid(2, 2))
            .arg(text.mid(4, 2))
            .arg(text.mid(6, 2))
            .arg(text.mid(8, 2)) };

        ui.numTelLineEdit->blockSignals(true);
        ui.numTelLineEdit->setText(formatted);
        ui.numTelLineEdit->setStyleSheet(""); // Reset style en cas d'erreur précédente
        ui.numTelLineEdit->blockSignals(false);
    }

    else 
    {
        // Numéro invalide - feedback visuel
        ui.numTelLineEdit->setStyleSheet("border: 2px solid red;");
        ui.numTelLineEdit->setToolTip("Le numéro doit contenir 10 chiffres et commencer par 0");
    }
}


void MainWindow::on_pricePresetComboBox_currentIndexChanged(int index)
{
    PricePreset selectedPreset{ index };
    m_tarification.loadSettings(selectedPreset);

    setupSettings();
}


PricePreset MainWindow::determinePresetFromDates(const QDate& dateChargement, const QDate& dateLivraison) const
{
    auto isHauteSaison = [](const QDate& date) -> bool {
        // 15 juin à 15 septembre
        return (date.month() == 6 && date.day() >= 15) ||
            (date.month() >= 7 && date.month() <= 8) ||
            (date.month() == 9 && date.day() <= 15);
        };

    return (isHauteSaison(dateChargement) || isHauteSaison(dateLivraison)) ? PricePreset::HauteSaison : PricePreset::BasseSaison;
}


void MainWindow::on_saveSettingsPushButton_clicked()
{
    updateSettingsVariables();

    PricePreset selectedPreset{ ui.pricePresetComboBox->currentIndex() };
    m_tarification.saveSettings(selectedPreset);
}


void MainWindow::on_departDateEdit_editingFinished()
{
    const QDate dateChargement{ ui.departDateEdit->date() };
    const QDate dateLivraison{ ui.livraisonDateEdit->date() };

    updateSeasonTypeLabel(dateLivraison);

    if (dateLivraison.month() < dateChargement.month() ||
        dateLivraison.year() < dateChargement.year() ||
        (dateLivraison.month() == dateChargement.month() && dateLivraison.day() < dateChargement.day()))
        ui.livraisonDateEdit->setDate(dateChargement);
}


void MainWindow::on_livraisonDateEdit_editingFinished()
{
    const QDate dateChargement{ ui.departDateEdit->date() };
    const QDate dateLivraison{ ui.livraisonDateEdit->date() };

    updateSeasonTypeLabel(dateLivraison);

    if (dateLivraison.month() < dateChargement.month() ||
        dateLivraison.year() < dateChargement.year() ||
        (dateLivraison.month() == dateChargement.month() && dateLivraison.day() < dateChargement.day()))
        ui.departDateEdit->setDate(dateLivraison);
}


void MainWindow::updateSeasonTypeLabel(const QDate& dateLivraison) const
{
    auto isHauteSaison = [](const QDate& date) {
        // 15 juin à 15 septembre
        return (date.month() == 6 && date.day() >= 15) ||
            (date.month() >= 7 && date.month() <= 8) ||
            (date.month() == 9 && date.day() <= 15);
        };


    bool hauteSaison{ isHauteSaison(dateLivraison) };
    ui.typeSaisonLabel->setText(hauteSaison ? "HAUTE SAISON" : "BASSE SAISON");
    ui.typeSaisonLabel->setStyleSheet(hauteSaison ? "color: red; font-weight: bold;" : "color: blue; font-weight: bold;");
}
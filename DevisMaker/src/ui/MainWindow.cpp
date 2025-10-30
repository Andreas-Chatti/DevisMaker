#include "MainWindow.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_addressCompleter = new AddressCompleter{ ui.adresseDepartLineEdit, ui.adresseLivraisonLineEdit, this };

    connect(m_PDFGenerator, &PDFGenerator::pdfGenerationStatusReport, this, &MainWindow::onGenerateDevisStatusReport);
    connect(m_addressCompleter->getStreetMap(), &OpenStreetMap::distanceCalculated, this, &MainWindow::onDistanceCalculated);
    connect(m_addressCompleter->getStreetMap(), &OpenStreetMap::calculationError, this, &MainWindow::onDistanceError);
    connect(m_inventoryAnalyzer, &InventoryAnalyzer::analysisComplete, m_client->getInventory(), &Inventory::handleInventoryAnalysis);
    connect(m_inventoryAnalyzer, &InventoryAnalyzer::analysisError, this, &MainWindow::handleInventoryAnalysisError);
    connect(m_inventoryAnalyzer, &InventoryAnalyzer::error, this, &MainWindow::onCriticalError);
    connect(m_client->getInventory(), &Inventory::sendNewInventory, this, &MainWindow::handleInventoryAnalysis);

    // Connexions pour la reconnaissance vocale
    connect(m_speechRecognizer, &WindowsSpeechRecognizer::textRecognized, this, &MainWindow::onTextRecognized);
    connect(m_speechRecognizer, &WindowsSpeechRecognizer::errorOccurred, this, [this](const QString& error) {
        QMessageBox::warning(this, "Erreur de reconnaissance vocale", error);
    });

    setupValidators();
    displaySettings();
    setupPlaceholderText();
    setupDateEdit();
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
        ui.suppAdresseLineEdit,
    };

    for (auto* field : doubleFields)
        field->setValidator(doubleRegexValidator);


    const auto intValidator{ new QIntValidator(0, SettingsConstants::MAX_INSURANCE_VALUE, this) };

    QList<QLineEdit*> intFields{
        ui.distanceLineEdit,
        ui.valeurAssuranceLineEdit,
    };

    for (auto* field : intFields)
        field->setValidator(intValidator);


    QRegularExpressionValidator* phoneValidator{ new QRegularExpressionValidator(QRegularExpression("^[0-9]{0,10}$"), this) };
    ui.numTelLineEdit->setValidator(phoneValidator);
    ui.numTelLineEdit->setMaxLength(14);
}


void MainWindow::displaySettings()
{
    ui.prixCamionLineEdit->setText(QString::number(m_tarification->getCoutCamion()));
    ui.coutKmLineEdit->setText(QString::number(m_tarification->getCoutKilometrique()));
    ui.prixEmballageLineEdit->setText(QString::number(m_tarification->getCoutEmballage()));
    ui.prixLocMatLineEdit->setText(QString::number(m_tarification->getPrixLocMateriel()));
    ui.coutFraisRouteLineEdit->setText(QString::number(m_tarification->getFraisRoute()));
    ui.moLineEdit->setText(QString::number(m_tarification->getCoutMO()));
    ui.fraisStatLineEdit->setText(QString::number(m_tarification->getCoutFraisStationnement()));
    ui.MMeublesLineEdit->setText(QString::number(m_tarification->getCoutMonteMeubles()));
    ui.suppAdresseLineEdit->setText(QString::number(m_tarification->getPrixSuppAdresse()));

    ui.prix150_400LineEdit->setText(QString::number(m_tarification->getPrixM3Route(Tarification::PriceKey::distance150_400)));
    ui.prix401_600LineEdit->setText(QString::number(m_tarification->getPrixM3Route(Tarification::PriceKey::distance401_600)));
    ui.prix601_760LineEdit->setText(QString::number(m_tarification->getPrixM3Route(Tarification::PriceKey::distance601_760)));
    ui.prix761_900LineEdit->setText(QString::number(m_tarification->getPrixM3Route(Tarification::PriceKey::distance761_900)));
    ui.prix901PlusLineEdit->setText(QString::number(m_tarification->getPrixM3Route(Tarification::PriceKey::distance901PLUS)));

    ui.prixm3EcoLineEdit->setText(QString::number(m_tarification->getPrixM3Urbain(Prestation::eco)));
    ui.prixm3EcoPlusLineEdit->setText(QString::number(m_tarification->getPrixM3Urbain(Prestation::ecoPlus)));
    ui.prixm3StandardLineEdit->setText(QString::number(m_tarification->getPrixM3Urbain(Prestation::standard)));
    ui.prixm3LuxeLineEdit->setText(QString::number(m_tarification->getPrixM3Urbain(Prestation::luxe)));
}


void MainWindow::on_generateDevisButton_clicked()
{
    // 0. Vérifier si tous les champs importants sont remplis avant de continuer
    if (!areAllFieldCompleted())
    {
        QMessageBox::warning(this, "Champs manquants ou invalides", QString::fromUtf8("Veuillez remplir correctement les champs avant de générer le devis"));
        return;
    }

    /*
    Déterminer le prix du mètre cube en fonctions des paramètres actuels
    */
    const PricePreset presetToUse{ determinePresetFromDates(ui.departDateEdit->date(), ui.livraisonDateEdit->date()) };
    double prixM3{ m_calculateurDevis->calculerPrixMetreCube(presetToUse) };
    m_tarification->setPrixMetreCube(prixM3);

    /* 
        * 2. Sélection du preset de tarifs HAUTE ou BASSE saison en fonction de la date
        * Les variables sont mises à jour correctement dans m_tarification
        * Puis ensuite mise à jour de l'affichage des valeurs dans les champs des PARAMETRES
    */

    const Tarification::PriceCalculation calculationMethod{ ui.priceCalculationComboBox->currentIndex() };
    m_tarification->loadSettings(presetToUse);
    displaySettings();
    ui.pricePresetComboBox->setCurrentIndex(static_cast<int>(presetToUse));


    // 3. Afficher les résultats dans l'onglet "Résultats et Devis"
    displayingResults();


    if (!ui.generatePdfButton->isEnabled())
        ui.generatePdfButton->setEnabled(true);
}


void MainWindow::on_volumelineEdit_textChanged(const QString& text)
{
    constexpr double maxValeurAssurance{ SettingsConstants::MAX_INSURANCE_VALUE };

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
    constexpr double urbainMaxDistance{ SettingsConstants::Distances::URBAN_DISTANCE_LIMIT };

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
Récupère les valeurs de chaque champ dans l'onglet "PARAMETRES"
Puis met à jour les variables dans la classe Tarification
*/
void MainWindow::updateSettingsVariables()
{
    double prixCamion{ ui.prixCamionLineEdit->text().toDouble() };
    m_tarification->setCoutCamion(prixCamion);
    double prixKilometrage{ ui.coutKmLineEdit->text().toDouble() };
    m_tarification->setCoutKilometrique(prixKilometrage);
    double prixEmballage{ ui.prixEmballageLineEdit->text().toDouble() };
    m_tarification->setCoutEmballage(prixEmballage);
    double prixLocMateriel{ ui.prixLocMatLineEdit->text().toDouble() };
    m_tarification->setPrixLocMateriel(prixLocMateriel);
    double prixFraisRoute{ ui.coutFraisRouteLineEdit->text().toDouble() };
    m_tarification->setFraisRoute(prixFraisRoute);
    double prixMO{ ui.moLineEdit->text().toDouble() };
    m_tarification->setCoutMO(prixMO);
    double prixFraisStationnement{ ui.fraisStatLineEdit->text().toDouble() };
    m_tarification->setCoutFraisStationnement(prixFraisStationnement);
    double prixMonteMeubles{ ui.MMeublesLineEdit->text().toDouble() };
    m_tarification->setCoutMonteMeubles(prixMonteMeubles);
    double prixSuppAdresse{ ui.suppAdresseLineEdit->text().toDouble() };
    m_tarification->setPrixSuppAdresse(prixSuppAdresse);


    double m3_150_400{ ui.prix150_400LineEdit->text().toDouble() };
    m_tarification->setPrixM3Route(Tarification::PriceKey::distance150_400, m3_150_400);
    double m3_401_600{ ui.prix401_600LineEdit->text().toDouble() };
    m_tarification->setPrixM3Route(Tarification::PriceKey::distance401_600, m3_401_600);
    double m3_601_760{ ui.prix601_760LineEdit->text().toDouble() };
    m_tarification->setPrixM3Route(Tarification::PriceKey::distance601_760, m3_601_760);
    double m3_761_900{ ui.prix761_900LineEdit->text().toDouble() };
    m_tarification->setPrixM3Route(Tarification::PriceKey::distance761_900, m3_761_900);
    double m3_901Plus{ ui.prix901PlusLineEdit->text().toDouble() };
    m_tarification->setPrixM3Route(Tarification::PriceKey::distance901PLUS, m3_901Plus);


    double m3Eco{ ui.prixm3EcoLineEdit->text().toDouble() };
    m_tarification->setPrixM3Urbain(Prestation::eco, m3Eco);
    double m3EcoPlus{ ui.prixm3EcoPlusLineEdit->text().toDouble() };
    m_tarification->setPrixM3Urbain(Prestation::ecoPlus, m3EcoPlus);
    double m3Standard{ ui.prixm3StandardLineEdit->text().toDouble() };
    m_tarification->setPrixM3Urbain(Prestation::standard, m3Standard);
    double m3Luxe{ ui.prixm3LuxeLineEdit->text().toDouble() };
    m_tarification->setPrixM3Urbain(Prestation::luxe, m3Luxe);
}


/* 
Affiche les résultats de chaques ligne du devis
*/
void MainWindow::displayingResults()
{
    setupDevisTable();

    const PricePreset preset{ determinePresetFromDates(ui.departDateEdit->date(), ui.livraisonDateEdit->date()) };
    const Tarification::PriceCalculation calculationMethod{ ui.priceCalculationComboBox->currentIndex() };
    ResultatsDevis result{ m_calculateurDevis->calculateDevis(preset, calculationMethod) };

    populateDevisTable(result, calculationMethod);
}


void MainWindow::on_AnalyseInventoryPushButton_clicked()
{
    QString inventoryText{ ui.inventaireTextEdit->toPlainText() };
    if (inventoryText.isEmpty()) 
    {
        QMessageBox::warning(this, "Inventaire vide", "Veuillez saisir une liste d'objets avant d'analyser.");
        return;
    }

    else if (m_client->getInventory()->objectsQuantity() > 0)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Écrasement d'inventaire");
        msgBox.setText("Attention, faire l'analyse va écraser l'inventaire actuel.");
        msgBox.setInformativeText("Voulez-vous continuer ?");

        QPushButton* ouiBtn = msgBox.addButton("Oui", QMessageBox::YesRole);
        QPushButton* nonBtn = msgBox.addButton("Non", QMessageBox::NoRole);

        msgBox.setDefaultButton(nonBtn);
        msgBox.exec();

        if (msgBox.clickedButton() == nonBtn)
            return;
    }

    // Changer le texte du bouton pour indiquer le chargement
    ui.AnalyseInventoryPushButton->setText("Analyse en cours...");
    ui.AnalyseInventoryPushButton->setEnabled(false);
    ui.modifyInventoryPushButton->setEnabled(false);
    ui.generateInventoryPushButton->setEnabled(false);

    // Lancer l'analyse avec l'IA
    m_inventoryAnalyzer->analyzeInventory(inventoryText);
}


void MainWindow::on_generateInventoryPushButton_clicked()
{
    if (m_client->getInventory()->isEmpty())
    {
        QMessageBox::warning(this, "Inventaire vide", "Impossible de générer ! L'inventaire est vide !");
        return;
    }

    QString filePath{ QFileDialog::getSaveFileName(
        this,
        "Sauvegarder l'inventaire",
        QString("Inventaire_%1_%2.pdf")
            .arg(m_client->getNom())
            .arg(QDate::currentDate().toString("yyyyMMdd")),
        "Fichiers PDF (*.pdf)"
    ) };

    if (filePath.isEmpty())
        return;

    m_PDFGenerator->generateInventoryPDF(*m_client, *m_user, filePath);
}


void MainWindow::handleInventoryAnalysis(const Inventory& inventory)
{
    ui.volumelineEdit->setText(QString::number(inventory.getTotalVolume(), 'f', 2));

    displayInventory(inventory);

    QString titre{ "Analyse terminée" };
    QString msg1{ QString{"Volume total: %1 m3, %2 "}.arg(QString::number(inventory.getTotalVolume(), 'f', 2)).arg(inventory.objectsQuantity()) };
    QString msg2{ inventory.objectsQuantity() >= 2 ? "objets détectés" : "objet détecté"};
    QMessageBox::information(this, titre, msg1 + msg2);

    ui.AnalyseInventoryPushButton->setText("Analyser inventaire");
    ui.AnalyseInventoryPushButton->setEnabled(true);
    ui.generateInventoryPushButton->setEnabled(true);
    ui.modifyInventoryPushButton->setEnabled(true);
}


void MainWindow::displayInventory(const Inventory& inventory) const
{
    ui.tableWidget->setRowCount(0);
    constexpr int columnCount{ 4 };
    ui.tableWidget->setColumnCount(columnCount);
    ui.tableWidget->verticalHeader()->setVisible(false);

    ui.tableWidget->setHorizontalHeaderLabels({
        QString::fromUtf8("Quantité"),
        "Objet",
        "Volume unitaire",
        "Volume - Total: " + QString::number(inventory.getTotalVolume(), 'f', 2) + " m\u00B3"
        });

    auto createCenteredItem = [](const QString& text) {
        auto item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        return item;
        };

    int currentRow{};

    const auto& areaList = inventory.getAreas();
    for (const auto& area : areaList)
    {
        ui.tableWidget->insertRow(currentRow);

        QString areaText{ QString("%1").arg(area.getName()) };

        QTableWidgetItem* areaItem{ new QTableWidgetItem(areaText) };

        // Style : texte en gras et plus gros
        QFont font{ areaItem->font() };
        font.setBold(true);
        font.setPointSize(font.pointSize() + 2);
        areaItem->setFont(font);

        // Couleur de fond pour distinguer visuellement
        areaItem->setBackground(QColor(230, 240, 255));  // Bleu clair

        // Centrer le texte
        areaItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

        // Ajouter l'item à la première colonne
        ui.tableWidget->setItem(currentRow, 0, areaItem);

        // Fusionner toutes les colonnes pour que le nom prenne toute la largeur
        ui.tableWidget->setSpan(currentRow, 0, 1, columnCount);

        currentRow++;  // ✅ Passe à la ligne suivante

        // ✅ Ajouter les objets de cette pièce
        for (const auto& object : area.getObjectsList())
        {
            ui.tableWidget->insertRow(currentRow);

            QString itemQuantity = QString::number(object.getQuantity());
            QString itemName = object.getName();
            QString itemUnitaryVolume = QString::number(object.getUnitaryVolume(), 'f', 1) + " m\u00B3";
            QString itemTotalVolume = QString::number(object.getTotalVolume(), 'f', 1) + " m\u00B3";

            ui.tableWidget->setItem(currentRow, 0, createCenteredItem(itemQuantity));
            ui.tableWidget->setItem(currentRow, 1, createCenteredItem(itemName));
            ui.tableWidget->setItem(currentRow, 2, createCenteredItem(itemUnitaryVolume));
            ui.tableWidget->setItem(currentRow, 3, createCenteredItem(itemTotalVolume));

            currentRow++;
        }
    }

    QHeaderView* header{ ui.tableWidget->horizontalHeader() };
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);  // Quantité : contenu
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);  // Objet : contenu
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);  // Volume unitaire
    header->setSectionResizeMode(3, QHeaderView::Stretch);           // Volume : étirement
}


void MainWindow::handleInventoryAnalysisError(const QString& errorMessage)
{
    QString titre{ "Erreur d'analyse" };
    QString message{ errorMessage };
    QMessageBox::critical(this, titre, message);

    ui.AnalyseInventoryPushButton->setText("Analyser inventaire");
    ui.AnalyseInventoryPushButton->setEnabled(true);
    ui.modifyInventoryPushButton->setEnabled(true);
    ui.generateInventoryPushButton->setEnabled(true);
}


void MainWindow::onDistanceCalculated(double distance) 
{
    ui.distanceLineEdit->setText(QString::number(distance, 'f', 1));
    m_client->setDistance(distance);
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


void MainWindow::populateDevisTable(const ResultatsDevis& resultat, const Tarification::PriceCalculation& calculationMethod)
{
    QVector<QPair<QString, QString>> devisItems
    {
        {"Méthode de calcul",  calculationMethod == Tarification::PriceCalculation::m3 ? "Prix par mètre cube" : "Cinq postes"},
        {"Volume total", QString::number(m_client->getVolume(), 'f', 2) + " m³"}
    };

    if (calculationMethod == Tarification::PriceCalculation::postes)
    {
        devisItems.push_back({ "Personnel affecté", QString::number(resultat.resultatsCinqPostes.nombreMO) + " déménageur" + (resultat.resultatsCinqPostes.nombreMO > 1 ? "s" : "") });
        devisItems.push_back({ "Nombre camion(s)", QString::number(resultat.resultatsCinqPostes.nombreCamion) + " camion" + (resultat.resultatsCinqPostes.nombreCamion > 1 ? "s" : "") });
        devisItems.push_back({ "Main d'œuvre", QString::number(resultat.resultatsCinqPostes.coutMainOeuvre, 'f', 2) + " € H.T." });
        devisItems.push_back({ "Camion(s)", QString::number(resultat.resultatsCinqPostes.coutCamion, 'f', 2) + " € H.T." });
        devisItems.push_back({ "Kilométrage", QString::number(resultat.resultatsCinqPostes.prixKilometrage, 'f', 2) + " € H.T." });
        devisItems.push_back({ "Emballage", QString::number(resultat.resultatsCinqPostes.prixEmballage, 'f', 2) + " € H.T." });
        devisItems.push_back({ "Location Matériel", QString::number(resultat.resultatsCinqPostes.prixLocMateriel, 'f', 2) + " € H.T." });
    }

    else
    {
        devisItems.push_back({ "Prix du m³", QString::number(resultat.prixMetreCube, 'f', 2) + " m³" });
        devisItems.push_back({ "Prix forfaitaire", QString::number(resultat.prixMetreCube * m_client->getVolume(), 'f', 2) + "€ H.T."});
    }

    devisItems.push_back({ "Assurance mobilier", QString::number(resultat.coutAssurance, 'f', 2) + " € H.T." });

    if (resultat.fraisRoute > 0)
        devisItems.push_back({ "Frais de route", QString::number(resultat.fraisRoute, 'f', 2) + " € H.T." });

    if (resultat.coutStationnement > 0)
        devisItems.push_back({ "Autorisation de stationnement", QString::number(resultat.coutStationnement, 'f', 2) + " € H.T." });

    if (resultat.fraisMonteMeubles > 0)
        devisItems.push_back({ "Monte-meubles", QString::number(resultat.fraisMonteMeubles, 'f', 2) + " € H.T." });

    if (resultat.prixSuppAdresse > 0)
    {
        QString nbAdresses{ QString::number(ui.suppAdresseSpinBox->value()) };
        devisItems.push_back({ "Supplément adresse (" + nbAdresses + ")", QString::number(resultat.prixSuppAdresse, 'f', 2) + " € H.T." });
    }

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
    const ResultatsDevis& devisResults{ m_calculateurDevis->getLastResults() };

    // DEMANDER à l'utilisateur où sauvegarder
    QString filePath{ QFileDialog::getSaveFileName(
        this,
        "Sauvegarder le devis",
        QString("DevisChatti_%1_%2_%3.pdf")
            .arg(m_client->getNom())
            .arg(m_PDFGenerator->getPrestationString(m_client->getPrestation()))
            .arg(QDate::currentDate().toString("yyyyMMdd")),
        "Fichiers PDF (*.pdf)"
    ) };

    if (filePath.isEmpty())
        return;

    PDFGenerator::TypeDevis typeDevis{ ui.priceCalculationComboBox->currentIndex() };
    m_PDFGenerator->generateDevisPDF(*m_client, devisResults, typeDevis, *m_user, filePath);
}


void MainWindow::onGenerateDevisStatusReport(PDFGenerator::PdfGenerationState generationState)
{
    QString notificationMessage{};

    switch (generationState)
    {
    case PDFGenerator::PdfGenerationState::success: notificationMessage = QString::fromUtf8("Le document à été créer avec succès.");
        break;
    case PDFGenerator::PdfGenerationState::blankFile: notificationMessage = "Error: blank file.";
        break;
    case PDFGenerator::PdfGenerationState::errorLoadingFile: notificationMessage = "Error loading file.";
        break;
    case PDFGenerator::PdfGenerationState::errorCreatingTemplateFile: notificationMessage = "Error creating template file.";
        break;
    case PDFGenerator::PdfGenerationState::errorCreatingTemplateDir: notificationMessage = "Error creating template directory.";
        break;
    }

    if (generationState == PDFGenerator::PdfGenerationState::success)
        QMessageBox::information(this, "Information", notificationMessage);

    else
        QMessageBox::warning(this, "Erreur", notificationMessage);
}


void MainWindow::on_pricePresetComboBox_currentIndexChanged(int index)
{
    PricePreset selectedPreset{ index };
    m_tarification->loadSettings(selectedPreset);

    displaySettings();
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
    m_tarification->saveSettings(selectedPreset, Tarification::PriceCalculation::postes);
    m_tarification->saveSettings(selectedPreset, Tarification::PriceCalculation::m3);
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


void MainWindow::onCriticalError(const QString& errorMessage)
{
    QString title{ "Error" };
    QString description{ errorMessage };
    QMessageBox::critical(this, title, description);
}


void MainWindow::setupPlaceholderText() const
{
    ui.typeSaisonLabel->setText(""); // Cacher le texte si l'utilisateur n'a pas encore mis de dates

    ui.inventaireTextEdit->setPlaceholderText(
        "Saisissez l'inventaire ici.\n\n"
        "Précisez le nombre (chiffre ou lettre) et le type d'objet séparés par des virgules.\n"
        "Exemple: 1 canapé trois place, 3 fauteuils, une chaise, une table 4 personnes, 4 chaises, etc...\n\n"
        "Notes: Il est possible de noter plusieurs fois les mêmes objets. Ils seront automatiquement comptés et additionés correctements dans le volume."
    );

    ui.nomLineEdit->setPlaceholderText("DUPONT");
    
    ui.prenomLineEdit->setPlaceholderText("David");

    ui.numTelLineEdit->setPlaceholderText("0199887766");

    ui.adresseDepartLineEdit->setPlaceholderText("Avenue de la Division Leclerc, 92160 ANTONY");
    ui.adresseLivraisonLineEdit->setPlaceholderText("Avenue de la Gare, 91300 MASSY");

    ui.distanceLineEdit->setPlaceholderText("406");
    ui.volumelineEdit->setPlaceholderText("30.4");
    ui.valeurAssuranceLineEdit->setPlaceholderText("25000");
}


void MainWindow::setupDateEdit() const
{
    QVector<QDateEdit*> datesEdit{ ui.departDateEdit, ui.livraisonDateEdit };

    for (auto& e : datesEdit)
    {
        e->setDate(QDate::currentDate());
        e->setDateRange(QDate::currentDate(), QDate::currentDate().addYears(2));
    }
}


void MainWindow::on_companyInfoPushButton_clicked()
{
    CompanyInfoDialog dialog{ *m_user, this };
    dialog.exec();
}

void MainWindow::on_modifyInventoryPushButton_clicked()
{
    auto dialog{ std::make_unique<InventoryModifyierDialog>(*m_client->getInventory(), this) };
    connect(dialog.get(), &InventoryModifyierDialog::removeItem, m_client->getInventory(), &Inventory::removeObject);
    dialog->exec();

    displayInventory(*(m_client->getInventory()));
    ui.volumelineEdit->setText(QString::number(m_client->getInventory()->getTotalVolume(), 'f', 2));
}


void MainWindow::on_nomLineEdit_editingFinished()
{
    m_client->setNom(ui.nomLineEdit->text());
}


void MainWindow::on_prenomLineEdit_editingFinished()
{
    m_client->setPrenom(ui.prenomLineEdit->text());
}


void MainWindow::on_numTelLineEdit_editingFinished()
{
    QString text{ ui.numTelLineEdit->text() };

    text.remove(QRegularExpression("[^0-9]"));

    if (text.isEmpty())
        return; // Champ vide, on ne fait rien

    constexpr int MAX_PHONE_NUMBER_LENGHT{ 10 };
    if (text.length() == MAX_PHONE_NUMBER_LENGHT && text.startsWith("0"))
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

    m_client->setNumTel(ui.numTelLineEdit->text());
}


void MainWindow::on_departDateEdit_editingFinished()
{
    const QDate dateChargement{ ui.departDateEdit->date() };
    const QDate dateLivraison{ ui.livraisonDateEdit->date() };

    updateSeasonTypeLabel(dateLivraison);

    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    if (dateLivraison.month() < dateChargement.month() ||
        dateLivraison.year() < dateChargement.year() ||
        (dateLivraison.month() == dateChargement.month() && dateLivraison.day() < dateChargement.day()))
    {
        ui.livraisonDateEdit->setDate(dateChargement);

        adresseArrivee.m_date = ui.livraisonDateEdit->date();
        m_client->setAdresseArrivee(adresseArrivee);
    }

    Adresse adresseDepart{ m_client->getAdresseDepart() };
    adresseDepart.m_date = ui.departDateEdit->date();
    m_client->setAdresseDepart(adresseDepart);
}


void MainWindow::on_adresseDepartLineEdit_editingFinished()
{
    Adresse adresseDepart{ m_client->getAdresseDepart() };
    adresseDepart.m_rue = ui.adresseDepartLineEdit->text();
    m_client->setAdresseDepart(adresseDepart);
}


void MainWindow::on_etageDepartSpinBox_editingFinished()
{
    Adresse adresseDepart{ m_client->getAdresseDepart() };
    adresseDepart.m_etage = ui.etageDepartSpinBox->value();
    m_client->setAdresseDepart(adresseDepart);
}


void MainWindow::on_ascDepartCheckBox_checked()
{
    Adresse adresseDepart{ m_client->getAdresseDepart() };
    adresseDepart.m_ascenseur = ui.ascDepartCheckBox->isChecked();
    m_client->setAdresseDepart(adresseDepart);
}


void MainWindow::on_mmDepartCheckBox_toggled(bool checked)
{
    Adresse adresseDepart{ m_client->getAdresseDepart() };
    adresseDepart.m_monteMeubles = checked;
    m_client->setAdresseDepart(adresseDepart);
}

void MainWindow::on_asDepartCheckBox_toggled(bool checked)
{
    Adresse adresseDepart{ m_client->getAdresseDepart() };
    adresseDepart.m_autStationnement = checked;
    m_client->setAdresseDepart(adresseDepart);
}

// Arrival address infos
void MainWindow::on_livraisonDateEdit_editingFinished()
{
    const QDate dateChargement{ ui.departDateEdit->date() };
    const QDate dateLivraison{ ui.livraisonDateEdit->date() };

    updateSeasonTypeLabel(dateLivraison);

    if (dateLivraison.month() < dateChargement.month() ||
        dateLivraison.year() < dateChargement.year() ||
        (dateLivraison.month() == dateChargement.month() && dateLivraison.day() < dateChargement.day()))
        ui.departDateEdit->setDate(dateLivraison);

    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    adresseArrivee.m_date = ui.livraisonDateEdit->date();
    m_client->setAdresseArrivee(adresseArrivee);
}

void MainWindow::on_adresseLivraisonLineEdit_editingFinished()
{
    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    adresseArrivee.m_rue = ui.adresseLivraisonLineEdit->text();
    m_client->setAdresseArrivee(adresseArrivee);
}

void MainWindow::on_etageLivraisonSpinBox_editingFinished()
{
    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    adresseArrivee.m_etage = ui.etageLivraisonSpinBox->value();
    m_client->setAdresseArrivee(adresseArrivee);
}

void MainWindow::on_ascLivraisonCheckBox_toggled(bool checked)
{
    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    adresseArrivee.m_ascenseur = checked;
    m_client->setAdresseArrivee(adresseArrivee);
}

void MainWindow::on_mmLivraisonCheckBox_toggled(bool checked)
{
    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    adresseArrivee.m_monteMeubles = checked;
    m_client->setAdresseArrivee(adresseArrivee);
}

void MainWindow::on_asLivraisonCheckBox_toggled(bool checked)
{
    Adresse adresseArrivee{ m_client->getAdresseArrivee() };
    adresseArrivee.m_autStationnement = checked;
    m_client->setAdresseArrivee(adresseArrivee);
}

// General infos
void MainWindow::on_distanceLineEdit_editingFinished()
{
    m_client->setDistance(ui.distanceLineEdit->text().toDouble());
}

void MainWindow::on_volumelineEdit_textChanged()
{
    m_client->setVolume(ui.volumelineEdit->text().toDouble());
}

void MainWindow::on_prestationComboBox_currentIndexChanged(int index)
{
    switch (index) 
    {
    case 0: m_client->setPrestation(Prestation::eco); break;
    case 1: m_client->setPrestation(Prestation::ecoPlus); break;
    case 2: m_client->setPrestation(Prestation::standard); break;
    case 3: m_client->setPrestation(Prestation::luxe); break;
    default: break;
    }
}

void MainWindow::on_natureComboBox_currentIndexChanged(int index)
{
    switch (index) 
    {
    case 0: m_client->setNature(Nature::urbain); break;
    case 1: m_client->setNature(Nature::special); break;
    case 2: m_client->setNature(Nature::groupage); break;
    default: break;
    }
}

void MainWindow::on_typeAssuranceComboBox_currentIndexChanged(int index)
{
    switch (index) 
    {
    case 0: m_client->setTypeAssurance(TypeAssurance::contractuelle); break;
    case 1: m_client->setTypeAssurance(TypeAssurance::dommage); break;
    default: break;
    }
}

void MainWindow::on_valeurAssuranceLineEdit_textChanged()
{   
    m_client->setValeurAssurance(ui.valeurAssuranceLineEdit->text().toInt());
}

void MainWindow::on_suppAdresseSpinBox_editingFinished()
{
    m_client->setNbAdresseSupp(ui.suppAdresseSpinBox->value());
}

void MainWindow::on_btnVoiceInput_clicked()
{
    // Bascule entre démarrer et arrêter la reconnaissance vocale
    // Analogie : C'est comme un interrupteur on/off

    if (m_speechRecognizer->isListening())
    {
        // Si on est en train d'écouter, on arrête
        m_speechRecognizer->stopListening();
        ui.btnVoiceInput->setText("Dicter l'inventaire");
        qDebug() << "Reconnaissance vocale arrêtée par l'utilisateur";
    }
    else
    {
        // Sinon, on démarre la reconnaissance
        bool success = m_speechRecognizer->startListening();

        if (success)
        {
            // Changer le texte du bouton pour indiquer qu'on écoute
            ui.btnVoiceInput->setText("Arrêter la dictée");
            qDebug() << "Reconnaissance vocale démarrée";
        }
        else
        {
            QMessageBox::warning(
                this,
                "Erreur",
                "Impossible de démarrer la reconnaissance vocale.\n"
                "Vérifiez que votre microphone est connecté et configuré."
            );
        }
    }
}

void MainWindow::onTextRecognized(const QString& text)
{
    // Cette méthode est appelée chaque fois que SAPI reconnaît du texte
    // On ajoute le texte progressivement dans le QTextEdit d'inventaire

    if (text.isEmpty())
    {
        return;
    }

    // Récupérer le contenu actuel du QTextEdit
    QString currentText = ui.inventaireTextEdit->toPlainText();

    // Ajouter le nouveau texte reconnu
    // Si le texte actuel n'est pas vide, on ajoute un espace avant
    if (!currentText.isEmpty() && !currentText.endsWith(' ') && !currentText.endsWith('\n'))
    {
        currentText += " ";
    }

    currentText += text;

    // Mettre à jour le QTextEdit
    ui.inventaireTextEdit->setPlainText(currentText);

    // Déplacer le curseur à la fin pour que l'utilisateur voie le nouveau texte
    QTextCursor cursor = ui.inventaireTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui.inventaireTextEdit->setTextCursor(cursor);

    qDebug() << "Texte ajouté à l'inventaire :" << text;
}
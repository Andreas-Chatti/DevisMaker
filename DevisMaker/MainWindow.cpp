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


    if (!areAllFieldCompleted())
    {
        QMessageBox::warning(this, "Champs manquants ou invalides", "Veuillez remplir correctement les champs avant de g\u00E9n\u00E9rer le devis");
        return;
    }


    // 1. Mettre à jour toutes les variables de l'onglet CLIENT depuis les champs rentrés par l'utilisateur


    updateClientVariables();


    // 2. Mettre à jour toutes les variables de l'onglet PARAMETRES depuis les champs rentrés par l'utilisateur

    
    updateSettingsVariables();


    // 3. Afficher les résultats dans l'onglet "Résultats et Devis"


    displayingResults();


    m_tarification.saveSettings();
}


void MainWindow::calculateDistance()
{
    QString adresseDepart{ ui.adresseDepartLineEdit->text() };
    QString adresseArrivee{ ui.adresseLivraisonLineEdit->text() };

    if (adresseDepart.isEmpty() || adresseArrivee.isEmpty())
        return;

    qDebug() << "Calcul de distance entre: " << adresseDepart << " et " << adresseArrivee;

    // Initialiser si nécessaire
    if (!m_networkManager) 
    {
        m_networkManager = new QNetworkAccessManager(this);
        connect(m_networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleDistanceReply);
    }

    // Première requête: géocoder l'adresse de départ
    QUrl url("https://nominatim.openstreetmap.org/search");
    QUrlQuery query;
    query.addQueryItem("format", "json");
    query.addQueryItem("q", adresseDepart);
    query.addQueryItem("limit", "1");

    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");

    QNetworkReply* reply{ m_networkManager->get(request) };
    reply->setProperty("adresseArrivee", adresseArrivee);
    reply->setProperty("requestType", "geocodeDepart");
}

void MainWindow::handleDistanceReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) 
    {
        QMessageBox::warning(this, "Erreur", "Erreur réseau: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QString requestType{ reply->property("requestType").toString() };
    QByteArray data{ reply->readAll() };
    QJsonDocument doc{ QJsonDocument::fromJson(data) };
    QJsonArray array{ doc.array() };

    if (array.isEmpty()) 
    {
        QMessageBox::warning(this, "Adress not found", "Cannot find specified adress");
        reply->deleteLater();
        return;
    }

    if (requestType == "geocodeDepart") 
    {
        // Obtenir les coordonnées de l'adresse de départ
        QJsonObject obj = array.at(0).toObject();
        QString lat = obj["lat"].toString();
        QString lon = obj["lon"].toString();

        // Géocoder l'adresse d'arrivée
        QString adresseArrivee{ reply->property("adresseArrivee").toString() };

        QUrl url("https://nominatim.openstreetmap.org/search");
        QUrlQuery query;
        query.addQueryItem("format", "json");
        query.addQueryItem("q", adresseArrivee);
        query.addQueryItem("limit", "1");

        url.setQuery(query);

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "DevisMaker/1.0");

        QNetworkReply* newReply{ m_networkManager->get(request) };
        newReply->setProperty("departLat", lat);
        newReply->setProperty("departLon", lon);
        newReply->setProperty("requestType", "geocodeArrivee");
    }

    else if (requestType == "geocodeArrivee") 
    {
        // Obtenir les coordonnées de l'adresse d'arrivée
        QJsonObject obj = array.at(0).toObject();
        QString arriveeLat = obj["lat"].toString();
        QString arriveeLon = obj["lon"].toString();

        // Récupérer les coordonnées de départ
        QString departLat = reply->property("departLat").toString();
        QString departLon = reply->property("departLon").toString();

        // Calculer la distance
        double distance = calculateHaversineDistance(
            departLat.toDouble(), departLon.toDouble(),
            arriveeLat.toDouble(), arriveeLon.toDouble()
        );

        // Afficher la distance calculée
        ui.distanceLineEdit->setText(QString::number(distance, 'f', 1));

        // Mettre à jour le modèle client
        m_client.setDistance(distance);

        qDebug() << "Distance calculée: " << distance << " km";
    }

    reply->deleteLater();
}

double MainWindow::calculateHaversineDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Rayon de la Terre en km
    constexpr double R{ 6371.0 };

    // Convertir en radians
    double dLat{ (lat2 - lat1) * M_PI / 180.0 };
    double dLon{ (lon2 - lon1) * M_PI / 180.0 };

    // Formule haversine
    double a{ sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
        sin(dLon / 2) * sin(dLon / 2) };

    double c{ 2 * atan2(sqrt(a), sqrt(1 - a)) };


    return R * c;
}


void MainWindow::on_volumelineEdit_textChanged(const QString& text)
{
    constexpr double maxValeurAssurance{ 45000 };

    // Vérifier si le texte est un nombre valide
    const double volume{ text.toDouble() };

    if (volume >= 0) 
    {
        // Calculer la valeur d'assurance (volume * 500)
        double valeurAssurance{ volume * 500.0 };

        if (valeurAssurance > maxValeurAssurance)
            valeurAssurance = maxValeurAssurance;

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
        ui.natureComboBox->setCurrentIndex(0);

    else if (distance > urbainMaxDistance)
        ui.natureComboBox->setCurrentIndex(1);
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


void MainWindow::updateClientVariables()
{
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
}


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
}


void MainWindow::displayingResults()
{
    // 1. Faire les calculs de tarification avec les variables qu'on a récupéré

    double volumeParPersonne{ m_tarification.calculerVolumeParPersonne(m_client.getVolume(), m_client.getPrestation()) };
    int nombreCamion{ m_tarification.calculerNombreCamion(m_client.getVolume(), m_client.getPrestation(), m_client.getNature(), m_client.getDistance()) };
    int nombreMO{ m_tarification.calculerNombreMO(m_client.getVolume(), m_client.getPrestation(), m_client.getNature(), nombreCamion, m_client.getAdresseDepart().m_monteMeubles || m_client.getAdresseArrivee().m_monteMeubles, m_client.getAdresseDepart().m_ascenseur || m_client.getAdresseArrivee().m_ascenseur, m_client.getDistance()) };

    double coutMOTotal{ m_tarification.calculerCoutMainOeuvreTotal(nombreMO) };

    double coutCamionTotal{ m_tarification.calculerCoutCamionTotal(nombreCamion) };

    double coutAutStatTotal{ m_tarification.calculerPrixStationnement(m_client.getAdresseDepart().m_autStationnement, m_client.getAdresseArrivee().m_autStationnement) };

    double fraisRouteTotal{ m_tarification.calculerCoutFraisRouteTotal(nombreCamion) };

    double coutAssurance{ m_tarification.calculerCoutAssurance(m_client.getValeurAssurance(), m_client.getTypeAssurance()) };

    m_tarification.setPrixMetreCube(m_client.getPrestation(), m_client.getNature(), m_client.getDistance());

    double prixTotalHT{ m_tarification.calculerCoutTotalHT(m_client.getVolume(), coutAssurance, coutAutStatTotal) };

    double arrhes{ m_tarification.calculerArrhes(prixTotalHT) };


    // 2. Afficher les résultats dans l'onglet "Résultats et Devis"

    // Format pour le volume (m³)
    ui.totalVolumeTextBrowser->setText(QString::number(m_client.getVolume(), 'f', 2) + " m\u00B3");

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
    ui.tableWidget->setColumnCount(2);
    ui.tableWidget->setHorizontalHeaderLabels({ "Objet", "Volume (m\u00B3)" });

    for (int i = 0; i < structuredItems.size(); ++i) {
        QString item = structuredItems[i];
        QStringList parts = item.split(" - ");

        if (parts.size() == 2) {
            ui.tableWidget->setItem(i, 0, new QTableWidgetItem(parts[0]));
            ui.tableWidget->setItem(i, 1, new QTableWidgetItem(parts[1]));
        }
    }

    // Ajuster la largeur des colonnes
    ui.tableWidget->resizeColumnsToContents();

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
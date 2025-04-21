#include "MainWindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setupConnections();

    // Initialiser la tarification avec des valeurs par défaut
    m_tarification.setTauxHoraireMO(220.0);
    m_tarification.setCoutKilometrique(1.5);
}

MainWindow::~MainWindow()
{
    // Qt détruit automatiquement ui car c'est un membre, pas un pointeur
}

void MainWindow::setupConnections()
{
    // Connectez explicitement vos boutons si nécessaire
    // Par exemple:
    // connect(ui.calculerButton, &QPushButton::clicked, this, &MainWindow::on_calculerButton_clicked);
}

void MainWindow::on_calculerButton_clicked()
{
    // Récupérer les informations du client depuis l'interface
    m_client.setNom(ui.nomLineEdit->text().toStdString());
    m_client.setAdresseDepart(ui.adresseDepartLineEdit->text().toStdString());
    m_client.setAdresseArrivee(ui.adresseArriveeLineEdit->text().toStdString());

    // Calculer la distance
    m_client.calculerDistance();

    // Analyser l'inventaire
    double volume = m_inventaire.analyser(ui.inventaireTextEdit->toPlainText().toStdString());

    // Calculer le coût
    double coutMainOeuvre = m_tarification.calculerCoutMainOeuvre(volume);
    double coutTransport = m_tarification.calculerCoutTransport(m_client.getDistance());
    double total = coutMainOeuvre + coutTransport;

    // Afficher le résultat
    ui.volumeResultatLabel->setText(QString::number(volume) + " m³");
    ui.coutMOResultatLabel->setText(QString::number(coutMainOeuvre) + " €");
    ui.coutTransportResultatLabel->setText(QString::number(coutTransport) + " €");
    ui.totalResultatLabel->setText(QString::number(total) + " €");
}

void MainWindow::on_genererDevisButton_clicked()
{
    // À implémenter plus tard
    QMessageBox::information(this, "Génération de devis", "Fonctionnalité à venir");
}

void MainWindow::on_nouvelInventaireButton_clicked()
{
    // Réinitialiser l'inventaire
    ui.inventaireTextEdit->clear();
}
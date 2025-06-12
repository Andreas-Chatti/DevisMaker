#pragma once
#include <QMainWindow>
#include <qvalidator.h>
#include <memory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QMessageBox>
#include <QTimer>
#include <cmath>
#include "AddressCompleter.h"
#include "ui_MainWindow.h"
#include "Client.h"
#include "Inventaire.h"
#include "Tarification.h"
#include "Devis.h"
#include "InventoryAnalyzer.h"
#include "streetMap.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        ui.setupUi(this);

        m_departCompleter = new AddressCompleter(ui.adresseDepartLineEdit, this);
        m_arriveeCompleter = new AddressCompleter(ui.adresseLivraisonLineEdit, this);

        m_openStreetMap = new OpenStreetMap(this);


        // Initialiser l'analyseur IA
        m_inventoryAnalyzer = new InventoryAnalyzer(this);

        connect(m_inventoryAnalyzer, &InventoryAnalyzer::analysisComplete, this, &MainWindow::handleInventoryAnalysis);

        connect(m_inventoryAnalyzer, &InventoryAnalyzer::analysisError, this, &MainWindow::handleInventoryAnalysisError);


        connect(ui.adresseDepartLineEdit, &QLineEdit::editingFinished, this, &OpenStreetMap::calculateDistance);
        connect(ui.adresseLivraisonLineEdit, &QLineEdit::editingFinished, this, &MainWindow::calculateDistance);


        setupValidators();

        setupSettings();
    }


    ~MainWindow()
    {
    }

private slots:

    void on_generateDevisButton_clicked();

    void on_volumelineEdit_textChanged(const QString& text);

    void on_distanceLineEdit_textChanged(const QString& text);

    void on_AnalyseInventoryPushButton_clicked();

    void handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems);

    void handleInventoryAnalysisError(const QString& errorMessage);

private:

    Ui::MainWindowClass ui;
    Client m_client;
    Inventaire m_inventaire;
    Tarification m_tarification;
    QNetworkAccessManager* m_networkManager{ nullptr };
    InventoryAnalyzer* m_inventoryAnalyzer;
    OpenStreetMap* m_openStreetMap;


    AddressCompleter* m_departCompleter{ nullptr };
    AddressCompleter* m_arriveeCompleter{ nullptr };


    void setupValidators();

    void setupSettings();


    bool areAllFieldCompleted();

    void updateClientVariables();

    void updateSettingsVariables();

    void displayingResults();


    void calculateDistance();
    void handleDistanceReply(QNetworkReply* reply);
    double calculateHaversineDistance(double lat1, double lon1, double lat2, double lon2);
};
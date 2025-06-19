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
#include "calculateurDevis.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr);

    ~MainWindow() = default;

private slots:

    void on_generateDevisButton_clicked();

    void on_volumelineEdit_textChanged(const QString& text);

    void on_distanceLineEdit_textChanged(const QString& text);

    void on_AnalyseInventoryPushButton_clicked();

    void handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems);

    void handleInventoryAnalysisError(const QString& errorMessage);

    void onDistanceCalculated(double distance);
    void onDistanceError(const QString& errorMessage);

    void on_suppAdresseCheckBox_toggled(bool isChecked);

private:

    Ui::MainWindowClass ui;
    Client m_client;
    Inventaire m_inventaire;
    Tarification m_tarification;
    QNetworkAccessManager* m_networkManager{ nullptr };
    InventoryAnalyzer* m_inventoryAnalyzer;
    OpenStreetMap* m_openStreetMap;
    CalculateurDevis* m_calculateurDevis;


    AddressCompleter* m_departCompleter{ nullptr };
    AddressCompleter* m_arriveeCompleter{ nullptr };


    ResultatsDevis calculerResultatsDevis();

    void setupValidators();

    void setupSettings();

    bool areAllFieldCompleted();

    void updateClientVariables();

    void updateSettingsVariables();

    void displayingResults();

    void setupDevisTable();

    void populateDevisTable(ResultatsDevis resultat);
};
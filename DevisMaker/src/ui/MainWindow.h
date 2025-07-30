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
#include "streetmap/AddressCompleter.h"
#include "ui_MainWindow.h"
#include "models/Client.h"
#include "utils/tarification.h"
#include "ia/InventoryAnalyzer.h"
#include "streetmap/streetMap.h"
#include "calculator/calculateurDevis.h"
#include "generators/pdfGenerator.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr);

    ~MainWindow() = default;

private slots:

    void on_generateDevisButton_clicked();

    void on_generatePdfButton_clicked();

    void on_volumelineEdit_textChanged(const QString& text);

    void on_distanceLineEdit_textChanged(const QString& text);

    void on_AnalyseInventoryPushButton_clicked();

    void handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems);

    void handleInventoryAnalysisError(const QString& errorMessage);

    void onDistanceCalculated(double distance);
    void onDistanceError(const QString& errorMessage);

    void on_suppAdresseCheckBox_toggled(bool isChecked);

    void on_numTelLineEdit_editingFinished();

    void on_pricePresetComboBox_currentIndexChanged(int index);

    void on_saveSettingsPushButton_clicked();

    void on_livraisonDateEdit_editingFinished();

    void on_departDateEdit_editingFinished();

    void onGenerateDevisStatusReport(PDFGenerator::PdfGenerationState generationState);

    void onCriticalError(const QString& errorMessage);

private:

    Ui::MainWindowClass ui;
    Client m_client;
    Tarification m_tarification;
    QNetworkAccessManager* m_networkManager{ nullptr };
    InventoryAnalyzer* m_inventoryAnalyzer;
    OpenStreetMap* m_openStreetMap;
    CalculateurDevis* m_calculateurDevis;
    PDFGenerator* m_PDFGenerator;


    AddressCompleter* m_departCompleter{ nullptr };
    AddressCompleter* m_arriveeCompleter{ nullptr };


    void setupValidators();

    void displaySettings();

    bool areAllFieldCompleted();

    void updateClientVariables();

    void updateSettingsVariables();

    void displayingResults();

    void setupDevisTable();

    void populateDevisTable(ResultatsDevis resultat);

    PricePreset determinePresetFromDates(const QDate& dateChargement, const QDate& dateLivraison) const;

    void updateSeasonTypeLabel(const QDate& dateLivraison) const;

    void setupPlaceholderText() const;
};
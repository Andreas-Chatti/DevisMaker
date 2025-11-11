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
#include "companyInfoDialog.h"
#include "InventoryModifyierDialog.h"
#include "user/user.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr);

    ~MainWindow() = default;

    static constexpr int WINDOW_WIDTH{ 800 };
    static constexpr int WINDOW_HEIGHT{ 600 };

private slots:

    void on_generateDevisButton_clicked();
    void on_generatePdfButton_clicked();

    void on_volumelineEdit_textChanged(const QString& text);

    void on_distanceLineEdit_textChanged(const QString& text);

    void on_AnalyseInventoryPushButton_clicked();
    void on_generateInventoryPushButton_clicked();

    void handleInventoryAnalysis(const Inventory& inventory);
    void handleInventoryAnalysisError(const QString& errorMessage);

    void onDistanceCalculated(double distance);
    void onDistanceError(const QString& errorMessage);

    void on_suppAdresseCheckBox_toggled(bool isChecked);

    void on_pricePresetComboBox_currentIndexChanged(int index);

    void on_saveSettingsPushButton_clicked();

    void onGenerateDevisStatusReport(PDFGenerator::PdfGenerationState generationState);

    void onCriticalError(const QString& errorMessage);

    void on_companyInfoPushButton_clicked();

    void on_modifyInventoryPushButton_clicked();


    // Updating Client variables in real time

    // Client personnal infos
    void on_nomLineEdit_editingFinished();
    void on_prenomLineEdit_editingFinished();
    void on_numTelLineEdit_editingFinished();

    // Departure adress infos
    void on_departDateEdit_editingFinished();
    void on_adresseDepartLineEdit_editingFinished();
    void on_etageDepartSpinBox_editingFinished();
    void on_ascDepartCheckBox_checked();
    void on_mmDepartCheckBox_toggled(bool checked);
    void on_asDepartCheckBox_toggled(bool checked);

    // Arrival address infos
    void on_livraisonDateEdit_editingFinished();
    void on_adresseLivraisonLineEdit_editingFinished();
    void on_etageLivraisonSpinBox_editingFinished();
    void on_ascLivraisonCheckBox_toggled(bool checked);
    void on_mmLivraisonCheckBox_toggled(bool checked);
    void on_asLivraisonCheckBox_toggled(bool checked);

    // General infos
    void on_distanceLineEdit_editingFinished();
    void on_volumelineEdit_textChanged();
    void on_prestationComboBox_currentIndexChanged(int index);
    void on_natureComboBox_currentIndexChanged(int index);
    void on_typeAssuranceComboBox_currentIndexChanged(int index);
    void on_valeurAssuranceLineEdit_textChanged();
    void on_suppAdresseSpinBox_editingFinished();

private:

    Ui::MainWindowClass ui;
    Client* m_client{ new Client{this} };
    Tarification* m_tarification{ new Tarification{this} };
    QNetworkAccessManager* m_networkManager{ nullptr };
    InventoryAnalyzer* m_inventoryAnalyzer{ new InventoryAnalyzer{this} };
    CalculateurDevis* m_calculateurDevis{ new CalculateurDevis{m_client, m_tarification} };
    PDFGenerator* m_PDFGenerator{ new PDFGenerator{this} };
    User* m_user{ new User{this} };
    AddressCompleter* m_addressCompleter{ nullptr };


    void setupValidators();

    void displaySettings();

    bool areAllFieldCompleted();

    void updateSettingsVariables();

    void displayingResults();

    void setupDevisTable();

    void populateDevisTable(const ResultatsDevis& resultat, const Tarification::PriceCalculation& calculationMethod);

    PricePreset determinePresetFromDates(const QDate& dateChargement, const QDate& dateLivraison) const;

    void updateSeasonTypeLabel(const QDate& dateLivraison) const;

    void setupPlaceholderText() const;

    void setupDateEdit() const;

    void displayInventory(const Inventory& inventory) const;
};
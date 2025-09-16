#pragma once
#include <QString>
#include <QTextDocument>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>
#include <QPrinter>
#include <QSettings>
#include "utils/resultatDevis.h"
#include "models/Client.h"
#include "utils/constants.h"
#include "user/user.h"

class PDFGenerator : public QObject
{
    Q_OBJECT

public:

    PDFGenerator(QObject* parent = nullptr)
        : m_htmlTemplate_m3{}
        , m_htmlTemplate_Postes{}
        , HTML_TEMPLATE_LOCATION_M3{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/devis_template_m3.html" }
        , HTML_TEMPLATE_LOCATION_POSTES{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/devis_template_postes.html" }
        , HTML_TEMPLATE_LOCATION_INVENTORY{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/inventory_template.html" }
    {
        m_htmlTemplate_m3 = load_HTML_Template(TypeDevis::PRIX_PAR_M3);
        m_htmlTemplate_Postes = load_HTML_Template(TypeDevis::CINQ_POSTES);
        m_htmlTemplate_Inventory = loadInventoryTemplate();
    }

    enum class PdfGenerationState
    {
        success,
        blankFile,
        errorLoadingFile,
        errorCreatingTemplateFile,
        errorCreatingTemplateDir,
    };

    enum class TypeDevis 
    {
        PRIX_PAR_M3,
        CINQ_POSTES
    };

    bool generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const TypeDevis& typeDevis, const User& user, const QString& outputPath = QString());
    bool generateInventoryPDF(const Client& client, const User& user, const QString& outputPath = QString());

    QString getNatureString(const Nature& nature) const;
    QString getPrestationString(const Prestation& prestation) const;

signals:

    void pdfGenerationStatusReport(PDFGenerator::PdfGenerationState generationState);

private:

    QString fillHTMLTemplate(const Client& client, const ResultatsDevis& resultats, QString& htmlTemplate, const User& user);
    QString fillInventoryTemplate(const Client& client, const User& user, QString& htmlTemplate);
    QString getDefaultOutputPath() const;
    QString formatCurrency(double value, const QString& suffix = " € H.T.") const;
    QString getCurrentDate() const;
    QString createSupplementsRows(const ResultatsDevis& resultats) const;
    QString load_HTML_Template(const TypeDevis& typeDevis);
    QString loadInventoryTemplate();
    QString get_Default_HTML_Template(const TypeDevis& typeDevis) const;
    QString getDefaultInventoryTemplate() const;
    QString generateClientNumber();
    QString generateInventoryRow(const QString& typeObjet, int quantite, double volumeIndividuel, double volumeTotal, bool demontage, bool remontage, bool dechetterie) const;
    bool createTemplateFile(const TypeDevis& typeDevis);
    bool createInventoryTemplateFile();
    bool createTemplateDir();

    const QString HTML_TEMPLATE_LOCATION_M3;
    const QString HTML_TEMPLATE_LOCATION_POSTES;
    const QString HTML_TEMPLATE_LOCATION_INVENTORY;
    QString m_htmlTemplate_m3;
    QString m_htmlTemplate_Postes;
    QString m_htmlTemplate_Inventory;
};

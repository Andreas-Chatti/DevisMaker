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
#include "utils/resultatDevis.h"
#include "models/Client.h"
#include "utils/constants.h"

class PDFGenerator : public QObject
{
    Q_OBJECT

public:

    PDFGenerator(QObject* parent = nullptr)
        : m_htmlTemplate_m3{}
        , m_htmlTemplate_Postes{}
        , HTML_TEMPLATE_LOCATION_M3{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/devis_template_m3.html" }
        , HTML_TEMPLATE_LOCATION_POSTES{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/devis_template_postes.html" }
    {
        m_htmlTemplate_m3 = load_HTML_Template(TypeDevis::PRIX_PAR_M3);
        m_htmlTemplate_Postes = load_HTML_Template(TypeDevis::CINQ_POSTES);
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

    bool generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const TypeDevis& typeDevis, const QString& outputPath = QString());

    QString getNatureString(const Nature& nature) const;
    QString getPrestationString(const Prestation& prestation) const;

signals:

    void pdfGenerationStatusReport(PDFGenerator::PdfGenerationState generationState);

private:

    QString fillHTMLTemplate(const Client& client, const ResultatsDevis& resultats, QString& htmlTemplate);
    QString getDefaultOutputPath() const;
    QString formatCurrency(double value, const QString& suffix = " € H.T.") const;
    QString getCurrentDate() const;
    QString createSupplementsRows(const ResultatsDevis& resultats) const;
    QString load_HTML_Template(const TypeDevis& typeDevis);
    QString get_Default_HTML_Template(const TypeDevis& typeDevis) const;
    bool createTemplateFile(const TypeDevis& typeDevis);
    bool createTemplateDir();

    const QString HTML_TEMPLATE_LOCATION_M3;
    const QString HTML_TEMPLATE_LOCATION_POSTES;
    QString m_htmlTemplate_m3;
    QString m_htmlTemplate_Postes;
};

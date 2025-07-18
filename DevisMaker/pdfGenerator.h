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
#include "resultatDevis.h"
#include "client.h"
#include "constants.h"

class PDFGenerator : public QObject
{
    Q_OBJECT

public:

    PDFGenerator(QObject* parent = nullptr)
        : m_htmlTemplate{}
        , HTML_TEMPLATE_LOCATION{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/devis_template.html" }
    {
        m_htmlTemplate = load_HTML_Template();
    }

    enum class PdfGenerationState
    {
        success,
        blankFile,
        errorLoadingFile,
        errorCreatingTemplateFile,
        errorCreatingTemplateDir,
    };

    bool generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const QString& outputPath = QString());

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
    QString load_HTML_Template();
    QString get_Default_HTML_Template() const;
    bool createTemplateFile();
    bool createTemplateDir();

    const QString HTML_TEMPLATE_LOCATION;
    QString m_htmlTemplate;
};

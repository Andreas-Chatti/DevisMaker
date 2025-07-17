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
public:

    PDFGenerator()
        : m_htmlTemplate{}
        , HTML_TEMPLATE_LOCATION{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH + "/devis_template.html" }
    {
        m_htmlTemplate = load_HTML_Template();
    }

    bool generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const QString& outputPath = QString());

    QString getNatureString(const Nature& nature) const;
    QString getPrestationString(const Prestation& prestation) const;

private:

    QString createHTMLTemplate(const Client& client, const ResultatsDevis& resultats);
    QString getDefaultOutputPath() const;
    QString formatCurrency(double value, const QString& suffix = " € H.T.") const;
    QString getCurrentDate() const;
    QString createSupplementsRows(const ResultatsDevis& resultats) const;
    QString load_HTML_Template() const;
    QString get_Default_HTML_Template() const;
    bool createTemplateFile() const;
    bool createTemplateDir() const;

    const QString HTML_TEMPLATE_LOCATION;
    QString m_htmlTemplate;
};

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

class PDFGenerator
{
public:

    PDFGenerator() = default;

    bool generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const QString& outputPath = QString()) const;

    QString getNatureString(const Nature& nature) const;
    QString getPrestationString(const Prestation& prestation) const;

private:

    QString createHTMLTemplate(const Client& client, const ResultatsDevis& resultats) const;
    QString getDefaultOutputPath() const;
    QString formatCurrency(double value, const QString& suffix = " € H.T.") const;
    QString getCurrentDate() const;
    QString createSupplementsRows(const ResultatsDevis& resultats) const;
};

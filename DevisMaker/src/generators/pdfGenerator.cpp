#include "pdfGenerator.h"


bool PDFGenerator::generateDevisPDF(const Client& client, const ResultatsDevis& resultats, TypeDevis typeDevis, const User& user, QString& outputPath)
{
    // Déterminer le chemin de sortie
    QString finalPath{ std::move(outputPath) };
    if (finalPath.isEmpty()) 
        finalPath = getDefaultOutputPath();

    QString m_htmlTemplate{ typeDevis == TypeDevis::PRIX_PAR_M3 ? m_htmlTemplate_m3 : m_htmlTemplate_Postes };
    if (m_htmlTemplate.isEmpty())
    {
        emit pdfGenerationStatusReport(PdfGenerationState::blankFile);
        return false;
    }

    // Créer le contenu HTML
    QString devis_html{ fillHTMLTemplate(client, resultats, m_htmlTemplate, user) };

    // Créer le document PDF
    QTextDocument document;
    document.setHtml(devis_html);

    // Configuration de l'impression
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(finalPath);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(1, 1, 1, 1), QPageLayout::Millimeter);

    // Générer le PDF
    document.print(&printer);

    emit pdfGenerationStatusReport(PdfGenerationState::success);

    return QFile::exists(finalPath);
}


QString PDFGenerator::fillHTMLTemplate(const Client& client, const ResultatsDevis& resultats, QString& htmlTemplate, const User& user)
{
    QString supplementsRows{ createSupplementsRows(resultats) };


    return htmlTemplate
        .replace("%CLIENT_NUMBER%", generateClientNumber())
        .replace("%DEVIS_NUMBER%", QString::number(QDateTime::currentSecsSinceEpoch() % 10000000))
        .replace("%DATE%", getCurrentDate())
        .replace("%VALIDITY_DATE%", QDate::currentDate().addDays(60).toString("dd/MM/yyyy"))
        .replace("%CLIENT_NAME%", client.getNom())
        .replace("%ADRESSE_DEPART%", client.getAdresseDepart().m_rue)
        .replace("%ADRESSE_ARRIVEE%", client.getAdresseArrivee().m_rue)
        .replace("%PHONE_NUMBER%", client.getNumTel())
        .replace("%PERIODE_CHARGEMENT%", QLocale{ QLocale::French, QLocale::France }.toString(client.getAdresseDepart().m_date, "dddd dd MMMM yyyy"))
        .replace("%PERIODE_LIVRAISON%", QLocale{ QLocale::French, QLocale::France }.toString(client.getAdresseArrivee().m_date, "dddd dd MMMM yyyy"))
        .replace("%ASCENSEUR_DEPART%", client.getAdresseDepart().m_ascenseur ? "Oui" : "Non")
        .replace("%ASCENSEUR_ARRIVEE%", client.getAdresseArrivee().m_ascenseur ? "Oui" : "Non")
        .replace("%MONTE_MEUBLES_DEPART%", client.getAdresseDepart().m_monteMeubles ? "Oui" : "Non")
        .replace("%MONTE_MEUBLES_ARRIVEE%", client.getAdresseArrivee().m_monteMeubles ? "Oui" : "Non")
        .replace("%STATIONNEMENT_DEPART%", client.getAdresseDepart().m_autStationnement ? "Oui" : "Non")
        .replace("%STATIONNEMENT_ARRIVEE%", client.getAdresseArrivee().m_autStationnement ? "Oui" : "Non")
        .replace("%VOLUME%", QString::number(client.getVolume(), 'f', 0))
        .replace("%DISTANCE%", QString::number(client.getDistance(), 'f', 0))
        .replace("%NATURE%", getNatureString(client.getNature()))
        .replace("%PRESTATION%", getPrestationString(client.getPrestation()))
        .replace("%MAIN_OEUVRE%", QString::number(resultats.resultatsCinqPostes.coutMainOeuvre, 'f', 2))
        .replace("%COUT_CAMION%", QString::number(resultats.resultatsCinqPostes.coutCamion, 'f', 2))
        .replace("%COUT_EMBALLAGE%", QString::number(resultats.resultatsCinqPostes.prixEmballage, 'f', 2))
        .replace("%COUT_LOCATION_MATERIEL%", QString::number(resultats.resultatsCinqPostes.prixLocMateriel, 'f', 2))
        .replace("%COUT_KILOMETRE%", QString::number(resultats.resultatsCinqPostes.prixKilometrage, 'f', 2))
        .replace("%ASSURANCE%", QString::number(resultats.coutAssurance, 'f', 2))
        .replace("%SUPPLEMENTS_ROWS%", supplementsRows)
        .replace("%TOTAL_HT%", QString::number(resultats.prixTotalHT, 'f', 2))
        .replace("%TVA%", QString::number(resultats.prixTotalHT * 0.20, 'f', 2))
        .replace("%TOTAL_TTC%", QString::number(resultats.prixTotalHT * 1.20, 'f', 2))
        .replace("%ARRHES%", QString::number(resultats.arrhes, 'f', 2))
        .replace("%SOLDE%", QString::number((resultats.prixTotalHT * 1.20) - resultats.arrhes, 'f', 2))
        .replace("%EMISSION_CO2%", QString::number(client.getDistance() * 0.42, 'f', 1))
        .replace("%PRIX_FORFAITAIRE%", QString::number(resultats.prixMetreCube * client.getVolume(), 'f', 2))
        .replace("%COMPANY_NAME%", user.getCompanyName())
        .replace("%COMPANY_ADRESS%", user.getCompanyAddress())
        .replace("%TVA_NUMBER%", user.getTvaNumber())
        .replace("%SIRET_NUMBER%", user.getSiretNumber())
        .replace("%APE_NUMBER%", user.getApeNumber())
        .replace("%COMPANY_NUMBER%", user.getCompanyPhoneNumber())
        .replace("%COMPANY_EMAIL%", user.getCompanyMail());
}


QString PDFGenerator::fillInventoryTemplate(const Client& client, const User& user, QString& htmlTemplate)
{
    QString supplementsRows{ generateInventoryRow(client.getInventory()) };

    return htmlTemplate
        .replace("%CLIENT_NUMBER%", generateClientNumber())
        .replace("%DATE%", getCurrentDate())
        .replace("%CLIENT_NAME%", client.getNom())
        .replace("%ADRESSE_CHARGEMENT%", client.getAdresseDepart().m_rue)
        .replace("%ADRESSE_LIVRAISON%", client.getAdresseArrivee().m_rue)
        .replace("%PHONE_NUMBER%", client.getNumTel())
        .replace("%PERIODE_CHARGEMENT%", QLocale{ QLocale::French, QLocale::France }.toString(client.getAdresseDepart().m_date, "dddd dd MMMM yyyy"))
        .replace("%PERIODE_LIVRAISON%", QLocale{ QLocale::French, QLocale::France }.toString(client.getAdresseArrivee().m_date, "dddd dd MMMM yyyy"))
        .replace("%VOLUME_TOTAL%", QString::number(client.getVolume(), 'f', 2))
        .replace("%COMPANY_NAME%", user.getCompanyName())
        .replace("%COMPANY_ADRESS%", user.getCompanyAddress())
        .replace("%COMPANY_NUMBER%", user.getCompanyPhoneNumber())
        .replace("%TVA_NUMBER%", user.getTvaNumber())
        .replace("%SIRET_NUMBER%", user.getSiretNumber())
        .replace("%APE_NUMBER%", user.getApeNumber())
        .replace("%COMPANY_EMAIL%", user.getCompanyMail())
        .replace("%INVENTAIRE_ROWS%", supplementsRows);
}


QString PDFGenerator::getNatureString(const Nature& nature) const
{
    switch (nature) 
    {
    case Nature::urbain: return "Urbain";
    case Nature::special: return "Spécial";
    case Nature::groupage: return "Groupage";
    default: return "UNDEFINED";
    }
}

QString PDFGenerator::getPrestationString(const Prestation& prestation) const
{
    switch (prestation) 
    {
    case Prestation::eco: return "ECONOMIQUE";
    case Prestation::ecoPlus: return "ECONOMIQUE +";
    case Prestation::standard: return "STANDARD";
    case Prestation::luxe: return "LUXE";
    default: return "UNDEFINED";
    }
}


QString PDFGenerator::createSupplementsRows(const ResultatsDevis& resultats) const
{
    QString rows;

    // Style inline identique aux autres lignes du tableau prix (padding réduit)
    QString cellStyleLeft = "border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;";
    QString cellStyleRight = "border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;";

    if (resultats.coutStationnement > 0) 
    {
        rows += QString("<tr><td style=\"%1\"><strong>Autorisation de stationnement</strong></td><td style=\"%2\">%3 €</td></tr>")
            .arg(cellStyleLeft)
            .arg(cellStyleRight)
            .arg(QString::number(resultats.coutStationnement, 'f', 2));
    }

    if (resultats.fraisMonteMeubles > 0) 
    {
        rows += QString("<tr><td style=\"%1\"><strong>Monte-meubles</strong></td><td style=\"%2\">%3 €</td></tr>")
            .arg(cellStyleLeft)
            .arg(cellStyleRight)
            .arg(QString::number(resultats.fraisMonteMeubles, 'f', 2));
    }

    if (resultats.prixSuppAdresse > 0) 
    {
        rows += QString("<tr><td style=\"%1\"><strong>Supplément adresse</strong></td><td style=\"%2\">%3 €</td></tr>")
            .arg(cellStyleLeft)
            .arg(cellStyleRight)
            .arg(QString::number(resultats.prixSuppAdresse, 'f', 2));
    }

    if (resultats.fraisRoute > 0) 
    {
        rows += QString("<tr><td style=\"%1\"><strong>Frais de route</strong></td><td style=\"%2\">%3 €</td></tr>")
            .arg(cellStyleLeft)
            .arg(cellStyleRight)
            .arg(QString::number(resultats.fraisRoute, 'f', 2));
    }

    return rows;
}


QString PDFGenerator::formatCurrency(double value, const QString& suffix) const
{
    return QString::number(value, 'f', 2) + suffix;
}


QString PDFGenerator::getCurrentDate() const
{
    return QDate::currentDate().toString("dd/MM/yyyy");
}


QString PDFGenerator::getDefaultOutputPath() const
{
    QString documentsPath{ QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) };
    QString fileName{ QString("Devis_DevisMaker_%1.pdf").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")) };

    return QDir(documentsPath).filePath(fileName);
}


QString PDFGenerator::load_HTML_Template(const TypeDevis& typeDevis)
{
    QString HTML_TEMPLATE_LOCATION{ typeDevis == TypeDevis::PRIX_PAR_M3 ? HTML_TEMPLATE_LOCATION_M3 : HTML_TEMPLATE_LOCATION_POSTES };

    QFile templateFile{ HTML_TEMPLATE_LOCATION };
    QFileInfo templateFileInfos{ templateFile };
    QDir templateDir{ templateFileInfos.absoluteDir() };

    if (!templateDir.exists())
        createTemplateDir();

    if (!templateFile.exists())
        createTemplateFile(typeDevis);

    if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorLoadingFile);
        qDebug() << "Erreur: Impossible d'ouvrir le fichier template:" << HTML_TEMPLATE_LOCATION;
        return QString{};
    }

    QTextStream lecteurTexte{ &templateFile };
    lecteurTexte.setEncoding(QStringConverter::Utf8);

    return lecteurTexte.readAll();
}


QString PDFGenerator::loadInventoryTemplate()
{
    QFile templateFile{ HTML_TEMPLATE_LOCATION_INVENTORY };
    QFileInfo templateFileInfos{ templateFile };
    QDir templateDir{ templateFileInfos.absoluteDir() };

    if (!templateDir.exists())
        createTemplateDir();

    if (!templateFile.exists())
        createInventoryTemplateFile();

    if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorLoadingFile);
        qDebug() << "Erreur: Impossible d'ouvrir le fichier template:" << HTML_TEMPLATE_LOCATION_INVENTORY;
        return QString{};
    }

    QTextStream lecteurTexte{ &templateFile };
    lecteurTexte.setEncoding(QStringConverter::Utf8);

    return lecteurTexte.readAll();
}


bool PDFGenerator::createInventoryTemplateFile()
{
    QFile templateFile{ HTML_TEMPLATE_LOCATION_INVENTORY };

    if (!templateFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorCreatingTemplateFile);
        qDebug() << "Erreur: Impossible de creer le fichier templates";
        return false;
    }

    QTextStream ecrivain(&templateFile);
    ecrivain.setEncoding(QStringConverter::Utf8);

    ecrivain << getDefaultInventoryTemplate();


    if (ecrivain.status() != QTextStream::Ok)
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorCreatingTemplateFile);
        qDebug() << "Erreur lors de l'écriture du template";
        return false;
    }

    qDebug() << "Creation du fichier devis_template.html avec succes !";
    return true;
}


bool PDFGenerator::createTemplateFile(const TypeDevis& typeDevis)
{
    QString HTML_TEMPLATE_LOCATION{ typeDevis == TypeDevis::PRIX_PAR_M3 ? HTML_TEMPLATE_LOCATION_M3 : HTML_TEMPLATE_LOCATION_POSTES };

    QFile templateFile{ HTML_TEMPLATE_LOCATION };

    if (!templateFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorCreatingTemplateFile);
        qDebug() << "Erreur: Impossible de creer le fichier templates";
        return false;
    }

    QTextStream ecrivain(&templateFile);
    ecrivain.setEncoding(QStringConverter::Utf8);

    ecrivain << get_Default_HTML_Template(typeDevis);


    if (ecrivain.status() != QTextStream::Ok) 
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorCreatingTemplateFile);
        qDebug() << "Erreur lors de l'écriture du template";
        return false;
    }

    qDebug() << "Creation du fichier devis_template.html avec succes !";
    return true;
}


bool PDFGenerator::createTemplateDir()
{
    qDebug() << "Creation du dossier 'templates' ...";

    QDir templateDir{ SettingsConstants::FileSettings::TEMPLATE_FILE_PATH };

    if(!templateDir.mkpath("."))
    {
        emit pdfGenerationStatusReport(PdfGenerationState::errorCreatingTemplateDir);
        qDebug() << "Impossible de creer le dossier templates";
        return false;
    }

    qDebug() << "Dossier templates creer avec succes !";
    return true;
}


QString PDFGenerator::generateClientNumber()
{
    QSettings settings;
    int lastClientNumber{ settings.value("lastClientNumber", 1000).toInt() };
    int newClientNumber{ lastClientNumber + 1 };
    settings.setValue("lastClientNumber", newClientNumber);

    return QString::number(newClientNumber);
}


QString PDFGenerator::get_Default_HTML_Template(const TypeDevis& typeDevis) const
{
    QString htmlHeader = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0; 
            padding: 0;
            color: #000;
            font-size: 10px;
            line-height: 1.2;
            background-color: #f5f5f5;
        }
        
        .container {
            width: 100%;
            margin: 0 auto;
            padding: 15px;
        }
        
        @media print {
            body {
                padding: 0;
                margin: 0;
            }
            .container {
                width: 100%;
                padding: 10mm;
                margin: 0;
            }
        }
        
        .header {
            margin-bottom: 15px;
            border-bottom: 2px solid #000;
            padding-bottom: 10px;
        }
        
        .company-name {
            font-size: 16px;
            font-weight: bold;
            margin-bottom: 5px;
            color: #2c3e50;
        }
        
        .company-details {
            font-size: 9px;
            line-height: 1.3;
        }
        
        .main-table {
            width: 100%;
            min-width: 100%;
            border-collapse: collapse;
            border: 2px solid #000;
            margin-bottom: 0px;
        }
        
        .main-table td {
            padding: 4px;
            font-size: 9px;
            text-align: center;
            vertical-align: middle;
        }
        
        .header-cell {
            background-color: #34495e;
            color: white;
            font-weight: bold;
            padding: 4px;
            font-size: 9px;
            text-align: center;
        }
        
        .yellow-cell {
            background-color: #f1c40f;
            color: #000;
            font-weight: bold;
        }
        
        .left-header-cell {
            background-color: #ecf0f1;
            font-weight: bold;
            text-align: center;
            width: 15%;
        }
        
        .text-left {
            text-align: left;
        }
        
        .price-section {
            margin-top: 0px;
        }
        
        .legal-section {
            margin-top: 10px;
            font-size: 8px;
            line-height: 1.2;
            padding: 8px;
            background-color: #f8f9fa;
        }
        
        .signature-section {
            margin-top: 10px;
            text-align: center;
            font-size: 9px;
            padding: 8px;
            border: 1px dashed #000;
        }
        
        .footer {
            margin-top: 10px;
            text-align: center;
            font-size: 7px;
            border-top: 2px solid #000;
            padding-top: 6px;
            color: #7f8c8d;
        }
        
        .access-details {
            font-size: 8px;
            line-height: 1.2;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="company-name">%COMPANY_NAME%</div>
            <div class="company-details">
                %COMPANY_ADRESS%<br>
                Tél: %COMPANY_NUMBER%<br>
                E-mail: <span style="color: #3498db; text-decoration: underline;">%COMPANY_EMAIL%</span>
            </div>
        </div>
    )");

    QString tableauCaracteristiques = QString(R"(
        <table class="main-table" width="100%" style="width: 100%;">
            <tr>
                <td class="header-cell" colspan="2" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Client: %CLIENT_NAME%</td>
                <td class="header-cell" colspan="2" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Client n°: %CLIENT_NUMBER%</td>
                <td class="header-cell" colspan="2" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Devis N°: %DEVIS_NUMBER%</td>
                <td class="header-cell" colspan="2" style="border-right: 0px; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Validité: %VALIDITY_DATE%</td>
            </tr>
            <tr>
                <td class="header-cell" colspan="2" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Volume: %VOLUME% m³</td>
                <td class="header-cell" colspan="2" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Distance: %DISTANCE% km</td>
                <td class="header-cell" colspan="2" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Nature: %NATURE%</td>
                <td class="header-cell yellow-cell" colspan="2" style="border-right: 0px; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Prestation: %PRESTATION%</td>
            </tr>
            <tr>
                <td class="header-cell" colspan="8" style="border-right: 0px; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Téléphone: %PHONE_NUMBER%</td>
            </tr>
        </table>
    )");

    QString tableauChargementLivraison = QString(R"(
        <table class="main-table" width="100%" style="width: 100%;">
            <tr>
                <td style="width: 15%; background-color: #ecf0f1; border: 1px solid #000; padding: 4px;"></td>
                <td class="header-cell" style="width: 42.5%; border-right: 1px solid #000; border-left: 1px solid #000; border-top: 1px solid #000; border-bottom: 1px solid #000;">Chargement</td>
                <td class="header-cell" style="width: 42.5%; border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000;">Livraison</td>
            </tr>
            <tr>
                <td class="left-header-cell" style="border: 1px solid #000;">Période</td>
                <td style="border-right: 1px solid #000; border-left: 1px solid #000; border-top: 1px solid #000; border-bottom: 1px solid #000; text-align: center; padding: 4px;">%PERIODE_CHARGEMENT%</td>
                <td style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000; text-align: center; padding: 4px;">%PERIODE_LIVRAISON%</td>
            </tr>
            <tr>
                <td class="left-header-cell" style="border: 1px solid #000;">Adresse</td>
                <td class="text-left" style="border-right: 1px solid #000; border-left: 1px solid #000; border-top: 1px solid #000; border-bottom: 1px solid #000; padding: 4px;">
                    %ADRESSE_DEPART%<br>
                    FRANCE
                </td>
                <td class="text-left" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000; padding: 4px;">
                    %ADRESSE_ARRIVEE%<br>
                    FRANCE
                </td>
            </tr>
            <tr>
                <td class="left-header-cell" style="border: 1px solid #000;">Accès</td>
                <td class="text-left" style="border-right: 1px solid #000; border-left: 1px solid #000; border-top: 1px solid #000; border-bottom: 1px solid #000; padding: 4px;">
                    <div class="access-details">
                        <strong>Ascenseur:</strong> %ASCENSEUR_DEPART%<br>
                        <strong>Monte-meubles:</strong> %MONTE_MEUBLES_DEPART%<br>
                        <strong>Autorisation stationnement:</strong> %STATIONNEMENT_DEPART%
                    </div>
                </td>
                <td class="text-left" style="border-right: 1px solid #000; border-left: 0px; border-top: 1px solid #000; border-bottom: 1px solid #000; padding: 4px;">
                    <div class="access-details">
                        <strong>Ascenseur:</strong> %ASCENSEUR_ARRIVEE%<br>
                        <strong>Monte-meubles:</strong> %MONTE_MEUBLES_ARRIVEE%<br>
                        <strong>Autorisation stationnement:</strong> %STATIONNEMENT_ARRIVEE%
                    </div>
                </td>
            </tr>
        </table>
    )");

    QString tableauPrix;

    QString tableauPrixHeader = QString(R"(
        <div class="price-section">
            <table width="100%" style="width: 100%; border-collapse: collapse; border: 2px solid #000; margin-bottom: 0px;">
                <tr>
                    <td colspan="2" style="background-color: #2c3e50; color: white; border-left: 1px solid #000; border-right: 1px solid #000; border-top: 1px solid #000; border-bottom: 1px solid #000; text-align: center; padding: 6px; font-weight: bold; font-size: 12px;">Détail du prix</td>
                </tr>
    )");

    QString lignesPrix;
    if (typeDevis == TypeDevis::PRIX_PAR_M3)
    {
        lignesPrix = QString(R"(
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Prix forfaitaire</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%PRIX_FORFAITAIRE% €</td>
                </tr>
        )");
    }

    else
    {
        lignesPrix = QString(R"(
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Main d'œuvre</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%MAIN_OEUVRE% €</td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Coût camion</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%COUT_CAMION% €</td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Frais kilométriques</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%COUT_KILOMETRE% €</td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Matériel d'emballage</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%COUT_EMBALLAGE% €</td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Location matériel</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%COUT_LOCATION_MATERIEL% €</td>
                </tr>
        )");
    }

    QString tableauPrixFooter = QString(R"(
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Assurance Garantie responsabilité Contractuelle</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%ASSURANCE% €</td>
                </tr>
                %SUPPLEMENTS_ROWS%
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; background-color: #c0392b; color: white; font-weight: bold;"><strong>Total H.T.</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #c0392b; color: white;"><strong>%TOTAL_HT% €</strong></td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;">TVA de 20.00%</td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;">%TVA% €</td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; background-color: #1e8449; color: white; font-weight: bold; font-size: 12px;"><strong>PRIX TTC EN EUROS (valable jusqu'au %VALIDITY_DATE%)</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 12px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #1e8449; color: white;"><strong>%TOTAL_TTC% €</strong></td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; background-color: #2471a3; color: white; font-weight: bold;"><strong>Arrhes (30% à la commande)</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #2471a3; color: white;"><strong>%ARRHES% €</strong></td>
                </tr>
                <tr>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle;"><strong>Solde à la livraison</strong></td>
                    <td style="border: 1px solid #000; border-top: 0px; padding: 3px; font-size: 10px; vertical-align: middle; text-align: right; font-weight: bold; width: 120px; background-color: #f8f9fa;"><strong>%SOLDE% €</strong></td>
                </tr>
            </table>
        </div>
    )");

    tableauPrix = tableauPrixHeader + lignesPrix + tableauPrixFooter;

    QString htmlFooter = QString(R"(
        <div class="legal-section">
            <p><strong>Le prix est définitif (article 1er de l'arrêté du 27 avril 2010) sauf cas précisés dans l'article 6 des CGV jointes au devis.</strong></p>
            
            <p><strong>Émission CO2 selon art L1431-3 Code Transport: %EMISSION_CO2% kg</strong></p>
            
            <p>En accord avec les termes de ce présent devis et de nos conditions générales de vente ci-jointes, nous vous prions de bien vouloir nous retourner ces documents signés et accompagnés du chèque d'arrhes correspondant à la prestation choisie afin de confirmer votre déménagement. Le solde sera à régler à la livraison de votre mobilier.</p>
        </div>
        
        <div class="signature-section">
            <strong>Fait à MARCOUSSIS, le %DATE%</strong><br><br>
            <strong>Signature du client:</strong><br><br><br>
            ____________________________
        </div>

        <div class="footer">
            <strong>%COMPANY_NAME% - TVA intracommunautaire: %TVA_NUMBER%</strong><br>
            <strong>SIRET: %SIRET_NUMBER% - APE: %APE_NUMBER% - RCS: Evry A 390 329 894 000 54</strong>
        </div>
        
    </div>
</body>
</html>
    )");

    return htmlHeader + tableauCaracteristiques + tableauChargementLivraison + tableauPrix + htmlFooter;
}


QString PDFGenerator::getDefaultInventoryTemplate() const
{
    QString htmlHeader = QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
</head>
<body style="font-family: Arial, sans-serif; margin: 0; padding: 0; color: #000; font-size: 10px; line-height: 1.2; background-color: #f5f5f5;">
    <div style="width: 100%; margin: 0 auto; padding: 15px;">
        <div style="margin-bottom: 15px; border-bottom: 2px solid #000; padding-bottom: 10px;">
            <div style="font-size: 16px; font-weight: bold; margin-bottom: 5px; color: #2c3e50;">%COMPANY_NAME%</div>
            <div style="font-size: 9px; line-height: 1.3;">
                %COMPANY_ADRESS%<br>
                Tél: %COMPANY_NUMBER%<br>
                E-mail: <span style="color: #3498db; text-decoration: underline;">%COMPANY_EMAIL%</span>
            </div>
        </div>
        
        <div style="font-size: 18px; font-weight: bold; text-align: center; margin: 15px 0; color: #2c3e50; text-transform: uppercase;">Inventaire de Déménagement</div>
    )");

    QString tableauInfosClient = QString(R"(
        <table width="100%" style="width: 100%; min-width: 100%; border-collapse: collapse; border: 2px solid #000; margin-bottom: 15px;">
            <tr>
                <td colspan="4" style="background-color: #34495e; color: white; font-weight: bold; padding: 4px; font-size: 9px; text-align: center; border: 1px solid #000;">Informations Client</td>
            </tr>
            <tr>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; width: 20%; padding: 4px; font-size: 9px; border: 1px solid #000;">Client N°:</td>
                <td style="padding: 4px; font-size: 9px; text-align: center; border: 1px solid #000; vertical-align: middle;">%CLIENT_NUMBER%</td>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; width: 20%; padding: 4px; font-size: 9px; border: 1px solid #000;">Nom et Prénom:</td>
                <td style="padding: 4px; font-size: 9px; text-align: left; border: 1px solid #000; vertical-align: middle;">%CLIENT_NAME%</td>
            </tr>
            <tr>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; padding: 4px; font-size: 9px; border: 1px solid #000;">Adresse Chargement:</td>
                <td style="padding: 4px; font-size: 9px; text-align: left; border: 1px solid #000; vertical-align: middle;">%ADRESSE_CHARGEMENT%</td>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; padding: 4px; font-size: 9px; border: 1px solid #000;">Adresse Livraison:</td>
                <td style="padding: 4px; font-size: 9px; text-align: left; border: 1px solid #000; vertical-align: middle;">%ADRESSE_LIVRAISON%</td>
            </tr>
            <tr>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; padding: 4px; font-size: 9px; border: 1px solid #000;">Période Chargement:</td>
                <td style="padding: 4px; font-size: 9px; text-align: left; border: 1px solid #000; vertical-align: middle;">%PERIODE_CHARGEMENT%</td>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; padding: 4px; font-size: 9px; border: 1px solid #000;">Période Livraison:</td>
                <td style="padding: 4px; font-size: 9px; text-align: left; border: 1px solid #000; vertical-align: middle;">%PERIODE_LIVRAISON%</td>
            </tr>
            <tr>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; padding: 4px; font-size: 9px; border: 1px solid #000;">Téléphone:</td>
                <td style="padding: 4px; font-size: 9px; text-align: center; border: 1px solid #000; vertical-align: middle;">%PHONE_NUMBER%</td>
                <td style="background-color: #ecf0f1; font-weight: bold; text-align: left; padding: 4px; font-size: 9px; border: 1px solid #000;">Volume Total:</td>
                <td style="padding: 4px; font-size: 12px; font-weight: bold; text-align: center; background-color: #f8f9fa; border: 1px solid #000; vertical-align: middle;">%VOLUME_TOTAL% m³</td>
            </tr>
        </table>
    )");

    QString tableauInventaireHeader = QString(R"(
        <table width="100%" style="width: 100%; min-width: 100%; border-collapse: collapse; border: 2px solid #000; margin-bottom: 15px;">
            <tr>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 9px; text-align: center; border: 1px solid #000; width: 25%; vertical-align: middle;">Type d'Objet</td>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 9px; text-align: center; border: 1px solid #000; width: 8%; vertical-align: middle;">Quantité</td>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 9px; text-align: center; border: 1px solid #000; width: 12%; vertical-align: middle;">Volume Individuel (m³)</td>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 9px; text-align: center; border: 1px solid #000; width: 12%; vertical-align: middle;">Volume Total (m³)</td>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 8px; text-align: center; border: 1px solid #000; width: 14%; vertical-align: middle;">Démontage</td>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 8px; text-align: center; border: 1px solid #000; width: 14%; vertical-align: middle;">Remontage</td>
                <td style="background-color: #34495e; color: white; font-weight: bold; padding: 6px; font-size: 8px; text-align: center; border: 1px solid #000; width: 15%; vertical-align: middle;">Lourd</td>
            </tr>
            %INVENTAIRE_ROWS%
    )");

    QString tableauInventaireFooter = QString(R"(
            <tr>
                <td colspan="3" style="background-color: #2c3e50; color: white; font-weight: bold; font-size: 11px; text-align: right; padding: 4px; border: 1px solid #000; vertical-align: middle;"><strong>VOLUME TOTAL:</strong></td>
                <td style="background-color: #2c3e50; color: white; font-weight: bold; font-size: 11px; text-align: center; padding: 4px; border: 1px solid #000; vertical-align: middle;"><strong>%VOLUME_TOTAL% m³</strong></td>
                <td colspan="3" style="background-color: #2c3e50; border: 1px solid #000;"></td>
            </tr>
        </table>
    )");

    QString htmlFooter = QString(R"(
        <div style="margin-top: 20px; font-size: 9px; line-height: 1.4;">
            <p style="font-weight: bold;">Observations particulières:</p>
            <div style="border: 1px solid #000; min-height: 40px; padding: 5px; margin-bottom: 15px;">
            </div>
            
            <table width="100%" style="width: 100%; margin-top: 20px;">
                <tr>
                    <td style="text-align: center; width: 50%; vertical-align: top;">
                        <p style="font-weight: bold;">Signature du Client:</p>
                        <div style="border-bottom: 1px solid #000; width: 200px; height: 50px; margin: 10px auto;"></div>
                        <p style="font-size: 8px;">Date: %DATE%</p>
                    </td>
                    <td style="text-align: center; width: 50%; vertical-align: top;">
                        <p style="font-weight: bold;">Signature du Déménageur:</p>
                        <div style="border-bottom: 1px solid #000; width: 200px; height: 50px; margin: 10px auto;"></div>
                        <p style="font-size: 8px;">Date: %DATE%</p>
                    </td>
                </tr>
            </table>
        </div>

        <div style="margin-top: 10px; text-align: center; font-size: 7px; border-top: 2px solid #000; padding-top: 6px; color: #7f8c8d;">
            <strong>%COMPANY_NAME% - TVA intracommunautaire: %TVA_NUMBER%</strong><br>
            <strong>SIRET: %SIRET_NUMBER% - APE: %APE_NUMBER%</strong><br>
            <strong>Inventaire généré automatiquement - Version du %DATE%</strong>
        </div>
        
    </div>
</body>
</html>
    )");

    return htmlHeader + tableauInfosClient + tableauInventaireHeader + tableauInventaireFooter + htmlFooter;
}


QString PDFGenerator::generateInventoryRow(const Inventory* const inventory) const
{
    QString supplementRows{};
    const auto& areaList{ inventory->getAreas() };
    for (const auto& area : areaList)
    {
        // TODO : Générer une ligne pour démarquer la pièce actuelle et ses objets
        const auto& objectList{ area.getObjectsList() };
        for (const auto& object : objectList)
        {
            supplementRows += QString(R"(
        <tr>
            <td style="padding: 4px; font-size: 9px; text-align: left; border: 1px solid #000;">%1</td>
            <td style="padding: 4px; font-size: 9px; text-align: center; border: 1px solid #000;">%2</td>
            <td style="padding: 4px; font-size: 9px; text-align: center; border: 1px solid #000;">%3</td>
            <td style="padding: 4px; font-size: 9px; text-align: center; background-color: #f8f9fa; font-weight: bold; border: 1px solid #000;">%4</td>
            <td style="padding: 4px; font-size: 12px; text-align: center; border: 1px solid #000;">%5</td>
            <td style="padding: 4px; font-size: 12px; text-align: center; border: 1px solid #000;">%6</td>
            <td style="padding: 4px; font-size: 12px; text-align: center; border: 1px solid #000;">%7</td>
        </tr>
        )").arg(object.getName())
                .arg(QString::number(object.getQuantity())) // object quantity
                .arg(QString::number(object.getUnitaryVolume(), 'f', 2)) // object unitary volume
                .arg(QString::number(object.getTotalVolume(), 'f', 2)) // total object's volume based on quantity
                .arg(QString{ object.isDisassembly() ? "☑" : "☐" })
                .arg(QString{ object.isAssembly() ? "☑" : "☐" })
                .arg(QString{ object.isHeavy() ? "☑" : "☐" });
        }
    }
    return supplementRows;
}


bool PDFGenerator::generateInventoryPDF(const Client& client, const User& user, QString& outputPath)
{
    // Déterminer le chemin de sortie
    QString finalPath{ std::move(outputPath) };
    if (finalPath.isEmpty())
        finalPath = getDefaultOutputPath();

    if (m_htmlTemplate_Inventory.isEmpty())
    {
        emit pdfGenerationStatusReport(PdfGenerationState::blankFile);
        return false;
    }

    // Créer le contenu HTML
    QString devis_html{ fillInventoryTemplate(client, user, m_htmlTemplate_Inventory)};

    // Créer le document PDF
    QTextDocument document;
    document.setHtml(devis_html);

    // Configuration de l'impression
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(finalPath);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(1, 1, 1, 1), QPageLayout::Millimeter);

    // Générer le PDF
    document.print(&printer);

    emit pdfGenerationStatusReport(PdfGenerationState::success);

    return QFile::exists(finalPath);
}
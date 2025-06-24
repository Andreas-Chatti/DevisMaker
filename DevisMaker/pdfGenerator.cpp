#include "pdfGenerator.h"


bool PDFGenerator::generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const QString& outputPath) const
{
    // Déterminer le chemin de sortie
    QString finalPath{ outputPath };
    if (finalPath.isEmpty()) 
        finalPath = getDefaultOutputPath();

    // Créer le contenu HTML
    QString htmlContent{ createHTMLTemplate(client, resultats) };

    // Créer le document PDF
    QTextDocument document;
    document.setHtml(htmlContent);

    // ✅ Configuration de l'impression
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(finalPath);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(1, 1, 1, 1), QPageLayout::Millimeter);

    // ✅ Générer le PDF
    document.print(&printer);

    return QFile::exists(finalPath);
}


QString PDFGenerator::createHTMLTemplate(const Client& client, const ResultatsDevis& resultats) const
{
    // Créer les lignes de suppléments seulement si nécessaire
    QString supplementsRows = createSupplementsRows(resultats);

    return QString(R"(
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
        
        /* Container principal - notre "carré centré" qui prend toute la largeur */
        .container {
            max-width: 210mm;
            width: 100%;
            margin: 0 auto;
            padding: 15px;
            background-color: white;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            border-radius: 8px;
        }
        
        /* Styles d'impression - important pour que ça marche bien à l'impression */
        @media print {
            body {
                background-color: white;
                padding: 0;
                margin: 0;
            }
            .container {
                max-width: none;
                width: 100%;
                padding: 10mm;
                box-shadow: none;
                margin: 0;
                border-radius: 0;
            }
        }
        
        /* En-tête principal */
        .header {
            display: flex;
            justify-content: space-between;
            margin-bottom: 15px;
            border-bottom: 2px solid #000;
            padding-bottom: 10px;
        }
        
        .left-header {
            flex: 1;
        }
        
        .right-header {
            flex: 1;
            text-align: right;
        }
        
        .company-name {
            font-size: 16px;
            font-weight: bold;
            margin-bottom: 5px;
            color: #2c3e50;
        }
        
        .company-details, .client-info {
            font-size: 9px;
            line-height: 1.3;
        }
        
        /* Section TVA */
        .tva-section {
            text-align: center;
            font-size: 8px;
            font-weight: bold;
            margin: 10px 0;
            padding: 8px;
            background-color: #ecf0f1;
            border-radius: 4px;
        }
        
        /* Tableaux */
        .main-table {
            width: 100%;
            border-collapse: collapse;
            border: 2px solid #000;
            margin-bottom: 15px;
        }
        
        .main-table th, .main-table td {
            border: 1px solid #000;
            padding: 4px;
            font-size: 9px;
            text-align: center;
            vertical-align: middle;
        }
        
        .main-table th {
            background-color: #34495e;
            color: white;
            font-weight: bold;
        }
        
        .yellow-cell {
            background-color: #f1c40f !important;
            color: #000 !important;
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
        
        /* Section prix - notre tableau principal simplifié */
        .price-section {
            margin-top: 15px;
        }
        
        .price-header {
            background-color: #2c3e50;
            color: white;
            border: 2px solid #000;
            text-align: center;
            padding: 8px;
            font-weight: bold;
            font-size: 12px;
        }
        
        .price-table {
            width: 100%;
            border-collapse: collapse;
            border: 2px solid #000;
            border-top: none;
        }
        
        .price-table td {
            border: 1px solid #000;
            padding: 6px;
            font-size: 10px;
            vertical-align: middle;
        }
        
        .price-amount {
            text-align: right;
            font-weight: bold;
            width: 120px;
            background-color: #f8f9fa;
        }
        
        .total-ht {
            background-color: #c0392b;
            color: white;
            font-weight: bold;
        }
        
        .total-ht .price-amount {
            background-color: #c0392b;
            color: white;
            font-weight: bold;
        }
        
        .total-ttc {
            background-color: #1e8449;
            color: white;
            font-weight: bold;
            font-size: 12px;
        }
        
        .total-ttc .price-amount {
            background-color: #1e8449;
            color: white;
            font-weight: bold;
            font-size: 12px;
        }
        
        .arrhes-row {
            background-color: #2471a3;
            color: white;
            font-weight: bold;
        }
        
        .arrhes-row .price-amount {
            background-color: #2471a3;
            color: white;
            font-weight: bold;
        }
        
        /* Section légale */
        .legal-section {
            margin-top: 15px;
            font-size: 8px;
            line-height: 1.3;
            padding: 10px;
            background-color: #f8f9fa;
            border-radius: 4px;
        }
        
        .signature-section {
            margin-top: 15px;
            text-align: center;
            font-size: 9px;
            padding: 10px;
            border: 1px dashed #000;
        }
        
        /* Footer */
        .footer {
            margin-top: 15px;
            text-align: center;
            font-size: 7px;
            border-top: 2px solid #000;
            padding-top: 8px;
            color: #7f8c8d;
        }
        
        /* Détails d'accès */
        .access-details {
            font-size: 8px;
            line-height: 1.2;
        }
    </style>
</head>
<body>
    <div class="container">
        <!-- En-tête avec logo et informations -->
        <div class="header">
            <div class="left-header">
                <div class="company-name">CHATTI DEMENAGEMENT</div>
                <div class="company-details">
                    2, Rue Jean Cocteau<br>
                    91460 MARCOUSSIS<br>
                    Tél: 01 69 75 18 22<br>
                    E-mail: <span style="color: #3498db; text-decoration: underline;">chattidemenagement@gmail.com</span>
                </div>
            </div>
            <div class="right-header">
                <div class="client-info">
                    <strong>Client n° %CLIENT_NUMBER%</strong><br>
                    <strong>Devis n° %DEVIS_NUMBER%</strong><br>
                    Date %DATE%<br>
                    Validité %VALIDITY_DATE%<br><br>
                    <strong>Mr %CLIENT_NAME%</strong><br>
                    %ADRESSE_CLIENT%
                </div>
            </div>
        </div>

        <!-- Section TVA -->
        <div class="tva-section">
            CHATTI DEMENAGEMENT - TVA intracommunautaire: FR68390329894<br>
            SIRET: 39032989400054 - APE: 4942Z - RCS: Evry A 390 329 894 000 54
        </div>

        <!-- Tableau caractéristiques -->
        <table class="main-table">
            <tr>
                <th>Client</th>
                <th>%CLIENT_NAME%</th>
                <th>Client n°</th>
                <th>%CLIENT_NUMBER%</th>
                <th>Devis N°</th>
                <th>%DEVIS_NUMBER%</th>
                <th>Validité</th>
                <th>%VALIDITY_DATE%</th>
            </tr>
            <tr>
                <th>Volume</th>
                <th>%VOLUME% m³</th>
                <th>Distance</th>
                <th>%DISTANCE% km</th>
                <th>Nature</th>
                <th>%NATURE%</th>
                <th>Prestation</th>
                <th class="yellow-cell">%PRESTATION%</th>
            </tr>
        </table>

        <!-- Tableau principal Chargement/Livraison -->
        <table class="main-table">
            <tr>
                <th style="width: 15%;"></th>
                <th style="width: 42.5%;">Chargement</th>
                <th style="width: 42.5%;">Livraison</th>
            </tr>
            <tr>
                <td class="left-header-cell">Période</td>
                <td>%PERIODE_CHARGEMENT%</td>
                <td>%PERIODE_LIVRAISON%</td>
            </tr>
            <tr>
                <td class="left-header-cell">Adresse</td>
                <td class="text-left">
                    %ADRESSE_DEPART%<br>
                    FRANCE
                </td>
                <td class="text-left">
                    %ADRESSE_ARRIVEE%<br>
                    FRANCE
                </td>
            </tr>
            <tr>
                <td class="left-header-cell">Téléphone</td>
                <td>%PHONE_DEPART%</td>
                <td>%PHONE_DEPART%</td>
            </tr>
            <tr>
                <td class="left-header-cell">Accès</td>
                <td class="text-left">
                    <div class="access-details">
                        <strong>Ascenseur:</strong> %ASCENSEUR_DEPART%<br>
                        <strong>Monte-meubles:</strong> %MONTE_MEUBLES_DEPART%<br>
                        <strong>Autorisation stationnement:</strong> %STATIONNEMENT_DEPART%
                    </div>
                </td>
                <td class="text-left">
                    <div class="access-details">
                        <strong>Ascenseur:</strong> %ASCENSEUR_ARRIVEE%<br>
                        <strong>Monte-meubles:</strong> %MONTE_MEUBLES_ARRIVEE%<br>
                        <strong>Autorisation stationnement:</strong> %STATIONNEMENT_ARRIVEE%
                    </div>
                </td>
            </tr>
        </table>

        <!-- Section détail du prix - SIMPLIFIÉ -->
        <div class="price-section">
            <div class="price-header">Détail du prix</div>
            <table class="price-table">
                <tr>
                    <td><strong>Main d'œuvre qualifiée déménageurs professionnels</strong></td>
                    <td class="price-amount">%MAIN_OEUVRE% €</td>
                </tr>
                <tr>
                    <td><strong>Camion en circuit organisé (licence intérieur et communautaire)</strong></td>
                    <td class="price-amount">%COUT_CAMION% €</td>
                </tr>
                <tr>
                    <td><strong>Kilomètres</strong></td>
                    <td class="price-amount">%COUT_KILOMETRE% €</td>
                </tr>
                <tr>
                    <td><strong>Assurance Garantie responsabilité Contractuelle</strong></td>
                    <td class="price-amount">%ASSURANCE% €</td>
                </tr>
                %SUPPLEMENTS_ROWS%
                <tr class="total-ht">
                    <td><strong>Total H.T.</strong></td>
                    <td class="price-amount"><strong>%TOTAL_HT% €</strong></td>
                </tr>
                <tr>
                    <td>TVA de 20.00%</td>
                    <td class="price-amount">%TVA% €</td>
                </tr>
                <tr class="total-ttc">
                    <td><strong>PRIX TTC EN EUROS (valable jusqu'au %VALIDITY_DATE%)</strong></td>
                    <td class="price-amount"><strong>%TOTAL_TTC% €</strong></td>
                </tr>
                <tr class="arrhes-row">
                    <td><strong>Arrhes (30% à la commande)</strong></td>
                    <td class="price-amount"><strong>%ARRHES% €</strong></td>
                </tr>
                <tr>
                    <td><strong>Solde à la livraison</strong></td>
                    <td class="price-amount"><strong>%SOLDE% €</strong></td>
                </tr>
            </table>
        </div>

        <!-- Section légale -->
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

        <!-- Footer -->
        <div class="footer">
            <strong>CHATTI DEMENAGEMENT - TVA intracommunautaire: FR68390329894</strong><br>
            <strong>SIRET: 39032989400054 - APE: 4942Z - RCS: Evry A 390 329 894 000 54</strong>
        </div>
        
    </div> <!-- Fin container -->
</body>
</html>
    )")
        // Informations de base (gardées)
        .replace("%CLIENT_NUMBER%", "951")
        .replace("%DEVIS_NUMBER%", QString::number(QDateTime::currentSecsSinceEpoch() % 10000000))
        .replace("%DATE%", getCurrentDate())
        .replace("%VALIDITY_DATE%", QDate::currentDate().addDays(60).toString("dd/MM/yyyy"))
        .replace("%CLIENT_NAME%", client.getNom())
        .replace("%ADRESSE_CLIENT%", QString::fromStdString(client.getAdresseDepart().m_rue))
        .replace("%ADRESSE_DEPART%", QString::fromStdString(client.getAdresseDepart().m_rue))
        .replace("%ADRESSE_ARRIVEE%", QString::fromStdString(client.getAdresseArrivee().m_rue))
        .replace("%PHONE_DEPART%", "01 69 75 18 22")
        .replace("%PERIODE_CHARGEMENT%", QDate::currentDate().toString("MMMM yyyy"))
        .replace("%PERIODE_LIVRAISON%", QDate::currentDate().toString("MMMM yyyy"))

        // Informations d'accès (gardées et simplifiées)
        .replace("%ASCENSEUR_DEPART%", client.getAdresseDepart().m_ascenseur ? "Oui" : "Non")
        .replace("%ASCENSEUR_ARRIVEE%", client.getAdresseArrivee().m_ascenseur ? "Oui" : "Non")
        .replace("%MONTE_MEUBLES_DEPART%", client.getAdresseDepart().m_monteMeubles ? "Oui" : "Non")
        .replace("%MONTE_MEUBLES_ARRIVEE%", client.getAdresseArrivee().m_monteMeubles ? "Oui" : "Non")
        .replace("%STATIONNEMENT_DEPART%", client.getAdresseDepart().m_autStationnement ? "Oui" : "Non")
        .replace("%STATIONNEMENT_ARRIVEE%", client.getAdresseArrivee().m_autStationnement ? "Oui" : "Non")

        // Informations du déménagement (gardées)
        .replace("%VOLUME%", QString::number(client.getVolume(), 'f', 0))
        .replace("%DISTANCE%", QString::number(client.getDistance(), 'f', 0))
        .replace("%NATURE%", getNatureString(client.getNature()))
        .replace("%PRESTATION%", getPrestationString(client.getPrestation()))

        // COÛTS PRINCIPAUX (gardés - ce sont nos éléments essentiels)
        .replace("%MAIN_OEUVRE%", QString::number(resultats.coutMainOeuvre, 'f', 2))
        .replace("%COUT_CAMION%", QString::number(resultats.coutCamion, 'f', 2))
        .replace("%COUT_KILOMETRE%", QString::number(resultats.fraisRoute, 'f', 2))
        .replace("%ASSURANCE%", QString::number(resultats.coutAssurance, 'f', 2))

        // Suppléments (seulement si nécessaire)
        .replace("%SUPPLEMENTS_ROWS%", supplementsRows)

        // Totaux (gardés)
        .replace("%TOTAL_HT%", QString::number(resultats.prixTotalHT, 'f', 2))
        .replace("%TVA%", QString::number(resultats.prixTotalHT * 0.20, 'f', 2))
        .replace("%TOTAL_TTC%", QString::number(resultats.prixTotalHT * 1.20, 'f', 2))
        .replace("%ARRHES%", QString::number(resultats.arrhes, 'f', 2))
        .replace("%SOLDE%", QString::number((resultats.prixTotalHT * 1.20) - resultats.arrhes, 'f', 2))
        .replace("%EMISSION_CO2%", QString::number(client.getDistance() * 0.42, 'f', 1));
}


QString PDFGenerator::getNatureString(Nature nature) const
{
    switch (nature) 
    {
    case Nature::urbain: return "Urbain";
    case Nature::special: return "Spécial";
    case Nature::groupage: return "Groupage";
    default: return "Urbain";
    }
}

QString PDFGenerator::getPrestationString(Prestation prestation) const
{
    switch (prestation) 
    {
    case Prestation::eco: return "ECONOMIQUE";
    case Prestation::ecoPlus: return "ECONOMIQUE +";
    case Prestation::standard: return "STANDARD";
    case Prestation::luxe: return "LUXE";
    default: return "STANDARD";
    }
}

QString PDFGenerator::createFraisRouteRow(const ResultatsDevis& resultats) const
{
    if (resultats.fraisRoute > 0) 
    {
        return QString("<tr><td>Frais de route</td><td class=\"price-amount\">%1 €</td></tr>")
            .arg(QString::number(resultats.fraisRoute, 'f', 0));
    }
    return "";
}



QString PDFGenerator::createSupplementsRows(const ResultatsDevis& resultats) const
{
    QString rows;

    if (resultats.coutStationnement > 0)
        rows += QString("<tr><td>Autorisation de stationnement</td><td class=\"amount\">%1</td></tr>").arg(formatCurrency(resultats.coutStationnement, " € H.T."));

    if (resultats.fraisMonteMeubles > 0)
        rows += QString("<tr><td>Monte-meubles</td><td class=\"amount\">%1</td></tr>").arg(formatCurrency(resultats.fraisMonteMeubles, " € H.T."));

    if (resultats.prixDechetterie > 0)
        rows += QString("<tr><td>Déchetterie</td><td class=\"amount\">%1</td></tr>").arg(formatCurrency(resultats.prixDechetterie, " € H.T."));

    if (resultats.prixSuppAdresse > 0)
        rows += QString("<tr><td>Supplément adresse</td><td class=\"amount\">%1</td></tr>").arg(formatCurrency(resultats.prixSuppAdresse, " € H.T."));

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
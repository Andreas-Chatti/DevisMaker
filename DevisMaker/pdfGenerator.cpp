#include "pdfGenerator.h"


bool PDFGenerator::generateDevisPDF(const Client& client, const ResultatsDevis& resultats, const QString& outputPath) const
{
    // ✅ Déterminer le chemin de sortie
    QString finalPath{ outputPath };
    if (finalPath.isEmpty()) 
        finalPath = getDefaultOutputPath();

    // ✅ Créer le contenu HTML
    QString htmlContent{ createHTMLTemplate(client, resultats) };

    // ✅ Créer le document PDF
    QTextDocument document;
    document.setHtml(htmlContent);

    // ✅ Configuration de l'impression
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(finalPath);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    // ✅ Générer le PDF
    document.print(&printer);

    return QFile::exists(finalPath);
}


QString PDFGenerator::createHTMLTemplate(const Client& client, const ResultatsDevis& resultats) const
{
    return QString(R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0; 
            padding: 15px;
            color: #000;
            font-size: 11px;
        }
        
        .header {
            display: table;
            width: 100%;
            margin-bottom: 20px;
        }
        
        .logo-section {
            display: table-cell;
            width: 40%;
            vertical-align: top;
        }
        
        .company-info {
            display: table-cell;
            width: 60%;
            text-align: right;
            vertical-align: top;
            font-size: 10px;
        }
        
        .company-name {
            font-size: 24px;
            font-weight: bold;
            color: #1a4c96;
            margin-bottom: 5px;
        }
        
        .company-subtitle {
            font-size: 14px;
            color: #666;
            margin-bottom: 15px;
        }
        
        .devis-info {
            border: 1px solid #000;
            padding: 8px;
            margin: 20px 0;
            text-align: center;
            background-color: #f0f0f0;
        }
        
        .devis-details {
            display: table;
            width: 100%;
            margin-bottom: 15px;
        }
        
        .devis-left, .devis-right {
            display: table-cell;
            width: 50%;
            padding: 5px;
        }
        
        .client-table {
            width: 100%;
            border-collapse: collapse;
            margin: 15px 0;
            border: 1px solid #000;
        }
        
        .client-table th {
            background-color: #e0e0e0;
            border: 1px solid #000;
            padding: 8px;
            text-align: center;
            font-weight: bold;
            font-size: 10px;
        }
        
        .client-table td {
            border: 1px solid #000;
            padding: 8px;
            font-size: 10px;
            vertical-align: top;
        }
        
        .characteristics-table {
            width: 100%;
            border-collapse: collapse;
            margin: 15px 0;
            border: 1px solid #000;
        }
        
        .characteristics-table td {
            border: 1px solid #000;
            padding: 5px;
            font-size: 10px;
            text-align: center;
        }
        
        .char-header {
            background-color: #e0e0e0;
            font-weight: bold;
        }
        
        .pricing-section {
            margin-top: 20px;
        }
        
        .pricing-title {
            text-align: center;
            font-weight: bold;
            background-color: #e0e0e0;
            padding: 8px;
            border: 1px solid #000;
            margin-bottom: 0;
        }
        
        .pricing-table {
            width: 100%;
            border-collapse: collapse;
            border: 1px solid #000;
        }
        
        .pricing-table td {
            border: 1px solid #000;
            padding: 6px;
            font-size: 10px;
        }
        
        .price-amount {
            text-align: right;
            font-weight: bold;
        }
        
        .total-row {
            background-color: #e0e0e0;
            font-weight: bold;
            font-size: 11px;
        }
        
        .conditions {
            margin-top: 20px;
            font-size: 9px;
            text-align: justify;
            line-height: 1.3;
        }
        
        .signature-section {
            margin-top: 30px;
            display: table;
            width: 100%;
        }
        
        .signature-left, .signature-right {
            display: table-cell;
            width: 50%;
            text-align: center;
            font-size: 10px;
            vertical-align: top;
        }
        
        .footer {
            margin-top: 30px;
            text-align: center;
            font-size: 9px;
            border-top: 1px solid #000;
            padding-top: 10px;
        }
        
        .prestation-highlight {
            background-color: #ffff00;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="header">
        <div class="logo-section">
            <div class="company-name">DEVIS MAKER</div>
            <div class="company-subtitle">Déménagement</div>
            <div style="font-size: 10px; margin-top: 10px;">
                <strong>DEVIS MAKER</strong><br>
                2 Rue de l'Innovation<br>
                75000 PARIS<br>
                Tél: 01 23 45 67 89<br>
                Email: contact@devismaker.com
            </div>
        </div>
        <div class="company-info">
            Client n°: %CLIENT_NUMBER%<br>
            Devis n°: %DEVIS_NUMBER%<br>
            Date: %DATE%<br>
            Validité: %VALIDITY_DATE%
        </div>
    </div>

    <div class="devis-info">
        <div class="devis-details">
            <div class="devis-left">
                <strong>Client:</strong> %CLIENT_NAME%<br>
                <strong>Client n°:</strong> %CLIENT_NUMBER%
            </div>
            <div class="devis-right">
                <strong>Devis n°:</strong> %DEVIS_NUMBER%<br>
                <strong>Validité:</strong> %VALIDITY_DATE%
            </div>
        </div>
    </div>

    <table class="client-table">
        <thead>
            <tr>
                <th style="width: 15%;">Période</th>
                <th style="width: 42.5%;">Chargement %CHARGE_DATE%</th>
                <th style="width: 42.5%;">Livraison %DELIVERY_DATE%</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td style="font-weight: bold;">Adresse</td>
                <td>%ADRESSE_DEPART%</td>
                <td>%ADRESSE_ARRIVEE%</td>
            </tr>
            <tr>
                <td style="font-weight: bold;">Téléphone</td>
                <td>%PHONE_DEPART%</td>
                <td>%PHONE_ARRIVEE%</td>
            </tr>
            <tr>
                <td style="font-weight: bold;">Accès</td>
                <td>
                    Escalier: %ESCALIER_DEPART%<br>
                    Passage: %PASSAGE_DEPART%<br>
                    Stationnement: %STATIONNEMENT_DEPART%<br>
                    Etage: %ETAGE_DEPART%
                </td>
                <td>
                    Escalier: %ESCALIER_ARRIVEE%<br>
                    Passage: %PASSAGE_ARRIVEE%<br>
                    Stationnement: %STATIONNEMENT_ARRIVEE%<br>
                    Etage: %ETAGE_ARRIVEE%
                </td>
            </tr>
        </tbody>
    </table>

    <table class="characteristics-table">
        <tr class="char-header">
            <td>Volume</td>
            <td>Distance</td>
            <td>Nature</td>
            <td>Prestation</td>
        </tr>
        <tr>
            <td>%VOLUME% m3</td>
            <td>%DISTANCE% km</td>
            <td>%NATURE%</td>
            <td class="prestation-highlight">%PRESTATION%</td>
        </tr>
    </table>

    <div class="pricing-section">
        <div class="pricing-title">Détail du prix</div>
        <table class="pricing-table">
            <tr>
                <td>Camion de déménagement et personnel qualifié 4h minimum</td>
                <td class="price-amount">%COUT_MO% €</td>
            </tr>
            <tr>
                <td>Déplacement</td>
                <td class="price-amount">%COUT_CAMION% €</td>
            </tr>
            <tr>
                <td>Frais d'emballage (uniquement déménagement Professionnel)</td>
                <td class="price-amount">%COUT_EMBALLAGE% €</td>
            </tr>
            %FRAIS_ROUTE_ROW%
            %SUPPLEMENTS_ROWS%
            <tr>
                <td>Assurance Générale responsabilité Contractuelle</td>
                <td class="price-amount">%COUT_ASSURANCE% €</td>
            </tr>
            <tr class="total-row">
                <td><strong>PRIX T.T.C EN EUROS (valable jusqu'au %VALIDITY_DATE%)</strong></td>
                <td class="price-amount"><strong>%TOTAL_TTC% €</strong></td>
            </tr>
            <tr>
                <td>30% d'arrhes à la commande. Solde à la livraison</td>
                <td class="price-amount">%ARRHES% €</td>
            </tr>
        </table>
    </div>

    <div class="signature-section">
        <div class="signature-left">
            Nous espérons l'acceptation<br>
            1 personne encadrement d'équipe<br>
            Bon pour accord et pour l'avance
        </div>
        <div class="signature-right">
            Prenomsignature client<br>
            Cachet et 2 signatures<br>
            Date et L.B.P
        </div>
    </div>

    <div class="conditions">
        En cas de débit, nous nous réservons le droit de s'y faire part par tous moyens 
        que nous jugerons opportune, tous frais de recouvrement à la charge du débiteur. 
        Les frais de recouvrement s'élèvent forfaitairement à l'exécution de livres commande 
        et accompagnée du règlement d'arrhes correspondant à la commande. 
        Le solde étant exigible à la livraison soit la remise des documents sous H.
        <br><br>
        <strong>Fait à PARIS/BANLIEUE, le %DATE%</strong>
    </div>

    <div class="footer">
        <strong>DEVIS MAKER DÉMÉNAGEMENT - Tél déménagement: 01 23 45 67 89</strong><br>
        SIRET: 12345678901234 - APE: 4942Z - RCS: Paris - TVA: FR12345678901
    </div>
</body>
</html>
    )")
        .replace("%CLIENT_NUMBER%", "001")
        .replace("%DEVIS_NUMBER%", QString::number(QDateTime::currentSecsSinceEpoch() % 100000))
        .replace("%DATE%", getCurrentDate())
        .replace("%VALIDITY_DATE%", QDate::currentDate().addDays(30).toString("dd/MM/yyyy"))
        .replace("%CLIENT_NAME%", client.getNom())
        .replace("%CHARGE_DATE%", getCurrentDate())
        .replace("%DELIVERY_DATE%", getCurrentDate())
        .replace("%ADRESSE_DEPART%", QString::fromStdString(client.getAdresseDepart().m_rue))
        .replace("%ADRESSE_ARRIVEE%", QString::fromStdString(client.getAdresseArrivee().m_rue))
        .replace("%PHONE_DEPART%", "Non renseigné")
        .replace("%PHONE_ARRIVEE%", "Non renseigné")
        .replace("%ESCALIER_DEPART%", client.getAdresseDepart().m_ascenseur ? "Ascenseur" : "Non")
        .replace("%ESCALIER_ARRIVEE%", client.getAdresseArrivee().m_ascenseur ? "Ascenseur" : "Non")
        .replace("%PASSAGE_DEPART%", "Normal")
        .replace("%PASSAGE_ARRIVEE%", "Normal")
        .replace("%STATIONNEMENT_DEPART%", client.getAdresseDepart().m_autStationnement ? "Facile" : "Normal")
        .replace("%STATIONNEMENT_ARRIVEE%", client.getAdresseArrivee().m_autStationnement ? "Facile" : "Normal")
        .replace("%ETAGE_DEPART%", QString::number(client.getAdresseDepart().m_etage))
        .replace("%ETAGE_ARRIVEE%", QString::number(client.getAdresseArrivee().m_etage))
        .replace("%VOLUME%", QString::number(client.getVolume(), 'f', 2))
        .replace("%DISTANCE%", QString::number(client.getDistance(), 'f', 0))
        .replace("%NATURE%", getNatureString(client.getNature()))
        .replace("%PRESTATION%", getPrestationString(client.getPrestation()))
        .replace("%COUT_MO%", QString::number(resultats.coutMainOeuvre, 'f', 0))
        .replace("%COUT_CAMION%", QString::number(resultats.coutCamion, 'f', 0))
        .replace("%COUT_EMBALLAGE%", "0")  // À adapter selon vos besoins
        .replace("%COUT_ASSURANCE%", QString::number(resultats.coutAssurance, 'f', 0))
        .replace("%TOTAL_TTC%", QString::number(resultats.prixTotalHT * 1.20, 'f', 0))  // +20% TVA
        .replace("%ARRHES%", QString::number(resultats.arrhes, 'f', 0))
        .replace("%FRAIS_ROUTE_ROW%", createFraisRouteRow(resultats))
        .replace("%SUPPLEMENTS_ROWS%", createSupplementsRows(resultats));
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
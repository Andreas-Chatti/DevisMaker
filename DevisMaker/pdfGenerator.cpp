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
    printer.setPageMargins(QMarginsF(5, 2, 5, 5), QPageLayout::Millimeter);

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
            padding: 5px;
            color: #000;
            font-size: 8px;
            line-height: 1.0;
        }
        
        /* En-tête principal */
        .main-header {
            display: table;
            width: 100%;
            margin-bottom: 5px;
        }
        
        .left-section {
            display: table-cell;
            width: 60%;
            vertical-align: top;
            padding-right: 15px;
        }
        
        .right-section {
            display: table-cell;
            width: 40%;
            vertical-align: top;
            text-align: right;
        }
        
        .company-logo {
            font-size: 16px;
            font-weight: bold;
            color: #000;
            margin-bottom: 1px;
        }
        
        .company-info {
            font-size: 7px;
            line-height: 1.1;
        }
        
        .client-header-info {
            font-size: 7px;
            line-height: 1.2;
        }
        
        /* Section TVA */
        .tva-section {
            text-align: center;
            font-size: 7px;
            font-weight: bold;
            margin: 3px 0;
        }
        
        /* Barre client */
        .client-bar {
            background-color: #d3d3d3;
            padding: 2px;
            text-align: center;
            font-size: 8px;
            font-weight: bold;
            border: 1px solid #000;
            margin: 3px 0;
        }
        
        /* Tableaux séparés mais cohérents */
        .table-section {
            width: 100%;
            border-collapse: collapse;
            border: 2px solid #000;
            margin-bottom: 3px;
        }
        
        .table-section td, .table-section th {
            border: 1px solid #000;
            padding: 2px;
            font-size: 7px;
            vertical-align: top;
        }
        
        .table-section th {
            background-color: #e8e8e8;
            text-align: center;
            font-weight: bold;
        }
        
        /* Tableau caractéristiques */
        .char-header {
            background-color: #e8e8e8;
            text-align: center;
            font-weight: bold;
        }
        
        .prestation-yellow {
            background-color: #ffff00 !important;
        }
        
        .row-label {
            background-color: #e8e8e8;
            font-weight: bold;
            text-align: center;
            width: 15%;
        }
        
        /* Tableau accès compact */
        .access-subtable {
            width: 100%;
            font-size: 6px;
            border: none;
        }
        
        .access-subtable td {
            border: none;
            padding: 0px 1px;
            text-align: left;
        }
        
        /* Section prix */
        .price-header {
            background-color: #e8e8e8;
            border: 2px solid #000;
            text-align: center;
            padding: 2px;
            font-weight: bold;
            font-size: 8px;
            margin-top: 3px;
        }
        
        .price-table {
            width: 100%;
            border-collapse: collapse;
            border: 2px solid #000;
            border-top: none;
        }
        
        .price-table td {
            border: 1px solid #000;
            padding: 1px 3px;
            font-size: 7px;
            vertical-align: top;
        }
        
        .price-amount {
            width: 80px;
            text-align: right;
            font-weight: bold;
        }
        
        .total-ht {
            background-color: #e8e8e8;
            font-weight: bold;
        }
        
        .total-ttc {
            background-color: #000;
            color: #fff;
            font-weight: bold;
        }
        
        /* Section suppléments */
        .supplements {
            margin-top: 3px;
            font-size: 6px;
        }
        
        .supplements-table {
            width: 100%;
            border-collapse: collapse;
            border: 1px solid #000;
        }
        
        .supplements-table td {
            border: 1px solid #000;
            padding: 1px 2px;
            font-size: 6px;
            text-align: center;
        }
        
        /* Section légale */
        .legal-text {
            margin-top: 3px;
            font-size: 6px;
            line-height: 1.1;
        }
        
        .signature-area {
            margin-top: 3px;
            text-align: center;
            font-size: 7px;
        }
        
        /* Footer */
        .footer {
            margin-top: 3px;
            text-align: center;
            font-size: 6px;
            border-top: 1px solid #000;
            padding-top: 2px;
        }
    </style>
</head>
<body>
    <!-- En-tête principal -->
    <div class="main-header">
        <div class="left-section">
            <div class="company-logo">CHATTI DEMENAGEMENT</div>
            <div class="company-info">
                2, Rue Jean Cocteau<br>
                91460 MARCOUSSIS<br>
                Tél: 01 69 75 18 22<br>
                E-mail: chattidemenagement@gmail.com
            </div>
        </div>
        <div class="right-section">
            <div class="client-header-info">
                Client n° %CLIENT_NUMBER%<br>
                Devis n° %DEVIS_NUMBER%<br>
                Date %DATE%<br>
                Validité %VALIDITY_DATE%<br><br>
                Mr %CLIENT_NAME%<br>
                %ADRESSE_DEPART%
            </div>
        </div>
    </div>

    <!-- Section TVA -->
    <div class="tva-section">
        CHATTI DEMENAGEMENT - TVA intracommunautaire: FR68390329894<br>
        SIRET: 39032989400054 - APE: 4942Z - RCS: Evry A 390 329 894 000 54
    </div>

    <!-- Barre client -->
    <div class="client-bar">
        Client %CLIENT_NAME% &nbsp;&nbsp; Client n° %CLIENT_NUMBER% &nbsp;&nbsp; Devis N° %DEVIS_NUMBER% &nbsp;&nbsp; Validité %VALIDITY_DATE%
    </div>

    <!-- Tableau caractéristiques -->
    <table class="table-section">
        <tr class="char-header">
            <td style="width: 15%;">Volume</td>
            <td style="width: 15%;">Distance</td>
            <td style="width: 15%;">Nature</td>
            <td style="width: 55%;">Prestation</td>
        </tr>
        <tr>
            <td>%VOLUME% m3</td>
            <td>%DISTANCE% km</td>
            <td>%NATURE%</td>
            <td class="prestation-yellow">%PRESTATION%</td>
        </tr>
    </table>

    <!-- Tableau principal -->
    <table class="table-section">
        <tr>
            <th style="width: 15%;"></th>
            <th style="width: 42.5%;">Chargement</th>
            <th style="width: 42.5%;">Livraison</th>
        </tr>
        <tr>
            <td class="row-label">Période</td>
            <td>%PERIODE_CHARGEMENT%</td>
            <td>%PERIODE_LIVRAISON%</td>
        </tr>
        <tr>
            <td class="row-label">Adresse</td>
            <td>%ADRESSE_DEPART%<br>FRANCE</td>
            <td>%ADRESSE_ARRIVEE%<br>FRANCE</td>
        </tr>
        <tr>
            <td class="row-label">Téléphone</td>
            <td>%PHONE_DEPART%</td>
            <td>%PHONE_ARRIVEE%</td>
        </tr>
        <tr>
            <td class="row-label">Accès</td>
            <td>
                <table class="access-subtable">
                    <tr><td><strong>Étage</strong></td><td><strong>Digicode</strong></td></tr>
                    <tr><td>Ascenseur %ASCENSEUR_DEPART%</td><td>Monte-meubles %MONTE_MEUBLES_DEPART%</td></tr>
                    <tr><td>Passage fenêtre %PASSAGE_FENETRE_DEPART%</td><td>Transbordement %TRANSBORDEMENT_DEPART%</td></tr>
                    <tr><td>Portage %PORTAGE_DEPART%</td><td>Stationnement %STATIONNEMENT_DEPART%</td></tr>
                    <tr><td colspan="2">Stationnement facile</td></tr>
                </table>
            </td>
            <td>
                <table class="access-subtable">
                    <tr><td><strong>Étage</strong></td><td><strong>Digicode</strong></td></tr>
                    <tr><td>Ascenseur %ASCENSEUR_ARRIVEE%</td><td>Monte-meubles %MONTE_MEUBLES_ARRIVEE%</td></tr>
                    <tr><td>Passage fenêtre %PASSAGE_FENETRE_ARRIVEE%</td><td>Transbordement %TRANSBORDEMENT_ARRIVEE%</td></tr>
                    <tr><td>Portage %PORTAGE_ARRIVEE%</td><td>Stationnement %STATIONNEMENT_ARRIVEE%</td></tr>
                    <tr><td colspan="2">Stationnement facile</td></tr>
                </table>
            </td>
        </tr>
    </table>

    <!-- Section prix -->
    <div class="price-header">Détail du prix</div>
    <table class="price-table">
        <tr><td>Camion en circuit organisé (licence intérieur et communautaire)</td><td class="price-amount">%COUT_CAMION% €</td></tr>
        <tr><td>Kilomètre</td><td class="price-amount">%COUT_KILOMETRE% €</td></tr>
        <tr><td>Petit matériel</td><td class="price-amount">%PETIT_MATERIEL% €</td></tr>
        <tr><td>Main d'œuvre qualifié déménageurs Professionnels</td><td class="price-amount">%MAIN_OEUVRE% €</td></tr>
        <tr><td>Emballage perdu cartons renforcés et adhésifs</td><td class="price-amount">%EMBALLAGE% €</td></tr>
        <tr><td>Remise Groupage selon disponibilité</td><td class="price-amount">%REMISE_GROUPAGE% €</td></tr>
        <tr><td>Pack Demande Autorisation stationnement (sur demande)</td><td class="price-amount">%PACK_AUTORISATION% €</td></tr>
        <tr><td>Matériel Balisage stationnement-Panneau déménagement</td><td class="price-amount">%MATERIEL_BALISAGE% €</td></tr>
        <tr><td>Assurance Garantie responsabilité Contractuelle</td><td class="price-amount">%ASSURANCE% €</td></tr>
        <tr><td>Valeur globale 30 000.00 € et 380,00 euros maxi par objet non listé</td><td class="price-amount"></td></tr>
        <tr class="total-ht"><td><strong>Total H.T.</strong></td><td class="price-amount"><strong>%TOTAL_HT% €</strong></td></tr>
        <tr><td>TVA de 20.00%</td><td class="price-amount">%TVA% €</td></tr>
        <tr class="total-ttc"><td><strong>PRIX TTC EN EUROS (valable jusqu'au %VALIDITY_DATE%)</strong></td><td class="price-amount"><strong>%TOTAL_TTC% €</strong></td></tr>
        <tr><td>30% d'arrhes à la commande, Solde à la livraison</td><td>Arrhes</td></tr>
        <tr><td></td><td>Solde</td></tr>
        <tr><td></td><td class="price-amount">%ARRHES% €</td></tr>
        <tr><td></td><td class="price-amount">%SOLDE% €</td></tr>
    </table>

    <!-- Section suppléments -->
    <div class="supplements">
        <div style="font-style: italic; margin-bottom: 1px;">En cas de difficultés non signalées, suppléments éventuels :</div>
        <table class="supplements-table">
            <tr><td><strong>Monte meubles (1/2 journée)</strong></td><td><strong>1 personne supplémentaire (1/2 journée)</strong></td><td><strong>Véhicule léger (1/2 journée)</strong></td></tr>
            <tr><td>298.80 € TTC</td><td>270.00 € TTC</td><td>120.00 € TTC</td></tr>
        </table>
    </div>

    <!-- Section légale -->
    <div class="legal-text">
        <p style="margin: 2px 0;">le prix est définitif (article 1er de l'arrêté du 27 avril 2010) sauf cas préciser dans l'article 6 des CGV jointes au devis.</p>
        
        <div style="font-weight: bold; margin: 2px 0;">Emission CO2 selon art L1431-3 Code Transport: %EMISSION_CO2% kg</div>
        
        <p style="margin: 2px 0;">En accord avec les termes de ce présent devis et de nos conditions générales de vente ci-jointes, nous vous prions de bien vouloir nous retourner ces documents signés et accompagnés du chèque d'arrhes correspondant à la prestation choisie afin de confirmer votre déménagement. Le solde sera à régler à la livraison de votre mobilier.</p>
        
        <div class="signature-area">
            <strong>Fait à MARCOUSSIS, le %DATE%</strong><br>
            <strong>Signature du client:</strong>
        </div>
    </div>

    <!-- Footer -->
    <div class="footer">
        <strong>CHATTI DEMENAGEMENT - TVA intracommunautaire: FR68390329894</strong><br>
        <strong>SIRET: 39032989400054 - APE: 4942Z - RCS: Evry A 390 329 894 000 54</strong>
    </div>
</body>
</html>
    )")
        .replace("%CLIENT_NUMBER%", "951")
        .replace("%DEVIS_NUMBER%", QString::number(QDateTime::currentSecsSinceEpoch() % 10000000))
        .replace("%DATE%", getCurrentDate())
        .replace("%VALIDITY_DATE%", QDate::currentDate().addDays(60).toString("dd/MM/yyyy"))
        .replace("%CLIENT_NAME%", client.getNom())
        .replace("%ADRESSE_DEPART%", QString::fromStdString(client.getAdresseDepart().m_rue))
        .replace("%ADRESSE_ARRIVEE%", QString::fromStdString(client.getAdresseArrivee().m_rue))
        .replace("%PHONE_DEPART%", "01 69 75 18 22")
        .replace("%PHONE_ARRIVEE%", "Non renseigné")
        .replace("%PERIODE_CHARGEMENT%", QDate::currentDate().toString("MMMM yyyy"))
        .replace("%PERIODE_LIVRAISON%", QDate::currentDate().toString("MMMM yyyy"))
        .replace("%ASCENSEUR_DEPART%", client.getAdresseDepart().m_ascenseur ? "Oui" : "Non")
        .replace("%ASCENSEUR_ARRIVEE%", client.getAdresseArrivee().m_ascenseur ? "Oui" : "Non")
        .replace("%MONTE_MEUBLES_DEPART%", client.getAdresseDepart().m_monteMeubles ? "Oui" : "Non")
        .replace("%MONTE_MEUBLES_ARRIVEE%", client.getAdresseArrivee().m_monteMeubles ? "Oui" : "Non")
        .replace("%PASSAGE_FENETRE_DEPART%", "Non")
        .replace("%PASSAGE_FENETRE_ARRIVEE%", "Non")
        .replace("%TRANSBORDEMENT_DEPART%", "Non")
        .replace("%TRANSBORDEMENT_ARRIVEE%", "Non")
        .replace("%PORTAGE_DEPART%", "Non")
        .replace("%PORTAGE_ARRIVEE%", "Non")
        .replace("%STATIONNEMENT_DEPART%", "Non")
        .replace("%STATIONNEMENT_ARRIVEE%", "Non")
        .replace("%VOLUME%", QString::number(client.getVolume(), 'f', 0))
        .replace("%DISTANCE%", QString::number(client.getDistance(), 'f', 0))
        .replace("%NATURE%", getNatureString(client.getNature()))
        .replace("%PRESTATION%", getPrestationString(client.getPrestation()))
        .replace("%COUT_CAMION%", QString::number(resultats.coutCamion, 'f', 2))
        .replace("%COUT_KILOMETRE%", QString::number(resultats.fraisRoute, 'f', 2))
        .replace("%PETIT_MATERIEL%", "6.00")
        .replace("%MAIN_OEUVRE%", QString::number(resultats.coutMainOeuvre, 'f', 2))
        .replace("%EMBALLAGE%", "30.00")
        //.replace("%REMISE_GROUPAGE%", QString::number(resultats.remise > 0 ? -resultats.remise : 0, 'f', 2))
        .replace("%PACK_AUTORISATION%", "1.00")
        .replace("%MATERIEL_BALISAGE%", "0.00")
        .replace("%ASSURANCE%", QString::number(resultats.coutAssurance, 'f', 2))
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
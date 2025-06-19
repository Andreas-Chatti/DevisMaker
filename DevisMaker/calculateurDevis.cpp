#include "calculateurDevis.h"

ResultatsDevis CalculateurDevis::calculate() const
{
    double volumeParPersonne{ m_tarification.calculerVolumeParPersonne(m_client.getVolume(), m_client.getPrestation()) };
    int nombreCamion{ m_tarification.calculerNombreCamion(m_client.getVolume(), m_client.getPrestation(), m_client.getNature(), m_client.getDistance()) };
    int nombreMO{ m_tarification.calculerNombreMO(m_client.getVolume(), m_client.getPrestation(), m_client.getNature(), nombreCamion, m_client.getAdresseDepart().m_monteMeubles || m_client.getAdresseArrivee().m_monteMeubles, m_client.getAdresseDepart().m_ascenseur || m_client.getAdresseArrivee().m_ascenseur, m_client.getDistance()) };

    double coutMOTotal{ m_tarification.calculerCoutMainOeuvreTotal(nombreMO) };

    double coutCamionTotal{ m_tarification.calculerCoutCamionTotal(nombreCamion) };

    double coutAutStatTotal{ m_tarification.calculerPrixStationnement(m_client.getAdresseDepart().m_autStationnement, m_client.getAdresseArrivee().m_autStationnement) };

    double fraisRouteTotal{ calculerFraisRouteTotal(nombreCamion) };

    double coutAssurance{ m_tarification.calculerCoutAssurance(m_client.getValeurAssurance(), m_client.getTypeAssurance()) };

    double fraisMMeubles{ m_tarification.calculerSupplementMM(m_client.getAdresseDepart(), m_client.getAdresseArrivee()) };

    double prixDechetterie{ m_client.getIsDE() ? m_tarification.getPrixDechetterie() : 0 };

    double prixSuppAdresse{ calculerPrixSuppAdresse() };

    double prixTotalHT{ m_tarification.calculerCoutTotalHT(m_client.getVolume(), coutAssurance, coutAutStatTotal, fraisMMeubles, prixDechetterie, fraisRouteTotal, prixSuppAdresse) };

    double arrhes{ m_tarification.calculerArrhes(prixTotalHT) };


    return ResultatsDevis{ volumeParPersonne, nombreCamion, nombreMO, coutMOTotal, coutCamionTotal,
        coutAutStatTotal, fraisRouteTotal, coutAssurance, fraisMMeubles, prixDechetterie, prixSuppAdresse, prixTotalHT, arrhes };
}


double CalculateurDevis::calculerFraisRouteTotal(double nombreCamion) const
{
    return m_client.getNature() != Nature::urbain ? m_tarification.calculerCoutFraisRouteTotal(nombreCamion) : 0;
}


double CalculateurDevis::calculerFraisDechetterie() const
{
    return m_client.getIsDE() ? m_tarification.getPrixDechetterie() : 0;
}


double CalculateurDevis::calculerPrixSuppAdresse() const
{
    return  m_ui.suppAdresseCheckBox->isChecked() ? m_ui.suppAdresseSpinBox->value() * m_tarification.getPrixSuppAdresse() : 0;
}
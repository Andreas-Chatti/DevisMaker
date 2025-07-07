#pragma once
#include "client.h"
#include "tarification.h"
#include "resultatDevis.h"


class CalculateurDevis
{
public:

	CalculateurDevis(const Client& client, const Tarification& tarification)
		: m_client{ client }
		, m_tarification{ tarification }
		, m_lastResults{}
	{
	}

	const ResultatsDevis& calculateDevis(const PricePreset& preset);

	const ResultatsDevis& getLastResults() const { return m_lastResults; }

	double calculerValeurAssurance() const { return m_client.getVolume() * 500.0; }

	double calculerValeurAssurance(double volume) const { return volume * 500.0; }

	double calculerVolumeParPersonne() const;

	int calculerNombreCamion(bool accesComplexe = false, bool montageImportant = false) const;

	int calculerNombreMO(int nombreCamions) const;

	double calculerCoutTotalHT() const
	{
		double volume{ m_client.getVolume() };
		double prixM3{ m_tarification.getPrixMetreCube() };
		double prixTotalAssurance{ calculerCoutAssurance() };
		double prixTotalStationnement{ calculerPrixStationnement() };
		double prixTotalMonteMeubles{ calculerSupplementMM() };
		double prixTotalFraisRoute{ calculerFraisRouteTotal() };
		double prixTotalSuppAdresse{ calculerSuppAdresseTotal() };

		return volume * prixM3 + prixTotalAssurance + prixTotalStationnement + prixTotalMonteMeubles + prixTotalFraisRoute + prixTotalSuppAdresse;
	}

	double calculerCoutCamionTotal() const { return m_tarification.getCoutCamion() * calculerNombreCamion(); }

	double calculerCoutKilometrageTotal() const { return m_client.getDistance() * m_tarification.getCoutKilometrique(); }

	double calculerCoutMainOeuvreTotal() const 
	{ 
		int nombreCamion{ calculerNombreCamion() };
		int nombreMO{ calculerNombreMO(nombreCamion) };

		return nombreMO * m_tarification.getCoutMO(); 
	}

	double calculerCoutLocMaterielTotal() const { return m_client.getVolume() * m_tarification.getPrixLocMateriel(); }

	double calculerFraisRouteTotal() const 
	{ 
		const Nature& nature{ m_client.getNature() };
		double fraisRouteTotal{ (m_tarification.getFraisRoute() * (calculerNombreCamion() - 1)) * 2 };


		return nature != Nature::urbain ? fraisRouteTotal : 0;
	}

	double calculerArrhes() const { return (calculerCoutTotalHT() * 1.2) * 0.3; }

	double calculerCoutAssurance() const;

	double calculerPrixStationnement() const;

	double calculerSupplementMM() const;

	double calculerSuppAdresseTotal() const { return m_client.getNbAdresseSupp() * m_tarification.getPrixSuppAdresse(); }

	double calculerPrixMetreCube(PricePreset preset) const;


private:

	const Client& m_client;
	const Tarification& m_tarification;
	ResultatsDevis m_lastResults;
};
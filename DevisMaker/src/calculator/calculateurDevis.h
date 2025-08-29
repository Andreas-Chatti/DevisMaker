#pragma once
#include "models/Client.h"
#include "utils/tarification.h"
#include "utils/resultatDevis.h"
#include "utils/constants.h"


class CalculateurDevis
{
public:

	CalculateurDevis(const Client& client, const Tarification* tarification)
		: m_client{ client }
		, m_tarification{ tarification }
		, m_lastResults{}
	{
	}

	const ResultatsDevis& calculateDevis(const PricePreset& preset, Tarification::PriceCalculation calculationMethod);

	const ResultatsDevis& calculateDevis_Postes(const PricePreset& preset, Tarification::PriceCalculation calculationMethod);
	const ResultatsDevis_M3& calculateDevis_M3(const PricePreset& preset, Tarification::PriceCalculation calculationMethod);

	const ResultatsDevis& getLastResults() const { return m_lastResults; }

	double calculerValeurAssurance() const { return m_client.getVolume() * Tarification::ASSURANCE_VALUE_PER_M3; }

	double calculerValeurAssurance(double volume) const { return volume * Tarification::ASSURANCE_VALUE_PER_M3; }

	double calculerVolumeParPersonne() const;

	int calculerNombreCamion(bool accesComplexe = false, bool montageImportant = false) const;

	int calculerNombreMO(int nombreCamions) const;

	double calculerCoutTotalHT(const Tarification::PriceCalculation& calculationMethod) const;

	double calculerCoutCamionTotal() const { return m_tarification->getCoutCamion() * calculerNombreCamion(); }

	double calculerCoutKilometrageTotal() const { return m_client.getDistance() * m_tarification->getCoutKilometrique(); }

	double calculerCoutMainOeuvreTotal() const 
	{ 
		int nombreCamion{ calculerNombreCamion() };
		int nombreMO{ calculerNombreMO(nombreCamion) };

		return nombreMO * m_tarification->getCoutMO(); 
	}

	double calculerCoutLocMaterielTotal() const { return m_client.getVolume() * m_tarification->getPrixLocMateriel(); }

	double calculerCoutEmballageTotal() const { return m_client.getVolume() * m_tarification->getCoutEmballage(); }

	double calculerFraisRouteTotal() const 
	{ 
		const Nature& nature{ m_client.getNature() };
		double fraisRouteTotal{ (m_tarification->getFraisRoute() * (calculerNombreCamion() - 1)) * 2 };


		return nature != Nature::urbain ? fraisRouteTotal : 0;
	}

	double calculerArrhes(double coutTotalHT) const { return (coutTotalHT * 1.2) * 0.3; }

	double calculerCoutAssurance() const;

	double calculerPrixStationnement() const;

	double calculerSupplementMM() const;

	double calculerSuppAdresseTotal() const { return m_client.getNbAdresseSupp() * m_tarification->getPrixSuppAdresse(); }

	double calculerPrixMetreCube(PricePreset preset) const;


private:

	const Client& m_client;
	const Tarification* m_tarification;
	ResultatsDevis m_lastResults;
};
#pragma once
#include "models/Client.h"
#include "utils/tarification.h"
#include "utils/resultatDevis.h"
#include "utils/constants.h"


class CalculateurDevis
{
public:

	explicit CalculateurDevis(const Client* client, const Tarification* tarification)
		: m_client{ client }
		, m_tarification{ tarification }
	{
	}

	~CalculateurDevis() = default;

	/*
		Calculer le devis complet
	*/
	ResultatsDevis calculateDevis(PricePreset preset, Tarification::PriceCalculation calculationMethod);

	/*
		Calculer devis version 5 postes
	*/
	ResultatsDevis_CinqPostes calculateDevis_Postes();

	/*
		Calculer la valeur mobilier en fonction du volume client
	*/
	double calculerValeurAssurance() const { return m_client->getVolume() * SettingsConstants::INSURANCE_RATE_PER_M3; }
	double calculerValeurAssurance(double volume) const { return volume * SettingsConstants::INSURANCE_RATE_PER_M3; }

	/*
		Calculer le nombre de volume effectué par chaque déménageur
	*/
	double calculerVolumeParPersonne() const;

	/*
		Calculer le nombre de camion nécessaire pour le déménagement
	*/
	int calculerNombreCamion(bool accesComplexe = false, bool montageImportant = false) const;


	/*
		Calculer le nombre nécessaire de main d'oeuvre pour le déménagement
	*/
	int calculerNombreMO(int nombreCamions) const;

	double calculerCoutTotalHT(Tarification::PriceCalculation calculationMethod) const;

	double calculerCoutCamionTotal() const { return m_tarification->getCoutCamion() * calculerNombreCamion(); }

	double calculerCoutKilometrageTotal() const { return m_client->getDistance() * m_tarification->getCoutKilometrique(); }

	double calculerCoutMainOeuvreTotal() const;

	double calculerCoutLocMaterielTotal() const { return m_client->getVolume() * m_tarification->getPrixLocMateriel(); }

	double calculerCoutEmballageTotal() const { return m_client->getVolume() * m_tarification->getCoutEmballage(); }

	double calculerFraisRouteTotal() const;

	double calculerArrhes(double coutTotalHT) const { return (coutTotalHT * 1.2) * 0.3; }

	double calculerCoutAssurance() const;

	/*
		Calcule le coût stationnement par adresse
	*/
	double calculerPrixStationnement() const;

	/*
		Calcule le coût total du supplément monte-meubles
	*/
	double calculerSupplementMM() const;

	/*
		Calcule le coût du supplément adresse total en fonction du nombre d'adresse(s)
	*/
	double calculerSuppAdresseTotal() const { return m_client->getNbAdresseSupp() * m_tarification->getPrixSuppAdresse(); }

	/*
		Calcule le prix du mètre cube en fonction de la saison
	*/
	double calculerPrixMetreCube(PricePreset preset) const;

	const ResultatsDevis& getLastResults() const { return m_lastResults; }

private:

	const Client* m_client{ nullptr };
	const Tarification* m_tarification{ nullptr };
	ResultatsDevis m_lastResults{};
};
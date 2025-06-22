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

	const ResultatsDevis& calculate(bool suppAdresseEnabled, int suppAdresseValue);

	const ResultatsDevis& getLastResults() const { return m_lastResults; }

private:

	const Client& m_client;
	const Tarification& m_tarification;
	ResultatsDevis m_lastResults;


	double calculerFraisRouteTotal(double nombreCamion) const;
	double calculerFraisDechetterie() const;
	double calculerPrixSuppAdresse(bool suppAdresseEnabled, int suppAdresseValue) const;
};
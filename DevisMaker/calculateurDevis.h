#pragma once
#include "client.h"
#include "tarification.h"
#include "resultatDevis.h"
#include "ui_MainWindow.h"


class CalculateurDevis
{
public:

	CalculateurDevis(const Client& client, const Tarification& tarification, const Ui::MainWindowClass* ui)
		: m_client{ client }
		, m_tarification{ tarification }
		, m_ui{ ui }
	{
	}

	ResultatsDevis calculate() const;

private:

	const Client& m_client;
	const Tarification& m_tarification;
	const Ui::MainWindowClass* m_ui;


	double calculerFraisRouteTotal(double nombreCamion) const;
	double calculerFraisDechetterie() const;
	double calculerPrixSuppAdresse() const;
};
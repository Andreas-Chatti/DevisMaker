#pragma once
#include <map>
#include <qstring.h>

namespace BasseSaison
{
	/*
	Coûts par défaut en basse saison
	*/
	constexpr double coutCamionDefault{ 75.0 };
	constexpr double coutKilometriqueDefault{ 1.3 };
	constexpr double coutEmballageDefault{ 5.0 };
	constexpr double prixLocMaterielDefault{ 0.5 };
	constexpr double fraisRouteDefault{ 65.0 };
	constexpr double coutMODefault{ 220.0 };
	constexpr double coutFraisStationnementDefault{ 50.0 };
	constexpr double prixMonteMeublesDefault{ 250.0 };
	constexpr double prixDechetterieDefault{ 200.0 };
	constexpr double prixSuppAdresseDefault{ 75.0 };


	const std::map<QString, double> pricesMap{
		{"CoutCamion", coutCamionDefault},
		{"CoutKilometrique", coutKilometriqueDefault},
		{"CoutEmballage", coutEmballageDefault},
		{"PrixLocMateriel", prixLocMaterielDefault},
		{"FraisRoute", fraisRouteDefault},
		{"CoutMO", coutMODefault},
		{"FraisStationnement", coutFraisStationnementDefault},
		{"PrixMM", prixMonteMeublesDefault},
		{"PrixDechetterie", prixDechetterieDefault},
		{"PrixSuppAdresse", prixSuppAdresseDefault}
	};
}


namespace HauteSaison
{
	/*
	Coûts par défaut en haute saison
	*/
	constexpr double coutCamionDefault{ 100.0 };
	constexpr double coutKilometriqueDefault{ 1.5 };
	constexpr double coutEmballageDefault{ 5.0 };
	constexpr double prixLocMaterielDefault{ 0.5 };
	constexpr double fraisRouteDefault{ 65.0 };
	constexpr double coutMODefault{ 250.0 };
	constexpr double coutFraisStationnementDefault{ 50.0 };
	constexpr double prixMonteMeublesDefault{ 250.0 };
	constexpr double prixDechetterieDefault{ 200.0 };
	constexpr double prixSuppAdresseDefault{ 100.0 };


	const std::map<QString, double> pricesMap{
		{"CoutCamion", coutCamionDefault},
		{"CoutKilometrique", coutKilometriqueDefault},
		{"CoutEmballage", coutEmballageDefault},
		{"PrixLocMateriel", prixLocMaterielDefault},
		{"FraisRoute", fraisRouteDefault},
		{"CoutMO", coutMODefault},
		{"FraisStationnement", coutFraisStationnementDefault},
		{"PrixMM", prixMonteMeublesDefault},
		{"PrixDechetterie", prixDechetterieDefault},
		{"PrixSuppAdresse", prixSuppAdresseDefault}
	};
}
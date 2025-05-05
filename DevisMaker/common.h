#ifndef COMMON_H
#define COMMON_H
#include <string>

struct Adresse
{
	std::string m_rue;
	int m_etage;
	bool m_ascenseur;
	bool m_monteMeubles;
	bool m_autStationnement;
};


enum class Prestation
{
	eco,
	ecoPlus,
	standard,
	luxe,
	max_prestations
};


enum class Nature
{
	urbain,
	special,
	groupage,
	max_natures
};


enum class TypeAssurance
{
	contractuelle,
	dommage,
	max_types
};

#endif
#ifndef COMMON_H
#define COMMON_H
#include <string>

struct Adresse
{
	std::string m_rue;
	std::uint16_t m_etage;
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
	route,
	groupage,
	max_natures
};


#endif
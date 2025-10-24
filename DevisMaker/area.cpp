#include "area.h"

Area::Area(QString name, AreaType type)
	: m_name{std::move(name)}
	, m_type{type}
{
}

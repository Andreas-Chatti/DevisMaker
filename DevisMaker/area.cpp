#include "area.h"

Area::Area()
	: m_name{"UNDEFINED"}
	, m_type{Area::AreaType::none}
	, m_objects{}
	, m_totalVolume{0.0}
{
}

Area::Area(QString name, AreaType type)
	: m_name{ std::move(name) }
	, m_type{ type }
{
}

void Area::addObject(MovingObject object)
{
	auto objectIt{ m_objects.find(object.getName()) };
	if (objectIt != m_objects.end())
	{
		objectIt->add(object.getQuantity());
		m_totalVolume += object.getTotalVolume();
		return;
	}

	m_totalVolume += object.getTotalVolume();
	m_objects[object.getName()] = std::move(object);
}

void Area::removeObject(const QString& objectName)
{
	auto it{ m_objects.find(objectName) };
	if (it != m_objects.end())
	{
		m_totalVolume -= it->getTotalVolume();
		m_objects.erase(it);
	}
}

void Area::removeObject(const QString& objectName, int quantity)
{
	auto it{ m_objects.find(objectName) };
	if (it != m_objects.end())
	{
		if (it->getQuantity() <= quantity)
		{
			m_totalVolume -= it->getTotalVolume();
			m_objects.erase(it);
		}

		else
		{
			double volumeRemoved{ it->getUnitaryVolume() * quantity };
			it->remove(quantity);
			m_totalVolume -= volumeRemoved;
		}
	}
}

void Area::replaceObjectWithNew(const MovingObject* objectToReplace, MovingObject newObject)
{
	Q_ASSERT(objectToReplace);
	if (!objectToReplace) 
	{
		qCritical() << "[Area::modifyObject] ObjectToReplace IS NULL";
		return;
	}

	auto object{ m_objects.find(objectToReplace->getName()) };
	if (object == m_objects.end())
	{
		qWarning() << "[Area::modifyObject] No matching objectToReplace found in " << m_name;
		return;
	}

	m_totalVolume -= object->getTotalVolume();
	m_totalVolume += newObject.getTotalVolume();
	*object = std::move(newObject);
}

const MovingObject* Area::findObject(const QString& objectKey) const
{
	const auto objectIt{ m_objects.find(objectKey) };
	if (objectIt == m_objects.end())
		return nullptr;

	return &(*objectIt);
}

void Area::updateObjectsAreaKey(const QString& newAreaKey)
{
	for (auto& object : m_objects)
		object.setAreaKey(newAreaKey);
}




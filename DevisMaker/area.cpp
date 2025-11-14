#include "area.h"

Area::Area(QString name)
	: m_name{ std::move(name) }
{
}

/*
	Only for QHash
	Should not be called
*/
Area::Area()
	: Area("")
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
	if (it == m_objects.end())
	{
		qWarning() << "[Area::removeObject] Object '" << objectName << "' does not exist in Area '" << m_name << "'";
		return;
	}

	m_totalVolume -= it->getTotalVolume();
	m_objects.erase(it);
}

void Area::removeObject(const QString& objectName, int quantity)
{
	Q_ASSERT(quantity > 0 && !objectName.isEmpty());
	if (quantity <= 0)
	{
		qWarning() << "[Area::removeObject] Quantity arg was <= 0";
		return;
	}

	auto it{ m_objects.find(objectName) };
	if (it == m_objects.end())
	{
		qWarning() << "Object '" << objectName << "' doesn't exist in Area '" << m_name << "'";
		return;
	}

	else if (it->getQuantity() <= quantity)
	{
		m_totalVolume -= it->getTotalVolume();
		m_objects.erase(it);
	}

	else
	{
		double removedVolume{ it->getUnitaryVolume() * quantity };
		it->remove(quantity);
		m_totalVolume -= removedVolume;
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




#pragma once
#include <QString>
#include <QVector>
#include <QHash>
#include <QDebug>
#include <memory>
#include "inventory/movingObject.h"

class Area
{
public:

	Area();

	explicit Area(QString name);

	void addObject(MovingObject object);
	void removeObject(const QString& objectName);
	void removeObject(const QString& objectName, int quantity);
	void replaceObjectWithNew(const MovingObject* objectToModify, MovingObject newObject);

	const QHash<QString, MovingObject>& getObjectsList() const { return m_objects; }
	const QString& getName() const { return m_name; }
	double getTotalVolume() const { return m_totalVolume; }
	int getObjectCount() const { return m_objects.size(); }

	void setName(QString name) { m_name = std::move(name); }

	const MovingObject* findObject(const QString& objectKey) const;

	void updateObjectsAreaKey(const QString& newKey);

private:

	QString m_name;
	QHash<QString, MovingObject> m_objects{};
	double m_totalVolume{ 0.0 };
};
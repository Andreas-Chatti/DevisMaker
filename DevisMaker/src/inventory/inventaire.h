#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <Qmap>
#include <Qvector>
#include <qdebug.h>
#include "movingObject.h"
#include "area.h"


class Inventory : public QObject
{
    Q_OBJECT

public:

    Inventory(QObject* parent = nullptr)
        : QObject{parent}
    {
    }

    double getTotalVolume() const { return m_totalVolume; }
    const QHash<QString, Area>& getAreas() const { return m_areas; }

    bool isEmpty() const { return m_areas.isEmpty(); }


private:

    QHash<QString, Area> m_areas{};
    double m_totalVolume{};

    void clear();


public slots:

    void handleInventoryAnalysis(double listTotalVolume, QVector<MovingObject>& objectList);

    void addObject(MovingObject movingObject, const QString& areaName);

    void removeObject(const QString& movingObject, const QString& areaName);

    void modifyObject(const MovingObject* objectToModify, MovingObject newObject);

    void addArea(QString areaName);
    //void addArea(QString areaName, Area::AreaType areaType);

    void removeArea(const QString& areaName);

    int objectsQuantity() const;

    const Area* findArea(const QString& areaKey) const;

    void modifyAreaName(const QString& currentName, const QString& newName);

signals:

    void sendNewInventory(const Inventory& inventory);
};
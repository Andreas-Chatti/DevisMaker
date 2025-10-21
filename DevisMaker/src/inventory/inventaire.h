#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <Qmap>
#include <Qvector>
#include <qdebug.h>
#include "movingObject.h"


class Inventory : public QObject
{
    Q_OBJECT

public:

    Inventory(QObject* parent = nullptr)
        : QObject{parent}
    {
    }

    double getTotalVolume() const { return m_totalVolume; }
    const QVector<MovingObject>& getInventory() const { return m_objects; }
    bool isEmpty() const { return m_objects.isEmpty(); }

private:

    //QMap<QString, MovingObject> m_dictionnary{};
    QVector<MovingObject> m_objects{};
    double m_totalVolume{};

    void clearInventory();

    //void loadDictionnary();  // Charge le dictionnaire d'objets standard

public slots:

    void handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems);
    
    // TODO: Implémenter les fonctions slots suivantes :
    // modifyObject(const MovingObject& movingObject)

    void addObjectByName(const QString& name, double volume, int quantity = 1);
    void addObject(const MovingObject& movingObject, int quantity = 1);

    void removeObject(const MovingObject& movingObject);
    void removeObjectByNameAndQuantity(const QString& name, int quantity = 0);
    void removeObjectByQuantity(const MovingObject& movingObject, int quantity);

    void modifyObject(const MovingObject& objectToModify, MovingObject newObject);

signals:

    void sendNewInventory(const Inventory& inventory);
};
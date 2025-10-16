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
    const QVector<MovingObject> getInventory() const { return m_objects; }

private:

    QMap<QString, MovingObject> m_dictionnary{};
    QVector<MovingObject> m_objects{};
    double m_totalVolume{};


    void addObject(const QString& name, double volume, int quantity = 1);
    void addObject(const MovingObject& movingObject, int quantity = 1);

    void removeObject(const QString& name, int quantity = 0);
    void removeObject(const MovingObject& movingObject, int quantity = 0);


    void clearInventory();

    //void loadDictionnary();  // Charge le dictionnaire d'objets standard

public slots:

    void handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems);

signals:

    void sendNewInventory(const Inventory& inventory);
};
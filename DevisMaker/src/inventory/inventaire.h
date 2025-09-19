#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <Qmap>
#include <Qvector>


struct ObjetDemenagement
{
    QString name;
    double unitaryVolume;

    bool operator== (const ObjetDemenagement& movingObject) const
    {
        return (this->name == movingObject.name) && (this->unitaryVolume == movingObject.unitaryVolume);
    }
};


class Inventory : public QObject
{
    Q_OBJECT

public:

    Inventory(QObject* parent = nullptr)
        : m_dictionnary{}
        , m_objects{}
        , m_totalVolume{}
    {
    }

    double analyse(const QString& inventoryText);

private:

    QMap<QString, ObjetDemenagement> m_dictionnary;
    QVector<ObjetDemenagement> m_objects;
    double m_totalVolume;


    void addObject(const QString& name, double volume, int quantity = 1);
    void addObject(const ObjetDemenagement& movingObject, int quantity = 1);

    void deleteObject(const QString& name, int quantity = 1);
    void deleteObject(const ObjetDemenagement& movingObject, int quantity = 1);


    void clearInventory();
    double getTotalVolume() const { return m_totalVolume; }

    //void loadDictionnary();  // Charge le dictionnaire d'objets standard

public slots:

    void handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems);
};
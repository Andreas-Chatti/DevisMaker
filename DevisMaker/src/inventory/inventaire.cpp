#include "inventaire.h"

void Inventory::handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems)
{
    m_totalVolume = totalVolume;

    for (qsizetype i{}; i < structuredItems.size(); ++i)
    {
        QString item{ structuredItems[i] };
        QStringList parts{ item.split(" - ") };

        if (parts.size() == 2)
        {
            QString fullName{ parts[0] };     // Ex: "2 matelas 1 place"
            QString volumeText{ parts[1] };   // Ex: "1.0 m³"


            QStringList words{ fullName.split(" ") };
            QString quantity{ "1" };  // Par défaut
            QString cleanName{ fullName };

            if (!words.isEmpty())
            {
                bool ok{};
                int qty{ words.first().toInt(&ok) };
                if (ok && qty > 0)
                {
                    quantity = QString::number(qty);
                    words.removeFirst();
                    cleanName = words.join(" ");
                }
            }
        }
    }
}


void Inventory::addObject(const QString& name, double volume, int quantity)
{
    for (int i{}; i < quantity; i++)
        m_objects.emplaceBack(ObjetDemenagement{ name, volume });
}

void Inventory::addObject(const ObjetDemenagement& movingObject, int quantity)
{
    for (int i{}; i < quantity; i++)
        m_objects.emplaceBack(movingObject);
}


void Inventory::deleteObject(const QString& name, int quantity)
{
    int deletedCount{};

    const auto isSameObject{ [&](const ObjetDemenagement& object) {
        if (object.name == name && deletedCount < quantity)
        {
            ++deletedCount;
            return true;
        }
        
        return false;
    } };

    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), isSameObject), m_objects.end());
}

void Inventory::deleteObject(const ObjetDemenagement& movingObject, int quantity)
{
    int deletedCount{};

    const auto isSameObject{ [&](const ObjetDemenagement& object) {
        if (object == movingObject && deletedCount < quantity)
        {
            ++deletedCount;
            return true;
        }

        return false;
    } };

    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), isSameObject), m_objects.end());
}


void Inventory::clearInventory()
{
    m_objects.clear();
}
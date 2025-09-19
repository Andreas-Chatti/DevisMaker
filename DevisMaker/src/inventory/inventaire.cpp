#include "inventaire.h"
#include <qdebug.h>

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
            int quantity{ 1 };  // Par défaut
            QString cleanName{ fullName };

            // Extraire la quantité du nom
            if (!words.isEmpty())
            {
                bool ok{};
                int qty{ words.first().toInt(&ok) };
                if (ok && qty > 0)
                {
                    quantity = qty;
                    words.removeFirst();
                    cleanName = words.join(" ");
                }
            }

            volumeText = volumeText.trimmed();
            if (volumeText.endsWith(" m\u00b3") || volumeText.endsWith(" m3"))
            {
                volumeText.chop(3);
                bool ok{};
                double unitVolume{ volumeText.toDouble(&ok) };

                if (ok && unitVolume > 0.0)
                    addObject(cleanName, unitVolume, quantity);
            }
        }
    }
}


void Inventory::addObject(const QString& name, double unitaryVolume, int quantity)
{
    for (int i{}; i < quantity; i++)
        m_objects.emplaceBack(ObjetDemenagement{ name, unitaryVolume });
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
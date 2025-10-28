#include "inventaire.h"

void Inventory::handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems)
{
    clear();

    m_totalVolume = totalVolume;

    for (qsizetype i{}; i < structuredItems.size(); ++i)
    {
        QString item{ structuredItems[i] };
        QStringList parts{ item.split(" - ") };

        // Format attendu: "nom - volume m³" ou "nom - volume m³ (D, R, L)" ou "nom - volume m³ - pièce" ou "nom - volume m³ (D, R, L) - pièce"
        if (parts.size() >= 2)
        {
            QString fullName{ parts[0] };        // Ex: "2 matelas 1 place"
            QString volumePart{ parts[1] };      // Ex: "1.0 m³" ou "1.0 m³ (D, R, L)"
            QString areaKey{ "divers" };         // Par défaut

            // Si on a 3 parties, la 3ème est l'areaKey (la pièce)
            if (parts.size() == 3)
                areaKey = parts[2];

            // Parser le nom pour extraire la quantité
            QStringList words{ fullName.split(" ") };
            int quantity{ 1 };  // Par défaut
            QString cleanName{ fullName };

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

            // Parser volumePart pour extraire volume et tags (D, R, L)
            // Format: "2.5 m³ (D, R, L)" ou "2.5 m³"
            QString volumeText{ volumePart };
            bool disassembly{ false };
            bool assembly{ false };
            bool heavy{ false };

            // Chercher les tags entre parenthèses
            int tagStart{ volumePart.indexOf("(") };
            if (tagStart >= 0)
            {
                int tagEnd{ volumePart.indexOf(")") };
                if (tagEnd > tagStart)
                {
                    // Extraire le texte des tags: "D, R, L"
                    QString tagsText{ volumePart.mid(tagStart + 1, tagEnd - tagStart - 1) };
                    QStringList tagList{ tagsText.split(", ") };

                    // Détecter quels tags sont présents
                    disassembly = tagList.contains("D");
                    assembly = tagList.contains("R");
                    heavy = tagList.contains("L");

                    // Extraire le volume sans les tags
                    volumeText = volumePart.left(tagStart).trimmed();
                }
            }

            // Parser le volume
            volumeText = volumeText.trimmed();
            if (volumeText.endsWith(" m\u00b3") || volumeText.endsWith(" m3"))
            {
                volumeText.chop(3);
                bool ok{};
                double unitVolume{ volumeText.toDouble(&ok) / quantity };

                if (ok && unitVolume > 0.0)
                {
                    // Créer le MovingObject avec tous les paramètres
                    MovingObject movingObject{ cleanName, unitVolume, areaKey, quantity, disassembly, assembly, heavy };
                    addObject(std::move(movingObject), areaKey);
                }
            }
        }
    }

    emit sendNewInventory(*this);
}

void Inventory::addObject(MovingObject movingObject, const QString& areaName)
{
    if (movingObject.getName().isEmpty())
        return;

    auto area{ m_areas.find(areaName) };
    if (area == m_areas.end())
        addArea(areaName);

    area = m_areas.find(areaName);
    if (area != m_areas.end())
    {
        m_totalVolume += movingObject.getUnitaryVolume() * movingObject.getQuantity();
        area->addObject(std::move(movingObject));
    }
}

void Inventory::modifyObject(const MovingObject* objectToModify, MovingObject newObject)
{
    const QString& oldAreaKey{ objectToModify->getAreaKey() };
    const QString& newAreaKey{ newObject.getAreaKey() };

    auto areaIt{ m_areas.find(oldAreaKey) };
    if (areaIt != m_areas.end())
    {
        m_totalVolume -= objectToModify->getTotalVolume();
        m_totalVolume += newObject.getTotalVolume();

        bool isSameArea{ oldAreaKey == newAreaKey };
        if (isSameArea)
            areaIt->modifyObject(objectToModify, std::move(newObject));

        else
        {
            areaIt->removeObject(objectToModify->getName()); // Remove object from current Area
            if (areaIt->getObjectsList().isEmpty())
                m_areas.erase(areaIt); // Erase area if empty

            areaIt = m_areas.find(newAreaKey);
            if (areaIt != m_areas.end())
            {
                objectToModify = areaIt->findObject(newObject.getName()); // Checking if there's another object of the same name in new Area
                if (objectToModify)
                {
                    if (objectToModify->getUnitaryVolume() == newObject.getUnitaryVolume())
                        newObject.add(objectToModify->getQuantity());

                    areaIt->modifyObject(objectToModify, std::move(newObject)); // REMPLACE L'OBJET PAR LE NOUVEAU
                }

                else
                    areaIt->addObject(std::move(newObject));
            }

            else
            {
                addArea(newAreaKey);
                addObject(std::move(newObject), newAreaKey);
            }
        }
    }
}

void Inventory::addArea(QString areaName)
{
    auto area{ m_areas.find(areaName) };
    if (area == m_areas.end())
        m_areas[areaName] = Area{ areaName };
    
    else
    {
        // TODO : Si une pièce existe déjà sous le nom areaName, ajouter "#2" en plus dans le nom pour pouvoir créer une pièce avec le même nom
        // ou sinon, juste créer un QMessageBox prévenant l'utilisateur de choisir un autre nom car celui-ci est déjà pris
    }
}

/*void Inventory::addArea(QString areaName, Area::AreaType areaType)
{
    auto area{ m_areas.find(areaName) };
    if (area == m_areas.end())
        m_areas[areaName] = Area{ std::move(areaName), areaType };

    else
    {
        // TODO : Si une pièce existe déjà sous le nom areaName, ajouter "#2" en plus dans le nom pour pouvoir créer une pièce avec le même nom
        // ou sinon, juste créer un QMessageBox prévenant l'utilisateur de choisir un autre nom car celui-ci est déjà pris
    }
}*/

void Inventory::removeArea(const QString& areaName)
{
    auto it{ m_areas.find(areaName) };
    if (it != m_areas.end())
    {
        m_totalVolume -= it->getTotalVolume();
        m_areas.erase(it);
    }
}

int Inventory::objectsQuantity() const
{
    int size{};
    for (const auto& area : m_areas)
    {
        size += static_cast<int>(area.getObjectsList().size());
    }

    return size;
}

const Area* Inventory::findArea(const QString& areaKey) const
{
    const auto areaIt{ m_areas.find(areaKey) };
    if (areaIt == m_areas.end())
        return nullptr;

    return &(*areaIt);
}

void Inventory::modifyAreaName(const QString& currentName, const QString& newName)
{
    auto areaIt = m_areas.find(currentName);
    if (areaIt != m_areas.end())
    {
        if (m_areas.find(newName) != m_areas.end())
            return;

        areaIt->updateObjectsAreaKey(newName);
        areaIt->setName(newName);

        Area areaCopy{ *areaIt };
        m_areas.erase(areaIt);
        m_areas.insert(newName, std::move(areaCopy));
    }
}


void Inventory::clear()
{
    m_areas.clear();
}

void Inventory::removeObject(const QString& movingObjectName, const QString& areaName)
{
    auto areaIt{ m_areas.find(areaName) };
    if (areaIt == m_areas.end())
        return;

    const QHash<QString, MovingObject>& objectsList{ areaIt->getObjectsList() };
    const auto objectIt{ objectsList.find(movingObjectName) };
    if (objectIt != objectsList.end())
    {
        m_totalVolume -= objectIt->getTotalVolume();
        areaIt->removeObject(movingObjectName);

        if (objectsList.isEmpty())
            m_areas.erase(areaIt);
    }
}

#include "inventaire.h"

void Inventory::handleInventoryAnalysis(double listTotalVolume, QVector<MovingObject>& objectList)
{
    clear();

    m_totalVolume = listTotalVolume;

    for (MovingObject& object : objectList)
    {
        auto areaIt{ m_areas.find(object.getAreaKey()) };
        if (areaIt == m_areas.end())
        {
            addArea(object.getAreaKey());
            
            areaIt = m_areas.find(object.getAreaKey());
            if (areaIt != m_areas.end())
                areaIt->addObject(std::move(object));
        }

        else
            areaIt->addObject(std::move(object));
    }

    emit sendNewInventory(*this);
}

/*
    ADD movingObject to areaName
    If areaName doesn't exist, CREATE a new Area of areaName then ADD
*/
void Inventory::addObject(MovingObject movingObject, const QString& areaName)
{
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
    Q_ASSERT(areaIt != m_areas.end());
    if (areaIt == m_areas.end())
    {
        qCritical() << "[Inventory::modifyObject] MODIFIED OBJECT SHOULD HAVE AN AREA BUT IT'S EMPTY !";
        return;
    }

    m_totalVolume -= objectToModify->getTotalVolume();
    m_totalVolume += newObject.getTotalVolume();
    
    bool isSameArea{ oldAreaKey == newAreaKey };
    if (isSameArea)
    {
        areaIt->replaceObjectWithNew(objectToModify, std::move(newObject));
        return;
    }

    areaIt->removeObject(objectToModify->getName()); // Remove object from current Area
    if (areaIt->getObjectsList().isEmpty())
        m_areas.erase(areaIt); // Erase area if empty
    
    areaIt = m_areas.find(newAreaKey);
    if (areaIt == m_areas.end())
    {
        addArea(newAreaKey);
        addObject(std::move(newObject), newAreaKey);
        return;
    }

    const MovingObject* existingObject{ areaIt->findObject(newObject.getName()) }; // Checking if there's another object of the same name in new Area
    if (existingObject)
    {
        if (existingObject->getUnitaryVolume() == newObject.getUnitaryVolume())
            newObject.add(existingObject->getQuantity());
    
        else
        areaIt->replaceObjectWithNew(existingObject, std::move(newObject)); // REMPLACE L'OBJET PAR LE NOUVEAU
    }
    
    else
        areaIt->addObject(std::move(newObject));
}

void Inventory::addArea(QString areaName)
{
    auto [it, inserted] = m_areas.insert(areaName, Area{ areaName });

    if (!inserted)
    {
        // La clé existait déjà - gérer le cas ici
        // TODO : Si une pièce existe déjà sous le nom areaName, ajouter "#2"
        // ou afficher QMessageBox pour prévenir l'utilisateur
    }
}

void Inventory::removeArea(const QString& areaName)
{
    auto it{ m_areas.find(areaName) };
    if (it != m_areas.end())
    {
        m_totalVolume -= it->getTotalVolume();
        m_areas.erase(it);
    }
}

int Inventory::getTotalObjectsQuantity() const
{
    int size{};
    for (const auto& area : m_areas)
        size += static_cast<int>(area.getObjectsList().size());

    return size;
}

const Area* Inventory::findArea(const QString& areaKey) const
{
    const auto areaIt{ m_areas.find(areaKey) };
    if (areaIt == m_areas.end())
        return nullptr;

    return &(*areaIt);
}

/*
    Modify Area currentName with a newName
    Does nothing is an Area with newName already exist
    Does nothing is currentName Area doesn't exist
*/
void Inventory::modifyAreaName(const QString& currentName, const QString& newName)
{
    auto areaIt{ m_areas.find(currentName) };
    if (areaIt == m_areas.end())
    {
        qWarning() << "[Inventory::modifyAreaName] AREA '" << currentName << "' DOESN'T EXIST";
        return;
    }

    else if (m_areas.find(newName) != m_areas.end())
    {
        qWarning() << "[Inventory::modifyAreaName] AREA '" << newName << "' ALREADY EXIST";
        return;
    }
    
    areaIt->updateObjectsAreaKey(newName);
    areaIt->setName(newName);
    
    Area areaCopy{ *areaIt };
    m_areas.erase(areaIt);
    m_areas.insert(newName, std::move(areaCopy));
}

/*
    Delete all Areas in the Inventory
*/
void Inventory::clear()
{
    m_areas.clear();
}

/*
    REMOVE movingObjectName FROM Area areaName
    Does NOTHING if Area areaName doesn't EXIST
    Does NOTHING if MovingObject movingObjectName doesn't EXIST in Area areaName
*/
void Inventory::removeObject(const QString& movingObjectName, const QString& areaName)
{
    auto areaIt{ m_areas.find(areaName) };
    if (areaIt == m_areas.end())
    {
        qWarning() << "[Inventory::removeObject] Area '" << areaName << "' does not exist";
        return;
    }

    const QHash<QString, MovingObject>& objectsList{ areaIt->getObjectsList() };
    const auto objectIt{ objectsList.find(movingObjectName) };
    if (objectIt == objectsList.end())
    {
        qWarning() << "[Inventory::removeObject] Moving Object '" << movingObjectName << "' doesn't EXIST in Area '" << areaName << "'";
        return;
    }

    m_totalVolume -= objectIt->getTotalVolume();
    areaIt->removeObject(movingObjectName);

    if (objectsList.isEmpty())
        m_areas.erase(areaIt);
}

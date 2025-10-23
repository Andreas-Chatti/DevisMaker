#include "inventaire.h"

void Inventory::handleInventoryAnalysis(double totalVolume, const QStringList& structuredItems)
{
    clearInventory();

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
                double unitVolume{ volumeText.toDouble(&ok) / quantity };

                if (ok && unitVolume > 0.0)
                    addObjectByName(cleanName, unitVolume, quantity);
            }
        }
    }

    emit sendNewInventory(*this);
}


void Inventory::addObjectByName(const QString& name, double unitaryVolume, int quantity)
{
    for (MovingObject& object : m_objects)
    {
        if (object.getName() == name && object.getUnitaryVolume() == unitaryVolume)
        {
            object.add(quantity);
            return;
        }
    }

    m_objects.emplaceBack(MovingObject{ name, unitaryVolume, quantity });
}

void Inventory::addObject(const MovingObject& movingObject, int quantity)
{
    if (movingObject.getName().isEmpty())
        return;

    for (MovingObject& object : m_objects)
    {
        if (object == movingObject)
        {
            object.add(quantity);
            m_totalVolume += object.getUnitaryVolume() * quantity;
            return;
        }
    }
    m_objects.emplaceBack(movingObject);
    m_totalVolume += movingObject.getTotalVolume();
}


void Inventory::removeObjectByNameAndQuantity(const QString& name, int quantity)
{
    const auto isSameObject{ [&](const MovingObject& object) {
        if (object.getName() == name)
        return true;

        return false;
    } };

    if (quantity <= 0)
        m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), isSameObject), m_objects.end());

    else
    {
        auto it{ std::find_if(m_objects.begin(), m_objects.end(), isSameObject) };

        if (it != m_objects.end())
        {
            int newQuantity{ it->remove(quantity) };
            if (newQuantity <= 0)
                m_objects.erase(it);
        }
    }
}

void Inventory::removeObjectByQuantity(const MovingObject& movingObject, int quantity)
{
    const auto isSameObject{ [&](const MovingObject& object) {
        if (object == movingObject)
            return true;

        return false;
    } };

    if (quantity <= 0)
        m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), isSameObject), m_objects.end());

    else
    {
        auto it{ std::find_if(m_objects.begin(), m_objects.end(), isSameObject) };

        if (it != m_objects.end())
        {
            int newQuantity{ it->remove(quantity) };
            if (newQuantity <= 0)
                m_objects.erase(it);
        }
    }
}

void Inventory::modifyObject(const MovingObject& objectToModify, MovingObject newObject)
{
    const auto isSameObject{ [&](const MovingObject& object) {
    if (object == objectToModify)
        return true;

    return false;
} };

    auto object{ std::find_if(m_objects.begin(), m_objects.end(), isSameObject) };

    if (object != m_objects.end())
    {
        m_totalVolume -= object->getTotalVolume();
        *object = std::move(newObject);
        m_totalVolume += object->getTotalVolume();
    }
}


void Inventory::clearInventory()
{
    m_objects.clear();
}

void Inventory::removeObject(const MovingObject& movingObject)
{
    const auto isSameObject{ [&](const MovingObject& object) {
    if (object == movingObject)
        return true;

    return false;
    } };

    auto it{ std::find_if(m_objects.begin(), m_objects.end(), isSameObject) };
    if (it != m_objects.end())
    {
        m_totalVolume -= it->getTotalVolume();
        m_objects.erase(it);
    }
}

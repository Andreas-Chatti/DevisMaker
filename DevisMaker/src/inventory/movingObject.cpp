#include "movingObject.h"

MovingObject::MovingObject(QString objectName, double unitaryVolume, QString areaKey, int quantity, bool disassembly, bool assembly, bool heavy, QString notes)
    : m_name{ std::move(objectName) }
    , m_unitaryVolume{ unitaryVolume }
    , m_areaKey{ std::move(areaKey) }
    , m_quantity{ quantity }
    , m_totalVolume{ m_unitaryVolume * m_quantity }
    , m_disassembly{ disassembly }
    , m_assembly{ assembly }
    , m_heavy{ heavy }
    , m_note{ std::move(notes) }
{
    Q_ASSERT(!m_name.isEmpty());
}

MovingObject::MovingObject()
    : MovingObject("UNDEFINED", 0.0, "default", 0, false, false, false, "UNDEFINED")
{
}

MovingObject::MovingObject(const MovingObject& object)
    : MovingObject(object.m_name, object.m_unitaryVolume, object.m_areaKey, object.m_quantity, object.m_disassembly, object.m_assembly,
        object.m_heavy, object.m_note)
{
}

MovingObject& MovingObject::operator=(const MovingObject& object)
{
    if (&object == this)
        return *this;

    m_name = object.m_name;
    m_unitaryVolume = object.m_unitaryVolume;
    m_areaKey = object.m_areaKey;
    m_quantity = object.m_quantity;
    m_totalVolume = object.m_unitaryVolume * object.m_quantity;
    m_disassembly = object.m_disassembly;
    m_assembly = object.m_assembly;
    m_heavy = object.m_heavy;
    m_note = object.m_note;

    return *this;
}

MovingObject::MovingObject(MovingObject&& object) noexcept
    : MovingObject(std::move(object.m_name), object.m_unitaryVolume, std::move(object.m_areaKey), object.m_quantity, object.m_disassembly, object.m_assembly,
        object.m_heavy, std::move(object.m_note))
{
}

MovingObject& MovingObject::operator=(MovingObject&& object) noexcept
{
    if (&object == this)
        return *this;

    m_name = std::move(object.m_name);
    m_unitaryVolume = object.m_unitaryVolume;
    m_areaKey = std::move(object.m_areaKey);
    m_quantity = object.m_quantity;
    m_totalVolume = object.m_unitaryVolume * object.m_quantity;
    m_disassembly = object.m_disassembly;
    m_assembly = object.m_assembly;
    m_heavy = object.m_heavy;
    m_note = std::move(object.m_note);

    return *this;
}

bool MovingObject::operator== (const MovingObject& movingObject) const
{
    return (this->m_name == movingObject.getName()) && (this->m_unitaryVolume == movingObject.getUnitaryVolume());
}

int MovingObject::add()
{
    m_totalVolume += m_unitaryVolume;
    return ++m_quantity;
}

int MovingObject::add(int quantity)
{
    m_totalVolume += m_unitaryVolume * quantity;
    return m_quantity += quantity;
}

int MovingObject::remove()
{
    if (m_quantity == 0)
        return 0;

    m_totalVolume -= m_unitaryVolume;
    return --m_quantity;
}

int MovingObject::remove(int quantity)
{
    if (quantity >= m_quantity)
    {
        m_totalVolume = 0;
        return m_quantity = 0;
    }

    m_totalVolume -= m_unitaryVolume * quantity;
    return m_quantity -= quantity;
}
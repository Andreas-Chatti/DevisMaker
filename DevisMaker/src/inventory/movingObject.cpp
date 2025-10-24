#include "movingObject.h"

MovingObject::MovingObject(QString objectName, double unitaryVolume, int quantity, bool disassembly, bool assembly, bool heavy, QString notes)
    : m_name{ objectName }
    , m_unitaryVolume{ unitaryVolume }
    , m_quantity{ quantity }
    , m_totalVolume{ m_unitaryVolume * m_quantity }
    , m_disassembly{ disassembly }
    , m_assembly{ assembly }
    , m_heavy{ heavy }
    , m_note{ notes }
{
}

MovingObject::MovingObject(const MovingObject& object)
    : m_name{ object.m_name }
    , m_unitaryVolume{ object.m_unitaryVolume }
    , m_quantity{ object.m_quantity }
    , m_totalVolume{ object.m_unitaryVolume * object.m_quantity }
    , m_disassembly{ object.m_disassembly }
    , m_assembly{ object.m_assembly }
    , m_heavy{ object.m_heavy }
    , m_note{ object.m_note }
{
}

MovingObject& MovingObject::operator=(const MovingObject& object)
{
    if (&object == this)
        return *this;

    m_name = object.m_name;
    m_unitaryVolume = object.m_unitaryVolume;
    m_quantity = object.m_quantity;
    m_totalVolume = object.m_unitaryVolume * object.m_quantity;
    m_disassembly = object.m_disassembly;
    m_assembly = object.m_assembly;
    m_heavy = object.m_heavy;
    m_note = object.m_note;

    return *this;
}

MovingObject::MovingObject(MovingObject&& object) noexcept
    : m_name{ std::move(object.m_name) }
    , m_unitaryVolume{ object.m_unitaryVolume }
    , m_quantity{ object.m_quantity }
    , m_totalVolume{ object.m_unitaryVolume * object.m_quantity }
    , m_disassembly{ object.m_disassembly }
    , m_assembly{ object.m_assembly }
    , m_heavy{ object.m_heavy }
    , m_note{ std::move(object.m_note) }
{
}

MovingObject& MovingObject::operator=(MovingObject&& object) noexcept
{
    if (&object == this)
        return *this;

    m_name = std::move(object.m_name);
    m_unitaryVolume = object.m_unitaryVolume;
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
    if (quantity > m_quantity)
    {
        m_totalVolume -= m_unitaryVolume * m_quantity;
        return 0;
    }

    m_totalVolume -= m_unitaryVolume * quantity;
    return m_quantity -= quantity;
}
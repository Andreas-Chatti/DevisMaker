#pragma once
#include <QString>

class MovingObject
{
public:

	explicit MovingObject(QString objectName, double unitaryVolume, int quantity = 1, bool disassembly = false, bool assembly = false, bool heavy = false)
		: m_name{ objectName }
		, m_unitaryVolume{ unitaryVolume }
		, m_quantity{ quantity }
		, m_totalVolume{ m_unitaryVolume * m_quantity }
		, m_disassembly{ disassembly }
		, m_assembly{ assembly }
		, m_heavy{ heavy }
	{
	}

	MovingObject(const MovingObject& object)
		: m_name{ object.m_name }
		, m_unitaryVolume{ object.m_unitaryVolume }
		, m_quantity{ object.m_quantity }
		, m_totalVolume{ object.m_unitaryVolume * object.m_quantity }
		, m_disassembly{ object.m_disassembly }
		, m_assembly{ object.m_assembly }
		, m_heavy{ object.m_heavy }
	{
	}

	MovingObject& operator=(const MovingObject& object)
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

		return *this;
	}

	MovingObject(MovingObject&& object) noexcept
		: m_name{ std::move(object.m_name) }
		, m_unitaryVolume{ object.m_unitaryVolume }
		, m_quantity{ object.m_quantity }
		, m_totalVolume{ object.m_unitaryVolume * object.m_quantity }
		, m_disassembly{ object.m_disassembly }
		, m_assembly{ object.m_assembly }
		, m_heavy{ object.m_heavy }
	{
	}

	MovingObject& operator=(MovingObject&& object) noexcept
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

		return *this;
	}

	bool operator== (const MovingObject& movingObject) const
	{
		return (this->m_name == movingObject.getName()) && (this->m_unitaryVolume == movingObject.getUnitaryVolume());
	}

	const QString& getName() const { return m_name; }
	double getUnitaryVolume() const { return m_unitaryVolume; }
	int getQuantity() const { return m_quantity; }
	double getTotalVolume() const { return m_totalVolume; }

	int add()
	{ 
		m_totalVolume += m_unitaryVolume;
		return ++m_quantity; 
	}

	int add(int quantity)
	{
		m_totalVolume += m_unitaryVolume * quantity;
		return m_quantity += quantity;
	}

	int remove() 
	{ 
		if (m_quantity == 0)
			return 0;

		m_totalVolume -= m_unitaryVolume;
		return --m_quantity; 
	}

	int remove(int quantity)
	{
		if (quantity > m_quantity)
		{
			m_totalVolume -= m_unitaryVolume * m_quantity;
			return 0;
		}

		m_totalVolume -= m_unitaryVolume * quantity;
		return m_quantity -= quantity;
	}

	bool isDisassembly() const { return m_disassembly; }
	bool isAssembly() const { return m_assembly; }
	bool isHeavy() const { return m_heavy; }

private:

	QString m_name;
	double m_unitaryVolume;
	int m_quantity;
	double m_totalVolume;
	bool m_disassembly;
	bool m_assembly;
	bool m_heavy;
};
#pragma once
#include <QString>

class MovingObject
{
public:

	MovingObject(QString objectName, double unitaryVolume, int quantity = 1, bool disassembly = false, bool assembly = false, bool heavy = false)
		: m_name{ objectName }
		, m_unitaryVolume{ unitaryVolume }
		, m_quantity{ quantity }
		, m_totalVolume{}
		, m_disassembly{ disassembly }
		, m_assembly{ assembly }
		, m_heavy{ heavy }
	{
		m_totalVolume = m_unitaryVolume * m_quantity;
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
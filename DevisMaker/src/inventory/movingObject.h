#pragma once
#include <QString>

class MovingObject
{
public:

	explicit MovingObject(QString objectName, double unitaryVolume, int quantity = 1, bool disassembly = false, bool assembly = false, bool heavy = false, QString notes = "");
	MovingObject(const MovingObject& object);
	MovingObject& operator=(const MovingObject& object);
	MovingObject(MovingObject&& object) noexcept;
	MovingObject& operator=(MovingObject&& object) noexcept;
	bool operator== (const MovingObject& movingObject) const;

	int add();
	int add(int quantity);
	int remove();
	int remove(int quantity);

	const QString& getName() const { return m_name; }
	double getUnitaryVolume() const { return m_unitaryVolume; }
	int getQuantity() const { return m_quantity; }
	double getTotalVolume() const { return m_totalVolume; }
	bool isDisassembly() const { return m_disassembly; }
	bool isAssembly() const { return m_assembly; }
	bool isHeavy() const { return m_heavy; }
	const QString& getNote() const { return m_note; }

	void setName(QString name) { m_name = std::move(name); }
	void setUnitaryVolume(double unitaryVolume) 
	{ 
		m_unitaryVolume = unitaryVolume; 
		m_totalVolume = m_unitaryVolume * m_quantity;
	}
	void setQuantity(int quantity) 
	{ 
		m_quantity = quantity; 
		m_totalVolume = m_unitaryVolume * m_quantity;
	}
	void setDisassembly(bool isDisassembly) { m_disassembly = isDisassembly; }
	void setAssembly(bool isAssembly) { m_assembly = isAssembly; }
	void setHeavy(bool isHeavy) { m_heavy = isHeavy; }
	void setNote(QString note) { m_note = std::move(note); }

private:

	QString m_name;
	double m_unitaryVolume;
	int m_quantity;
	double m_totalVolume;
	bool m_disassembly;
	bool m_assembly;
	bool m_heavy;
	QString m_note;
};
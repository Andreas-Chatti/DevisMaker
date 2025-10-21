#include "ItemModifyierDialog.h"

ItemModifyierDialog::ItemModifyierDialog(EditState state, QWidget* parent, MovingObject movingObject)
	: QDialog{ parent }
	, m_object{ std::move(movingObject) }
	, m_state{ state }
{
	ui.setupUi(this);

	if (m_state == EditState::MODIFY)
	{
		ui.nameLineEdit->setText(m_object.getName());
		ui.quantitySpinBox->setValue(m_object.getQuantity());
		ui.volumeDoubleSpinBox->setValue(m_object.getUnitaryVolume());
		ui.demontageCheckBox->setChecked(m_object.isDisassembly());
		ui.remontageCheckBox->setChecked(m_object.isAssembly());
		ui.heavyCheckBox->setChecked(m_object.isHeavy());
		ui.noteTextEdit->setText("");
	}
}

void ItemModifyierDialog::on_buttonBox_accepted()
{
	if (ui.nameLineEdit->text().isEmpty() || ui.nameLineEdit->text().length() <= 3)
	{
		QMessageBox::warning(this, "Erreur", QString::fromUtf8("Veuillez entrer un nom de plus de 3 caractères pour cet élément."));
		return;
	}

	switch (m_state)
	{
	case ItemModifyierDialog::EditState::MODIFY:
		emit editObjectFromInventory(m_object,
			MovingObject{
			ui.nameLineEdit->text(),
			ui.volumeDoubleSpinBox->value(),
			ui.quantitySpinBox->value(),
			ui.demontageCheckBox->isChecked(),
			ui.remontageCheckBox->isChecked(),
			ui.heavyCheckBox->isChecked() });
		break;

	case ItemModifyierDialog::EditState::ADD:
		emit addObjectToInventory(MovingObject{
			ui.nameLineEdit->text(),
			ui.volumeDoubleSpinBox->value(),
			ui.quantitySpinBox->value(),
			ui.demontageCheckBox->isChecked(),
			ui.remontageCheckBox->isChecked(),
			ui.heavyCheckBox->isChecked() },
			ui.quantitySpinBox->value());
		break;
	}
}

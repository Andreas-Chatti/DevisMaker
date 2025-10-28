#include "ItemModifyierDialog.h"

ItemModifyierDialog::ItemModifyierDialog(EditState state, const Inventory* inventory, QWidget* parent, const MovingObject* movingObject)
	: QDialog{ parent }
	, m_inventory{ inventory }
	, m_modifiedObject{ movingObject }
	, m_state{ state }
{
	ui.setupUi(this);

	const auto& areas{ m_inventory->getAreas() };
	for (const auto& area : areas)
		ui.areaComboBox->addItem(area.getName());

	if (m_state == EditState::MODIFY)
	{
		ui.nameLineEdit->setText(m_modifiedObject->getName());
		ui.quantitySpinBox->setValue(m_modifiedObject->getQuantity());
		ui.volumeDoubleSpinBox->setValue(m_modifiedObject->getUnitaryVolume());
		ui.demontageCheckBox->setChecked(m_modifiedObject->isDisassembly());
		ui.remontageCheckBox->setChecked(m_modifiedObject->isAssembly());
		ui.heavyCheckBox->setChecked(m_modifiedObject->isHeavy());
		ui.noteTextEdit->setText(m_modifiedObject->getNote());

		const QString& AreaKey{ m_modifiedObject->getAreaKey() };
		int areaIndex{ ui.areaComboBox->findText(AreaKey) };
		if (areaIndex != -1)
			ui.areaComboBox->setCurrentIndex(areaIndex);
	}

	if (m_state == EditState::ADD)
		connect(this, &ItemModifyierDialog::addObjectToInventory, m_inventory, &Inventory::addObject);
	else
		connect(this, &ItemModifyierDialog::editObjectFromInventory, m_inventory, &Inventory::modifyObject);

	connect(this, &ItemModifyierDialog::addAreaToInventory, m_inventory, &Inventory::addArea);
	disconnect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void ItemModifyierDialog::on_buttonBox_accepted()
{
	if (ui.nameLineEdit->text().isEmpty() || ui.nameLineEdit->text().length() <= 2)
	{
		QMessageBox::warning(this, "Erreur", "Veuillez entrer un nom de plus de 2 caractères pour cet élément.");
		return;
	}

	else if (ui.areaComboBox->currentIndex() == -1)
	{
		QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une pièce pour cet élément.");
		return;
	}

	MovingObject newObject{
		ui.nameLineEdit->text().trimmed().toLower(),
		ui.volumeDoubleSpinBox->value(),
		ui.areaComboBox->currentText(),
		ui.quantitySpinBox->value(),
		ui.demontageCheckBox->isChecked(),
		ui.remontageCheckBox->isChecked(),
		ui.heavyCheckBox->isChecked(),
		ui.noteTextEdit->toPlainText()
	};

	switch (m_state)
	{
	case ItemModifyierDialog::EditState::MODIFY:
	{
		const Area* newObjectArea{ m_inventory->findArea(newObject.getAreaKey()) };
		const MovingObject* existingObject{ nullptr };
		if (newObjectArea)
			existingObject = newObjectArea->findObject(newObject.getName());

		if (existingObject && existingObject->getUnitaryVolume() != newObject.getUnitaryVolume())
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle("Objet existant détecté");
			msgBox.setText("Un objet portant le même nom avec des données différentes existe déjà dans la pièce sélectionnée.\n"
				"Continuer va écraser l'objet existant par le nouveau.\nVoulez-vous continuer ?");
			msgBox.setIcon(QMessageBox::Warning);
			QAbstractButton* continueButton = msgBox.addButton("Continuer", QMessageBox::AcceptRole);
			QAbstractButton* cancelButton = msgBox.addButton("Annuler", QMessageBox::RejectRole);
			msgBox.setDefaultButton(static_cast<QPushButton*>(continueButton));
			msgBox.exec();

			if (msgBox.clickedButton() == cancelButton)
				return;
		}

		emit editObjectFromInventory(m_modifiedObject, newObject);
		break;
	}
	case ItemModifyierDialog::EditState::ADD:
		emit addObjectToInventory(newObject, ui.areaComboBox->currentText());
		break;
	}

	accept();
}

void ItemModifyierDialog::on_addAreaButton_clicked()
{
	QInputDialog dialog(this);
	dialog.setWindowTitle("Nouvelle pièce");
	dialog.setLabelText("Nom de la pièce :");
	dialog.setTextValue("");
	dialog.setInputMode(QInputDialog::TextInput);

	QLineEdit* lineEdit{ dialog.findChild<QLineEdit*>() };
	if (lineEdit)
	{
		lineEdit->setMaxLength(20);
		lineEdit->setPlaceholderText("Ex: Chambre 1, Salon principal...");
	}

	if (dialog.exec() == QDialog::Accepted)
	{
		QString areaName{ dialog.textValue().trimmed().toLower() };

		if (!areaName.isEmpty())
		{
			const auto& areas{ m_inventory->getAreas() };
			const auto it{ areas.find(areaName) };
			if (it == areas.end())
			{
				ui.areaComboBox->addItem(areaName);
				int areaIndex{ ui.areaComboBox->findText(areaName) };
				if (areaIndex != -1)
					ui.areaComboBox->setCurrentIndex(areaIndex);

				emit addAreaToInventory(std::move(areaName));
			}

			else
				QMessageBox::warning(
					this,
					"Nom existant",
					"Le nom de la pièce existe déjà\n Essayez avec un autre nom."
				);
		}

		else
		{
			QMessageBox::warning(
				this,
				"Nom invalide",
				"Le nom de la pièce ne peut pas être vide."
			);
		}
	}
}

#include "InventoryModifyierDialog.h"

InventoryModifyierDialog::InventoryModifyierDialog(const Inventory& inventory, QWidget* parent)
	: QDialog{parent}
    , m_inventory{inventory}
{
	ui.setupUi(this);
    setupUi();
    displayInventory();

    connect(this, &InventoryModifyierDialog::removeArea, &m_inventory, &Inventory::removeArea);
    connect(this, &InventoryModifyierDialog::modifyAreaName, &m_inventory, &Inventory::modifyAreaName);
}

void InventoryModifyierDialog::on_addItemButton_clicked()
{
    auto dialog{ std::make_unique<ItemModifyierDialog>(ItemModifyierDialog::EditState::ADD, &m_inventory, this) };
    dialog->exec();
    displayInventory();
}

void InventoryModifyierDialog::on_modifyItemButton_clicked()
{
    std::optional<ItemType> itemType{ getSelectedItemType() };
    if (itemType && itemType == ItemType::AreaHeader)
    {
        QDialog renameDialog(this);
        renameDialog.setWindowTitle("Renommer pièce");

        QVBoxLayout* layout{ new QVBoxLayout(&renameDialog) };

        QLabel* label{ new QLabel("Saisissez un nouveau nom pour cette pièce :", &renameDialog) };
        layout->addWidget(label);

        QLineEdit* lineEdit{ new QLineEdit(&renameDialog) };
        lineEdit->setMaxLength(20);
        layout->addWidget(lineEdit);

        QHBoxLayout* buttonLayout{ new QHBoxLayout() };
        QPushButton* changeButton{new QPushButton("Changer", &renameDialog)};
        QPushButton* cancelButton{ new QPushButton("Annuler", &renameDialog) };
        buttonLayout->addWidget(changeButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);

        QObject::connect(changeButton, &QPushButton::clicked, &renameDialog, &QDialog::accept);
        QObject::connect(cancelButton, &QPushButton::clicked, &renameDialog, &QDialog::reject);

        if (renameDialog.exec() == QDialog::Accepted)
        {
            QString newAreaName{ lineEdit->text().trimmed().toLower() };
            std::optional<QString> currentAreaName{ getSelectedMovingObjectArea() };
            if (!newAreaName.isEmpty() && currentAreaName)
            {
                if(currentAreaName == newAreaName)
                    QMessageBox::critical(this, "Erreur", "Vous devez mettre un nom différent du nom actuel !");

                else if (m_inventory.findArea(newAreaName))
                    QMessageBox::critical(this, "Erreur", "Une pièce avec le même nom existe déjà !");
                
                else if(!m_inventory.findArea(currentAreaName.value()))
                    QMessageBox::critical(this, "Erreur", "La pièce sélectionnée n'existe pas !");

                else
                {
                    emit modifyAreaName(currentAreaName.value(), newAreaName);
                    displayInventory();
                }
            }
        }
    }

    else
    {
        auto dialog{ std::make_unique<ItemModifyierDialog>(ItemModifyierDialog::EditState::MODIFY, &m_inventory, this, getMovingObjectFromSelection()) };
        dialog->exec();
    }

    displayInventory();
}

void InventoryModifyierDialog::on_removeItemButton_clicked()
{
    std::optional<ItemType> itemType{ getSelectedItemType() };
    if (itemType && itemType == ItemType::AreaHeader)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Supprimer la pièce");
        msgBox.setText("Si vous supprimez cette pièce, tous les objets à l'intérieur seront supprimés aussi.\nVoulez-vous continuer ?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.button(QMessageBox::Ok)->setText("Supprimer");
        msgBox.button(QMessageBox::Cancel)->setText("Annuler");

        int ret{ msgBox.exec() };
        bool confirmed{ (ret == QMessageBox::Ok) };

        if (!confirmed)
            return;

        std::optional<QString> areaName{ getSelectedMovingObjectArea() };
        if (areaName)
        {
            emit removeArea(areaName.value());
            int selectedRow{ ui.inventoryTableWidget->currentRow() };
            ui.inventoryTableWidget->removeRow(selectedRow);
        }
    }

    else
    {
        const MovingObject* selectedObject{ getMovingObjectFromSelection() };
        std::optional<QString> areaName{ getSelectedMovingObjectArea() };
        if (selectedObject && areaName)
            emit removeItem(selectedObject->getName(), areaName.value());
    }

    displayInventory();
}

void InventoryModifyierDialog::on_inventoryTableWidget_itemSelectionChanged()
{
    bool hasSelection{ !ui.inventoryTableWidget->selectedItems().isEmpty() };
    if (hasSelection)
    {
        ui.modifyItemButton->setEnabled(true);
        ui.removeItemButton->setEnabled(true);
    }

    else
    {
        ui.modifyItemButton->setEnabled(false);
        ui.removeItemButton->setEnabled(false);
    }
}

void InventoryModifyierDialog::setupUi()
{
    ui.inventoryTableWidget->setColumnWidth(0, 150);  // Élément
    ui.inventoryTableWidget->setColumnWidth(1, 80);   // Quantité
    ui.inventoryTableWidget->setColumnWidth(2, 100);  // Volume
    ui.inventoryTableWidget->setColumnWidth(3, 100);  // Démontage
    ui.inventoryTableWidget->setColumnWidth(4, 100);  // Remontage
    ui.inventoryTableWidget->setColumnWidth(5, 100);  // Objet Lourd
    ui.inventoryTableWidget->horizontalHeader()->setStretchLastSection(true);
}

void InventoryModifyierDialog::displayInventory()
{
    ui.inventoryTableWidget->setRowCount(0);

    int objectsQuantity{ m_inventory.objectsQuantity() };
    if (objectsQuantity <= 0)
    {
        ui.titleLabel->setText("Inventaire vide");
        return;
    }

    ui.titleLabel->setText(QString{ "Inventaire (%1 " }.arg(objectsQuantity) + QString{ objectsQuantity >= 2 ? "éléments)" : "élément)" });

    const QHash<QString, Area>& areas{ m_inventory.getAreas() };
    for (const auto& area : areas)
    {
        addAreaItemToTable(area);
        const QHash<QString, MovingObject>& objects{ area.getObjectsList() };
        for (const auto& object : objects)
            addInventoryItemToTable(object, area.getName());
    }
}

void InventoryModifyierDialog::addAreaItemToTable(const Area& area)
{
    int row{ ui.inventoryTableWidget->rowCount() };
    ui.inventoryTableWidget->insertRow(row);

    QString areaName{ QString{ "%1" }.arg(area.getName()) };

    QTableWidgetItem* areaItem{ new QTableWidgetItem(areaName) };
    areaItem->setData(ITEM_TYPE_ROLE, static_cast<int>(ItemType::AreaHeader));
    areaItem->setData(AREA_NAME_ROLE, areaName);

    QFont font{ areaItem->font() };
    font.setBold(true);
    font.setPointSize(font.pointSize() + 2);
    areaItem->setFont(font);

    areaItem->setBackground(QColor(230, 240, 255));

    areaItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    ui.inventoryTableWidget->setItem(row, 0, areaItem);

    int columnCount{ ui.inventoryTableWidget->columnCount() };
    ui.inventoryTableWidget->setSpan(row, 0, 1, columnCount);
}

void InventoryModifyierDialog::addInventoryItemToTable(const MovingObject& movingObject, const QString& areaName)
{
    int row{ ui.inventoryTableWidget->rowCount() };
    ui.inventoryTableWidget->insertRow(row);

    // Colonne 0 : Nom de l'élément
    QTableWidgetItem* nameItem{ new QTableWidgetItem(movingObject.getName()) };
    nameItem->setData(AREA_NAME_ROLE, areaName);
    nameItem->setData(ITEM_TYPE_ROLE, static_cast<int>(ItemType::MovingObject));
    ui.inventoryTableWidget->setItem(row, 0, nameItem);

    // Colonne 1 : Quantité
    QTableWidgetItem* quantityItem{ new QTableWidgetItem(QString::number(movingObject.getQuantity())) };
    quantityItem->setTextAlignment(Qt::AlignCenter);
    ui.inventoryTableWidget->setItem(row, 1, quantityItem);

    // Colonne 2 : Volume (formaté avec 2 décimales)
    QTableWidgetItem* volumeItem{ new QTableWidgetItem(QString::number(movingObject.getTotalVolume(), 'f', 2)) };
    volumeItem->setTextAlignment(Qt::AlignCenter);
    ui.inventoryTableWidget->setItem(row, 2, volumeItem);

    // Colonne 3 : Démontage
    QTableWidgetItem* disassemblyItem{ new QTableWidgetItem(movingObject.isDisassembly() ? "Oui" : "Non") };
    disassemblyItem->setTextAlignment(Qt::AlignCenter);
    if (movingObject.isDisassembly())
        disassemblyItem->setForeground(QColor(0, 120, 0));
    ui.inventoryTableWidget->setItem(row, 3, disassemblyItem);

    // Colonne 4 : Remontage
    QTableWidgetItem* assemblyItem{ new QTableWidgetItem(movingObject.isAssembly() ? "Oui" : "Non") };
    assemblyItem->setTextAlignment(Qt::AlignCenter);
    if (movingObject.isAssembly())
        assemblyItem->setForeground(QColor(0, 120, 0));
    ui.inventoryTableWidget->setItem(row, 4, assemblyItem);

    // Colonne 5 : Objet lourd
    QTableWidgetItem* heavyItem{ new QTableWidgetItem(movingObject.isHeavy() ? "Oui" : "Non") };
    heavyItem->setTextAlignment(Qt::AlignCenter);
    if (movingObject.isHeavy())
        heavyItem->setForeground(QColor(200, 0, 0));
    ui.inventoryTableWidget->setItem(row, 5, heavyItem);

    // Colonne 6 : Note
    QTableWidgetItem* noteItem{ new QTableWidgetItem(movingObject.getNote()) };
    ui.inventoryTableWidget->setItem(row, 6, noteItem);
}

const MovingObject* InventoryModifyierDialog::getMovingObjectFromSelection() const
{
    int selectedRow{ ui.inventoryTableWidget->currentRow() };

    if (selectedRow < 0)
        return nullptr;

    QTableWidgetItem* item{ ui.inventoryTableWidget->item(selectedRow, 0) };
    if (!item)
        return nullptr;

    QString areaKey{ item->data(AREA_NAME_ROLE).toString() };
    const Area* area{ m_inventory.findArea(areaKey) };
    if (!area)
        return nullptr;

    QString objectKey{ item->text() };

    return area->findObject(objectKey);
}

std::optional<QString> InventoryModifyierDialog::getSelectedMovingObjectArea() const
{
    int selectedRow{ ui.inventoryTableWidget->currentRow() };

    if (selectedRow < 0)
        return std::nullopt;

    QTableWidgetItem* item{ ui.inventoryTableWidget->item(selectedRow, 0) };
    if (!item)
        return std::nullopt;

    return item->data(AREA_NAME_ROLE).toString();
}

std::optional<InventoryModifyierDialog::ItemType> InventoryModifyierDialog::getSelectedItemType() const
{
    int selectedRow{ ui.inventoryTableWidget->currentRow() };
    if (selectedRow < 0)
        return std::nullopt;

    QTableWidgetItem* item{ ui.inventoryTableWidget->item(selectedRow, 0) };
    if (!item)
        return std::nullopt;

    return std::make_optional(static_cast<ItemType>(item->data(ITEM_TYPE_ROLE).toInt()));
}

#include "InventoryModifyierDialog.h"

InventoryModifyierDialog::InventoryModifyierDialog(const Inventory& inventory, QWidget* parent)
	: QDialog{parent}
    , m_inventory{inventory}
{
	ui.setupUi(this);
    setupUi();
    displayInventory();
}

void InventoryModifyierDialog::on_addItemButton_clicked()
{
    // TODO: Créer la logique qui différenciera la sélection entre un item inventaire et une pièce
    // comme ça on pourra aussi ajouter/modifier/supprimer des pièces
    if (getSelectedItemType() == ItemType::AreaHeader)
    {

    }

    else
    {
        auto dialog{ std::make_unique<ItemModifyierDialog>(ItemModifyierDialog::EditState::ADD, &m_inventory, this) };
        dialog->exec();
    }

    displayInventory();
}

void InventoryModifyierDialog::on_modifyItemButton_clicked()
{
    // TODO: Créer la logique qui différenciera la sélection entre un item inventaire et une pièce
    // comme ça on pourra aussi ajouter/modifier/supprimer des pièces

    if (getSelectedItemType() == ItemType::AreaHeader)
    {

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
    // TODO: Créer la logique qui différenciera la sélection entre un item inventaire et une pièce
    // comme ça on pourra aussi ajouter/modifier/supprimer des pièces
    if (getSelectedItemType() == ItemType::AreaHeader)
    {

    }

    else
    {
        const MovingObject* selectedObject{ getMovingObjectFromSelection() };
        if (selectedObject)
            emit removeItem(selectedObject->getName(), getSelectedMovingObjectArea());
    }

    int selectedRow{ ui.inventoryTableWidget->currentRow() };
    ui.inventoryTableWidget->removeRow(selectedRow);
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

    QString areaText{ QString{ "%1" }.arg(area.getName()) };

    QTableWidgetItem* areaItem{ new QTableWidgetItem(areaText) };
    areaItem->setData(ITEM_TYPE_ROLE, static_cast<int>(ItemType::AreaHeader));

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
    nameItem->setData(AREA_NAME_ROLE, areaName); // Pas sûr de cette ligne, à vérifier plus tard
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

    QString areaKey{ item->data(Qt::UserRole).toString() };
    const Area* area{ m_inventory.findArea(areaKey) };
    if (!area)
        return nullptr;

    QString objectKey{ item->text() };

    return area->findObject(objectKey);
}

QString InventoryModifyierDialog::getSelectedMovingObjectArea() const
{
    int selectedRow{ ui.inventoryTableWidget->currentRow() };

    if (selectedRow < 0)
        return nullptr;

    QTableWidgetItem* item{ ui.inventoryTableWidget->item(selectedRow, 0) };
    if (!item)
        return nullptr;

    return item->data(Qt::UserRole).toString();
}

InventoryModifyierDialog::ItemType InventoryModifyierDialog::getSelectedItemType() const
{
    int selectedRow{ ui.inventoryTableWidget->currentRow() };
    if (selectedRow < 0)
        return;
    QTableWidgetItem* item{ ui.inventoryTableWidget->item(selectedRow, 0) };
    if (!item)
        return;

    return static_cast<ItemType>(item->data(ITEM_TYPE_ROLE).toInt());
}

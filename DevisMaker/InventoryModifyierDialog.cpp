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
    auto dialog{ std::make_unique<ItemModifyierDialog>(ItemModifyierDialog::EditState::ADD, this) };
    connect(dialog.get(), &ItemModifyierDialog::addObjectToInventory, &m_inventory, &Inventory::addObject);
    dialog->exec();
    displayInventory();
}

void InventoryModifyierDialog::on_modifyItemButton_clicked()
{
    auto dialog{ std::make_unique<ItemModifyierDialog>(ItemModifyierDialog::EditState::MODIFY, this, getMovingObjectFromSelection()) };
    connect(dialog.get(), &ItemModifyierDialog::editObjectFromInventory, &m_inventory, &Inventory::modifyObject);
    dialog->exec();
    displayInventory();
}

void InventoryModifyierDialog::on_removeItemButton_clicked()
{
    emit removeItem(getMovingObjectFromSelection());

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

    const QVector<MovingObject>& objects{ m_inventory.getInventory() };

    if (objects.isEmpty())
    {
        ui.titleLabel->setText("Inventaire vide");
        return;
    }

    ui.titleLabel->setText(QString{ "Inventaire (%1 " }.arg(objects.size()) + QString{ objects.size() >= 2 ? "éléments)" : "élément)" });

    for (size_t i{}; i < objects.size(); i++)
        addInventoryItemToTable(objects[i], i, "");
}

void InventoryModifyierDialog::addInventoryItemToTable(const MovingObject& movingObject, size_t inventoryIndex, const QString& note)
{
    int row{ ui.inventoryTableWidget->rowCount() };
    ui.inventoryTableWidget->insertRow(row);

    // Colonne 0 : Nom de l'élément
    QTableWidgetItem* nameItem{ new QTableWidgetItem(movingObject.getName()) };
    nameItem->setData(Qt::UserRole, inventoryIndex);
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
        disassemblyItem->setForeground(QColor(0, 120, 0));  // Vert
    ui.inventoryTableWidget->setItem(row, 3, disassemblyItem);

    // Colonne 4 : Remontage
    QTableWidgetItem* assemblyItem{ new QTableWidgetItem(movingObject.isAssembly() ? "Oui" : "Non") };
    assemblyItem->setTextAlignment(Qt::AlignCenter);
    if (movingObject.isAssembly())
        assemblyItem->setForeground(QColor(0, 120, 0));  // Vert
    ui.inventoryTableWidget->setItem(row, 4, assemblyItem);

    // Colonne 5 : Objet lourd
    QTableWidgetItem* heavyItem{ new QTableWidgetItem(movingObject.isHeavy() ? "Oui" : "Non") };
    heavyItem->setTextAlignment(Qt::AlignCenter);
    if (movingObject.isHeavy())
        heavyItem->setForeground(QColor(200, 0, 0));  // Rouge
    ui.inventoryTableWidget->setItem(row, 5, heavyItem);

    // Colonne 6 : Note
    QTableWidgetItem* noteItem{ new QTableWidgetItem(note) };
    ui.inventoryTableWidget->setItem(row, 6, noteItem);
}

MovingObject InventoryModifyierDialog::getMovingObjectFromSelection() const
{
    int selectedRow{ ui.inventoryTableWidget->currentRow() };

    if (selectedRow < 0)
        return MovingObject{"", 0.0};

    // Récupérer l'index stocké
    QTableWidgetItem* item{ ui.inventoryTableWidget->item(selectedRow, 0) };
    if (!item)
        return MovingObject{ "", 0.0 };

    size_t inventoryIndex{ static_cast<size_t>(item->data(Qt::UserRole).toInt()) };

    // Accéder à l'objet original via l'index
    const QVector<MovingObject>& objects{ m_inventory.getInventory() };

    if (inventoryIndex >= objects.size())
        return MovingObject{ "", 0.0 };

    return MovingObject{ objects[inventoryIndex] };
}

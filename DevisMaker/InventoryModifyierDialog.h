#pragma once
#include <QDialog>
#include "ui_InventoryModifyierDialog.h"
#include "ItemModifyierDialog.h"
#include "inventory/inventaire.h"

class InventoryModifyierDialog : public QDialog
{
    Q_OBJECT

public:

    explicit InventoryModifyierDialog(const Inventory& inventory, QWidget* parent);
    ~InventoryModifyierDialog() = default;

signals:

    void removeItem(const MovingObject& movingObject);

private slots:

    void on_addItemButton_clicked();

    void on_modifyItemButton_clicked();

    void on_removeItemButton_clicked();

    void on_inventoryTableWidget_itemSelectionChanged();

private:

    Ui::InventoryModifyierDialog ui;
    const Inventory& m_inventory;
    bool hasEditedInventory{ false };

    void setupUi();
    void displayInventory();
    void addInventoryItemToTable(const MovingObject& movingObject, size_t inventoryIndex, const QString& note);
    MovingObject getMovingObjectFromSelection() const;
};

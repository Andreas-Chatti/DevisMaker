#pragma once
#include <QMessageBox>
#include <QDialog>
#include <optional>
#include "ui_InventoryModifyierDialog.h"
#include "ItemModifyierDialog.h"
#include "inventory/inventaire.h"

class InventoryModifyierDialog : public QDialog
{
    Q_OBJECT

public:

    enum class ItemType
    {
        AreaHeader,
        MovingObject
    };

    explicit InventoryModifyierDialog(const Inventory& inventory, QWidget* parent);
    ~InventoryModifyierDialog() = default;

    static constexpr int AREA_NAME_ROLE{ Qt::UserRole };
    static constexpr int ITEM_TYPE_ROLE{ Qt::UserRole + 1 };

signals:

    void removeItem(const QString& movingObjectName, const QString& areaName);
    void removeArea(const QString& areaName);
    void modifyAreaName(const QString& oldAreaName, const QString& newAreaName);

private slots:

    void on_addItemButton_clicked();

    void on_modifyItemButton_clicked();

    void on_removeItemButton_clicked();

    void on_inventoryTableWidget_itemSelectionChanged();

private:

    Ui::InventoryModifyierDialog ui;
    const Inventory& m_inventory;

    void setupUi();
    void displayInventory();
    void addAreaItemToTable(const Area& area);
    void addInventoryItemToTable(const MovingObject& movingObject, const QString& areaName);
    const MovingObject* getMovingObjectFromSelection() const;
    std::optional<QString> getSelectedMovingObjectArea() const;
    std::optional<ItemType> getSelectedItemType() const;
};

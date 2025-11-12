#pragma once
#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "ui_ItemModifyierDialog.h"
#include "inventory/movingObject.h"
#include "inventory/inventaire.h"

class ItemModifyierDialog : public QDialog
{
    Q_OBJECT

public:

    enum class EditState
    {
        MODIFY,
        ADD,
    };

    ItemModifyierDialog(EditState state, const Inventory* inventory, QWidget* parent = nullptr, const MovingObject* movingObject = nullptr);
    ~ItemModifyierDialog() = default;

signals:

    void addObjectToInventory(MovingObject movingObject, const QString& areaName);
    void editObjectFromInventory(const MovingObject* objectToModify, MovingObject newObject);
    void addAreaToInventory(QString areaName);

private slots:

    void on_buttonBox_accepted();
    void on_addAreaButton_clicked();

private:

    Ui::ItemModifyierDialog ui;
    const Inventory* m_inventory;
    const MovingObject* m_modifiedObject;
    const EditState m_state;

    QInputDialog* makeAddAreaDialog();
    QMessageBox* makeObjectOverrideConfirmationDialog();

    static constexpr int OBJECT_NAME_MINIMUM_LENGHT{ 2 };
};
#pragma once
#include <QDialog>
#include "ui_ItemModifyierDialog.h"
#include "inventory/movingObject.h"

class ItemModifyierDialog : public QDialog
{
    Q_OBJECT

public:

    enum class EditState
    {
        MODIFY,
        ADD,
    };

    ItemModifyierDialog(EditState state, QWidget* parent = nullptr, MovingObject movingObject = MovingObject{ "", 1.0 });
    ~ItemModifyierDialog() = default;

signals:

    void addObjectToInventory(MovingObject movingObject, int quantity);

private slots:

    void on_buttonBox_accepted();

private:

    Ui::ItemModifyierDialog ui;
    MovingObject m_object;
    const EditState m_state;
    //void setupUi();
};
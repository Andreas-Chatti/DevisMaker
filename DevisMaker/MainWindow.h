#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "Client.h"
#include "Inventaire.h"
#include "Tarification.h"
#include "Devis.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        ui.setupUi(this);
        //setupConnections();
    }


    ~MainWindow() = default;

private slots:

    void on_generateDevisButton_clicked();

private:

    Ui::MainWindowClass ui;
    Client m_client;
    Inventaire m_inventaire;
    Tarification m_tarification;

    //void setupConnections();
};
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowTitle("DevisMaker - Generateur de Devis");
    w.resize(800, 600);
    w.show();

    return app.exec();
}
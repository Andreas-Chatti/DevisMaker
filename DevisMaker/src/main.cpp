#include "ui/MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/DevisMaker/logo.ico"));

    MainWindow w;
    w.setWindowTitle("DevisMaker - Generateur de Devis");
    w.resize(MainWindow::WINDOW_WIDTH, MainWindow::WINDOW_HEIGHT);
    w.show();

    return app.exec();
}
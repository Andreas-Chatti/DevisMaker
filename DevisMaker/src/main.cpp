#include "ui/MainWindow.h"
#include "utils/FileManager.h"
#include <QApplication>

void customMessageHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
    QString level;
    switch (type) 
    {
    case QtDebugMsg:    level = "DEBUG"; break;
    case QtInfoMsg:     level = "INFO"; break;
    case QtWarningMsg:  level = "WARN"; break;
    case QtCriticalMsg: level = "ERROR"; break;
    default: return;
    }

    QString timestamp{ QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") };
    QString logMessage{ QString("%1 [%2] %3\n").arg(timestamp, level, msg) };

    QString logPath{ QCoreApplication::applicationDirPath() + "/app.log" };
    QFile logFile(logPath);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    logFile.write(logMessage.toUtf8());
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qInstallMessageHandler(customMessageHandler);

    FileManager::ensureDirectoryStructure();

    app.setWindowIcon(QIcon(":/DevisMaker/logo.ico"));

    MainWindow w;
    w.setWindowTitle("DevisMaker - Generateur de Devis");
    w.resize(MainWindow::WINDOW_WIDTH, MainWindow::WINDOW_HEIGHT);
    w.show();

    return app.exec();
}
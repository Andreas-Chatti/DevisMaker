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

    static QString logPath = []() {
        QString timestampPath{ QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") };
        return FileManager::getLogsPath() + "/devisMaker_" + timestampPath + ".log";
    }();

    QFile logFile(logPath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        logFile.write(logMessage.toUtf8());
        logFile.close();
    }
}

void setupApplicationStyle(QApplication& app)
{
    app.setStyle("Fusion");

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::white);
    palette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(palette);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    setupApplicationStyle(app);

    qInstallMessageHandler(customMessageHandler);

    FileManager::ensureDirectoryStructure();

    app.setWindowIcon(QIcon(":/DevisMaker/logo.ico"));

    MainWindow w;
    w.setWindowTitle("DevisMaker - Generateur de Devis");
    w.resize(MainWindow::WINDOW_WIDTH, MainWindow::WINDOW_HEIGHT);
    w.show();

    return app.exec();
}
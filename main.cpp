#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include "logger.h"
#include <QDir>
#include <QDebug>

using namespace std;

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);

        QString logsDirPath = QDir::currentPath();
        QDir logsDir(logsDirPath);

        qDebug() << "Logs will be saved to directory:" << logsDirPath;

        QString logFilePath = logsDirPath + "/dictionary_app.log";
        qDebug() << "Log file will be created at:" << logFilePath;

        if (!Logger::init(logFilePath.toStdString())) {
            qDebug() << "Failed to initialize the logging system. The application will continue running without logging.";
        }

        MainWindow mainWindow;
        mainWindow.show();

        return QApplication::exec();
    } catch (const exception& e) {
        qFatal("Critical error: %s", e.what());
        return 1;
    } catch (...) {
        qFatal("Unknown critical error!");
        return 1;
    }
}

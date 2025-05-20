#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

using namespace std;

ofstream Logger::logFile;
mutex Logger::logMutex;
Logger::LogLevel Logger::currentLevel = Logger::Info;
bool Logger::initialized = false;

bool Logger::init(const string& logFilePath) {
    lock_guard<mutex> lock(logMutex);

    if (initialized && logFile.is_open()) {
        logFile.close();
    }

    QFileInfo fileInfo(QString::fromStdString(logFilePath));
    QDir dir = fileInfo.dir();
    
    qDebug() << "Checking log file directory:" << dir.path();

    if (!dir.exists()) {
        qDebug() << "Directory does not exist, attempting to create:" << dir.path();
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create directory for log file:" << dir.path();
            cerr << "Failed to create directory for log file: "
                      << dir.path().toStdString() << endl;
            return false;
        }
    }

    try {
        logFile.open(logFilePath, ios::app);
        if (!logFile.is_open()) {
            qDebug() << "Failed to open log file:" << QString::fromStdString(logFilePath);
            cerr << "Failed to open log file: " << logFilePath << endl;
            return false;
        }

        initialized = true;

        logFile << "===============================================" << endl;
        logFile << getCurrentTimeString() << " [INFO] Logging started" << endl;
        logFile.flush();

        qDebug() << "Logger successfully initialized";
        return true;
    } catch (const exception& e) {
        qDebug() << "Exception during logger initialization:" << e.what();
        cerr << "Exception during logger initialization: " << e.what() << endl;
        return false;
    } catch (...) {
        qDebug() << "Unknown exception during logger initialization";
        cerr << "Unknown exception during logger initialization" << endl;
        return false;
    }
}

void Logger::log(LogLevel level, const string& message) {
    if (level < currentLevel) {
        return;
    }

    try {
        lock_guard<mutex> lock(logMutex);

        if (!initialized || !logFile.is_open()) {
            cerr << getCurrentTimeString() << " [" << levelToString(level)
                      << "] " << message << endl;
            return;
        }

        logFile << getCurrentTimeString() << " [" << levelToString(level)
                << "] " << message << endl;
        logFile.flush();
    } catch (const exception& e) {
        cerr << "Exception during logging: " << e.what() << endl;
    } catch (...) {
        cerr << "Unknown exception during logging" << endl;
    }
}

void Logger::setLogLevel(LogLevel level) {
    try {
        lock_guard<mutex> lock(logMutex);
        currentLevel = level;

        if (initialized && logFile.is_open()) {
            logFile << getCurrentTimeString() << " [INFO] Log level changed to: "
                    << levelToString(level) << endl;
            logFile.flush();
        }
    } catch (...) {
    }
}

Logger::LogLevel Logger::getLogLevel() {
    return currentLevel;
}

void Logger::close() {
    try {
        lock_guard<mutex> lock(logMutex);

        if (initialized && logFile.is_open()) {
            logFile << getCurrentTimeString() << " [INFO] Logging ended" << endl;
            logFile.close();
            initialized = false;
        }
    } catch (...) {
    }
}

string Logger::levelToString(LogLevel level) {
    switch (level) {
        case Debug:   return "DEBUG";
        case Info:    return "INFO";
        case Warning: return "WARNING";
        case Error:   return "ERROR";
        default:      return "UNKNOWN";
    }
}

string Logger::getCurrentTimeString() {
    try {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        ostringstream oss;
        oss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << '.' << setfill('0') << setw(3) << ms.count();

        return oss.str();
    } catch (...) {
        return "ERROR-TIMESTAMP";
    }
}

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <QString>

using namespace std;

class Logger {
public:
    enum LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    static bool init(const string& logFilePath);

    static void log(LogLevel level, const string& message);

    static void setLogLevel(LogLevel level);

    static LogLevel getLogLevel();

    static void close();

private:
    static ofstream logFile;
    static mutex logMutex;
    static LogLevel currentLevel;
    static bool initialized;

    static string levelToString(LogLevel level);

    static string getCurrentTimeString();
};

#endif
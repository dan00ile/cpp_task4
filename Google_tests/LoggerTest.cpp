#include "gtest/gtest.h"
#include "../logger.h"
#include <fstream>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <thread>
#include <chrono>
#include <QDebug>
#include <iostream>
#include <QDir>
#include <QStandardPaths>

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        QString testDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/LoggerTest";
        QDir dir(testDir);

        if (dir.exists()) {
            dir.removeRecursively();
        }

        bool created = dir.mkpath(".");
        ASSERT_TRUE(created);

        logFilePath = (testDir + "/test_log.log").toStdString();
        qDebug() << "Log path:" << QString::fromStdString(logFilePath);
        std::cout << "Log path: " << logFilePath << std::endl;
    }

    void TearDown() override {
        Logger::close();

        QDir dir = QFileInfo(QString::fromStdString(logFilePath)).dir();
        if (dir.exists()) {
            dir.removeRecursively();
        }
    }

    std::string readLogFile() {
        QFile file(QString::fromStdString(logFilePath));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open log file for reading:" << QString::fromStdString(logFilePath);
            std::cout << "Failed to open log file for reading: " << logFilePath << std::endl;
            return "";
        }
        
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();
        
        qDebug() << "Log file content:" << content;
        std::cout << "Log file content: " << content.toStdString() << std::endl;
        
        return content.toStdString();
    }

    std::string logFilePath;
};

TEST_F(LoggerTest, Initialization) {
    bool result = Logger::init(logFilePath);
    EXPECT_TRUE(result);

    std::ifstream file(logFilePath);
    EXPECT_TRUE(file.good());
    if (!file.good()) {
        qDebug() << "Log file was not created:" << QString::fromStdString(logFilePath);
        std::cout << "Log file was not created: " << logFilePath << std::endl;
    }
    file.close();

    std::string content = readLogFile();
    EXPECT_TRUE(content.find("Logging started") != std::string::npos);
}

TEST_F(LoggerTest, CloseLogger) {
    ASSERT_TRUE(Logger::init(logFilePath));
    Logger::close();

    std::string content = readLogFile();
    EXPECT_TRUE(content.find("Logging ended") != std::string::npos);
}

TEST_F(LoggerTest, LogLevels) {
    qDebug() << "Starting LogLevels test";
    std::cout << "Starting LogLevels test" << std::endl;
    
    bool initResult = Logger::init(logFilePath);
    ASSERT_TRUE(initResult);
    if (!initResult) {
        qDebug() << "Failed to initialize logger";
        std::cout << "Failed to initialize logger" << std::endl;
        return;
    }

    {
        std::ifstream file(logFilePath);
        bool fileExists = file.good();
        ASSERT_TRUE(fileExists);
        if (!fileExists) {
            qDebug() << "Log file does not exist after initialization";
            std::cout << "Log file does not exist after initialization" << std::endl;
            return;
        }
        file.close();
    }

    Logger::setLogLevel(Logger::Debug);

    Logger::log(Logger::Debug, "Debug message");
    Logger::log(Logger::Info, "Info message");
    Logger::log(Logger::Warning, "Warning message");
    Logger::log(Logger::Error, "Error message");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Logger::close();

    std::string content = readLogFile();
    
    qDebug() << "Log content length:" << content.length();
    std::cout << "Log content length: " << content.length() << std::endl;

    bool hasDebug = content.find("DEBUG") != std::string::npos;
    bool hasInfo = content.find("INFO") != std::string::npos;
    bool hasWarning = content.find("WARNING") != std::string::npos;
    bool hasError = content.find("ERROR") != std::string::npos;
    
    qDebug() << "Has DEBUG:" << hasDebug;
    qDebug() << "Has INFO:" << hasInfo;
    qDebug() << "Has WARNING:" << hasWarning;
    qDebug() << "Has ERROR:" << hasError;
    
    std::cout << "Has DEBUG: " << (hasDebug ? "true" : "false") << std::endl;
    std::cout << "Has INFO: " << (hasInfo ? "true" : "false") << std::endl;
    std::cout << "Has WARNING: " << (hasWarning ? "true" : "false") << std::endl;
    std::cout << "Has ERROR: " << (hasError ? "true" : "false") << std::endl;
    
    EXPECT_TRUE(hasDebug);
    EXPECT_TRUE(hasInfo);
    EXPECT_TRUE(hasWarning);
    EXPECT_TRUE(hasError);
    
    bool hasDebugMsg = content.find("Debug message") != std::string::npos;
    bool hasInfoMsg = content.find("Info message") != std::string::npos;
    bool hasWarningMsg = content.find("Warning message") != std::string::npos;
    bool hasErrorMsg = content.find("Error message") != std::string::npos;
    
    qDebug() << "Has 'Debug message':" << hasDebugMsg;
    qDebug() << "Has 'Info message':" << hasInfoMsg;
    qDebug() << "Has 'Warning message':" << hasWarningMsg;
    qDebug() << "Has 'Error message':" << hasErrorMsg;
    
    std::cout << "Has 'Debug message': " << (hasDebugMsg ? "true" : "false") << std::endl;
    std::cout << "Has 'Info message': " << (hasInfoMsg ? "true" : "false") << std::endl;
    std::cout << "Has 'Warning message': " << (hasWarningMsg ? "true" : "false") << std::endl;
    std::cout << "Has 'Error message': " << (hasErrorMsg ? "true" : "false") << std::endl;
    
    EXPECT_TRUE(hasDebugMsg);
    EXPECT_TRUE(hasInfoMsg);
    EXPECT_TRUE(hasWarningMsg);
    EXPECT_TRUE(hasErrorMsg);
}

TEST_F(LoggerTest, SetLogLevel) {
    ASSERT_TRUE(Logger::init(logFilePath));

    Logger::setLogLevel(Logger::Warning);
    EXPECT_EQ(Logger::getLogLevel(), Logger::Warning);

    Logger::log(Logger::Debug, "Debug message");
    Logger::log(Logger::Info, "Info message");
    Logger::log(Logger::Warning, "Warning message");
    Logger::log(Logger::Error, "Error message");

    std::string content = readLogFile();
    EXPECT_TRUE(content.find("Debug message") == std::string::npos);
    EXPECT_TRUE(content.find("Info message") == std::string::npos);
    EXPECT_TRUE(content.find("Warning message") != std::string::npos);
    EXPECT_TRUE(content.find("Error message") != std::string::npos);
}

TEST_F(LoggerTest, MultithreadedLogging) {
    qDebug() << "Starting MultithreadedLogging test";
    std::cout << "Starting MultithreadedLogging test" << std::endl;
    
    bool initResult = Logger::init(logFilePath);
    ASSERT_TRUE(initResult);
    if (!initResult) {
        qDebug() << "Failed to initialize logger";
        std::cout << "Failed to initialize logger" << std::endl;
        return;
    }

    {
        std::ifstream file(logFilePath);
        bool fileExists = file.good();
        ASSERT_TRUE(fileExists);
        if (!fileExists) {
            qDebug() << "Log file does not exist after initialization";
            std::cout << "Log file does not exist after initialization" << std::endl;
            return;
        }
        file.close();
    }

    Logger::setLogLevel(Logger::Info);

    std::vector<std::thread> threads;
    const int numThreads = 5;
    const int numMessagesPerThread = 3;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([i, numMessagesPerThread]() {
            for (int j = 0; j < numMessagesPerThread; ++j) {
                std::string message = "Thread " + std::to_string(i) + " message " + std::to_string(j);
                Logger::log(Logger::Info, message);
                // Небольшая задержка между записями
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    Logger::close();

    std::string content = readLogFile();
    
    qDebug() << "Log content length after multithreaded logging:" << content.length();
    std::cout << "Log content length after multithreaded logging: " << content.length() << std::endl;

    bool allThreadsLogged = true;
    for (int i = 0; i < numThreads; ++i) {
        std::string threadTag = "Thread " + std::to_string(i);
        bool hasThread = content.find(threadTag) != std::string::npos;
        
        qDebug() << "Has messages from" << QString::fromStdString(threadTag) << ":" << hasThread;
        std::cout << "Has messages from " << threadTag << ": " << (hasThread ? "true" : "false") << std::endl;
        
        if (!hasThread) {
            allThreadsLogged = false;
        }
        
        EXPECT_TRUE(hasThread);
    }
    
    EXPECT_TRUE(allThreadsLogged);
} 
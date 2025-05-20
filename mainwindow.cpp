#include "mainwindow.h"
#include "logger.h"
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QFont>
#include <QHeaderView>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(nullptr)
{
    try {
        Logger::log(Logger::Info, "Главное окно инициализируется");
    } catch (...) {
    }

    setupUi();
    createMenus();

    setWindowTitle("Словарь - Анализатор текстов");
    updateStatusBar();

    resize(800, 600);
    setGeometry(QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        size(),
        QGuiApplication::primaryScreen()->availableGeometry()
    ));
    
    try {
        Logger::log(Logger::Info, "Главное окно создано и показано");
    } catch (...) {
    }

    QString logsDirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString logFilePath = logsDirPath + "/dictionary_app.log";
    
    QMessageBox::information(this, "Информация о логах", 
                           "Файл журнала (лог) будет сохранен по пути:\n" + 
                           logFilePath + "\n\n" +
                           "Если у вас возникли проблемы с логированием, проверьте права доступа к этой папке.");
}

MainWindow::~MainWindow()
{
    try {
        Logger::log(Logger::Info, "Приложение закрывается");
        Logger::close();
    } catch (...) {
    }
    
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    try {
        Logger::log(Logger::Info, "Пользователь закрыл главное окно");
        Logger::close();
    } catch (...) {
    }

    event->accept();
    QApplication::quit();
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *controlLayout = new QHBoxLayout();

    QPushButton *loadTextButton = new QPushButton("Загрузить текстовый файл", this);
    QPushButton *saveDictButton = new QPushButton("Сохранить словарь", this);
    QPushButton *loadDictButton = new QPushButton("Загрузить словарь", this);
    QPushButton *clearDictButton = new QPushButton("Очистить словарь", this);
    
    controlLayout->addWidget(loadTextButton);
    controlLayout->addWidget(saveDictButton);
    controlLayout->addWidget(loadDictButton);
    controlLayout->addWidget(clearDictButton);

    mainLayout->addLayout(controlLayout);

    QGroupBox *sortGroup = new QGroupBox("Сортировка", this);
    QHBoxLayout *sortLayout = new QHBoxLayout(sortGroup);
    
    QPushButton *sortAlphaButton = new QPushButton("По алфавиту", this);
    QPushButton *sortFreqButton = new QPushButton("По частоте", this);
    
    sortLayout->addWidget(sortAlphaButton);
    sortLayout->addWidget(sortFreqButton);
    
    mainLayout->addWidget(sortGroup);

    tableWidget = new QTableWidget(0, 2, this);
    QStringList headers;
    headers << "Слово" << "Частота";
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    
    mainLayout->addWidget(tableWidget);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    
    QLabel *logLevelLabel = new QLabel("Уровень логирования:", this);
    logLevelComboBox = new QComboBox(this);
    logLevelComboBox->addItem("Отладка (Debug)");
    logLevelComboBox->addItem("Информация (Info)");
    logLevelComboBox->addItem("Предупреждения (Warning)");
    logLevelComboBox->addItem("Ошибки (Error)");
    
    bottomLayout->addWidget(logLevelLabel);
    bottomLayout->addWidget(logLevelComboBox);
    bottomLayout->addStretch();
    
    mainLayout->addLayout(bottomLayout);

    statusLabel = new QLabel("Словарь пуст", this);
    statusBar()->addWidget(statusLabel);

    connect(loadTextButton, &QPushButton::clicked, this, &MainWindow::onLoadTextFile);
    connect(saveDictButton, &QPushButton::clicked, this, &MainWindow::onSaveDictionary);
    connect(loadDictButton, &QPushButton::clicked, this, &MainWindow::onLoadDictionary);
    connect(clearDictButton, &QPushButton::clicked, this, &MainWindow::onClearDictionary);
    connect(sortAlphaButton, &QPushButton::clicked, this, &MainWindow::onSortAlphabetically);
    connect(sortFreqButton, &QPushButton::clicked, this, &MainWindow::onSortByFrequency);
    connect(logLevelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onChangeLogLevel);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    
    QAction *loadTextAction = new QAction("Загрузить текстовый файл", this);
    QAction *saveDictAction = new QAction("Сохранить словарь", this);
    QAction *loadDictAction = new QAction("Загрузить словарь", this);
    QAction *clearDictAction = new QAction("Очистить словарь", this);
    QAction *exitAction = new QAction("Выход", this);
    
    fileMenu->addAction(loadTextAction);
    fileMenu->addAction(saveDictAction);
    fileMenu->addAction(loadDictAction);
    fileMenu->addAction(clearDictAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *sortMenu = menuBar()->addMenu("Сортировка");
    
    QAction *sortAlphaAction = new QAction("По алфавиту", this);
    QAction *sortFreqAction = new QAction("По частоте", this);
    
    sortMenu->addAction(sortAlphaAction);
    sortMenu->addAction(sortFreqAction);

    QMenu *helpMenu = menuBar()->addMenu("Справка");
    
    QAction *aboutAction = new QAction("О программе", this);
    
    helpMenu->addAction(aboutAction);

    connect(loadTextAction, &QAction::triggered, this, &MainWindow::onLoadTextFile);
    connect(saveDictAction, &QAction::triggered, this, &MainWindow::onSaveDictionary);
    connect(loadDictAction, &QAction::triggered, this, &MainWindow::onLoadDictionary);
    connect(clearDictAction, &QAction::triggered, this, &MainWindow::onClearDictionary);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    connect(sortAlphaAction, &QAction::triggered, this, &MainWindow::onSortAlphabetically);
    connect(sortFreqAction, &QAction::triggered, this, &MainWindow::onSortByFrequency);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::onLoadTextFile()
{
    try {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Выберите текстовый файл", QDir::homePath(), 
            "Текстовые файлы (*.txt);;Все файлы (*.*)");
            
        if (filePath.isEmpty()) {
            Logger::log(Logger::Debug, "Пользователь отменил выбор файла");
            return;
        }
        
        Logger::log(Logger::Info, "Выбран файл для загрузки: " + filePath.toStdString());

        if (dictionary.addWordsFromFile(filePath)) {
            updateWordTable(dictionary.getWordsAlphabetically());
            updateStatusBar();
            QMessageBox::information(this, "Успех", 
                                     "Слова успешно загружены из файла:\n" + filePath);
        } else {
            QMessageBox::warning(this, "Ошибка", 
                                 "Не удалось загрузить слова из файла:\n" + filePath);
        }
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Исключение при загрузке текстового файла: " + 
                   string(e.what()));
        QMessageBox::critical(this, "Ошибка", 
                             "Произошла ошибка при загрузке файла: " + 
                             QString::fromStdString(e.what()));
    }
}

void MainWindow::onSaveDictionary()
{
    try {
        if (dictionary.size() == 0) {
            QMessageBox::warning(this, "Предупреждение", "Словарь пуст, нечего сохранять.");
            Logger::log(Logger::Warning, "Попытка сохранить пустой словарь");
            return;
        }

        QString filePath = QFileDialog::getSaveFileName(
            this, "Сохранить словарь", QDir::homePath(), 
            "Словари (*.dict);;Текстовые файлы (*.txt);;Все файлы (*.*)");
            
        if (filePath.isEmpty()) {
            Logger::log(Logger::Debug, "Пользователь отменил сохранение файла");
            return;
        }
        
        Logger::log(Logger::Info, "Выбран файл для сохранения: " + filePath.toStdString());

        if (dictionary.saveToFile(filePath)) {
            QMessageBox::information(this, "Успех", 
                                     "Словарь успешно сохранен в файл:\n" + filePath);
        } else {
            QMessageBox::warning(this, "Ошибка", 
                                 "Не удалось сохранить словарь в файл:\n" + filePath);
        }
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Исключение при сохранении словаря: " + 
                   string(e.what()));
        QMessageBox::critical(this, "Ошибка", 
                             "Произошла ошибка при сохранении словаря: " + 
                             QString::fromStdString(e.what()));
    }
}

void MainWindow::onLoadDictionary()
{
    try {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Загрузить словарь", QDir::homePath(), 
            "Словари (*.dict);;Текстовые файлы (*.txt);;Все файлы (*.*)");
            
        if (filePath.isEmpty()) {
            Logger::log(Logger::Debug, "Пользователь отменил выбор файла словаря");
            return;
        }
        
        Logger::log(Logger::Info, "Выбран словарь для загрузки: " + filePath.toStdString());

        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Загрузка словаря", 
            "Заменить текущий словарь?\nНажмите 'Да' для замены или 'Нет' для добавления к текущему словарю.",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            
        if (reply == QMessageBox::Cancel) {
            Logger::log(Logger::Debug, "Пользователь отменил загрузку словаря");
            return;
        }
        
        if (reply == QMessageBox::Yes) {
            if (dictionary.loadFromFile(filePath)) {
                updateWordTable(dictionary.getWordsAlphabetically());
                updateStatusBar();
                QMessageBox::information(this, "Успех", 
                                         "Словарь успешно загружен из файла:\n" + filePath);
            } else {
                QMessageBox::warning(this, "Ошибка", 
                                     "Не удалось загрузить словарь из файла:\n" + filePath);
            }
        } else {
            Dictionary tempDict;
            if (tempDict.loadFromFile(filePath)) {
                auto words = tempDict.getWordsAlphabetically();
                for (const auto& [word, count] : words) {
                    for (int i = 0; i < count; i++) {
                        dictionary.addWord(word);
                    }
                }
                
                updateWordTable(dictionary.getWordsAlphabetically());
                updateStatusBar();
                QMessageBox::information(this, "Успех", 
                                         "Слова успешно добавлены из файла:\n" + filePath);
            } else {
                QMessageBox::warning(this, "Ошибка", 
                                     "Не удалось загрузить слова из файла:\n" + filePath);
            }
        }
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Исключение при загрузке словаря: " + 
                   string(e.what()));
        QMessageBox::critical(this, "Ошибка", 
                             "Произошла ошибка при загрузке словаря: " + 
                             QString::fromStdString(e.what()));
    }
}

void MainWindow::onClearDictionary()
{
    try {
        if (dictionary.size() == 0) {
            QMessageBox::information(this, "Информация", "Словарь уже пуст.");
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Очистка словаря", 
            "Вы уверены, что хотите очистить словарь?\nЭто действие нельзя отменить.",
            QMessageBox::Yes | QMessageBox::No);
            
        if (reply == QMessageBox::No) {
            Logger::log(Logger::Debug, "Пользователь отменил очистку словаря");
            return;
        }

        dictionary.clear();
        updateWordTable(dictionary.getWordsAlphabetically());
        updateStatusBar();
        
        QMessageBox::information(this, "Успех", "Словарь успешно очищен.");
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Исключение при очистке словаря: " + 
                   string(e.what()));
        QMessageBox::critical(this, "Ошибка", 
                             "Произошла ошибка при очистке словаря: " + 
                             QString::fromStdString(e.what()));
    }
}

void MainWindow::onSortAlphabetically()
{
    try {
        if (dictionary.size() == 0) {
            QMessageBox::information(this, "Информация", "Словарь пуст, нечего сортировать.");
            return;
        }
        
        updateWordTable(dictionary.getWordsAlphabetically());
        Logger::log(Logger::Info, "Словарь отсортирован по алфавиту");
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Исключение при сортировке по алфавиту: " + 
                   string(e.what()));
        QMessageBox::critical(this, "Ошибка", 
                             "Произошла ошибка при сортировке: " + 
                             QString::fromStdString(e.what()));
    }
}

void MainWindow::onSortByFrequency()
{
    try {
        if (dictionary.size() == 0) {
            QMessageBox::information(this, "Информация", "Словарь пуст, нечего сортировать.");
            return;
        }
        
        updateWordTable(dictionary.getWordsByFrequency());
        Logger::log(Logger::Info, "Словарь отсортирован по частоте");
    } catch (const exception& e) {
        Logger::log(Logger::Error, "Исключение при сортировке по частоте: " + 
                   string(e.what()));
        QMessageBox::critical(this, "Ошибка", 
                             "Произошла ошибка при сортировке: " + 
                             QString::fromStdString(e.what()));
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "О программе", 
                      "Словарь - Анализатор текстов\n\n"
                      "Программа предназначена для анализа частоты слов в текстовых файлах.\n\n"
                      "Возможности:\n"
                      "- Загрузка слов из текстовых файлов\n"
                      "- Сохранение и загрузка словаря\n"
                      "- Сортировка по алфавиту и по частоте\n"
                      "- Подробное логирование действий\n\n"
                      "Версия 1.0\n");
}

void MainWindow::onChangeLogLevel(int index)
{
    Logger::LogLevel level;
    
    switch (index) {
        case 0: level = Logger::Debug; break;
        case 1: level = Logger::Info; break;
        case 2: level = Logger::Warning; break;
        case 3: level = Logger::Error; break;
        default: level = Logger::Info;
    }
    
    try {
        Logger::setLogLevel(level);
    } catch (...) {
        QMessageBox::warning(this, "Предупреждение", 
                         "Не удалось изменить уровень логирования. Проверьте работу системы логирования.");
    }
}

void MainWindow::updateWordTable(const vector<pair<string, int>>& words)
{
    tableWidget->setRowCount(0);

    tableWidget->setRowCount(static_cast<int>(words.size()));
    
    for (size_t i = 0; i < words.size(); ++i) {
        const auto& [word, count] = words[i];
        
        QTableWidgetItem *wordItem = new QTableWidgetItem(QString::fromStdString(word));
        QTableWidgetItem *countItem = new QTableWidgetItem(QString::number(count));

        countItem->setTextAlignment(Qt::AlignCenter);
        
        tableWidget->setItem(static_cast<int>(i), 0, wordItem);
        tableWidget->setItem(static_cast<int>(i), 1, countItem);
    }

}

void MainWindow::updateStatusBar()
{
    statusLabel->setText(QString("Словарь содержит %1 уникальных слов")
                        .arg(QString::number(dictionary.size())));
} 
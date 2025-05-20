#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QComboBox>
#include <QCloseEvent>
#include "dictionary.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoadTextFile();
    void onSaveDictionary();
    void onLoadDictionary();
    void onClearDictionary();
    void onSortAlphabetically();
    void onSortByFrequency();
    void onAbout();
    void onChangeLogLevel(int index);

private:
    Ui::MainWindow *ui;
    Dictionary dictionary;

    QTableWidget *tableWidget;
    QLabel *statusLabel;
    QComboBox *logLevelComboBox;

    void setupUi();
    void createMenus();

    void updateWordTable(const std::vector<std::pair<std::string, int>>& words);
    void updateStatusBar();
};
#endif // MAINWINDOW_H 
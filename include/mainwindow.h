#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "include/imt4reader.h"
#include "include/csvwriter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow  *ui;
    IMt4Reader      *historyReader = 0;
    CsvWriter       *forecastWriter = 0;
    QString         filePath = "C:/";

private slots:
    void on_findFileButton_clicked();
    void on_saveCsvButton_clicked();
    void on_action_triggered();
    void on_readButton_clicked();
};

#endif // MAINWINDOW_H

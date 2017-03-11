/*****************************************************************************/
/*                                                                           */
/*   HST-Reader                                                              */
/*   https://github.com/terentjew-alexey/Hst-reader                          */
/*                                                                           */
/*   M A I N W I N D O W   C L A S S   H E A D E R                           */
/*                                                                           */
/*   Aleksey Terentew                                                        */
/*   terentew.aleksey@ya.ru                                                  */
/*                                                                           */
/*****************************************************************************/

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
    void setFilePath(const QString fPath);
    void print(const QString text);//debug

private:
    Ui::MainWindow  *ui;
    IMt4Reader      *historyReader;
    CsvWriter       *csvWriter;
    QString         filePath = "C:/";

private slots:
    void on_findFileButton_clicked();
    void on_findPathButton_clicked();
    void readFile();
    void saveCsvFile();
    void savePredictionExample();
    void saveXYFiles();
    void on_actionClearText_triggered();
    void about();

private:
    void setConnections(void);
    bool checkFilePath(const QString fPath);
    void getMinPeriod(const QMap<QString, IMt4Reader *> &readers,
                      qint32 &period);
    void getEntryTime(const QMap<QString, IMt4Reader *> &readers,
                      qint64 &first, qint64 &last);
    double getDoubleTimeSymbol(const QString &symbol,
                               const qint64 &timeCurrentIter);
};

#endif // MAINWINDOW_H

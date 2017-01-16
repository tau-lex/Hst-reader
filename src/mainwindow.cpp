#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QLineEdit>
#include "include/hstreader.h"
#include "include/csvreader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    if( historyReader )
        delete historyReader;
    if( forecastWriter )
        delete forecastWriter;
}

void MainWindow::on_findFileButton_clicked()
{
    filePath = QFileDialog::getOpenFileName( this, tr("Open history file"),
                                             filePath,
                                             tr("Timeseries (*.hst *.csv);;\
                                             History (*.hst);;\
                                             Csv files (*.csv)") );
    ui->filePathEdit->setText( filePath );
    QPalette pal = ui->filePathEdit->palette();
    if( filePath.contains(".hst") || filePath.contains(".csv") ) {
        pal.setColor( QPalette::Text, Qt::darkGreen );
        ui->textBrowser->insertPlainText( filePath + "\n" );
        if( historyReader ) {
            delete historyReader;
            historyReader = 0;
        }
        if( filePath.contains(".hst") )
            historyReader = new HstReader( filePath );
        else if( filePath.contains(".csv") )
            historyReader = new CsvReader( filePath );
    } else {
        pal.setColor( QPalette::Text, Qt::darkRed );
        if( historyReader ) {
            delete historyReader;
            historyReader = 0;
        }
    }
    ui->filePathEdit->setPalette( pal );
}

void MainWindow::on_saveCsvButton_clicked()
{
    if( !historyReader ) {
        ui->textBrowser->insertPlainText("Not find opened file.\n");
        return;
    }
    if( historyReader->getHistorySize() <= 0 )
        historyReader->readFile();
    QString outFile = QApplication::applicationDirPath();
    outFile = QString("%1/%2%3.csv").arg( outFile )
                                .arg( QString(historyReader->getHeader()->Symbol) )
                                .arg( historyReader->getHeader()->Period );
    if( forecastWriter )
        delete forecastWriter;
    forecastWriter = new CsvWriter( outFile );
    HeaderWr *header = forecastWriter->getHeader();
    header->Symbol = QString(historyReader->getHeader()->Symbol);
    header->Period = historyReader->getHeader()->Period;
    header->Digits = historyReader->getHeader()->Digits;
    header->Depth = 1;

    std::vector<Forecast*> *forecast = forecastWriter->getForecastVector();
    for( size_t i = 0; i < historyReader->getHistorySize() - 2; i++ )
    {
        Forecast *newFLine = new Forecast;
        newFLine->Time = (qint64)historyReader->getHistory(i)[0];
        newFLine->High[0] =  historyReader->getHistory(i+1)[2];
        newFLine->Low[0] =   historyReader->getHistory(i+1)[3];
        newFLine->Close[0] = historyReader->getHistory(i+1)[4];
        newFLine->High[1] =  historyReader->getHistory(i+2)[2];
        newFLine->Low[1] =   historyReader->getHistory(i+2)[3];
        newFLine->Close[1] = historyReader->getHistory(i+2)[4];
        forecast->push_back( newFLine );
        forecastWriter->setSize(i);
    }
    forecastWriter->writeFile();
    ui->textBrowser->insertPlainText( tr("Saved csv file - %1\n").arg( outFile ) );
}

void MainWindow::on_action_triggered()
{
    ui->textBrowser->clear();
}

void MainWindow::on_readButton_clicked()
{
    if( !historyReader )
        return;
    historyReader->readFile();
    ui->textBrowser->insertPlainText( historyReader->getHeaderString() + "\n" );
    for( size_t i = 0; i < historyReader->getHistorySize(); i++ )
        ui->textBrowser->insertPlainText( historyReader->getHistoryString( i ) + "\n" );
    ui->textBrowser->insertPlainText( QString( "MW: File readed. History size - %1\n\n" )
                                      .arg( historyReader->getHistorySize() ) );
}

#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QLineEdit>
#include "include/hstreader.h"
#include "include/csvreader.h"
#include "include/csvpredictionwriter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    historyReader(nullptr),
    csvWriter(nullptr)
{
    ui->setupUi(this);
    setConnections();
}

MainWindow::~MainWindow()
{
    if( historyReader )
        delete historyReader;
    if( csvWriter )
        delete csvWriter;
    delete ui;
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

void MainWindow::readFile()
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

void MainWindow::saveCsvFile()
{
    if( !historyReader ) {
        ui->textBrowser->insertPlainText("Not find opened file.\n");
        return;
    }
    if( historyReader->getHistorySize() <= 0 )
        historyReader->readFile();
    QString outFile = QString("%1/%2%3.csv").arg( qApp->applicationDirPath() )
                                .arg( QString(historyReader->getHeader()->Symbol) )
                                .arg( historyReader->getHeader()->Period );
    if( csvWriter )
        delete csvWriter;
    csvWriter = new CsvWriter( outFile );
	csvWriter->setZeroColumnIsTime( true );
    csvWriter->setPrecision( historyReader->getHeader()->Digits );
    for( size_t idx = 0; idx < historyReader->getHistorySize(); idx++ ) {
        csvWriter->getDataPtr()->append( historyReader->getHistory()->at(idx) );
    }
    csvWriter->writeFile();
    ui->textBrowser->insertPlainText( tr("Saved csv file - %1.\n").arg( outFile ) );
}

void MainWindow::savePredictionExample()
{
    if( !historyReader ) {
        ui->textBrowser->insertPlainText("Not find opened file.\n");
        return;
    }
    if( historyReader->getHistorySize() <= 0 )
        historyReader->readFile();
    QString outFile = QString("%1/example%2%3.csv").arg( qApp->applicationDirPath() )
                                .arg( QString(historyReader->getHeader()->Symbol) )
                                .arg( historyReader->getHeader()->Period );
    CsvPredictionWriter csvPWriter( outFile );
    PHeader *header = csvPWriter.getHeader();
    header->Symbol = QString( historyReader->getHeader()->Symbol );
    header->Period = historyReader->getHeader()->Period;
    header->Digits = historyReader->getHeader()->Digits;
    header->TimeSign = historyReader->getHeader()->TimeSign;
    header->LastSync = historyReader->getHeader()->LastSync;
    header->Depth  = 5;
    QList<Forecast *> *forecast = csvPWriter.getDataPredictionPtr();
    for( size_t idx = 0; idx < historyReader->getHistorySize() - 5; idx++ )
    {
        Forecast *newPLine = new Forecast;
        newPLine->Time = (qint64)historyReader->getHistory(idx)[0];
        for( qint32 j = 0; j < 5; j++ ) {
            newPLine->High[j] =  historyReader->getHistory(idx+j)[2];
            newPLine->Low[j] =   historyReader->getHistory(idx+j)[3];
            newPLine->Close[j] = historyReader->getHistory(idx+j)[4];
        }
        forecast->append( newPLine );
    }
    csvPWriter.writeFile();
    ui->textBrowser->insertPlainText( tr("Saved csv file - %1.\n").arg( outFile ) );
}

void MainWindow::on_actionClearText_triggered()
{
    ui->textBrowser->clear();
}

void MainWindow::setConnections(void)
{
    connect( ui->actionOpen, SIGNAL( triggered(bool) ),
             this, SLOT( on_findFileButton_clicked() ) );
    connect( ui->actionRead_File, SIGNAL( triggered(bool) ),
             this, SLOT( readFile() ) );
    connect( ui->actionSaveCsv, SIGNAL( triggered(bool) ),
             this, SLOT( saveCsvFile() ) );
    connect( ui->actionSave_Example, SIGNAL( triggered(bool) ),
             this, SLOT( savePredictionExample() ) );
    connect( ui->actionExit, SIGNAL( triggered(bool) ),
             this, SLOT( close() ) );
}

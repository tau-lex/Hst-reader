#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
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
    QScrollBar *v = ui->textBrowser->verticalScrollBar();
    v->setValue( v->maximum() );
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
    ui->textBrowser->insertPlainText( tr("Saved .csv file - %1.\n").arg( outFile ) );
    QScrollBar *v = ui->textBrowser->verticalScrollBar();
    v->setValue( v->maximum() );
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
    ui->textBrowser->insertPlainText( tr("Saved .csv file (example prediction) - %1.\n").arg( outFile ) );
    QScrollBar *v = ui->textBrowser->verticalScrollBar();
    v->setValue( v->maximum() );
}

void MainWindow::on_actionClearText_triggered()
{
    ui->textBrowser->clear();
}

void MainWindow::about()
{

    QMessageBox::about(this, tr("About Hst-Reader"),
             tr("The <b>Hst-Reader</b> converts .hst files in standard .сsv file.<br/>"
                ".hst files - timeseries data storage, a series of programs Meta Trader. <br/><br/>"
                "Version - %1.<br/>Author - %2.<br/><a href=\"%3\">%3</a>")
                       .arg( qApp->applicationVersion() )
                       .arg( qApp->organizationName() )
                       .arg( qApp->organizationDomain() ) );
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
    connect( ui->actionAbout, SIGNAL( triggered(bool) ),
             this, SLOT( about() ) );
    connect( ui->actionAbout_Qt, SIGNAL( triggered(bool) ),
             qApp, SLOT( aboutQt() ) );
}

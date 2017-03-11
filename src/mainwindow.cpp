/*****************************************************************************/
/*                                                                           */
/*   HST-Reader                                                              */
/*   https://github.com/terentjew-alexey/Hst-reader                          */
/*                                                                           */
/*   M A I N W I N D O W   C L A S S                                         */
/*                                                                           */
/*   Aleksey Terentew                                                        */
/*   terentew.aleksey@ya.ru                                                  */
/*                                                                           */
/*****************************************************************************/

#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QFileDialog>
#include <QLineEdit>
#include <QDateTime>
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

void MainWindow::setFilePath(const QString fPath)
{
    if( fPath.size() <= 0 )
        return;
    if( historyReader ) {
        delete historyReader;
        historyReader = 0;
    }
    filePath = fPath;
    ui->filePathEdit->setText( filePath );
    QPalette pal = ui->filePathEdit->palette();
    if( checkFilePath(filePath) ) {
        pal.setColor( QPalette::Text, Qt::darkGreen );
        if( filePath.contains(".hst") )
            historyReader = new HstReader( filePath );
        else if( filePath.contains(".csv") )
            historyReader = new CsvReader( filePath );
    } else {
        pal.setColor( QPalette::Text, Qt::darkRed );
    }
    ui->filePathEdit->setPalette( pal );
    if( true )
        readFile();
}

void MainWindow::print(const QString text)
{
    ui->textBrowser->insertPlainText( QString("%1\n").arg( text ) );
    QScrollBar *v = ui->textBrowser->verticalScrollBar();
    v->setValue( v->maximum() );
}

void MainWindow::on_findFileButton_clicked()
{
    filePath = QFileDialog::getOpenFileName( this, tr("Open history file"),
                                             filePath,
                                             tr("Timeseries (*.hst *.csv);;\
                                             History (*.hst);;\
                                             Csv files (*.csv)") );
    if( historyReader ) {
        delete historyReader;
        historyReader = 0;
    }
    ui->filePathEdit->setText( filePath );
    QPalette pal = ui->filePathEdit->palette();
    if( checkFilePath(filePath) ) {
        pal.setColor( QPalette::Text, Qt::darkGreen );
        if( filePath.contains(".hst") )
            historyReader = new HstReader( filePath );
        else if( filePath.contains(".csv") )
            historyReader = new CsvReader( filePath );
    } else {
        pal.setColor( QPalette::Text, Qt::darkRed );
    }
    ui->filePathEdit->setPalette( pal );
}

void MainWindow::on_findPathButton_clicked()
{
    filePath = QFileDialog::getExistingDirectory( this,
                                                  tr("Open history directory"),
                                                  filePath );
    if( historyReader ) {
        delete historyReader;
        historyReader = 0;
    }
    ui->filePathEdit->setText( filePath );
    QPalette pal = ui->filePathEdit->palette();
    if( checkFilePath(filePath) ) {
        pal.setColor( QPalette::Text, Qt::darkGreen );
    } else {
        pal.setColor( QPalette::Text, Qt::darkRed );
    }
    ui->filePathEdit->setPalette( pal );
}

void MainWindow::readFile()
{
    if( !historyReader ) {
        print( "Please, select a file." );
        return;
    }
    historyReader->readFile();
    ui->textBrowser->insertPlainText( historyReader->getHeaderString() );
    for( qint32 i = 0; i < historyReader->getHistorySize(); i++ ) {
        print( historyReader->getHistoryString(i) );
    }
    print( QString( "File readed. History size - %1" )
           .arg( historyReader->getHistorySize() ) );
}

void MainWindow::saveCsvFile()
{
    if( !historyReader ) {
        print( "Please, select a file." );
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
    for( qint32 idx = 0; idx < historyReader->getHistorySize(); idx++ ) {
        csvWriter->getDataPtr()->append( historyReader->getHistory()->at(idx) );
    }
    csvWriter->writeFile();
    print( tr("Saved .csv file - %1.").arg( outFile ) );
}

void MainWindow::savePredictionExample()
{
    if( !historyReader ) {
        print( "Please, select a file." );
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
    for( qint32 idx = 0; idx < historyReader->getHistorySize() - 5; idx++ )
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
    print( tr("Saved .csv file (example prediction) - %1.").arg( outFile ) );
}

void MainWindow::saveXYFiles()
{
    if( filePath.contains(".hst") || filePath.contains(".csv") ) {
        ui->textBrowser->insertPlainText( "Please, select a history directory.\n" );
        return;
    }
    QDir path( filePath );
    QStringList nameFilter;
    nameFilter << "*.hst" << "*.csv";
    QStringList files = path.entryList( nameFilter, QDir::Files, QDir::Name );
    if( files.size() <= 0 ) {
        print( "Please, select a history directory." );
        return;
    }
    path.mkdir("result");
//  R E A D E R S
    bool readVolume = false;
    QMap<QString, qint32> iters;
    QMap<QString, IMt4Reader* > readers;
    CsvWriter* fileX;
    CsvWriter* fileY;
    QMap<QString, CsvWriter* > writers;
    std::vector<qint32> precisionVecX;
    try {
        for( qint32 idxT = 0; idxT < 6; idxT++ ) // Precision for time data
            precisionVecX.push_back( 0 );
        foreach( QString file, files ) {
            iters[file] = -1;
            if( file.contains(".hst") ) {
                readers[file] = new HstReader( QString("%1/%2").arg( filePath ).arg( file ) );
            } else {
                readers[file] = new CsvReader( QString("%1/%2").arg( filePath ).arg( file ) );
            }
            if( readers[file]->readFile() ) {
                print( tr("History file \"%1\" succeful loaded.").arg( file ) );
            } else {
                print( tr("History file \"%1\" cannot be loaded.").arg( file ) );
                throw 11;
            }
            for( qint32 idxD = 0; idxD < 4; idxD++ ) // Precision for TS data
                precisionVecX.push_back( readers[file]->getHeader()->Digits );
            if( readVolume )
                precisionVecX.push_back( readers[file]->getHeader()->Digits );
        }
        print( tr("Succeful loaded %1 files.").arg( readers.size() ) );
//  W R I T E R S
        fileX = new CsvWriter( QString("%1/result/input_data_x.csv").arg( filePath ) );
        fileX->setPrecision( precisionVecX );
        fileY = new CsvWriter( QString("%1/result/output_data_y.csv").arg( filePath ) );
        fileY->setPrecision( precisionVecX );
        foreach( QString file, files ) {
            QString name = file.left( file.length() - 4 );
            writers[file] = new CsvWriter( QString("%1/result/%2_y.csv").arg( filePath ).arg( name ) );
            writers[file]->setPrecision( readers[file]->getHeader()->Digits );
        }
//  L O A D   D A T A   A N D   P R E P A R E   W R I T E R S
        /// from neuralnetworkanalysis.cpp
        qint32 iterPeriod;
        getMinPeriod( readers, iterPeriod );
        qint64 firstEntryTime, lastEntryTime;
        getEntryTime( readers, firstEntryTime, lastEntryTime );
        print( tr("The data set belongs to the interval of time:\n * %1 - %2.")
               .arg( QDateTime::fromTime_t( firstEntryTime ).toString("yyyy.MM.dd hh:mm:ss") )
               .arg( QDateTime::fromTime_t( lastEntryTime ).toString("yyyy.MM.dd hh:mm:ss") ) );
        bool lastBarInTS = false;
        qint32 idxRow, idxFile;
        qint64 iterTime = firstEntryTime;
        qint64 iterEnd = lastEntryTime;// - iterPeriod;
        // D O
        for( idxRow = 0; iterTime < iterEnd; iterTime += iterPeriod ) {
            if( QDateTime::fromTime_t( iterTime ).date().dayOfWeek() == 6 ||
                    QDateTime::fromTime_t( iterTime ).date().dayOfWeek() == 7 )
                continue;
            // N E W   L I N E   X
            std::vector<double> newRow;
            newRow.push_back( getDoubleTimeSymbol( "HOUR", iterTime ) );
            newRow.push_back( getDoubleTimeSymbol( "MINUTE", iterTime ) );
            newRow.push_back( getDoubleTimeSymbol( "WEEKDAY", iterTime ) );
            newRow.push_back( getDoubleTimeSymbol( "DAY", iterTime ) );
            newRow.push_back( getDoubleTimeSymbol( "MONTH", iterTime ) );
            newRow.push_back( getDoubleTimeSymbol( "YEAR", iterTime ) );
            foreach( QString file, files ) {
                if( iters[file] < 0 && iterTime >= (*readers[file]->getHistory())[0][0] )
                    iters[file]++;
                if( iters[file] >= 0 ) {
                    idxFile = iters[file];
                    newRow.push_back( (*readers[file]->getHistory())[idxFile][1] );
                    newRow.push_back( (*readers[file]->getHistory())[idxFile][2] );
                    newRow.push_back( (*readers[file]->getHistory())[idxFile][3] );
                    newRow.push_back( (*readers[file]->getHistory())[idxFile][4] );
                    if( readVolume )
                        newRow.push_back( (*readers[file]->getHistory())[idxFile][5] );
                    iters[file]++;
                } else {
                    for( qint32 idxTemp = 1; idxTemp <= 4; idxTemp++ )
                        newRow.push_back( 0.0 );
                    if( readVolume )
                        newRow.push_back( 0.0 );
                }
                if( readers[file]->getHistorySize() == iters[file] )
                    lastBarInTS = true;
            }
            if( !lastBarInTS )
                fileX->getDataPtr()->append( newRow );
            if( idxRow > 0 ) { // skip first row
                // N E W   L I N E   Y
                std::vector<double> newRowY(newRow);
                fileY->getDataPtr()->append( newRowY );
                // N E W   L I N E   T O   Y - F I L E S
                qint32 idxY = 6;
                foreach( QString file, files ) {
                    std::vector<double> newRowYY;
                    for( qint32 idxYY = idxY; idxYY < idxY + 4; idxYY++ )
                        newRowYY.push_back( newRowY[idxYY] );
                    idxY += 4;
                    if( readVolume )
                        idxY += 1;
                    writers[file]->getDataPtr()->append( newRowYY );
                }
            }
            // E N D   I T E R
            idxRow += 1;
            if( lastBarInTS )
                break;
        }
//  W R I T E   A L L   F I L E S
        fileX->writeFile();
        print( tr("File input_data_x.csv saved.") );
        fileY->writeFile();
        print( tr("File output_data_y.csv saved.") );
        foreach( QString file, files ) {
            writers[file]->writeFile();
            print( tr("File %1_y.csv saved.").arg( file ) );
        }
    } catch( qint32 e ) {
        print( tr("Stop process. Error %1.").arg(e) );
    }
//  F R E E   M E M O R Y
    foreach( QString file, files ) {
        if( readers[file] )
            delete readers[file];
    }
    if( fileX )
        delete fileX;
    if( fileY )
        delete fileY;
    foreach( QString file, files ) {
        if( writers[file] )
            delete writers[file];
    }
}

void MainWindow::on_actionClearText_triggered()
{
    ui->textBrowser->clear();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Hst-Reader"),
             tr("The <b>Hst-Reader</b> converts HST files in standard CSV file.<br/><br/>"
                "HST files - timeseries data storage, a series of programs Meta Trader.<br/>"
                "Also the program can prepare data for the tasks of machine learning.<br/><br/>"
                "Version: %1.<br/><br/>"
                "%2.<br/>"
                "<a href=\"%4\">%4</a><br/>"
                "<a href=\"%3\">GitHub page.</a>")
                       .arg( qApp->applicationVersion() )
                       .arg( qApp->organizationName() )
                       .arg( qApp->organizationDomain() )
                       .arg( "terentjew.alexey@ya.ru" ));
}

void MainWindow::setConnections(void)
{
    connect( ui->actionOpen, SIGNAL( triggered(bool) ),
             this, SLOT( on_findFileButton_clicked() ) );
    connect( ui->actionOpenFolder, SIGNAL( triggered(bool) ),
             this, SLOT( on_findPathButton_clicked() ) );
    connect( ui->actionRead_File, SIGNAL( triggered(bool) ),
             this, SLOT( readFile() ) );
    connect( ui->actionSaveCsv, SIGNAL( triggered(bool) ),
             this, SLOT( saveCsvFile() ) );
    connect( ui->actionSave_Example, SIGNAL( triggered(bool) ),
             this, SLOT( savePredictionExample() ) );
    connect( ui->actionPrepare_XY_Data, SIGNAL( triggered(bool) ),
             this, SLOT( saveXYFiles() ) );
    connect( ui->actionExit, SIGNAL( triggered(bool) ),
             this, SLOT( close() ) );
    connect( ui->actionAbout, SIGNAL( triggered(bool) ),
             this, SLOT( about() ) );
    connect( ui->actionAbout_Qt, SIGNAL( triggered(bool) ),
             qApp, SLOT( aboutQt() ) );
}

bool MainWindow::checkFilePath(const QString fPath)
{
    if( fPath.contains(".hst") || fPath.contains(".csv") ) {
        print( fPath );
        return true;
    }
    QDir path( fPath );
    QStringList nameFilter;
    nameFilter << "*.hst" << "*.csv";
    QStringList files = path.entryList( nameFilter, QDir::Files );
    if( files.size() > 0 ) {
        print( tr("%1:").arg( fPath ) );
        foreach( QString file, files )
            print( file );
        return true;
    }
    return false;
}

void MainWindow::getMinPeriod(const QMap<QString, IMt4Reader *> &readers, qint32 &period)
{
    period = 43200;
    QMapIterator<QString, IMt4Reader *> i(readers);
    while( i.hasNext() ) {
        i.next();
        if( i.value()->getHeader()->Period < period ) {
            period = i.value()->getHeader()->Period;
        }
    }
    period *= 60;
}

void MainWindow::getEntryTime(const QMap<QString, IMt4Reader *> &readers,
                                              qint64 &first, qint64 &last)
{
    first = std::numeric_limits<qint64>::max();
    last = 0;
    QMapIterator<QString, IMt4Reader *> i(readers);
    while( i.hasNext() ) {
        i.next();
        if( i.value()->getHistorySize() > 0 ) {
            if( (*i.value()->getHistory())[0][0] < first )
                first = (*i.value()->getHistory())[0][0];
            if( (*i.value()->getHistory())[i.value()->getHistorySize()-1][0] > last )
                last = (*i.value()->getHistory())[i.value()->getHistorySize()-1][0];
        }
    }
}

double MainWindow::getDoubleTimeSymbol(const QString &symbol,
                                                  const qint64 &timeCurrentIter)
{
    if( symbol == "YEAR" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).date().year());
    else if( symbol == "MONTH" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).date().month());
    else if( symbol == "DAY" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).date().day());
    else if( symbol == "YEARDAY" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).date().dayOfYear());
    else if( symbol == "HOUR" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).time().hour());
    else if( symbol == "MINUTE" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).time().minute());
    else if( symbol == "WEEKDAY" )
        return static_cast<double>(QDateTime::fromTime_t( timeCurrentIter ).date().dayOfWeek());
    else throw 25;                      // !err not timeSymbol
    return -1.0;
}

#include "include/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setOrganizationName( "Terentew Aleksey" );
    qApp->setOrganizationDomain( "https://github.com/terentjew-alexey/Hst-reader" );
    qApp->setApplicationVersion( "1.5.201702-beta" );
    MainWindow w;
    w.show();
    if( argc > 1 )
        w.setFilePath( QString( argv[1] ) );

    return a.exec();
}

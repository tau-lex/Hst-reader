#include "include/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setOrganizationName( "Terentew Aleksey" );
    qApp->setOrganizationDomain( "https://github.com/terentjew-alexey/Hst-reader" );
    qApp->setApplicationVersion( "1.3.201701-beta" );
    MainWindow w;
    w.show();

    return a.exec();
}

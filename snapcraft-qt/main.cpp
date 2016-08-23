#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QApplication::setApplicationName("Snapcraft-gui");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("org.keshavnrj.Snapcraft-gui");
    QApplication::setOrganizationDomain("com.keshavnrj.Snapcraft-gui");
    w.show();

    return a.exec();
}

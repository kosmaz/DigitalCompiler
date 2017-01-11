#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath(QCoreApplication::applicationDirPath()+"/plugins");
    MainWindow w;
    w.setMinimumHeight(600);
    w.setMinimumWidth(800);
    w.showMaximized();
    return a.exec();
}

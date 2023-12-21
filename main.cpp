#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*QFile cssfile("H:/QT_Project/ElectroCalc 2.0/ElectroCalc 2.0/style.css");
    cssfile.open(QFile::ReadOnly);
    a.setStyleSheet(cssfile.readAll());*/

    MainWindow w;
    w.show();

    return a.exec();
}



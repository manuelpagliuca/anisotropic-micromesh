#include "Mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mainwindow w;
    w.show();  

    w.loadPallas500();

    return a.exec();
}

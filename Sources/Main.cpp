#include "Headers/Mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Mainwindow w;
  w.show();
  w.on_demo125faces_clicked();
  return a.exec();
}
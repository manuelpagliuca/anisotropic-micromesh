#include "Mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Mainwindow w;

  if (argc > 1) {
    qDebug() << "Processing...";
    exit(1);
  }

  w.show();
  w.on_demo125faces_clicked();

  return a.exec();
}

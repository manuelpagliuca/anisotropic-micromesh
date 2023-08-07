#include "Mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Mainwindow w;

  w.on_demo125faces_clicked();

  if (argc > 1) {
    QCommandLineParser parser;
    parser.setApplicationDescription("Description of your application.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption comparisonSamplesOption("comparison_samples", "Comparison samples");
    parser.addOption(comparisonSamplesOption);

    parser.process(a);

    QString comparisonSamplesValue = parser.value(comparisonSamplesOption);

    QStringList args = parser.positionalArguments();
    QMap<QString, QString> valuesMap;

    for (const QString& arg : args) {
      int index = arg.indexOf('=');
      if (index != -1) {
        QString key = arg.mid(0, index);
        QString value = arg.mid(index + 1);
        valuesMap[key] = value;
      }
    }

    QString nSamples = valuesMap.value("n");
    QString minValValue = valuesMap.value("min_val");
    QString maxValValue = valuesMap.value("max_val");
    QString targetFacesValue = valuesMap.value("target_faces");

    w.exportDisplacedSamples(nSamples.toInt(), minValValue.toDouble(), maxValValue.toDouble(), targetFacesValue.toInt());

    exit(1);
  }

  w.show();

  return a.exec();
}

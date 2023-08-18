#include "Headers/Mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QCommandLineParser parser;

  Mainwindow w;

  if (argc > 1) {
    QString cmd = argv[1];

    QCommandLineOption samplesOption("n", "Number of samples option (default: 5)", "n");
    parser.addOption(samplesOption);

    QCommandLineOption minEdgeOption("min-edge", "Minimum edge value (default: 0.7)", "min-edge");
    parser.addOption(minEdgeOption);

    QCommandLineOption maxEdgeOption("max-edge", "Maximum edge value (default: 10.0)", "max-edge");
    parser.addOption(maxEdgeOption);

    QCommandLineOption targetOption("target-mesh", "Target mesh (default: pallas_5000.obj)", "target-mesh");
    parser.addOption(targetOption);

    QCommandLineOption baseMeshOption("base-mesh", "Base mesh (default: pallas_124.obj)", "base-mesh");
    parser.addOption(baseMeshOption);

    QCommandLineOption metricOption("metric", "Metric option (default: same-target-edges)", "same-target-edges");
    parser.addOption(metricOption);

    QCommandLineOption presetFileNameOption("preset", "Preset file name (default: )", "preset");
    parser.addOption(presetFileNameOption);

    parser.process(a);

    QString metric = parser.value(metricOption);
    QString nSamples = parser.value(samplesOption);
    QString minEdge = parser.value(minEdgeOption);
    QString maxEdge = parser.value(maxEdgeOption);
    QString targetMesh = parser.value(targetOption);
    QString baseMesh = parser.value(baseMeshOption);
    QString presetFileName = parser.value(presetFileNameOption);

    if (metric.isEmpty())           metric = "same-target-edges";
    if (nSamples.isEmpty())         nSamples = "5";
    if (minEdge.isEmpty())          minEdge = "0.7";
    if (maxEdge.isEmpty())          maxEdge = "10.0";
    if (targetMesh.isEmpty())       targetMesh = "pallas_5000.obj";
    if (baseMesh.isEmpty())         baseMesh = "pallas_124.obj";
    if (presetFileName.isEmpty())   presetFileName = "";

    QLocale locale(QLocale::C);
    minEdge = locale.toString(minEdge.toDouble(), 'f', 1);
    maxEdge = locale.toString(maxEdge.toDouble(), 'f', 1);

    if (cmd == "gen-samples") {
      w.loadBaseMesh(baseMesh);
      w.loadTargetMesh(targetMesh);

      if (metric == "same-target-edges") {
        qDebug() << "Generating samples for anisotropic and isotropic schemes with same target edge lenght.";
        w.exportDisplacedSamplesSameTargetEdgeMetric(nSamples.toInt(), minEdge.toDouble(), maxEdge.toDouble());
      } else if(metric == "same-microfaces") {
        qDebug() << "Generating samples for anisotropic and isotropic schemes with same amount of microfaces.";
        w.exportDisplacedSamplesWithSameFacesAmount(minEdge.toDouble(), maxEdge.toDouble(), presetFileName);
      }
    } else if (cmd == "gen-samples-presets") {
      qDebug() << "Generating same-microfaces samples presets for anisotropic and isotropic schemes.";
      w.loadBaseMesh(baseMesh);
      w.exportSameMicrofacesPresets(minEdge.toDouble(), maxEdge.toDouble());
    }
    exit(0);
  }

  w.on_demo124faces_clicked();
  w.show();

  return a.exec();
}

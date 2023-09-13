#include "Mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;

    Mainwindow w;

    if (argc > 1)
    {
        QString cmd = argv[1];

        QCommandLineOption samplesOption("n", "Number of samples option (default: 5)", "n");
        parser.addOption(samplesOption);

        QCommandLineOption minEdgeOption("min-edge", "Minimum edge value (default: 1.0)", "min-edge");
        parser.addOption(minEdgeOption);

        QCommandLineOption maxEdgeOption("max-edge", "Maximum edge value (default: 20.0)", "max-edge");
        parser.addOption(maxEdgeOption);

        QCommandLineOption targetOption("target-mesh", "Target mesh (default: original_pallas_triquad.obj)", "target-mesh");
        parser.addOption(targetOption);

        QCommandLineOption baseMeshOption("base-mesh", "Base mesh (default: pallas_124.obj)", "base-mesh");
        parser.addOption(baseMeshOption);

        QCommandLineOption criterionOption("criterion", "Criterion option (default: same-target-edges)", "same-target-edges");
        parser.addOption(criterionOption);

        QCommandLineOption presetFileNameOption("preset", "Preset file name", "preset");
        parser.addOption(presetFileNameOption);

        QCommandLineOption microfacesAmountOption("microfaces", "Microfaces amount (default: two times base-mesh faces)", "microfaces");
        parser.addOption(microfacesAmountOption);

        QCommandLineOption subdivisionSchemeOption("scheme", "Scheme (default: micro)", "scheme");
        parser.addOption(subdivisionSchemeOption);

        parser.process(a);

        QString criterion = parser.value(criterionOption);
        QString nSamples = parser.value(samplesOption);
        QString minEdge = parser.value(minEdgeOption);
        QString maxEdge = parser.value(maxEdgeOption);
        QString targetMesh = parser.value(targetOption);
        QString baseMesh = parser.value(baseMeshOption);
        QString presetFileName = parser.value(presetFileNameOption);
        QString microfacesAmount = parser.value(microfacesAmountOption);
        QString subdivisionScheme = parser.value(subdivisionSchemeOption);

        Scheme scheme = ISOTROPIC;

        if (criterion.isEmpty())
            criterion = "same-target-edges";
        if (nSamples.isEmpty())
            nSamples = "5";
        if (minEdge.isEmpty())
            minEdge = "0.1";
        if (maxEdge.isEmpty())
            maxEdge = "20.0";
        if (targetMesh.isEmpty())
            targetMesh = "original_pallas_triquad.obj";
        if (baseMesh.isEmpty())
            baseMesh = "pallas_124.obj";
        if (presetFileName.isEmpty())
            presetFileName = "";
        if (microfacesAmount.isEmpty())
            microfacesAmount = "0";
        if (subdivisionScheme == "aniso")
            scheme = ANISOTROPIC;

        QLocale locale(QLocale::C);
        minEdge = locale.toString(minEdge.toDouble(), 'f', 1);
        maxEdge = locale.toString(maxEdge.toDouble(), 'f', 1);

        w.loadBaseMesh(baseMesh);

        if (cmd == "gen-samples")
        {
            w.loadTargetMesh(targetMesh);
            if (criterion == "same-target-edges")
            {
                qDebug() << "Generating samples for anisotropic and isotropic schemes with same target edge lenght.";
                w.exportDisplacedSamplesSameTargetEdgeMetric(nSamples.toInt(), minEdge.toDouble(), maxEdge.toDouble());
            }
            else if (criterion == "same-microfaces")
            {
                qDebug() << "Generating samples for anisotropic and isotropic schemes with same amount of microfaces.";
                w.exportDisplacedSamplesWithSameFacesAmount(minEdge.toDouble(), maxEdge.toDouble(), presetFileName);
            }
        }
        else if (cmd == "gen-samples-preset")
        {
            qDebug() << "Generating same-microfaces samples presets for anisotropic and isotropic schemes.";
            w.exportSameMicrofacesPreset(minEdge.toDouble(), maxEdge.toDouble());
        }
        else if (cmd == "gen-sample")
        {
            w.loadTargetMesh(targetMesh);
            int microFaces = int(w.targetMesh.faces.size()) * 2;
            microFaces = microfacesAmount.toInt() > microFaces ? microfacesAmount.toInt() : microFaces;
            qDebug() << "Generating a sample with: " << microFaces << " micro-faces.";
            w.exportDisplacedBaseMesh(microFaces, scheme);
        }

        exit(0);
    }

    w.on_demo124faces_clicked();
    w.show();
    return a.exec();
}

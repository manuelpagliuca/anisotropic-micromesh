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

        QCommandLineOption targetOption("target-mesh", "Target mesh (default: original_pallas_triquad.obj)", "target-mesh");
        parser.addOption(targetOption);

        QCommandLineOption baseMeshOption("base-mesh", "Base mesh (default: pallas_124.obj)", "base-mesh");
        parser.addOption(baseMeshOption);

        QCommandLineOption subdivisionSchemeOption("scheme", "Scheme (default: micro)", "scheme");
        parser.addOption(subdivisionSchemeOption);

        QCommandLineOption microfacesFactorOption("factor", "Microfaces factor (default: 2.0)", "factor");
        parser.addOption(microfacesFactorOption);

        parser.process(a);

        QString targetMesh = parser.value(targetOption);
        QString baseMesh = parser.value(baseMeshOption);
        QString microfacesAmount = parser.value(microfacesAmountOption);
        QString subdivisionScheme = parser.value(subdivisionSchemeOption);
        QString microfacesFactorStr = parser.value(microfacesFactorOption);

        Scheme scheme = ISOTROPIC;

        if (targetMesh.isEmpty())          targetMesh = "original_pallas_triquad.obj";
        if (baseMesh.isEmpty())            baseMesh = "pallas_1000.obj";
        if (microfacesFactorStr.isEmpty()) microfacesFactorStr = "2.0";
        if (subdivisionScheme == "aniso")  scheme = ANISOTROPIC;

        w.loadBaseMesh(baseMesh);

        if (cmd == "gen-sample")
        {
            w.loadTargetMesh(targetMesh);
            int microFaces = microfacesAmount.isEmpty() ? int(w.targetMesh.faces.size() * microfacesFactorStr.toDouble()) : microfacesAmount.toInt();
            microFaces = microfacesAmount.toInt() > microFaces ? microfacesAmount.toInt() : microFaces;
            qDebug() << "Generating a sample with: " << microFaces << " micro-faces., using a factor of " << microfacesFactorStr;
            w.exportDisplacedBaseMesh(microfacesFactorStr.toDouble(), scheme);
        }

        exit(0);
    }

    w.on_demo124faces_clicked();
    w.show();
    return a.exec();
}

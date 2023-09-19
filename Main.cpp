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

        QCommandLineOption baseMeshOption("base-mesh", "Base mesh (default: pallas_1000.obj)", "base-mesh");
        parser.addOption(baseMeshOption);

        QCommandLineOption subdivisionSchemeOption("scheme", "Scheme (default: micro)", "scheme");
        parser.addOption(subdivisionSchemeOption);

        QCommandLineOption microfacesFactorOption("factor", "Microfaces factor (default: 2.0)", "factor");
        parser.addOption(microfacesFactorOption);

        QCommandLineOption microfacesOption("microfaces", "Microfaces (default: 0)", "microfaces");
        parser.addOption(microfacesOption);

        parser.process(a);

        QString targetMesh = parser.value(targetOption);
        QString baseMesh = parser.value(baseMeshOption);
        QString subdivisionScheme = parser.value(subdivisionSchemeOption);
        QString microFacesFactorStr = parser.value(microfacesFactorOption);
        QString microFacesStr = parser.value(microfacesOption);

        Scheme scheme = ISOTROPIC;

        if (targetMesh.isEmpty())          targetMesh = "original_pallas_triquad.obj";
        if (baseMesh.isEmpty())            baseMesh = "pallas_1000.obj";
        if (microFacesFactorStr.isEmpty()) microFacesFactorStr = "2.0";
        if (subdivisionScheme == "aniso")  scheme = ANISOTROPIC;
        if (microFacesStr.isEmpty())       microFacesStr = "0";

        w.loadBaseMesh(baseMesh);

        if (cmd == "gen-sample")
        {
            w.loadTargetMesh(targetMesh);
            w.exportDisplacedBaseMesh(microFacesFactorStr.toDouble(), scheme);
        }
        else if (cmd == "gen-subdivided-sample" && !microFacesStr.isEmpty())
        {
            w.exportSubdividedBaseMesh(microFacesStr.toInt(), scheme);
        }

        exit(0);
    }

    w.on_demo124faces_clicked();
    w.show();
    return a.exec();
}

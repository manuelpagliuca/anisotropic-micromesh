#include "Mainwindow.h"

void Mainwindow::exportDisplacedSamplesSameTargetEdgeMetric(int nSamples, double minEdge, double maxEdge)
{
    if (nSamples <= 0)
    {
        perror("Invalid number of samples.\n");
        return;
    }

    double step = (maxEdge - minEdge) / nSamples;
    std::vector<float> displacements;

    morphingCurrentValue = 100;

    scheme = ISOTROPIC;

    for (double length = minEdge; length < maxEdge; length += step)
    {
        edgeLengthCurrentValue = length;

        projectedMesh = baseMesh;
        projectedMesh.updateEdgesSubdivisionLevelsMicromesh(edgeLengthCurrentValue);
        projectedMesh = projectedMesh.micromeshSubdivide();

        displacements.clear();
        displacements = projectedMesh.getDisplacements(targetMesh);

        int vertexIdx = 0;

        for (const float &disp : displacements)
            projectedMesh.displaceVertex(vertexIdx++, disp);

        std::string pathAndName =
            "Evaluation/same_target_edges/micro/" + baseMeshNameAndDetail + "_to_" + std::to_string(targetMesh.faces.size()) + "_disp_100_edge_" + std::to_string(length);

        projectedMesh.exportOBJ(pathAndName.c_str());
    }

    scheme = ANISOTROPIC;

    for (double length = minEdge; length < maxEdge; length += step)
    {
        edgeLengthCurrentValue = length;

        projectedMesh = baseMesh;
        projectedMesh.updateEdgesSubdivisionLevelsAniso(edgeLengthCurrentValue);
        projectedMesh = projectedMesh.anisotropicMicromeshSubdivide();

        displacements.clear();
        displacements = projectedMesh.getDisplacements(targetMesh);

        int vertexIdx = 0;

        for (const float &disp : displacements)
            projectedMesh.displaceVertex(vertexIdx++, disp);

        std::string pathAndName =
            "Evaluation/same_target_edges/aniso/" + baseMeshNameAndDetail + "_to_" + std::to_string(targetMesh.faces.size()) + "_disp_100_edge_" + std::to_string(length);

        projectedMesh.exportOBJ(pathAndName.c_str());
    }
}

QString Mainwindow::exportSameMicrofacesPreset(double minEdge, double maxEdge)
{
    Mesh micro, aniso;
    std::vector<float> microDisplacements, anisoDisplacements;

    std::string minEdgeStr = std::to_string(minEdge);
    std::string maxEdgeStr = std::to_string(maxEdge);
    minEdgeStr = minEdgeStr.substr(0, minEdgeStr.find(".") + 3);
    maxEdgeStr = maxEdgeStr.substr(0, maxEdgeStr.find(".") + 3);

    QString presetFileName = QString::fromStdString(baseMeshNameAndDetail + "_minEdge_" + minEdgeStr + "_maxEdge_" + maxEdgeStr + ".txt");
    QString outputFilePath = QString::fromStdString("./Output/Evaluation/same_microfaces/presets/") + presetFileName;
    QFile presetFile(outputFilePath);

    if (!presetFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing.";
        return QString("");
    }

    std::vector<int> microMeshFaces;

    for (int microLengthIdx = 0; microLengthIdx < ((maxEdge - minEdge) / 0.01); ++microLengthIdx)
    {
        double microLength = minEdge + microLengthIdx * 0.01;
        micro = baseMesh;
        micro.updateEdgesSubdivisionLevelsMicromesh(microLength);
        micro = micro.micromeshSubdivide();
        microMeshFaces.push_back(int(micro.faces.size()));

        qDebug() << "Progress (microMeshFaces): " << microLengthIdx << "/" << ((maxEdge - minEdge) / 0.01) << ", microLength: " << microLength;
    }

    std::vector<int> anisoMeshFaces;

    for (int anisoLengthIdx = 0; anisoLengthIdx < ((maxEdge - minEdge) / 0.01); ++anisoLengthIdx)
    {
        double anisoLength = minEdge + anisoLengthIdx * 0.01;
        aniso = baseMesh;
        aniso.updateEdgesSubdivisionLevelsAniso(anisoLength);
        aniso = aniso.anisotropicMicromeshSubdivide();
        anisoMeshFaces.push_back(int(aniso.faces.size()));

        qDebug() << "Progress (anisoMeshFaces): " << anisoLengthIdx << "/" << ((maxEdge - minEdge) / 0.01) << ", anisoLength: " << anisoLength;
        ;
    }

    QTextStream out(&presetFile);
    out << "Micro\t| Aniso\t| Faces\n";
    int lastFacesMatch = -1;

    for (size_t i = 0; i < microMeshFaces.size(); ++i)
    {
        for (size_t j = 0; j < anisoMeshFaces.size(); ++j)
        {
            if (microMeshFaces[i] == anisoMeshFaces[j] && microMeshFaces[i] != lastFacesMatch)
            {
                lastFacesMatch = microMeshFaces[i];

                double microLength = minEdge + i * 0.01;
                double anisoLength = minEdge + j * 0.01;

                out << microLength << "\t| " << anisoLength << "\t| " << microMeshFaces[i] << "\n";
            }
        }
    }

    qDebug() << "Preset generated at " << outputFilePath;

    presetFile.close();
    return presetFileName;
}

void Mainwindow::exportDisplacedSamplesWithSameFacesAmount(double minEdge, double maxEdge, QString presetFileName)
{
    if (presetFileName.isEmpty())
        presetFileName = exportSameMicrofacesPreset(minEdge, maxEdge);

    QString presetDirPath(QString::fromStdString("./Output/Evaluation/same_microfaces/presets/") + presetFileName);
    exportDisplacedSamples(presetDirPath);
}

void Mainwindow::exportDisplacedSamples(const QString presetDirPath)
{
    QFile presetFile = QFile(presetDirPath);

    if (!presetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for reading.";
        return;
    }

    QTextStream in(&presetFile);
    in.readLine();

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList values = line.split("|", Qt::SkipEmptyParts);

        if (values.size() >= 3)
        {
            QString microLengthStr = values.at(0);
            QString anisoLengthStr = values.at(1);
            QString facesStr = values.at(2);

            float microLength = microLengthStr.toFloat();
            float anisoLength = anisoLengthStr.toFloat();
            int faces = facesStr.toInt();

            Mesh micro = baseMesh;
            micro.updateEdgesSubdivisionLevelsMicromesh(microLength);
            micro = micro.micromeshSubdivide();

            std::vector<float> displacements = micro.getDisplacements(targetMesh);

            int vertexIdx = 0;

            for (const float &disp : displacements)
                micro.displaceVertex(vertexIdx++, disp);

            QString microDir =
                "Evaluation/same_microfaces/micro/" +
                QString::fromStdString(baseMeshNameAndDetail + "/");
            QDir microDirPath(QString::fromStdString("./Output/") + microDir);

            if (!microDirPath.exists())
            {
                if (!microDirPath.mkpath("."))
                {
                    qDebug() << "Error during the creation of the directory: " << microDir;
                    return;
                }
            }

            std::string microFilePath =
                microDir.toStdString() + "to_" + std::to_string(targetMesh.faces.size()) + "_faces_" + std::to_string(faces);

            micro.exportOBJ(microFilePath.c_str());
            qDebug() << "Micromesh sample exported " + QString::fromStdString(microFilePath);

            Mesh aniso = baseMesh;

            aniso.updateEdgesSubdivisionLevelsAniso(anisoLength);
            aniso = aniso.anisotropicMicromeshSubdivide();
            displacements.clear();
            displacements = aniso.getDisplacements(targetMesh);

            vertexIdx = 0;
            for (const float &disp : displacements)
                aniso.displaceVertex(vertexIdx++, disp);

            QString anisoDir =
                "Evaluation/same_microfaces/aniso/" +
                QString::fromStdString(baseMeshNameAndDetail + "/");
            QDir anisoDirPath(QString::fromStdString("./Output/") + anisoDir);

            if (!anisoDirPath.exists())
            {
                if (!anisoDirPath.mkpath("."))
                {
                    qDebug() << "Error during the creation of the directory: " << anisoDir;
                    return;
                }
            }

            std::string anisoFilePath =
                anisoDir.toStdString() + "to_" + std::to_string(targetMesh.faces.size()) + "_faces_" + std::to_string(faces);

            aniso.exportOBJ(anisoFilePath.c_str());
            qDebug() << "Anisotropic micro-mesh sample exported " + QString::fromStdString(anisoFilePath);
        }
    }

    presetFile.close();
}

void Mainwindow::exportDisplacedBaseMesh(int microFaces, Scheme scheme)
{
    assert(baseMesh.isValid() == true);

    double targetEdgeLength = binarySearchTargetEdgeLength(microFaces, scheme, baseMesh.bbox.diagonal() / 10000,  baseMesh.bbox.diagonal() / 10);

    subdividedMesh = subdivideBaseMesh(scheme);

    assert(subdividedMesh.isValid() == true);
    assert(targetMesh.isValid() == true);

    std::vector<float> displacements = subdividedMesh.getDisplacements(targetMesh);
    projectedMesh = subdividedMesh;

    int vertexIdx = 0;

    for (const float &disp : displacements)
        projectedMesh.displaceVertex(vertexIdx++, disp);

    QString fileName = QString::fromStdString("displacedTo") +
                       QString::number(targetMesh.faces.size()) +
                       "_ApprxMicroFaces_" +
                       QString::number(microFaces);

    QDir outputDir =
        QDir("Evaluation/same_microfaces/" + QString::fromStdString(enumToString(scheme)) + "/" +
        QString::fromStdString(baseMeshNameAndDetail) + "/");

    if (!outputDir.exists())
    {
        if (!outputDir.mkpath("."))
        {
            qDebug() << "Error during the creation of the directory: " << outputDir;
            return;
        }
    }

    QString outputPath = outputDir.path() + "/";

    projectedMesh.exportOBJ(fileName.toStdString().c_str(), outputPath);
}

double Mainwindow::binarySearchTargetEdgeLength(int targetMicroFaces, Scheme scheme, double a, double b)
{
    int aFaces = -1, bFaces = -2;
    int patience = 10;

    while (1)
    {
        double c = (a + b) / 2.0;

        int cFaces = predictMicroFaces(scheme, c);

        if (cFaces == aFaces || cFaces == bFaces)
            if (patience-- == 0)
                return (abs(targetMicroFaces - aFaces) < abs(targetMicroFaces - bFaces)) ? a : b;

        if (cFaces < targetMicroFaces)
        {
            bFaces = cFaces;
            b = c;
        }
        else
        {
            aFaces = cFaces;
            a = c;
        }

        if (cFaces == targetMicroFaces)
            return c;
    }
}

int Mainwindow::predictMicroFaces(Scheme scheme, double edgeLength)
{
    if (scheme == ANISOTROPIC)
    {
        baseMesh.updateEdgesSubdivisionLevelsAniso(edgeLength);
        return baseMesh.anisotropicMicroMeshPredictFaces();
    }

    baseMesh.updateEdgesSubdivisionLevelsMicromesh(edgeLength);
    return baseMesh.micromeshPredictFaces();
}

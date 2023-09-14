#include "Mainwindow.h"

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

void Mainwindow::exportDisplacedBaseMesh(double mfsFactor, Scheme scheme)
{
    int microFaces = int(targetMesh.faces.size() * mfsFactor);

    qDebug() << "Binary searching the appropriate target edge length...";
    edgeLengthCurrentValue = binarySearchTargetEdgeLength(microFaces, scheme, 0,  baseMesh.bbox.diagonal() / 10);

    qDebug() << "Target Edge lengh of : " << edgeLengthCurrentValue
             << ", approx. the " << enumToString(scheme)
             << " scheme will use "
             << predictMicroFaces(scheme, edgeLengthCurrentValue)
             << "(x" << mfsFactor << " faces of target-mesh).";

    qDebug() << "Now subdividing the base mesh according using the found target edge length";
    subdivideBaseMesh(scheme);

    qDebug() << "The subdivided mesh has " << subdividedMesh.faces.size()
             << " microfaces";

    qDebug() << "Starting the ray casting towards the target mesh";
    std::vector<float> displacements = subdividedMesh.getDisplacements(targetMesh);

    qDebug() << "Displacements extracted successfully!";


    qDebug() << "Applying displacements on the subdivided mesh...";
    projectedMesh = subdividedMesh;
    int vertexIdx = 0;
    for (const float &disp : displacements)
        projectedMesh.displaceVertex(vertexIdx++, disp);

    qDebug() << "Projected mesh constructed successfully!";
    QString fileName = QString::fromStdString("to_") +
                       QString::number(targetMesh.faces.size()) +
                       QString::fromStdString("_mfs_") +
                       QString::number(projectedMesh.faces.size()) +
                       QString::fromStdString("_factor_") +
                       QString::number(mfsFactor);

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
    qDebug() << "Diagonal AABB of the target mesh: " << targetMesh.bbox.diagonal();
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

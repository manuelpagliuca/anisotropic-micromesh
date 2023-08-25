#include "Mainwindow.h"

void Mainwindow::exportDisplacedSamplesSameTargetEdgeMetric(int nSamples, double minEdge, double maxEdge)
{
  if (nSamples <= 0) {
    perror("Invalid number of samples.\n");
    return;
  }

  double step = (maxEdge - minEdge) / nSamples;
  std::vector<float> displacements;

  morphingCurrentValue = 100;
  isAniso = false;

  for (double length = minEdge; length < maxEdge; length += step) {
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
      "Evaluation/same_target_edges/micro/"
      + baseMeshNameAndDetail
      + "_to_" + std::to_string(targetMesh.faces.size())
      + "_disp_100_edge_" + std::to_string(length);

    projectedMesh.exportOBJ(pathAndName.c_str());
  }

  isAniso = true;

  for (double length = minEdge; length < maxEdge; length += step) {
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
      "Evaluation/same_target_edges/aniso/"
      + baseMeshNameAndDetail
      + "_to_" + std::to_string(targetMesh.faces.size())
      + "_disp_100_edge_" + std::to_string(length);

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
  QFile presetFile (outputFilePath);

  if (!presetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to open file for writing.";
    return QString("");
  }

  std::vector<int> microMeshFaces;

  #pragma omp parallel for
  for (int microLengthIdx = 0; microLengthIdx < ((maxEdge - minEdge) / 0.01); ++microLengthIdx) {
    double microLength = minEdge + microLengthIdx * 0.01;
    micro = baseMesh;
    micro.updateEdgesSubdivisionLevelsMicromesh(microLength);
    micro = micro.micromeshSubdivide();
    microMeshFaces.push_back(int(micro.faces.size()));

    #pragma omp critical
    {
      qDebug() << "Progress (microMeshFaces): " << microLengthIdx << "/" << ((maxEdge - minEdge) / 0.01) << ", microLength: " << microLength;
    }
  }

  std::vector<int> anisoMeshFaces;

  #pragma omp parallel for
  for (int anisoLengthIdx = 0; anisoLengthIdx < ((maxEdge - minEdge) / 0.01); ++anisoLengthIdx) {
    double anisoLength = minEdge + anisoLengthIdx * 0.01;
    aniso = baseMesh;
    aniso.updateEdgesSubdivisionLevelsAniso(anisoLength);
    aniso = aniso.anisotropicMicromeshSubdivide();
    anisoMeshFaces.push_back(int(aniso.faces.size()));

    #pragma omp critical
    {
      qDebug() << "Progress (anisoMeshFaces): " << anisoLengthIdx << "/" << ((maxEdge - minEdge) / 0.01) << ", anisoLength: " << anisoLength;;
    }
  }

  QTextStream out(&presetFile);
  out << "Micro\t| Aniso\t| Faces\n";
  int lastFacesMatch = -1;

  #pragma omp parallel for
  for (size_t i = 0; i < microMeshFaces.size(); ++i) {
    for (size_t j = 0; j < anisoMeshFaces.size(); ++j) {
      if (microMeshFaces[i] == anisoMeshFaces[j] && microMeshFaces[i] != lastFacesMatch) {
        lastFacesMatch = microMeshFaces[i];

        double microLength = minEdge + i * 0.01;
        double anisoLength = minEdge + j * 0.01;

        #pragma omp critical
        {
          out << microLength << "\t| " << anisoLength << "\t| " << microMeshFaces[i] << "\n";
        }
      }
    }
  }

  qDebug() << "Preset generated at " << outputFilePath;

  presetFile.close();
  return presetFileName;
}

void Mainwindow::exportDisplacedSamplesWithSameFacesAmount(double minEdge, double maxEdge, QString presetFileName)
{
  if (presetFileName.isEmpty()) {
    presetFileName = exportSameMicrofacesPreset(minEdge, maxEdge);
  }

  QString presetDirPath(QString::fromStdString("./Output/Evaluation/same_microfaces/presets/") + presetFileName);
  exportDisplacedSamples(presetDirPath);
}

void Mainwindow::exportDisplacedSamples(const QString presetDirPath)
{
  QFile presetFile = QFile(presetDirPath);

  if (!presetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open file for reading.";
    return;
  }

  QTextStream in(&presetFile);
  in.readLine();

  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList values = line.split("|", Qt::SkipEmptyParts);

    if (values.size() >= 3) {
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

      if (!microDirPath.exists()) {
        if (!microDirPath.mkpath(".")) {
          qDebug() << "Error during the creation of the directory: " << microDir;
          return;
        }
      }

      std::string microFilePath =
        microDir.toStdString()
        + "to_" + std::to_string(targetMesh.faces.size())
        + "_faces_" + std::to_string(faces);

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

      if (!anisoDirPath.exists()) {
        if (!anisoDirPath.mkpath(".")) {
          qDebug() << "Error during the creation of the directory: " << anisoDir;
          return;
        }
      }

      std::string anisoFilePath =
        anisoDir.toStdString()
        + "to_" + std::to_string(targetMesh.faces.size())
        + "_faces_" + std::to_string(faces);

      aniso.exportOBJ(anisoFilePath.c_str());
      qDebug() << "Anisotropic micro-mesh sample exported " + QString::fromStdString(anisoFilePath);
    }
  }

  presetFile.close();
}

void Mainwindow::exportDisplacedBaseMesh(int microFaces, QString subdivisionScheme, double minEdge, double maxEdge)
{
  double targetEdgeLength = binarSearchTargetEdgeLength(microFaces, subdivisionScheme, minEdge, maxEdge);

  if (subdivisionScheme == "aniso") {
    baseMesh.updateEdgesSubdivisionLevelsAniso(targetEdgeLength);
    subdividedMesh = baseMesh.anisotropicMicromeshSubdivide();
  } else {
    baseMesh.updateEdgesSubdivisionLevelsMicromesh(targetEdgeLength);
    subdividedMesh = baseMesh.micromeshSubdivide();
  }

  qDebug() << "Given " << microFaces
           << " we need a target edge length of: "
           << targetEdgeLength << ", level of : "
           << subdividedMesh.faces.size();
}

double Mainwindow::binarSearchTargetEdgeLength(int targetMicroFaces, QString subdivisionScheme, double a, double b)
{
  int aFaces, bFaces;

  if (subdivisionScheme == "aniso") {
    baseMesh.updateEdgesSubdivisionLevelsAniso(a);
    aFaces = int(baseMesh.anisotropicMicromeshSubdivide().faces.size());

    baseMesh.updateEdgesSubdivisionLevelsAniso(b);
    bFaces = int(baseMesh.anisotropicMicromeshSubdivide().faces.size());
  } else {
    baseMesh.updateEdgesSubdivisionLevelsMicromesh(a);
    aFaces = int(baseMesh.micromeshSubdivide().faces.size());

    baseMesh.updateEdgesSubdivisionLevelsMicromesh(b);
    bFaces = int(baseMesh.micromeshSubdivide().faces.size());
  }

  while (true) {
    double c = (a + b) / 2.0;
    int cFaces;

    if (subdivisionScheme == "aniso") {
      baseMesh.updateEdgesSubdivisionLevelsAniso(c);
      cFaces = int(baseMesh.anisotropicMicromeshSubdivide().faces.size());
    } else {
      baseMesh.updateEdgesSubdivisionLevelsMicromesh(c);
      cFaces = int(baseMesh.micromeshSubdivide().faces.size());
    }

    if (cFaces == aFaces || cFaces == bFaces) {
      qDebug () << cFaces << aFaces << bFaces;
      return abs(targetMicroFaces - aFaces) < abs(targetMicroFaces - bFaces) ? a : b;
    }

    if (cFaces == targetMicroFaces) return c;

    if (cFaces < targetMicroFaces) {
      b = c;
      bFaces = cFaces;
    }

    if (cFaces > targetMicroFaces) {
      a = c;
      aFaces = cFaces;
    }
  }
}

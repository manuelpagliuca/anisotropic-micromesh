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

void Mainwindow::exportDisplacedSamplesWithSameFacesAmount(double minEdge, double maxEdge)
{
  Mesh micro, aniso;
  std::vector<float> microDisplacements, anisoDisplacements;

  std::string outputFileName = "./Output/Evaluation/same_microfaces/presets/" + baseMeshNameAndDetail + "_micro_aniso_target_lengths_matches.txt";
  QFile presetFile (outputFileName.c_str());

  if (!presetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to open file for writing.";
    return;
  }

  QTextStream out(&presetFile);
  out << "Micro\t| Aniso\t| Faces\n";

  std::vector<int> microMeshFaces;

  #pragma omp parallel for
  for (int microLengthIdx = 0; microLengthIdx < ((maxEdge - minEdge) / 0.01); ++microLengthIdx) {
    double microLength = minEdge + microLengthIdx * 0.01;
    micro = baseMesh;
    micro.updateEdgesSubdivisionLevelsMicromesh(microLength);
    micro = micro.micromeshSubdivide();
    microMeshFaces.push_back(micro.faces.size());

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
    anisoMeshFaces.push_back(aniso.faces.size());

    #pragma omp critical
    {
      qDebug() << "Progress (anisoMeshFaces): " << anisoLengthIdx << "/" << ((maxEdge - minEdge) / 0.01) << ", anisoLength: " << anisoLength;;
    }
  }

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

  presetFile.close();

  exportDisplacedSamples(presetFile);
}

void Mainwindow::exportDisplacedSamples(QFile &presetFile)
{
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

      QString microDirPath =
        "Evaluation/same_microfaces/micro/" +
        QString::fromStdString(baseMeshNameAndDetail +
        "/");
      QDir microDir(QString::fromStdString("./Output/") + microDirPath);

      if (!microDir.exists()) {
        if (!microDir.mkpath(".")) {
          qDebug() << "Error during the creation of the directory: " << microDirPath;
          return;
        }
      }

      std::string microFilePath =
        microDirPath.toStdString()
        + "to_" + std::to_string(targetMesh.faces.size())
        + "_faces_" + std::to_string(faces);

      micro.exportOBJ(microFilePath.c_str());

      Mesh aniso = baseMesh;

      aniso.updateEdgesSubdivisionLevelsAniso(anisoLength);
      aniso = aniso.anisotropicMicromeshSubdivide();
      displacements.clear();
      displacements = aniso.getDisplacements(targetMesh);

      vertexIdx = 0;
      for (const float &disp : displacements)
        aniso.displaceVertex(vertexIdx++, disp);

      QString anisoDirPath =
        "Evaluation/same_microfaces/aniso/" +
        QString::fromStdString(baseMeshNameAndDetail +
        "/");
      QDir anisoDir(QString::fromStdString("./Output/") + anisoDirPath);

      if (!anisoDir.exists()) {
        if (!anisoDir.mkpath(".")) {
          qDebug() << "Error during the creation of the directory: " << anisoDirPath;
          return;
        }
      }

      std::string anisoFilePath =
        anisoDirPath.toStdString()
        + "to_" + std::to_string(targetMesh.faces.size())
        + "_faces_" + std::to_string(faces);

      aniso.exportOBJ(anisoFilePath.c_str());
    }
  }

  presetFile.close();
}

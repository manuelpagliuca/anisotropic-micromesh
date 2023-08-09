#include "Mainwindow.h"

void Mainwindow::exportDisplacedSamplesSameTargetEdgeMetric(int nSamples, double minVal, double maxVal)
{
  if (nSamples <= 0) {
    perror("Invalid number of samples.\n");
    return;
  }

  double step = (maxVal - minVal) / nSamples;
  std::vector<float> displacements;

  morphingCurrentValue = 100;
  isAniso = false;

  for (double length = minVal; length < maxVal; length += step) {
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

  for (double length = minVal; length < maxVal; length += step) {
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

void Mainwindow::exportDisplacedSamplesWithSameFacesAmount(double minVal, double maxVal)
{
  Mesh micro, aniso;
  std::vector<float> microDisplacements, anisoDisplacements;

  std::string outputFileName = baseMeshNameAndDetail + "_micro_aniso_target_lengths_matches.txt";
  QFile file (outputFileName.c_str());

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to open file for writing.";
    return;
  }

  QTextStream out(&file);
  out << "Matching target edge lengths for Micromesh and Anisotropic Micromesh schemes\n";
  out << "Micro | Aniso | Faces\n";

  for (double microLength = minVal; microLength < maxVal; microLength += 0.01) {
    micro = baseMesh;
    micro.updateEdgesSubdivisionLevelsMicromesh(microLength);
    micro = micro.micromeshSubdivide();

    for (double anisoLength = minVal; anisoLength < maxVal; anisoLength += 0.01) {
      aniso = baseMesh;
      aniso.updateEdgesSubdivisionLevelsAniso(anisoLength);
      aniso = aniso.anisotropicMicromeshSubdivide();

      if (micro.faces.size() == aniso.faces.size()) {
        qDebug() << "Micromesh target edge length: " << microLength
                 << " and Anisotropic Micromesh of : " << anisoLength
                 << ", we have the same number of faces "<< micro.faces.size();

        out << microLength << " | " << anisoLength << " | " << micro.faces.size() << "\n";

        microDisplacements = micro.getDisplacements(targetMesh);
        anisoDisplacements = aniso.getDisplacements(targetMesh);

        int vertexIdx = 0;

        for (const float &disp : microDisplacements)
          micro.displaceVertex(vertexIdx++, disp);

        vertexIdx = 0;

        for (const float &disp : anisoDisplacements)
          aniso.displaceVertex(vertexIdx++, disp);

        std::string microPath =
          "Evaluation/same_microfaces/micro/" +
          baseMeshNameAndDetail + "_to_" +
          std::to_string(targetMesh.faces.size()) +
          "_disp_100_edge_" + std::to_string(microLength);

        std::string anisoPath =
          "Evaluation/same_microfaces/aniso/" +
          baseMeshNameAndDetail + "_to_" +
          std::to_string(targetMesh.faces.size()) +
          "_disp_100_edge_" + std::to_string(anisoLength);

        micro.exportOBJ(microPath);
        aniso.exportOBJ(anisoPath);
      }

    }
  }

  file.close();
}

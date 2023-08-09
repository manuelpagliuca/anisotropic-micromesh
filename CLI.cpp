#include "Mainwindow.h"

#include <iostream>
#include <vector>
#include <omp.h>

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
  QFile file (outputFileName.c_str());

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to open file for writing.";
    return;
  }

  QTextStream out(&file);
  out << "Matching target edge lengths for Micromesh and Anisotropic Micromesh schemes\n";
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

  // Esporta i matching in un file esterno
  int lastSameFaceMatch;

  #pragma omp parallel for
  for (size_t i = 0; i < microMeshFaces.size(); ++i) {
    for (size_t j = 0; j < anisoMeshFaces.size(); ++j) {
      if (microMeshFaces[i] == anisoMeshFaces[j] && microMeshFaces[i] != lastSameFaceMatch) {
        lastSameFaceMatch = microMeshFaces[i];
        double microLength = minEdge + i * 0.01;
        double anisoLength = minEdge + j * 0.01;

        #pragma omp critical
        {
          qDebug() << "Same number of faces: " << microMeshFaces[i]
                   << ", at length (micro): " << microLength
                   << " and length (aniso): " << anisoLength << "\n";

          out << microLength << "\t| " << anisoLength << "\t| " << micro.faces.size() << "\n";
        }
      }
    }
  }

  // displacement dei campioni




//  #pragma omp parallel for
//  for (int microLengthIdx = 0; microLengthIdx < ((maxEdge - minEdge) / 0.01); ++microLengthIdx) {
//    double microLength = minEdge + microLengthIdx * 0.01;
//    micro = baseMesh;
//    micro.updateEdgesSubdivisionLevelsMicromesh(microLength);
//    micro = micro.micromeshSubdivide();

//    #pragma omp parallel for
//    for (int anisoLengthIdx = 0; anisoLengthIdx < ((maxEdge - minEdge) / 0.01); ++anisoLengthIdx) {
//      double anisoLength = minEdge + anisoLengthIdx * 0.01;
//      aniso = baseMesh;
//      aniso.updateEdgesSubdivisionLevelsAniso(anisoLength);
//      aniso = aniso.anisotropicMicromeshSubdivide();

//      if (micro.faces.size() == aniso.faces.size()) {
//        qDebug() << "Micromesh target edge length: " << microLength
//                 << " and Anisotropic Micromesh of : " << anisoLength
//                 << ", we have the same number of faces "<< micro.faces.size();

//        out << microLength << " | " << anisoLength << " | " << micro.faces.size() << "\n";

//        microDisplacements = micro.getDisplacements(targetMesh);
//        anisoDisplacements = aniso.getDisplacements(targetMesh);

//        int vertexIdx = 0;

//        for (const float &disp : microDisplacements)
//          micro.displaceVertex(vertexIdx++, disp);

//        vertexIdx = 0;

//        for (const float &disp : anisoDisplacements)
//          aniso.displaceVertex(vertexIdx++, disp);

//        std::string microPath =
//          "Evaluation/same_microfaces/micro/" +
//          baseMeshNameAndDetail + "_to_" +
//          std::to_string(targetMesh.faces.size()) +
//          "_disp_100_edge_" + std::to_string(microLength);

//        std::string anisoPath =
//          "Evaluation/same_microfaces/aniso/" +
//          baseMeshNameAndDetail + "_to_" +
//          std::to_string(targetMesh.faces.size()) +
//          "_disp_100_edge_" + std::to_string(anisoLength);

//        micro.exportOBJ(microPath);
//        aniso.exportOBJ(anisoPath);
//      }

//      #pragma omp critical // Aggiungi una sezione critica per stampare progressi in modo sincronizzato
//      {
//        qDebug() << "Progress: Micromesh target edge length: " << microLength
//                 << " and Anisotropic Micromesh of: " << anisoLength;
//      }
//    }
//  }

  file.close();
}

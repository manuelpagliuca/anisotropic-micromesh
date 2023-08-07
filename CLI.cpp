#include "Mainwindow.h"

void Mainwindow::exportDisplacedSamples(int nSamples, double minVal, double maxVal, int targetFaces)
{
  if (nSamples <= 0) {
    perror("Invalid number of samples.\n");
    return;
  }

  switch(targetFaces) {
    case 250:
      on_target250faces_clicked();
      break;
    case 500:
      on_target500faces_clicked();
      break;
    case 1000:
      on_target1000faces_clicked();
      break;
    case 5000:
      on_target5000faces_clicked();
      break;
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

    on_exportCurrentOBJ_clicked();
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

    on_exportCurrentOBJ_clicked();
  }
}

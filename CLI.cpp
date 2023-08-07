#include "Mainwindow.h"

void Mainwindow::exportDisplacedSamples(int nSamples, double minVal, double maxVal)
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

#include "Headers/Mesh.h"

void Mesh::setInitialEdgeSubdivisionLevelsTest()
{
  for (Edge &e : edges) e.subdivisions = 0;
  edges.at(0).subdivisions = 6;
}

void Mesh::sanityCheckEdge()
{
  for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
    Edge e = edges[edgeIndex];

    for (int edgeSide = 0; edgeSide < 2; edgeSide++) {
      int faceIndex = e.faces[edgeSide];
      int faceSide = e.side[edgeSide];
      assert(faces[faceIndex].edgesIndices[faceSide] == edgeIndex);
    }
  }

  for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++) {
    Face f = faces[faceIndex];

    for (int faceEdge = 0; faceEdge < 3; faceEdge++) {
      int edgeIndex = f.edgesIndices[faceEdge];
      Edge e = edges[edgeIndex];
      assert(e.faces[0] == faceIndex || e.faces[1] == faceIndex);
    }
  }
}

bool Mesh::isMicromeshScheme() const
{
  int correctFaces = 0;

  for (const Face &f : faces) {
    int i = edges.at(f.edgesIndices[0]).subdivisions;
    int j = edges.at(f.edgesIndices[0]).subdivisions;
    int k = edges.at(f.edgesIndices[0]).subdivisions;

    int totalDelta = std::abs(i - j) + std::abs(j - k) + std::abs(i - k);

    if (totalDelta <= 2) correctFaces++;
  }

  return correctFaces == faces.size();
}

bool Mesh::isValid() const
{
  return !vertices.empty() && !faces.empty();
}

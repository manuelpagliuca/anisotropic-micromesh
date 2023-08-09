#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_mainwindow.h"

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <QtOpenGL>
#include <QDebug>
#include <QtOpenGLWidgets>
#include <QFileDialog>
#include <iostream>
#include <string>

#include "Mesh.h"

class Mainwindow : public QMainWindow
{
  Q_OBJECT

public:
  Mainwindow(QWidget *parent = nullptr);
  ~Mainwindow() {}
  void loadDemoMesh();

public:
  // Command Line functions
  void exportDisplacedSamplesSameTargetEdgeMetric(int nSamples = 5, double minEdge = 1.0, double maxEdge = 10.0);
  void exportDisplacedSamplesWithSameFacesAmount(double minEdge = 1.0, double maxEdge = 10.0);

  void loadBaseMesh(const QString &fileName);
  void loadTargetMesh(const QString &fileName);

protected:
  void mousePressEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void wheelEvent(QWheelEvent *ev);
  void keyPressEvent(QKeyEvent *ev);

  void initUI();
  void setTargetAndResetDisplacementSlider(const Mesh &targetMesh);
  void setBaseMeshAndUI(const Mesh &mesh);
  void disableSubdivisionsBox();
  void disableEdgeLengthSlider();
  void disableDisplacementSlider();
  void resetSubdividedMeshLabels();
  void resetTargetMeshLabels();
  void findTargetEdgeLengthCombinations();
  void setDisplacementsDelta(std::vector<float> displacements);

  std::string extractFileName(const std::string& fullPath);

private:
  Ui::mainWindowClass ui;
  std::string readFile(const char *file_loc);

  // Mesh loading
  Mesh baseMesh, subdividedMesh, targetMesh, projectedMesh, swapMesh;
  std::string baseMeshNameAndDetail;
  float edgeLengthCurrentValue;

  // Mouse Trackball
  QPoint startPos;
  QPoint newPos;

  // Morphing
  bool isAniso = false;
  std::vector<float> displacementsDeltas;
  int morphingCurrentValue;

public slots:
  void on_actionLoad_triggered();
  void on_actionUnload_triggered();
  void on_actionSave_triggered();
  void on_actionExit_triggered();

  void on_actionWireframe_triggered();
  void on_actionVertex_displacement_triggered();
  void on_actionFace_displacement_triggered();
  void on_actionExtract_displacements_triggered();
  void on_actionMidpoint_subdivision_triggered();
  void on_actionUniform_subdivision_triggered();

  void on_demo125faces_clicked();
  void on_demo250faces_clicked();
  void on_demo500faces_clicked();
  void on_demo1000faces_clicked();

  void on_wireframeToggle_stateChanged();
  void on_micromesh_subdivision_clicked();
  void on_anisotropic_micromesh_subdivision_clicked();

  void on_edgeLengthSlider_valueChanged(int value);
  void on_displacementSlider_valueChanged(int value);

  void on_target250faces_clicked();
  void on_target500faces_clicked();
  void on_target1000faces_clicked();
  void on_target2500faces_clicked();
  void on_target5000faces_clicked();

  void on_exportCurrentOBJ_clicked();
  void on_exportCurrentOFF_clicked();

private slots:
  void on_loadBaseMesh_clicked();
  void on_loadTargetMesh_clicked();
};

#endif // MAIN_WINDOW_H

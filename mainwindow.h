#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_mainwindow.h"

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <QtOpenGL>
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

protected:
  void mousePressEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void wheelEvent(QWheelEvent *ev);
  void keyPressEvent(QKeyEvent *ev);

  void initUI();
  void setTargetMeshAndResetSlider(const Mesh &targetMesh);
  void setBaseMeshAndUI(const Mesh &mesh);
  void updateBaseMeshAndDisableSubdivisionsBox();
  void setDisplacementsDelta(std::vector<float> displacements);
  int extractPolyDetails(const std::string &str);

private:
  Ui::mainWindowClass ui;
  std::string readFile(const char *file_loc);

  int polyBaseMesh, polyTargetMesh;

  // Mesh loading
  Mesh baseMesh;

  // Mouse Trackball
  QPoint startPos;
  QPoint newPos;

  // Morphing
  Mesh targetMesh, projectedMesh;
  std::vector<float> displacementsDeltas;
  int morphingCurrentValue;

public slots:
  void on_actionLoad_triggered();
  void on_actionUnload_triggered();
  void on_actionSave_triggered();
  void on_actionExit_triggered();

  void on_actionWireframe_triggered();
  void on_actionSubdivide_triggered();
  void on_actionVertex_displacement_triggered();
  void on_actionFace_displacement_triggered();
  void on_actionExtract_displacements_triggered();
  void on_actionSubdivision_surfaces_Uniform_triggered();
  void on_actionSubdivision_surfaces_Adaptive_triggered();
  void on_actionSubdivision_surfaces_Micromesh_triggered();

  void on_demo125faces_clicked();
  void on_demo250faces_clicked();
  void on_demo500faces_clicked();
  void on_demo1000faces_clicked();
  void on_checkBox_stateChanged();
  void on_midpoint_subdivision_clicked();
  void on_uniform_subdivision_clicked();
  void on_micromesh_subdivision_clicked();
  void on_anisotropic_micromesh_subdivision_clicked();
  void on_horizontalSlider_valueChanged(int value);
  void on_morph250faces_clicked();
  void on_morph500faces_clicked();
  void on_morph1000faces_clicked();
  void on_morph2500faces_clicked();
  void on_morph5000faces_clicked();
  void on_reloadShadersButton_clicked();
  void on_exportCurrentOBJ_clicked();
  void on_exportCurrentOFF_clicked();

private slots:
  void on_loadBaseMesh_clicked();
  void on_loadTargetMesh_clicked();
};

#endif // MAIN_WINDOW_H

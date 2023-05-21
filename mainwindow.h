#pragma once

#include "ui_mainwindow.h"

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <QtOpenGL>
#include <QtOpenGLWidgets>
#include <QFileDialog>

#include "Mesh.h"
#include "morphdialog.h"

class Mainwindow : public QMainWindow
{
  Q_OBJECT

public:
  Mainwindow(QWidget *parent = nullptr);
  ~Mainwindow(){};
  void loadDemoMesh();

protected:
  void mousePressEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void wheelEvent(QWheelEvent *ev);
  void keyPressEvent(QKeyEvent *ev);

private:
  Ui::mainWindowClass ui;
  std::string readFile(const char *file_loc);
  MorphDialog *morphDialog;

  // Mesh loading
  Mesh baseMesh;
  Mesh targetMesh;

  // Mouse Trackball
  QPoint startPos;
  QPoint newPos;

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
private slots:
  void on_checkBox_stateChanged(int arg1);
  void on_midpoint_subdivision_clicked();
  void on_uniform_subdivision_clicked();
  void on_micromesh_subdivision_clicked();
  void on_anisotropic_micromesh_subdivision_clicked();
  void on_horizontalSlider_sliderMoved(int position);
  void on_horizontalSlider_valueChanged(int value);
  void on_vertexDisplacementSlider_valueChanged(int value);
  void on_vertexDisplacementSlider_rangeChanged(int min, int max);
  void on_faceDisplacementSlider_sliderMoved(int position);
  void on_faceDisplacementSlider_valueChanged(int value);
};

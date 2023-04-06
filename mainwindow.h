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
    Mainwindow(QWidget* parent = nullptr);
    ~Mainwindow() {};
    void loadDemoMesh();
protected:
    void mousePressEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* ev);
    void wheelEvent(QWheelEvent* ev);
    void keyPressEvent(QKeyEvent* ev);

private:
    Ui::mainWindowClass ui;
	std::string readFile(const char* file_loc);
    MorphDialog *morphDialog;

    // Mesh loading
    Mesh baseMesh;
    Mesh targetMesh;

    // Mouse Trackball
    QPoint startPos;
    QPoint newPos;

public slots: void on_actionLoad_triggered();
public slots: void on_actionUnload_triggered();
public slots: void on_actionSave_triggered();
public slots: void on_actionExit_triggered();

public slots: void on_actionWireframe_triggered();
public slots: void on_actionSubdivide_triggered();
public slots: void on_actionVertex_displacement_triggered();
public slots: void on_actionFace_displacement_triggered();
public slots: void on_actionExtract_displacements_triggered();
public slots: void on_actionSubdivision_surfaces_Uniform_triggered();
};

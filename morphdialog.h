#ifndef MORPHDIALOG_H
#define MORPHDIALOG_H

#include <QDialog>
#include "Mesh.h"
#include "GLWidget.h"

namespace Ui
{
  class MorphWindows;
}

class MorphDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MorphDialog(QWidget *parent = nullptr);
  void setMesh(Mesh &mesh);
  void setGLWidget(GLWidget *glWidget);
  void setDisplacementsDelta(std::map<int, float> displacements);
  ~MorphDialog();

private slots:
  void on_horizontalSlider_valueChanged(int value);

private:
  Ui::MorphWindows *ui;
  Mesh baseMesh, targetMesh;
  GLWidget *glWidget;
  std::map<int, float> displacementsDeltas;
  int previousValue, currentValue;
};

#endif // MORPHDIALOG_H

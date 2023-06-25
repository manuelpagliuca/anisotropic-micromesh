#include "morphdialog.h"
#include "ui_morphdialog.h"

MorphDialog::MorphDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MorphWindows)
{
  ui->setupUi(this);
  ui->horizontalSlider->setMaximum(99);
  ui->horizontalSlider->setMinimum(0);
  ui->horizontalSlider->setValue(0);
  previousValue = 0;
}

void MorphDialog::setMesh(Mesh &baseMesh)
{
  this->baseMesh = baseMesh;
}

void MorphDialog::setGLWidget(GLWidget *glWidget)
{
  this->glWidget = glWidget;
}

void MorphDialog::setDisplacementsDelta(std::map<int, float> displacements)
{
  displacementsDeltas = displacements;
}

MorphDialog::~MorphDialog()
{
  delete ui;
}

void MorphDialog::on_horizontalSlider_valueChanged(int value)
{
  previousValue = value;
  targetMesh = baseMesh;

  for (const auto &e : displacementsDeltas) {
    auto &[index, t] = e;
    targetMesh.displaceVertex(index, t * value);
  }

  glWidget->updateMeshData(targetMesh);
}

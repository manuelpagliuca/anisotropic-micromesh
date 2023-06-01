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

void MorphDialog::setDisplacementsDelta(std::vector<std::tuple<int, float>> displacements)
{
  displacementsDeltas = displacements;

  for (auto &e : displacementsDeltas) {
    auto &[index, t] = e;
    t = t / 100.f;
  }
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

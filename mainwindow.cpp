#include "Mainwindow.h"

Mainwindow::Mainwindow(QWidget *parent) : QMainWindow(parent)
{
  ui.setupUi(this);
  const int x = (screen()->size().width() / 2) - (width() / 2);
  const int y = (screen()->size().height() / 2) - (height() / 2);
  move(x, y);

  ui.actionSave->setEnabled(false);
  ui.actionSubdivide->setEnabled(false);
  ui.actionUnload->setEnabled(false);
  ui.actionWireframe->setEnabled(false);
  ui.actionVertex_displacement->setEnabled(false);
  ui.horizontalSlider->setEnabled(false);
  ui.horizontalSlider->setMaximum(99);
  ui.horizontalSlider->setMinimum(0);
  ui.horizontalSlider->setValue(0);
  ui.checkBox->toggle();
  previousValue = 0;
}

void Mainwindow::setDisplacementsDelta(std::vector<std::tuple<int, float>> displacements)
{
  displacementsDeltas = displacements;

  for (auto &e : displacementsDeltas)
  {
    auto &[index, t] = e;
    t = t / 100.f;
  }
}

void Mainwindow::mousePressEvent(QMouseEvent *ev)
{
  ui.openGLWidget->trackBall.setFistClick(ev->pos());
}

void Mainwindow::mouseMoveEvent(QMouseEvent *ev)
{
  ui.openGLWidget->trackBall.trackMousePositions(
    ev->pos(),
    ui.openGLWidget->width(),
    ui.openGLWidget->height());
}

void Mainwindow::wheelEvent(QWheelEvent *ev)
{
  ui.openGLWidget->trackBall.trackWheelIncrement(ev->angleDelta().y());
}

void Mainwindow::keyPressEvent(QKeyEvent *ev)
{
  if (ev->key() == Qt::Key_Escape) exit(1);
  else if (ev->key() == Qt::Key_W) ui.checkBox->toggle();
  else if (ev->key() == Qt::Key_E) on_actionExtract_displacements_triggered();
  else if (ev->key() == Qt::Key_S) on_actionSubdivide_triggered();
  else if (ev->key() == Qt::Key_O) on_actionSubdivision_surfaces_Uniform_triggered();
  else if (ev->key() == Qt::Key_L) on_actionLoad_triggered();
  else if (ev->key() == Qt::Key_U) on_actionUnload_triggered();
  else if (ev->key() == Qt::Key_R) ui.openGLWidget->reloadShaders();
  else if (ev->key() == Qt::Key_1) on_demo125faces_clicked();
  else if (ev->key() == Qt::Key_2) on_demo250faces_clicked();
  else if (ev->key() == Qt::Key_3) on_demo500faces_clicked();
  else if (ev->key() == Qt::Key_4) on_demo1000faces_clicked();
}

void Mainwindow::on_actionSave_triggered()
{
  QString filePath = QFileDialog::getSaveFileName(this,
                                                  tr("Save Mesh"), ".\\mesh",
                                                  tr("OFF File (*.off);;OBJ File (*.obj)"));

  if (!filePath.isEmpty())
  {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string fileNameExt = filePath.mid(filePath.lastIndexOf("/")).toStdString();
    std::string fileName = fileNameExt.substr(1, fileNameExt.size() - 5);

    if (ext == ".off") baseMesh.exportOFF(fileName);
    else if (ext == ".obj") baseMesh.exportOBJ(fileName);
  }
}

void Mainwindow::on_actionLoad_triggered()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), ".\\mesh",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);; OBJ Files (*.obj)"));

  if (!filePath.isEmpty())
  {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());
    baseMesh = Mesh();
    if (ext == ".off")
    {
      baseMesh = Mesh::parseOFF(file);
      ui.openGLWidget->loadMeshData(baseMesh);
      ui.actionSave->setEnabled(true);
      ui.actionSubdivide->setEnabled(true);
      ui.actionWireframe->setEnabled(true);
      ui.actionUnload->setEnabled(true);
      ui.actionVertex_displacement->setEnabled(true);
    }
    else if (ext == ".obj")
    {
      baseMesh = Mesh::parseOBJ(file);
      ui.openGLWidget->loadMeshData(baseMesh);
      ui.actionSave->setEnabled(true);
      ui.actionSubdivide->setEnabled(true);
      ui.actionWireframe->setEnabled(true);
      ui.actionUnload->setEnabled(true);
      ui.actionVertex_displacement->setEnabled(true);
    }
  }
}

void Mainwindow::on_actionVertex_displacement_triggered()
{
  bool isValid;
  double k = QInputDialog::getDouble(
    this,
    tr("Insert vertex displacement factor"),
    tr("Amount:"), 0.0, -3.0, 5.0, 4, &isValid,
    this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  if (isValid)
  {
    baseMesh.displaceVertices(k);
    ui.openGLWidget->updateMeshData(baseMesh);
  }
}

void Mainwindow::on_actionFace_displacement_triggered()
{
  bool isValid;
  double k = QInputDialog::getDouble(
      this,
      tr("Insert face displacement factor"),
      tr("Amount:"), 0.0, -3.0, 5.0, 4, &isValid,
      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  if (isValid)
  {
    baseMesh.displaceFaces(k);
    ui.openGLWidget->updateMeshData(baseMesh);
  }
}

void Mainwindow::on_actionExtract_displacements_triggered()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), ".\\mesh",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

  if (!filePath.isEmpty())
  {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());

    Mesh targetMesh;

    if (ext == ".off") targetMesh = Mesh::parseOFF(file);
    else if (ext == ".obj") targetMesh = Mesh::parseOBJ(file);

    bool isValid;
    int k = QInputDialog::getInt(
      this,
      tr("Insert the number of subdivision to perform"),
      tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    if (isValid)
    {
      for (int i = 0; i < k; i++) baseMesh = baseMesh.subdivide();
      ui.openGLWidget->updateMeshData(baseMesh);
    }

    Mesh tmpMesh = baseMesh;
    auto displacements = tmpMesh.displaceVerticesTowardsTargetMesh(targetMesh);

    morphDialog = new MorphDialog(this);
    morphDialog->show();
    morphDialog->setMesh(baseMesh);
    morphDialog->setGLWidget(ui.openGLWidget);
    morphDialog->setDisplacementsDelta(displacements);
  }
}

void Mainwindow::on_actionUnload_triggered()
{
  ui.openGLWidget->unloadMeshData();
  ui.actionSave->setEnabled(false);
  ui.actionSubdivide->setEnabled(false);
  ui.actionUnload->setEnabled(false);
  ui.actionWireframe->setEnabled(false);
  ui.actionVertex_displacement->setEnabled(false);
}

void Mainwindow::on_actionWireframe_triggered()
{
  ui.checkBox->toggle();
}

void Mainwindow::on_actionSubdivide_triggered()
{
  baseMesh = baseMesh.subdivide();
  ui.openGLWidget->updateMeshData(baseMesh);
  ui.morphingGroupBox->setEnabled(true);
}

void Mainwindow::on_actionExit_triggered()
{
  exit(1);
}

std::string Mainwindow::readFile(const char *file_loc)
{
  std::string content;
  std::ifstream file_stream(file_loc, std::ios::in);

  if (!file_stream.is_open())
  {
    printf("Failed to read %s! File doesn't exist.", file_loc);
    return "";
  }

  std::string line = "";

  while (!file_stream.eof())
  {
    std::getline(file_stream, line);
    content.append(line + "\n");
  }

  file_stream.close();
  return content;
}

void Mainwindow::on_actionSubdivision_surfaces_Uniform_triggered()
{
  bool isValid;
  int subdivisions = QInputDialog::getInt(
                      this, tr("Insert the number of subdivision to perform"),
                      tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
                      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  if (isValid)
  {
    baseMesh = baseMesh.nSubdivide(subdivisions);
    ui.openGLWidget->updateMeshData(baseMesh);
    ui.openGLWidget->updateMeshData(baseMesh);
    ui.morphingGroupBox->setEnabled(true);
  }
}

void Mainwindow::on_actionSubdivision_surfaces_Adaptive_triggered()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), ".\\mesh",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

  if (!filePath.isEmpty())
  {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());

    Mesh targetMesh;

    if (ext == ".off") targetMesh = Mesh::parseOFF(file);
    else if (ext == ".obj") targetMesh = Mesh::parseOBJ(file);

    baseMesh = baseMesh.adaptiveSubdivide();
    ui.openGLWidget->updateMeshData(baseMesh);

    Mesh tmpMesh = baseMesh;
    auto displacements = tmpMesh.displaceVerticesTowardsTargetMesh(targetMesh);

    morphDialog = new MorphDialog(this);
    morphDialog->show();
    morphDialog->setMesh(baseMesh);
    morphDialog->setGLWidget(ui.openGLWidget);
    morphDialog->setDisplacementsDelta(displacements);
    ui.morphingGroupBox->setEnabled(true);
  }
}

void Mainwindow::on_actionSubdivision_surfaces_Micromesh_triggered()
{
  baseMesh = baseMesh.micromeshSubdivide();
  ui.openGLWidget->updateMeshData(baseMesh);
}

void Mainwindow::on_demo125faces_clicked()
{
  std::string pallasOBJ125 = readFile("./mesh/pallas_125.obj");
  baseMesh = Mesh::parseOBJ(pallasOBJ125);
  ui.openGLWidget->loadMeshData(baseMesh);
  ui.actionSave->setEnabled(true);
  ui.actionSubdivide->setEnabled(true);
  ui.actionWireframe->setEnabled(true);
  ui.actionUnload->setEnabled(true);
  ui.actionVertex_displacement->setEnabled(true);
  ui.morphingGroupBox->setEnabled(false);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(true);
}

void Mainwindow::on_demo250faces_clicked()
{
  std::string pallasOBJ250 = readFile("./mesh/pallas_250.obj");
  baseMesh = Mesh::parseOBJ(pallasOBJ250);
  ui.openGLWidget->loadMeshData(baseMesh);
  ui.actionSave->setEnabled(true);
  ui.actionSubdivide->setEnabled(true);
  ui.actionWireframe->setEnabled(true);
  ui.actionUnload->setEnabled(true);
  ui.actionVertex_displacement->setEnabled(true);
  ui.morphingGroupBox->setEnabled(false);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(true);
}

void Mainwindow::on_demo500faces_clicked()
{
  std::string pallasOBJ500 = readFile("./mesh/pallas_500.obj");
  baseMesh = Mesh::parseOBJ(pallasOBJ500);
  ui.openGLWidget->loadMeshData(baseMesh);
  ui.actionSave->setEnabled(true);
  ui.actionSubdivide->setEnabled(true);
  ui.actionWireframe->setEnabled(true);
  ui.actionUnload->setEnabled(true);
  ui.actionVertex_displacement->setEnabled(true);
  ui.morphingGroupBox->setEnabled(false);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(true);
}

void Mainwindow::on_demo1000faces_clicked()
{
  std::string pallasOBJ1000 = readFile("./mesh/pallas_1000.obj");
  baseMesh = Mesh::parseOBJ(pallasOBJ1000);
  ui.openGLWidget->loadMeshData(baseMesh);
  ui.actionSave->setEnabled(true);
  ui.actionSubdivide->setEnabled(true);
  ui.actionWireframe->setEnabled(true);
  ui.actionUnload->setEnabled(true);
  ui.actionVertex_displacement->setEnabled(true);
  ui.morphingGroupBox->setEnabled(false);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(true);
}

void Mainwindow::on_midpoint_subdivision_clicked()
{
  baseMesh = baseMesh.subdivide();
  ui.openGLWidget->updateMeshData(baseMesh);
  ui.morphingGroupBox->setEnabled(true);
  ui.horizontalSlider->setEnabled(false);
  ui.horizontalSlider->setValue(0);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(false);
}

void Mainwindow::on_uniform_subdivision_clicked()
{
  bool isValid;
  int subdivisions = QInputDialog::getInt(
                      this, tr("Insert the number of subdivision to perform"),
                      tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
                      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  if (isValid)
  {
    baseMesh = baseMesh.nSubdivide(subdivisions);
    ui.openGLWidget->updateMeshData(baseMesh);
    ui.morphingGroupBox->setEnabled(true);
    ui.horizontalSlider->setEnabled(false);
    ui.horizontalSlider->setValue(0);
    previousValue = 0;
    ui.subdivisionsGroupBox->setEnabled(false);
  }
}

void Mainwindow::on_micromesh_subdivision_clicked()
{
  baseMesh = baseMesh.adaptiveSubdivide();
  ui.openGLWidget->updateMeshData(baseMesh);
  ui.morphingGroupBox->setEnabled(true);
  ui.horizontalSlider->setEnabled(false);
  ui.horizontalSlider->setValue(0);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(false);
}

void Mainwindow::on_anisotropic_micromesh_subdivision_clicked()
{
  baseMesh = baseMesh.anisotropicMicromeshSubdivide();
  ui.openGLWidget->updateMeshData(baseMesh);
  ui.horizontalSlider->setEnabled(false);
  ui.morphingGroupBox->setEnabled(true);
  ui.horizontalSlider->setValue(0);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(false);
}

void Mainwindow::on_morph250faces_clicked()
{
  std::string pallasOBJ250 = readFile("./mesh/pallas_250.obj");
  Mesh targetMesh = Mesh::parseOBJ(pallasOBJ250);
  setDisplacementsDelta(baseMesh.displaceVerticesTowardsTargetMesh(targetMesh));
  ui.horizontalSlider->setEnabled(true);
  if (previousValue != 0) {
    ui.horizontalSlider->setValue(0);
    previousValue = 0;
    ui.openGLWidget->updateMeshData(baseMesh);
  }
}

void Mainwindow::on_morph500faces_clicked()
{
  std::string pallasOBJ500 = readFile("./mesh/pallas_500.obj");
  Mesh targetMesh = Mesh::parseOBJ(pallasOBJ500);
  setDisplacementsDelta(baseMesh.displaceVerticesTowardsTargetMesh(targetMesh));
  ui.horizontalSlider->setEnabled(true);
  ui.horizontalSlider->setValue(0);
  previousValue = 0;
}

void Mainwindow::on_morph1000faces_clicked()
{
  std::string pallasOBJ1000 = readFile("./mesh/pallas_1000.obj");
  Mesh targetMesh = Mesh::parseOBJ(pallasOBJ1000);
  setDisplacementsDelta(baseMesh.displaceVerticesTowardsTargetMesh(targetMesh));
  ui.horizontalSlider->setEnabled(true);
  ui.horizontalSlider->setValue(0);
  previousValue = 0;
}

void Mainwindow::on_morph2500faces_clicked()
{
  std::string pallasOBJ2500 = readFile("./mesh/pallas_2500.obj");
  Mesh targetMesh = Mesh ::parseOBJ(pallasOBJ2500);
  setDisplacementsDelta(baseMesh.displaceVerticesTowardsTargetMesh(targetMesh));
  ui.horizontalSlider->setEnabled(true);

  ui.horizontalSlider->setValue(0);
  previousValue = 0;
  ui.subdivisionsGroupBox->setEnabled(false);
}

void Mainwindow::on_morph5000faces_clicked()
{
  std::string pallasOBJ5000 = readFile("./mesh/pallas_5000.obj");
  Mesh targetMesh = Mesh::parseOBJ(pallasOBJ5000);
  setDisplacementsDelta(baseMesh.displaceVerticesTowardsTargetMesh(targetMesh));
  ui.horizontalSlider->setEnabled(true);
  ui.horizontalSlider->setValue(0);
  previousValue = 0;
}

void Mainwindow::on_horizontalSlider_valueChanged(int value) {
  if (value == 0) return;

  previousValue = value;

  Mesh targetMesh = baseMesh;

  for (const auto &e : displacementsDeltas)
  {
    auto &[index, t] = e;
    targetMesh.displaceVertex(index, float(t * value));
  }

  ui.openGLWidget->updateMeshData(targetMesh);
}

void Mainwindow::on_reloadShadersButton_clicked()
{
  ui.openGLWidget->reloadShaders();
}

void Mainwindow::on_checkBox_stateChanged(int arg1)
{
  ui.openGLWidget->wireframePaint();
}

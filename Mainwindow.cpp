#include "Mainwindow.h"

Mainwindow::Mainwindow(QWidget *parent) : QMainWindow(parent)
{
  ui.setupUi(this);
  const int x = (screen()->size().width() / 2) - (width() / 2);
  const int y = (screen()->size().height() / 2) - (height() / 2);
  move(x, y);
  initUI();
}

void Mainwindow::initUI()
{
  ui.actionSave->setEnabled(false);
  ui.actionSubdivide->setEnabled(false);
  ui.actionUnload->setEnabled(false);
  ui.actionWireframe->setEnabled(false);
  ui.actionVertex_displacement->setEnabled(false);

  ui.edgeLengthSlider->setEnabled(true);
  int minIntValue = 0;    // 0.1
  int maxIntValue = 1000; // 10.0
  ui.edgeLengthSlider->setRange(minIntValue, maxIntValue);
  double decimalPrecision = 0.01;
  int numDecimalValues = (maxIntValue - minIntValue) / (decimalPrecision * 100);
  ui.edgeLengthSlider->setSingleStep(numDecimalValues);
  ui.edgeLengthSlider->setValue(100); // 1.0

  ui.horizontalSlider->setEnabled(false);
  ui.horizontalSlider->setMaximum(100);
  ui.horizontalSlider->setMinimum(0);
  ui.horizontalSlider->setValue(0);

  ui.exportCurrentOBJ->setEnabled(false);
  ui.exportCurrentOFF->setEnabled(false);

  ui.checkBox->toggle();
}

void Mainwindow::setTargetMeshAndResetSlider(const Mesh &target)
{
  bool baseMeshIsDisplaced = !displacementsDeltas.empty();

  if (baseMeshIsDisplaced)
    ui.openGLWidget->updateMeshData(subdividedMesh);

  setDisplacementsDelta(subdividedMesh.getDisplacements(target));

  ui.horizontalSlider->setEnabled(true);
  ui.horizontalSlider->setValue(0);
  ui.exportCurrentOBJ->setEnabled(true);
  ui.exportCurrentOFF->setEnabled(true);
}

void Mainwindow::setBaseMeshAndUI(const Mesh &mesh)
{
  baseMesh = mesh;
  ui.openGLWidget->loadMeshData(baseMesh);
  ui.actionSave->setEnabled(true);
  ui.actionSubdivide->setEnabled(true);
  ui.actionWireframe->setEnabled(true);
  ui.actionUnload->setEnabled(true);
  ui.actionVertex_displacement->setEnabled(true);
  ui.morphingGroupBox->setEnabled(false);
  ui.subdivisionsGroupBox->setEnabled(true);
}

void Mainwindow::disableSubdivisionsBox()
{
  ui.subdivisionsGroupBox->setEnabled(false);
  ui.morphingGroupBox->setEnabled(true);
  ui.horizontalSlider->setEnabled(false);
  ui.horizontalSlider->setValue(0);
  ui.exportCurrentOBJ->setEnabled(false);
  ui.exportCurrentOFF->setEnabled(false);
}

void Mainwindow::setDisplacementsDelta(std::vector<float> displacements)
{
  displacementsDeltas = displacements;

  for (auto &disp : displacementsDeltas)
    disp = disp / 100.f;
}

int Mainwindow::extractPolyDetails(const std::string &str)
{
  size_t lastUnderscorePos = str.rfind("_");

  if (lastUnderscorePos != std::string::npos) {
    std::string numberString = str.substr(lastUnderscorePos + 1);
    return std::stoll(numberString);
  }

  return 0;
}

std::string Mainwindow::extractFileNameWithoutExtension(const std::string &fullPath)
{
  size_t lastSlashPos = fullPath.find_last_of("/\\");
  std::string fileName = fullPath.substr(lastSlashPos + 1);
  size_t lastDotPos = fileName.find_last_of(".");

  if (lastDotPos != std::string::npos) fileName = fileName.substr(0, lastDotPos);

  return fileName;
}

std::string Mainwindow::readFile(const char *file_loc)
{
  std::string content;
  std::ifstream file_stream(file_loc, std::ios::in);

  if (!file_stream.is_open()) {
    printf("Failed to read %s! File doesn't exist.", file_loc);
    return "";
  }

  std::string line = "";

  while (!file_stream.eof()) {
    std::getline(file_stream, line);
    content.append(line + "\n");
  }

  file_stream.close();
  return content;
}

void Mainwindow::mousePressEvent(QMouseEvent *ev)
{
  if (ui.openGLWidget->underMouse())
    ui.openGLWidget->trackBall.setFistClick(ev->pos());
}

void Mainwindow::mouseMoveEvent(QMouseEvent *ev)
{
  if (ui.openGLWidget->underMouse()) {
    ui.openGLWidget->trackBall.trackMousePositions(
      ev->pos(),
      ui.openGLWidget->width(),
      ui.openGLWidget->height());
  }
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
                                                  tr("Save Mesh"), "..\\Samples",
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
    tr("Load Mesh"), "..\\Samples",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);; OBJ Files (*.obj)"));

  if (!filePath.isEmpty()) {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());
    baseMesh = Mesh();

    if (ext == ".off") {
      setBaseMeshAndUI(Mesh::parseOFF(file));
    } else if (ext == ".obj") {
      setBaseMeshAndUI(Mesh::parseOBJ(file));
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

  if (isValid) {
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

  if (isValid) {
    baseMesh.displaceFaces(k);
    ui.openGLWidget->updateMeshData(baseMesh);
  }
}

void Mainwindow::on_actionExtract_displacements_triggered()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), "..\\Samples",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

  if (!filePath.isEmpty()) {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());

    if (ext == ".off") targetMesh = Mesh::parseOFF(file);
    else if (ext == ".obj") targetMesh = Mesh::parseOBJ(file);

    bool isValid;
    int k = QInputDialog::getInt(
      this,
      tr("Insert the number of subdivision to perform"),
      tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    if (isValid) {
      for (int i = 0; i < k; i++) baseMesh = baseMesh.subdivide();
      ui.openGLWidget->updateMeshData(baseMesh);
    }

    Mesh tmpMesh = baseMesh;
    auto displacements = tmpMesh.getDisplacements(targetMesh);
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

void Mainwindow::on_actionSubdivision_surfaces_Uniform_triggered()
{
  bool isValid;
  int subdivisions = QInputDialog::getInt(
                      this, tr("Insert the number of subdivision to perform"),
                      tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
                      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  if (isValid) {
    baseMesh = baseMesh.subdivideNtimes(subdivisions);
    ui.openGLWidget->updateMeshData(baseMesh);
    ui.morphingGroupBox->setEnabled(true);
  }
}

void Mainwindow::on_actionSubdivision_surfaces_Adaptive_triggered()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), "..\\Samples",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

  if (!filePath.isEmpty()) {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());

    if (ext == ".off") targetMesh = Mesh::parseOFF(file);
    else if (ext == ".obj") targetMesh = Mesh::parseOBJ(file);

    baseMesh.updateEdgesSubdivisionLevelsMicromesh(1.0f);
    baseMesh = baseMesh.micromeshSubdivide();
    ui.openGLWidget->updateMeshData(baseMesh);

    Mesh tmpMesh = baseMesh;
    auto displacements = tmpMesh.getDisplacements(targetMesh);
  }
}

void Mainwindow::on_actionSubdivision_surfaces_Micromesh_triggered()
{
  baseMesh.updateEdgesSubdivisionLevelsMicromesh(edgeLengthCurrentValue);
  baseMesh = baseMesh.micromeshSubdivide();
  ui.openGLWidget->updateMeshData(baseMesh);
}

void Mainwindow::on_demo125faces_clicked()
{
  std::string pallasOBJ125 = readFile("./Samples/pallas_125.obj");
  setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ125));
  baseMeshNameAndDetail = extractFileNameWithoutExtension("./Samples/pallas_125.obj");
}

void Mainwindow::on_demo250faces_clicked()
{
  std::string pallasOBJ250 = readFile("./Samples/pallas_250.obj");
  setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ250));
  baseMeshNameAndDetail = extractFileNameWithoutExtension("./Samples/pallas_250.obj");
}

void Mainwindow::on_demo500faces_clicked()
{
  std::string pallasOBJ500 = readFile("./Samples/pallas_500.obj");
  setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ500));
  baseMeshNameAndDetail = extractFileNameWithoutExtension("./Samples/pallas_500.obj");
}

void Mainwindow::on_demo1000faces_clicked()
{
  std::string pallasOBJ1000 = readFile("./Samples/pallas_1000.obj");
  setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ1000));
  baseMeshNameAndDetail = extractFileNameWithoutExtension("./Samples/pallas_1000.obj");
}

void Mainwindow::on_midpoint_subdivision_clicked()
{
  baseMesh = baseMesh.subdivide();
  disableSubdivisionsBox();
}

void Mainwindow::on_uniform_subdivision_clicked()
{
  bool isValid;
  int subdivisions = QInputDialog::getInt(
                      this, tr("Insert the number of subdivision to perform"),
                      tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
                      this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  if (isValid) {
    baseMesh = baseMesh.subdivideNtimes(subdivisions);
    disableSubdivisionsBox();
  }
}

void Mainwindow::on_micromesh_subdivision_clicked()
{
  isAniso = false;
  baseMesh.updateEdgesSubdivisionLevelsMicromesh(edgeLengthCurrentValue);
  subdividedMesh = baseMesh.micromeshSubdivide();
  ui.openGLWidget->updateMeshData(subdividedMesh);
//  disableSubdivisionsBox();
  ui.morphingGroupBox->setEnabled(true);

}

void Mainwindow::on_anisotropic_micromesh_subdivision_clicked()
{
  isAniso = true;
  baseMesh.updateEdgesSubdivisionLevelsAniso(1.0f);
  subdividedMesh = baseMesh.anisotropicMicromeshSubdivide();
  ui.openGLWidget->updateMeshData(subdividedMesh);
  ui.morphingGroupBox->setEnabled(true);

//  disableSubdivisionsBox();
}

void Mainwindow::on_morph250faces_clicked()
{
  std::string pallasOBJ250 = readFile("./Samples/pallas_250.obj");
  setTargetMeshAndResetSlider(Mesh::parseOBJ(pallasOBJ250));
  polyTargetMesh = 250;
}

void Mainwindow::on_morph500faces_clicked()
{
  std::string pallasOBJ500 = readFile("./Samples/pallas_500.obj");
  setTargetMeshAndResetSlider(Mesh::parseOBJ(pallasOBJ500));
  polyTargetMesh = 500;
}

void Mainwindow::on_morph1000faces_clicked()
{
  std::string pallasOBJ1000 = readFile("./Samples/pallas_1000.obj");
  setTargetMeshAndResetSlider(Mesh::parseOBJ(pallasOBJ1000));
  polyTargetMesh = 1000;
}

void Mainwindow::on_morph2500faces_clicked()
{
  std::string pallasOBJ2500 = readFile("./Samples/pallas_2500.obj");
  setTargetMeshAndResetSlider(Mesh ::parseOBJ(pallasOBJ2500));
  polyTargetMesh = 2500;
}

void Mainwindow::on_morph5000faces_clicked()
{
  std::string pallasOBJ5000 = readFile("./Samples/pallas_5000.obj");
  setTargetMeshAndResetSlider(Mesh::parseOBJ(pallasOBJ5000));
  polyTargetMesh = 5000;
}

void Mainwindow::on_horizontalSlider_valueChanged(int value) {
  morphingCurrentValue = value;
  ui.displacementCurrentValue->setText(std::to_string(value).c_str());

  if (value == 0) return;

  projectedMesh = subdividedMesh;

  int vertexIdx = 0;

  for (const auto &disp : displacementsDeltas)
    projectedMesh.displaceVertex(vertexIdx++, disp * float(value));

  ui.openGLWidget->updateMeshData(projectedMesh);
}

void Mainwindow::on_edgeLengthSlider_valueChanged(int value)
{
  edgeLengthCurrentValue = 0.01f * value;
  ui.lengthCurrentValue->setText(std::to_string(edgeLengthCurrentValue).c_str());
}

void Mainwindow::on_reloadShadersButton_clicked()
{
  ui.openGLWidget->reloadShaders();
}

void Mainwindow::on_checkBox_stateChanged()
{
  ui.openGLWidget->wireframePaint();
}

void Mainwindow::on_loadBaseMesh_clicked()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), ".\\Samples",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);; OBJ Files (*.obj)"));

  if (!filePath.isEmpty()) {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());
    baseMeshNameAndDetail = extractFileNameWithoutExtension(filePath.toStdString());

    baseMesh = Mesh();

    if (ext == ".off")
      baseMesh = Mesh::parseOFF(file);
    else if (ext == ".obj")
      baseMesh = Mesh::parseOBJ(file);
  }
}

void Mainwindow::on_loadTargetMesh_clicked()
{
  QString filePath = QFileDialog::getOpenFileName(
    this,
    tr("Load Mesh"), ".\\Samples",
    tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

  if (!filePath.isEmpty()) {
    std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
    std::string file = readFile(filePath.toStdString().c_str());

    if (ext == ".off")
      targetMesh = Mesh::parseOFF(file);
    else if (ext == ".obj")
      targetMesh = Mesh::parseOBJ(file);

    polyTargetMesh = extractPolyDetails(file);
    setDisplacementsDelta(baseMesh.getDisplacements(targetMesh));
    ui.horizontalSlider->setEnabled(true);
    ui.horizontalSlider->setValue(0);
    ui.subdivisionsGroupBox->setEnabled(false);
  }
}

void Mainwindow::on_exportCurrentOBJ_clicked()
{
  std::ostringstream fileNameStream;
  fileNameStream << "displaced\\" << baseMeshNameAndDetail << "to" << polyTargetMesh << "at" << morphingCurrentValue;
  std::string fileName = fileNameStream.str();

  if (isAniso) fileNameStream << "_aniso";

  projectedMesh.exportOBJ(fileName);
}

void Mainwindow::on_exportCurrentOFF_clicked()
{
  std::ostringstream fileNameStream;
  fileNameStream << "displaced\\" << baseMeshNameAndDetail << "to" << polyTargetMesh << "at" << morphingCurrentValue;

  if (isAniso) fileNameStream << "_aniso";

  std::string fileName = fileNameStream.str();

  projectedMesh.exportOFF(fileName);
}

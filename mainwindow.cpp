#include "Mainwindow.h"

Mainwindow::Mainwindow(QWidget* parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	int x = screen()->size().width() / 2 - width() / 2;
	int	y = screen()->size().height() / 2 - height() / 2;
	move(x, y);

	ui.actionSave->setEnabled(false);
	ui.actionSubdivide->setEnabled(false);
	ui.actionUnload->setEnabled(false);
	ui.actionWireframe->setEnabled(false);
    ui.actionVertex_displacement->setEnabled(false);
}

void Mainwindow::loadCube()
{
    std::string cube = readFile("./mesh/cube.off");
    mesh = Mesh::parseOFF(cube);
    ui.openGLWidget->loadMeshData(mesh);
    ui.actionSave->setEnabled(true);
    ui.actionSubdivide->setEnabled(true);
    ui.actionWireframe->setEnabled(true);
    ui.actionUnload->setEnabled(true);
    ui.actionVertex_displacement->setEnabled(true);
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
}

void Mainwindow::on_actionSave_triggered() {
    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Save Mesh"), ".\\mesh",
		tr("OFF File (*.off);;OBJ File (*.obj)"));

    if (!filePath.isEmpty()) {
        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string fileNameExt = filePath.mid(filePath.lastIndexOf("/")).toStdString();
        std::string fileName = fileNameExt.substr(1, fileNameExt.size() - 5);

        if (ext == ".off") mesh.exportOFF(fileName);
        else if (ext == ".obj") mesh.exportOBJ(fileName);
	}
}

void Mainwindow::on_actionLoad_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Load Mesh"), ".\\mesh",
		tr("3D Mesh(*.off *.obj);;OFF Files (*.off);; OBJ Files (*.obj)"));
    if (!filePath.isEmpty()) {
        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string file = readFile(filePath.toStdString().c_str());

		if (ext == ".off") {
            mesh = Mesh::parseOFF(file);
            ui.openGLWidget->loadMeshData(mesh);
			ui.actionSave->setEnabled(true);
			ui.actionSubdivide->setEnabled(true);
			ui.actionWireframe->setEnabled(true);
			ui.actionUnload->setEnabled(true);
			ui.actionVertex_displacement->setEnabled(true);
		}
		else if (ext == ".obj") {
            mesh = Mesh::parseOBJ(file);
            ui.openGLWidget->loadMeshData(mesh);
			ui.actionSave->setEnabled(true);
			ui.actionSubdivide->setEnabled(true);
			ui.actionWireframe->setEnabled(true);
			ui.actionUnload->setEnabled(true);
			ui.actionVertex_displacement->setEnabled(true);
		}
	}
}

void Mainwindow::on_actionVertex_displacement_triggered() {
	bool valid;
	double k = QInputDialog::getDouble(this, tr("Insert vertex displacement factor"),
        tr("Amount:"), 0.0, -3.0, 5.0, 4, &valid);

	if (valid) {
        mesh.displaceVertices(k);
        ui.openGLWidget->updateMeshData(mesh);
	}
}

void Mainwindow::on_actionFace_displacement_triggered() {
	bool valid;
	double k = QInputDialog::getDouble(this,
		tr("Insert face displacement factor"),
        tr("Amount:"), 0.0, -3.0, 5.0, 4, &valid);

	if (valid) {
        mesh.displaceFace(k);
        ui.openGLWidget->updateMeshData(mesh);
	}
}

void Mainwindow::on_actionUnload_triggered() {
    ui.openGLWidget->unloadMeshData();
	ui.actionSave->setEnabled(false);
	ui.actionSubdivide->setEnabled(false);
	ui.actionUnload->setEnabled(false);
	ui.actionWireframe->setEnabled(false);
	ui.actionVertex_displacement->setEnabled(false);
}

void Mainwindow::on_actionWireframe_triggered() {
    ui.openGLWidget->wireframePaint();
}

void Mainwindow::on_actionSubdivide_triggered() {
	mesh = mesh.subdivide();
    ui.openGLWidget->updateMeshData(mesh);
}

void Mainwindow::on_actionExit_triggered() {
	exit(1);
}

std::string Mainwindow::readFile(const char* file_loc)
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

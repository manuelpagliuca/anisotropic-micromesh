#include "Sources/Mainwindow.h"
#include <omp.h>

Mainwindow::Mainwindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(QString("Master's Degree Final Project, Manuel Pagliuca (975169), A.Y. 2022/2023"));
    const int x = (screen()->size().width() / 2) - (width() / 2);
    const int y = (screen()->size().height() / 2) - (height() / 2);
    move(x, y);
    initUI();
}

void Mainwindow::initUI()
{
    ui.actionSubdivide->setEnabled(false);
    ui.actionUnload->setEnabled(false);
    ui.actionWireframe->setEnabled(false);
    ui.actionVertex_displacement->setEnabled(false);
    ui.exportCurrentOBJ->setEnabled(false);
    ui.exportCurrentOFF->setEnabled(false);
    ui.actionSubdivided_mesh->setEnabled(false);
    ui.actionProjected_mesh->setEnabled(false);

    ui.wireframeToggle->toggle();

    // disable microfaces slider
    ui.microFacesSlider->setEnabled(false);

    // init displacements slider
    ui.displacementSlider->setEnabled(false);
    ui.displacementSlider->setMaximum(100);
    ui.displacementSlider->setMinimum(0);
    ui.displacementSlider->setValue(0);
}

void Mainwindow::setTargetAndResetDisplacementSlider(const Mesh &target)
{
    targetMesh = target;
    bool baseMeshIsDisplaced = !displacementsDeltas.empty();

    if (baseMeshIsDisplaced)
    {
        ui.openGLWidget->updateMeshData(subdividedMesh);
        ui.currentMeshLabel->setText("Subdivided mesh");
    }

    setDisplacementsDelta(subdividedMesh.getDisplacements(targetMesh));

    ui.displacementSlider->setEnabled(true);
    ui.displacementSlider->setValue(0);
    ui.exportCurrentOBJ->setEnabled(true);
    ui.exportCurrentOFF->setEnabled(true);
}

void Mainwindow::setBaseMeshAndUI(const Mesh &mesh)
{
    baseMesh = mesh;
    ui.openGLWidget->loadMeshData(baseMesh);
    ui.actionSubdivide->setEnabled(true);
    ui.actionWireframe->setEnabled(true);
    ui.actionUnload->setEnabled(true);
    ui.actionVertex_displacement->setEnabled(true);
    ui.morphingGroupBox->setEnabled(false);
    ui.subdivisionsGroupBox->setEnabled(true);
    ui.baseMeshFaces->setText(QString::number(baseMesh.faces.size()));
    ui.baseMeshVertices->setText(QString::number(baseMesh.vertices.size()));
}

void Mainwindow::setMicroFacesSliderRange()
{
    ui.microFacesSlider->setMaximum(GUI_MICROFACES_LIMIT);
    ui.microFacesSlider->setMinimum(int(baseMesh.faces.size()));
}

void Mainwindow::disableSubdivisionsBox()
{
    ui.subdivisionsGroupBox->setEnabled(false);
    ui.morphingGroupBox->setEnabled(true);
    ui.displacementSlider->setEnabled(false);
    ui.displacementSlider->setValue(0);
    ui.exportCurrentOBJ->setEnabled(false);
    ui.exportCurrentOFF->setEnabled(false);
}

void Mainwindow::disableDisplacementSlider()
{
    ui.displacementSlider->setEnabled(false);
    ui.displacementSlider->setValue(0);
    targetMesh = projectedMesh = Mesh();
}

void Mainwindow::resetSubdividedMeshLabels()
{
    ui.subdividedMeshFaces->setText("");
    ui.subdividedMeshVertices->setText("");
    ui.subdivisionSchemeLabel->setText("");
}

void Mainwindow::resetTargetMeshLabels()
{
    ui.targetMeshVertices->setText("");
    ui.targetMeshFaces->setText("");
}

void Mainwindow::findTargetEdgeLengthCombinations()
{
    const float maxEdgeLength = 5.0f;
    const float step = 0.01f;
    std::vector<float> matchingLengths;

    for (int i = 0; i < static_cast<int>((maxEdgeLength - 1.0f) / step); ++i)
    {
        float microEdgeLength = 1.0f + i * step;
        baseMesh.updateEdgesSubdivisionLevelsMicromesh(microEdgeLength);
        Mesh micromesh = baseMesh.subdivide();

        for (float anisoEdgeLength = 1.0f; anisoEdgeLength < maxEdgeLength; anisoEdgeLength += step)
        {
            baseMesh.updateEdgesSubdivisionLevelsAniso(anisoEdgeLength);
            Mesh anisoMicromesh = baseMesh.micromeshSubdivide();

            if (micromesh.faces.size() == anisoMicromesh.faces.size())
            {
                matchingLengths.push_back(microEdgeLength);
                matchingLengths.push_back(anisoEdgeLength);
                qDebug() << "Match: " << microEdgeLength << " " << anisoEdgeLength << "\n";
            }
            qDebug() << "Step: " << microEdgeLength << ", " << anisoEdgeLength << "\n";
        }
    }

    QFile file("matching_lengths.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << baseMeshNameAndDetail.c_str() << "\n"
            << "Micromesh | Aniso Micromesh\n";
        for (size_t i = 0; i < matchingLengths.size(); i += 2)
        {
            if (i + 1 < matchingLengths.size())
                out << matchingLengths[i] << " " << matchingLengths[i + 1] << "\n";
            else
                out << matchingLengths[i] << "\n";
        }

        file.close();
        qDebug() << "Vettore salvato correttamente in matching_lengths.txt";
    }
    else
        qWarning() << "Impossibile aprire il file per la scrittura";
}

void Mainwindow::setDisplacementsDelta(std::vector<float> displacements)
{
    displacementsDeltas = displacements;

    for (auto &disp : displacementsDeltas)
        disp = disp / 100.f;
}

void Mainwindow::loadBaseMesh(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        projectedMesh = subdividedMesh = Mesh();

        QString filePath = "./Models/" + fileName;
        std::string ext = fileName.mid(fileName.lastIndexOf(".")).toStdString();
        std::string file = readFile(filePath.toStdString().c_str());

        if (ext == ".off") setBaseMeshAndUI(Mesh::parseOFF(file));
        else if (ext == ".obj") setBaseMeshAndUI(Mesh::parseOBJ(file));

        baseMeshNameAndDetail = extractFileName(fileName.toStdString());

        qDebug() << fileName << " has been loaded as base mesh.";

        ui.baseMeshVertices->setText(std::to_string(baseMesh.vertices.size()).c_str());
        ui.baseMeshFaces->setText(std::to_string(baseMesh.faces.size()).c_str());
    }
}

void Mainwindow::loadTmpMesh(const QString &filePath)
{
    if (!filePath.isEmpty())
    {
        qDebug() << filePath;
        std::string file = readFile(filePath.toStdString().c_str());

        tmpMesh = Mesh::parseOBJ(file);
        qDebug() << "tmpMesh has been loaded as base mesh.";

        qDebug() << tmpMesh.faces.size();
    }
}

void Mainwindow::loadTargetMesh(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        QString filePath = "./Models/" + fileName;
        std::string ext = fileName.mid(fileName.lastIndexOf(".")).toStdString();
        std::string file = readFile(filePath.toStdString().c_str());

        if (ext == ".off")
            setTargetAndResetDisplacementSlider(Mesh::parseOFF(file));
        else if (ext == ".obj")
            setTargetAndResetDisplacementSlider(Mesh::parseOBJ(file));

        qDebug() << fileName << " has been loaded as target mesh.";

        ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
        ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
    }
}

std::string Mainwindow::extractFileName(const std::string &fullPath)
{
    size_t lastSlashPos = fullPath.find_last_of("/\\");
    std::string fileName = fullPath.substr(lastSlashPos + 1);
    size_t lastDotPos = fileName.find_last_of(".");

    if (lastDotPos != std::string::npos)
        fileName = fileName.substr(0, lastDotPos);

    return fileName;
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

void Mainwindow::mousePressEvent(QMouseEvent *ev)
{
    if (ui.openGLWidget->underMouse())
        ui.openGLWidget->trackBall.setFistClick(ev->pos());
}

void Mainwindow::mouseMoveEvent(QMouseEvent *ev)
{
    if (ui.openGLWidget->underMouse())
    {
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
    if (ev->key() == Qt::Key_Escape)
        exit(1);
    else if (ev->key() == Qt::Key_B && baseMesh.isValid())
    {
        ui.openGLWidget->updateMeshData(baseMesh);
        ui.currentMeshLabel->setText("Base mesh");
    }
    else if (ev->key() == Qt::Key_T && targetMesh.isValid())
    {
        ui.openGLWidget->updateMeshData(targetMesh);
        ui.currentMeshLabel->setText("Target mesh");
    }
    else if (ev->key() == Qt::Key_S && subdividedMesh.isValid())
    {
        ui.openGLWidget->updateMeshData(subdividedMesh);
        ui.currentMeshLabel->setText("Subdivided mesh");
    }
    else if (ev->key() == Qt::Key_P && projectedMesh.isValid())
    {
        ui.openGLWidget->updateMeshData(projectedMesh);
        ui.currentMeshLabel->setText("Projected mesh");
    }
    else if (ev->key() == Qt::Key_W)
        ui.wireframeToggle->toggle();
    else if (ev->key() == Qt::Key_E)
        on_actionExtract_displacements_triggered();
    else if (ev->key() == Qt::Key_U)
        on_actionUnload_triggered();
    else if (ev->key() == Qt::Key_1)
        on_demo124faces_clicked();
    else if (ev->key() == Qt::Key_2)
        on_demo250faces_clicked();
    else if (ev->key() == Qt::Key_3)
        on_demo500faces_clicked();
    else if (ev->key() == Qt::Key_4)
        on_demo1000faces_clicked();
    else if (ev->key() == Qt::Key_5)
        on_actionLoad_triggered();
}

void Mainwindow::on_actionProjected_mesh_triggered()
{
    if (!projectedMesh.isValid())
    {
        qDebug() << "There isn't a subdivision scheme applied to the base mesh.";
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Save Mesh"), "./Models",
                                                    tr("OFF File (*.off);;OBJ File (*.obj)"));

    if (!filePath.isEmpty())
    {
        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string fileNameExt = filePath.mid(filePath.lastIndexOf("/")).toStdString();
        std::string fileName = fileNameExt.substr(1, fileNameExt.size() - 5);

        if (ext == ".off")
            projectedMesh.exportOFF(fileName, filePath);
        else if (ext == ".obj")
            projectedMesh.exportOBJ(fileName, filePath);
    }
}

void Mainwindow::on_actionSubdivided_mesh_triggered()
{
    if (!subdividedMesh.isValid())
    {
        qDebug() << "There is no displacement applied to the subdivided mesh.";
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Save Mesh"), "./Models",
                                                    tr("OFF File (*.off);;OBJ File (*.obj)"));

    if (!filePath.isEmpty())
    {
        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string fileNameExt = filePath.mid(filePath.lastIndexOf("/")).toStdString();
        std::string fileName = fileNameExt.substr(1, fileNameExt.size() - 5);

        if (ext == ".off")
            subdividedMesh.exportOFF(fileName, filePath);
        else if (ext == ".obj")
            subdividedMesh.exportOBJ(fileName, filePath);
    }
}

void Mainwindow::on_actionLoad_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Load Mesh"), "./Models",
        tr("3D Mesh(*.off *.obj);;OFF Files (*.off);; OBJ Files (*.obj)"));

    if (!filePath.isEmpty())
    {
        projectedMesh = subdividedMesh = Mesh();

        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string file = readFile(filePath.toStdString().c_str());
        baseMesh = Mesh();

        if (ext == ".off")
            setBaseMeshAndUI(Mesh::parseOFF(file));
        else if (ext == ".obj")
            setBaseMeshAndUI(Mesh::parseOBJ(file));

        baseMeshNameAndDetail = extractFileName(filePath.toStdString());
        setMicroFacesSliderRange();
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
        ui.currentMeshLabel->setText("Base mesh");
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
        ui.currentMeshLabel->setText("Base mesh");
    }
}

// TODO: to get fixed, extract the displacements somewhere as .txt
void Mainwindow::on_actionExtract_displacements_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Load Mesh"), "./Models",
        tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

    if (!filePath.isEmpty())
    {
        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string file = readFile(filePath.toStdString().c_str());

        if (ext == ".off")
            targetMesh = Mesh::parseOFF(file);
        else if (ext == ".obj")
            targetMesh = Mesh::parseOBJ(file);

        bool isValid;

        int k = QInputDialog::getInt(
            this,
            tr("Insert the number of mipoint subdivision to execute"),
            tr("Midpoint subdivisions iterations:"), 1, 1, 9, 1, &isValid,
            this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

        if (isValid)
        {
            for (int i = 0; i < k; i++)
                baseMesh = baseMesh.subdivide();

            ui.openGLWidget->updateMeshData(baseMesh);
            ui.currentMeshLabel->setText("Base mesh");
        }

        Mesh tmpMesh = baseMesh;
        auto displacements = tmpMesh.getDisplacements(targetMesh);

        QString outputFilePath = QFileDialog::getSaveFileName(this, tr("Save Displacements"), ".\\", tr("Text Files (*.txt)"));

        if (!outputFilePath.isEmpty())
        {
            QFile outputFile(outputFilePath);

            if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream stream(&outputFile);

                for (float displacement : displacements)
                    stream << displacement << "\n";

                outputFile.close();

                qDebug() << "Displacements saved to file:" << outputFilePath;
            }
            else
                qDebug() << "Error: Unable to open file for writing.";
        }
    }
}

void Mainwindow::on_actionMidpoint_subdivision_triggered()
{
    subdividedMesh = baseMesh.subdivide();
    ui.openGLWidget->updateMeshData(subdividedMesh);
    ui.currentMeshLabel->setText("Subdivided mesh");
}

void Mainwindow::on_actionUniform_subdivision_triggered()
{
    bool isValid;
    int subdivisions = QInputDialog::getInt(
        this, tr("Insert the number of subdivision to perform"),
        tr("Number of subdivisions:"), 1, 1, 9, 1, &isValid,
        this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    if (isValid)
    {
        baseMesh = baseMesh.subdivideNtimes(subdivisions);
        ui.openGLWidget->updateMeshData(baseMesh);
        ui.currentMeshLabel->setText("Base mesh");
        ui.morphingGroupBox->setEnabled(true);
    }
}

void Mainwindow::on_actionUnload_triggered()
{
    ui.openGLWidget->unloadMeshData();
    ui.actionSubdivide->setEnabled(false);
    ui.actionUnload->setEnabled(false);
    ui.actionWireframe->setEnabled(false);
    ui.actionVertex_displacement->setEnabled(false);
    ui.actionProjected_mesh->setEnabled(false);
    ui.actionSubdivided_mesh->setEnabled(false);
}

void Mainwindow::on_actionWireframe_triggered()
{
    ui.wireframeToggle->toggle();
}

void Mainwindow::on_actionExit_triggered()
{
    exit(1);
}

void Mainwindow::on_demo124faces_clicked()
{
    std::string pallasOBJ125 = readFile("./Models/pallas_124.obj");
    setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ125));
    baseMeshNameAndDetail = extractFileName("./Models/pallas_124.obj");

    ui.currentMeshLabel->setText("Base mesh");
    ui.baseMeshVertices->setText(std::to_string(baseMesh.vertices.size()).c_str());
    ui.baseMeshFaces->setText(std::to_string(baseMesh.faces.size()).c_str());

    projectedMesh = subdividedMesh = Mesh();

    setMicroFacesSliderRange();
    resetSubdividedMeshLabels();
    resetTargetMeshLabels();
}

void Mainwindow::on_demo250faces_clicked()
{
    std::string pallasOBJ250 = readFile("./Models/pallas_250.obj");
    setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ250));
    baseMeshNameAndDetail = extractFileName("./Models/pallas_250.obj");

    ui.currentMeshLabel->setText("Base mesh");
    ui.baseMeshVertices->setText(std::to_string(baseMesh.vertices.size()).c_str());
    ui.baseMeshFaces->setText(std::to_string(baseMesh.faces.size()).c_str());

    projectedMesh = subdividedMesh = Mesh();

    setMicroFacesSliderRange();
    resetSubdividedMeshLabels();
    resetTargetMeshLabels();
}

void Mainwindow::on_demo500faces_clicked()
{
    std::string pallasOBJ500 = readFile("./Models/pallas_500.obj");
    setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ500));
    baseMeshNameAndDetail = extractFileName("./Models/pallas_500.obj");

    ui.currentMeshLabel->setText("Base mesh");
    ui.baseMeshVertices->setText(std::to_string(baseMesh.vertices.size()).c_str());
    ui.baseMeshFaces->setText(std::to_string(baseMesh.faces.size()).c_str());

    projectedMesh = subdividedMesh = Mesh();

    setMicroFacesSliderRange();
    resetSubdividedMeshLabels();
    resetTargetMeshLabels();
}

void Mainwindow::on_demo1000faces_clicked()
{
    std::string pallasOBJ1000 = readFile("./Models/pallas_1000.obj");
    setBaseMeshAndUI(Mesh::parseOBJ(pallasOBJ1000));

    baseMeshNameAndDetail = extractFileName("./Models/pallas_1000.obj");

    ui.currentMeshLabel->setText("Base mesh");
    ui.baseMeshVertices->setText(std::to_string(baseMesh.vertices.size()).c_str());
    ui.baseMeshFaces->setText(std::to_string(baseMesh.faces.size()).c_str());

    projectedMesh = subdividedMesh = Mesh();

    setMicroFacesSliderRange();
    resetSubdividedMeshLabels();
    resetTargetMeshLabels();
}

void Mainwindow::on_micromesh_subdivision_clicked()
{
    // Setting target edge length + micromesh subdivision scheme
    scheme = ISOTROPIC;
    baseMesh.updateEdgesSubdivisionLevelsMicromesh(edgeLengthCurrentValue);

    subdividedMesh = baseMesh.micromeshSubdivide();

//    int idx = 0;

//    for (Edge e: subdividedMesh.edges)
//    {
//        qDebug()
//            << subdividedMesh.edges.at(subdividedMesh.faces.at(e.faces[0]).edges[0]).subdivisions
//            << subdividedMesh.edges.at(subdividedMesh.faces.at(e.faces[0]).edges[1]).subdivisions
//            << subdividedMesh.edges.at(subdividedMesh.faces.at(e.faces[0]).edges[2]).subdivisions
//            << subdividedMesh.edges.at(subdividedMesh.faces.at(e.faces[1]).edges[0]).subdivisions
//            << subdividedMesh.edges.at(subdividedMesh.faces.at(e.faces[1]).edges[1]).subdivisions
//            << subdividedMesh.edges.at(subdividedMesh.faces.at(e.faces[1]).edges[2]).subdivisions;

//        qDebug()
//            << idx++
//            << subdividedMesh.faces.at(e.faces[0]).edges[0]
//            << subdividedMesh.faces.at(e.faces[0]).edges[1]
//            << subdividedMesh.faces.at(e.faces[0]).edges[2]
//            << subdividedMesh.faces.at(e.faces[1]).edges[0]
//            << subdividedMesh.faces.at(e.faces[1]).edges[1]
//            << subdividedMesh.faces.at(e.faces[1]).edges[2];
//    }


    // Updating mesh data
    ui.openGLWidget->updateMeshData(subdividedMesh);

    // Updating text labels (scheme, current mesh, sub-mesh vertices and faces)
    ui.subdivisionSchemeLabel->setText("Micromesh");
    ui.currentMeshLabel->setText("Subdivided mesh");
    ui.subdividedMeshVertices->setText(std::to_string(subdividedMesh.vertices.size()).c_str());
    ui.subdividedMeshFaces->setText(std::to_string(subdividedMesh.faces.size()).c_str());

    // Enable microfaces slider
    ui.microFacesSlider->setEnabled(true);

    // Enable displacement group box
    ui.morphingGroupBox->setEnabled(true);

    // Allow switching to subdivided mesh through keyboard
    ui.actionSubdivided_mesh->setEnabled(true);

    // If the displacement slider is enabled it has to be reset to 0
    if (ui.displacementSlider->isEnabled())
    {
        // Delete target and projected mesh
        disableDisplacementSlider();
        // Reset labels for target vertices and faces
        resetTargetMeshLabels();
    }
}

void Mainwindow::on_anisotropic_micromesh_subdivision_clicked()
{
    // Setting target edge length + anisotropic micromesh subdivision scheme
    scheme = ANISOTROPIC;
    baseMesh.updateEdgesSubdivisionLevelsAniso(edgeLengthCurrentValue);
    subdividedMesh = baseMesh.anisotropicMicromeshSubdivide();

    // Updating mesh data
    ui.openGLWidget->updateMeshData(subdividedMesh);

    // Updating text labels (scheme, current mesh, sub-mesh vertices and faces)
    ui.subdivisionSchemeLabel->setText("Anisotropic M.");
    ui.currentMeshLabel->setText("Subdivided mesh");
    ui.subdividedMeshVertices->setText(std::to_string(subdividedMesh.vertices.size()).c_str());
    ui.subdividedMeshFaces->setText(std::to_string(subdividedMesh.faces.size()).c_str());

    // Enable microfaces slider
    ui.microFacesSlider->setEnabled(true);

    // Enable displacement group box
    ui.morphingGroupBox->setEnabled(true);

    // Allow switching to subdivided mesh through keyboard
    ui.actionSubdivided_mesh->setEnabled(true);

    // If the displacement slider is enabled it has to be reset to 0
    if (ui.displacementSlider->isEnabled())
    {
        // Delete target and projected mesh
        disableDisplacementSlider();
        // Reset labels for target vertices and faces
        resetTargetMeshLabels();
    }
}

void Mainwindow::on_target250faces_clicked()
{
    std::string pallasOBJ250 = readFile("./Models/pallas_250.obj");
    setTargetAndResetDisplacementSlider(Mesh::parseOBJ(pallasOBJ250));
    ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
    ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
    ui.actionProjected_mesh->setEnabled(true);
}

void Mainwindow::on_target500faces_clicked()
{
    std::string pallasOBJ500 = readFile("./Models/pallas_500.obj");
    setTargetAndResetDisplacementSlider(Mesh::parseOBJ(pallasOBJ500));
    ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
    ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
    ui.actionProjected_mesh->setEnabled(true);
}

void Mainwindow::on_target1000faces_clicked()
{
    std::string pallasOBJ1000 = readFile("./Models/models/pallas_1000.obj");
    setTargetAndResetDisplacementSlider(Mesh::parseOBJ(pallasOBJ1000));
    ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
    ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
    ui.actionProjected_mesh->setEnabled(true);
}

void Mainwindow::on_target2500faces_clicked()
{
    std::string pallasOBJ2500 = readFile("./Models/pallas_2500.obj");
    setTargetAndResetDisplacementSlider(Mesh ::parseOBJ(pallasOBJ2500));
    ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
    ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
    ui.actionProjected_mesh->setEnabled(true);
}

void Mainwindow::on_target5000faces_clicked()
{
    std::string pallasOBJ5000 = readFile("./Models/pallas_5000.obj");
    setTargetAndResetDisplacementSlider(Mesh::parseOBJ(pallasOBJ5000));
    ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
    ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
    ui.actionProjected_mesh->setEnabled(true);
}

void Mainwindow::on_microFacesSlider_valueChanged(int microFaces)
{
    if (microFaces == baseMesh.faces.size())
        return;

    ui.microFacesCurrentValue->setText(QString::number(microFaces));
    edgeLengthCurrentValue = binarySearchTargetEdgeLength(microFaces, scheme, 0, baseMesh.bbox.diagonal() * 10);

    subdivideBaseMesh(scheme);

    ui.openGLWidget->updateMeshData(subdividedMesh);
    ui.subdividedMeshFaces->setText(QString::number(subdividedMesh.faces.size()));
    ui.subdividedMeshVertices->setText(QString::number(subdividedMesh.vertices.size()));
}

void Mainwindow::on_displacementSlider_valueChanged(int value)
{
    morphingCurrentValue = value;
    ui.displacementCurrentValue->setText(std::to_string(value).c_str());

    if (value == 0) return;

    projectedMesh = subdividedMesh;

    int vertexIdx = 0;

    for (const auto &disp : displacementsDeltas)
        projectedMesh.displaceVertex(vertexIdx++, disp * float(value));

    ui.openGLWidget->updateMeshData(projectedMesh);
    ui.currentMeshLabel->setText("Projected mesh");
}

void Mainwindow::subdivideBaseMesh(Scheme scheme)
{
    if (scheme == ANISOTROPIC)
    {
        baseMesh.updateEdgesSubdivisionLevelsAniso(edgeLengthCurrentValue);
        subdividedMesh = baseMesh.anisotropicMicromeshSubdivide();
    }
    else
    {
        baseMesh.updateEdgesSubdivisionLevelsMicromesh(edgeLengthCurrentValue);
        subdividedMesh = baseMesh.micromeshSubdivide();
    }
}

void Mainwindow::on_wireframeToggle_stateChanged()
{
    ui.openGLWidget->wireframePaint();
}

void Mainwindow::on_loadBaseMesh_clicked()
{
    on_actionLoad_triggered();
}

void Mainwindow::on_loadTargetMesh_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Load Mesh"), "./Models",
        tr("3D Mesh(*.off *.obj);;OFF Files (*.off);;OBJ Files (*.obj)"));

    if (!filePath.isEmpty())
    {
        std::string ext = filePath.mid(filePath.lastIndexOf(".")).toStdString();
        std::string file = readFile(filePath.toStdString().c_str());

        if (ext == ".off")
            setTargetAndResetDisplacementSlider(Mesh::parseOFF(file));
        else if (ext == ".obj")
            setTargetAndResetDisplacementSlider(Mesh::parseOBJ(file));

        ui.targetMeshVertices->setText(std::to_string(targetMesh.vertices.size()).c_str());
        ui.targetMeshFaces->setText(std::to_string(targetMesh.faces.size()).c_str());
        ui.actionProjected_mesh->setEnabled(true);
    }
}

void Mainwindow::on_exportCurrentOBJ_clicked()
{
    std::ostringstream fileNameStream;

    if (scheme == ANISOTROPIC) fileNameStream << "displaced/aniso";
    else fileNameStream << "displaced/micro";

    fileNameStream << baseMeshNameAndDetail << "_to_" << targetMesh.faces.size() << "_disp_" << morphingCurrentValue << "_edge_" << edgeLengthCurrentValue;
    std::string fileName = fileNameStream.str();

    if (ui.displacementSlider->value() == 0)
        subdividedMesh.exportOBJ(fileName);
    else
        projectedMesh.exportOBJ(fileName);
}

void Mainwindow::on_exportCurrentOFF_clicked()
{
    std::ostringstream fileNameStream;

    if (scheme == ANISOTROPIC)
        fileNameStream << "displaced/aniso";
    else
        fileNameStream << "displaced/micro";

    fileNameStream << baseMeshNameAndDetail << "_to_"
                   << targetMesh.faces.size() << "_disp_"
                   << morphingCurrentValue << "_edge_"
                   << edgeLengthCurrentValue;

    std::string fileName = fileNameStream.str();

    if (ui.displacementSlider->value() == 0)
        subdividedMesh.exportOFF(fileName);
    else
        projectedMesh.exportOFF(fileName);
}

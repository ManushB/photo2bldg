#include "MainWindow.h"
#include <QFileDialog>
#include "MassReconstructionDialog.h"
#include "OptionDialog.h"
#include <QInputDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QActionGroup* groupRendering = new QActionGroup(this);
	groupRendering->addAction(ui.actionLineRendering);
	groupRendering->addAction(ui.actionHatchingRendering);
	groupRendering->addAction(ui.actionSSAORendering);

	ui.actionPenVanishingLine->setChecked(true);

	connect(ui.actionClearBackground, SIGNAL(triggered()), this, SLOT(onClearBackground()));
	connect(ui.actionOpenImage, SIGNAL(triggered()), this, SLOT(onOpenImage()));
	connect(ui.actionClearSilhouette, SIGNAL(triggered()), this, SLOT(onClearSilhouette()));
	connect(ui.actionLoadSilhouette, SIGNAL(triggered()), this, SLOT(onLoadSilhouette()));
	connect(ui.actionSaveSilhouette, SIGNAL(triggered()), this, SLOT(onSaveSilhouette()));
	connect(ui.actionOpenCGA, SIGNAL(triggered()), this, SLOT(onOpenCGA()));
	connect(ui.actionSaveCGA, SIGNAL(triggered()), this, SLOT(onSaveCGA()));
	connect(ui.actionSaveOBJ, SIGNAL(triggered()), this, SLOT(onSaveOBJ()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
	connect(ui.actionBuildingReconstruction, SIGNAL(triggered()), this, SLOT(onBuildingReconstruction()));
	connect(ui.actionMassReconstruction, SIGNAL(triggered()), this, SLOT(onMassReconstruction()));
	connect(ui.actionAutoTest, SIGNAL(triggered()), this, SLOT(onAutoTest()));
	connect(ui.actionFacadeReconstruction, SIGNAL(triggered()), this, SLOT(onFacadeReconstruction()));
	connect(ui.actionOption, SIGNAL(triggered()), this, SLOT(onOption()));
	connect(ui.actionLineRendering, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));
	connect(ui.actionSSAORendering, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));

	glWidget = new GLWidget3D(this);
	this->setCentralWidget(glWidget);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onClearBackground() {
	glWidget->clearBackground();
}

void MainWindow::onOpenImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image file..."), "", tr("Image Files (*.jpg *.png *.bmp)"));
	if (filename.isEmpty()) return;

	image_filename = filename;

	glWidget->clearGeometry();
	glWidget->loadImage(filename.toUtf8().constData());
}

void MainWindow::onClearSilhouette() {
	glWidget->clearSilhouette();
}

void MainWindow::onLoadSilhouette() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open contour file..."), "", tr("Contour Files (*.ctr)"));
	if (filename.isEmpty()) return;

	glWidget->loadSilhouette(filename.toUtf8().constData());
}

void MainWindow::onSaveSilhouette() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save contour file..."), "", tr("Contour Files (*.ctr)"));
	if (filename.isEmpty()) return;

	glWidget->saveSilhouette(filename.toUtf8().constData());
}

void MainWindow::onOpenCGA() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open CGA file..."), "", tr("CGA Files (*.xml)"));
	if (filename.isEmpty()) return;

	glWidget->loadCGA(filename.toUtf8().data());
}

void MainWindow::onSaveCGA() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save CGA file..."), "", tr("CGA Files (*.xml)"));
	if (filename.isEmpty()) return;

	glWidget->saveCGA(filename.toUtf8().constData());
}

void MainWindow::onSaveOBJ() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save OBJ file..."), "", tr("OBJ Files (*.obj)"));
	if (filename.isEmpty()) return;

	glWidget->saveOBJ(filename.toUtf8().constData());
}

void MainWindow::onUndo() {
	glWidget->undo();
}

void MainWindow::onBuildingReconstruction() {
	// extract number from the currently loaded image file name
	int index1 = image_filename.lastIndexOf("/");
	int index2 = image_filename.indexOf(".");
	QString current_name = image_filename.mid(index1 + 1, index2 - index1 - 1);
	std::cout << current_name.toUtf8().constData() << std::endl;

	int num_floors = 1;
	int num_columns = 1;

	// get #floors/#columns
	QFile file("floors_columns.txt");
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString filename;
		in >> filename >> num_floors >> num_columns;

		int index = filename.indexOf(".png");
		if (filename.mid(0, index) == current_name) {
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "#floors = " << num_floors << ", #columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////	
	glWidget->massReconstruction(true, 0, 227, 25, -40, 0, -70, -20, -10, 10, 20, 90, -0.8, 0.8, -0.8, 0.8, -15, 15, -15, 15, 1, true, 3000, 0);
	glWidget->grammar_type = GLWidget3D::GRAMMAR_TYPE_FACADE;
	glWidget->facadeReconstruction();

	glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
	glWidget->clearBackground();
	glWidget->clearSilhouette();
	glWidget->update();
}

void MainWindow::onMassReconstruction() {
	MassReconstructionDialog dlg;
	if (dlg.exec()) {
		bool automaticRecognition = dlg.ui.checkBoxAutomaticRecognition->isChecked();
		int grammarSnippetId = dlg.ui.lineEditGrammarSnippet->text().toInt() - 1;
		int image_size = dlg.ui.lineEditImageSize->text().toInt();
		float cameraDistanceBase = dlg.ui.lineEditCameraDistance->text().toFloat();
		float xrotMin = dlg.ui.lineEditXrotMin->text().toFloat();
		float xrotMax = dlg.ui.lineEditXrotMax->text().toFloat();
		float yrotMin = dlg.ui.lineEditYrotMin->text().toFloat();
		float yrotMax = dlg.ui.lineEditYrotMax->text().toFloat();
		float zrotMin = dlg.ui.lineEditZrotMin->text().toFloat();
		float zrotMax = dlg.ui.lineEditZrotMax->text().toFloat();
		float fovMin = dlg.ui.lineEditFovMin->text().toFloat();
		float fovMax = dlg.ui.lineEditFovMax->text().toFloat();
		float oxMin = dlg.ui.lineEditOXMin->text().toFloat();
		float oxMax = dlg.ui.lineEditOXMax->text().toFloat();
		float oyMin = dlg.ui.lineEditOYMin->text().toFloat();
		float oyMax = dlg.ui.lineEditOYMax->text().toFloat();
		float xMin = dlg.ui.lineEditXMin->text().toFloat();
		float xMax = dlg.ui.lineEditXMax->text().toFloat();
		float yMin = dlg.ui.lineEditYMin->text().toFloat();
		float yMax = dlg.ui.lineEditYMax->text().toFloat();
		int silhouette_line_type = dlg.ui.radioButtonSilhouetteLine8->isChecked() ? 0 : 1;
		bool refinement = dlg.ui.checkBoxRefinement->isChecked();
		int maxIters = dlg.ui.lineEditIterations->text().toInt();
		int refinement_method = dlg.ui.radioButtonRefinementBobyqa->isChecked() ? 0 : 1;

		glWidget->massReconstruction(automaticRecognition, grammarSnippetId, image_size, cameraDistanceBase, xrotMin, xrotMax, yrotMin, yrotMax, zrotMin, zrotMax, fovMin, fovMax, oxMin, oxMax, oyMin, oyMax, xMin, xMax, yMin, yMax, silhouette_line_type, refinement, maxIters, refinement_method);
	}
}

void MainWindow::onAutoTest() {
	MassReconstructionDialog dlg;
	if (dlg.exec()) {
		bool automaticRecognition = dlg.ui.checkBoxAutomaticRecognition->isChecked();
		int grammarSnippetId = dlg.ui.lineEditGrammarSnippet->text().toInt() - 1;
		int image_size = dlg.ui.lineEditImageSize->text().toInt();
		float cameraDistanceBase = dlg.ui.lineEditCameraDistance->text().toFloat();
		float xrotMin = dlg.ui.lineEditXrotMin->text().toFloat();
		float xrotMax = dlg.ui.lineEditXrotMax->text().toFloat();
		float yrotMin = dlg.ui.lineEditYrotMin->text().toFloat();
		float yrotMax = dlg.ui.lineEditYrotMax->text().toFloat();
		float zrotMin = dlg.ui.lineEditZrotMin->text().toFloat();
		float zrotMax = dlg.ui.lineEditZrotMax->text().toFloat();
		float fovMin = dlg.ui.lineEditFovMin->text().toFloat();
		float fovMax = dlg.ui.lineEditFovMax->text().toFloat();
		float oxMin = dlg.ui.lineEditOXMin->text().toFloat();
		float oxMax = dlg.ui.lineEditOXMax->text().toFloat();
		float oyMin = dlg.ui.lineEditOYMin->text().toFloat();
		float oyMax = dlg.ui.lineEditOYMax->text().toFloat();
		float xMin = dlg.ui.lineEditXMin->text().toFloat();
		float xMax = dlg.ui.lineEditXMax->text().toFloat();
		float yMin = dlg.ui.lineEditYMin->text().toFloat();
		float yMax = dlg.ui.lineEditYMax->text().toFloat();
		int silhouette_line_type = dlg.ui.radioButtonSilhouetteLine8->isChecked() ? 0 : 1;
		bool refinement = dlg.ui.checkBoxRefinement->isChecked();
		int maxIters = dlg.ui.lineEditIterations->text().toInt();

		glWidget->autoTest(grammarSnippetId, image_size, "params_multi10.txt", xrotMin, xrotMax, yrotMin, yrotMax, zrotMin, zrotMax, fovMin, fovMax, oxMin, oxMax, oyMin, oyMax, xMin, xMax, yMin, yMax, silhouette_line_type, refinement);
	}
}

void MainWindow::onFacadeReconstruction() {
	glWidget->grammar_type = GLWidget3D::GRAMMAR_TYPE_FACADE;
	
	glWidget->facadeReconstruction();
	glWidget->update();
}

void MainWindow::onOption() {
	OptionDialog dlg;
	dlg.setSilhouetteWidth(glWidget->silhouetteWidth);
	dlg.setSilhouetteColor(glWidget->silhouetteColor);
	dlg.setMaterial(glWidget->grammar_ids["material"]);

	if (dlg.exec()) {
		glWidget->silhouetteWidth = dlg.getSilhouetteWidth();
		glWidget->silhouetteColor = dlg.getSilhouetteColor();
		glWidget->grammar_ids["material"] = dlg.getMaterial();
		glWidget->updateGeometry();
		glWidget->update();
	}
}

void MainWindow::onRenderingChanged() {
	if (ui.actionLineRendering->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
	}
	else if (ui.actionHatchingRendering->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_HATCHING;
	}
	else if (ui.actionSSAORendering->isChecked()) {
		glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
	}
	glWidget->update();
}
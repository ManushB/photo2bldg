#include "Client.h"
#include <QFileDialog>
#include "BuildingReconstructionDialog.h"
#include "MassReconstructionDialog.h"
#include "OptionDialog.h"
#include <QInputDialog>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QString>
#include <QMovie>
#include <iostream>

Client::Client(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);

//    QActionGroup* groupRendering = new QActionGroup(this);
//    groupRendering->addAction(ui.actionLineRendering);
//    groupRendering->addAction(ui.actionHatchingRendering);
//    groupRendering->addAction(ui.actionSSAORendering);

    ui.actionPenVanishingLine->setChecked(true);

    connect(ui.actionClearBackground, SIGNAL(triggered()), this, SLOT(onClearBackground()));
    connect(ui.actionOpenImage, SIGNAL(triggered()), this, SLOT(onOpenImage()));
    connect(ui.actionClearSilhouette, SIGNAL(triggered()), this, SLOT(onClearSilhouette()));
    connect(ui.actionLoadSilhouette, SIGNAL(triggered()), this, SLOT(onLoadSilhouette()));
    connect(ui.actionSaveSilhouette, SIGNAL(triggered()), this, SLOT(onSaveSilhouette()));
    connect(ui.actionOpenCGA, SIGNAL(triggered()), this, SLOT(onOpenCGA()));
    connect(ui.actionSaveCGA, SIGNAL(triggered()), this, SLOT(onSaveCGA()));
    connect(ui.actionLoadOBJ, SIGNAL(triggered()), this, SLOT(onLoadOBJ()));
    connect(ui.actionSaveOBJ, SIGNAL(triggered()), this, SLOT(onSaveOBJ()));
    connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));
    connect(ui.actionBuildingReconstruction, SIGNAL(triggered()), this, SLOT(onBuildingReconstruction()));
    connect(ui.actionMassReconstruction, SIGNAL(triggered()), this, SLOT(onMassReconstruction()));
    connect(ui.actionFacadeReconstruction, SIGNAL(triggered()), this, SLOT(onFacadeReconstruction()));
//    connect(ui.actionOption, SIGNAL(triggered()), this, SLOT(onOption()));
//    connect(ui.actionLineRendering, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));
//    connect(ui.actionSSAORendering, SIGNAL(triggered()), this, SLOT(onRenderingChanged()));

    glWidget = new GLWidget3D(this);
    this->setCentralWidget(glWidget);
}

void Client::keyPressEvent(QKeyEvent* e) {
    glWidget->keyPressEvent(e);
}

void Client::keyReleaseEvent(QKeyEvent* e) {
    glWidget->keyReleaseEvent(e);
}

void Client::onClearBackground() {
    glWidget->clearBackground();
    setWindowTitle("PhotoTo3D");
}

void Client::onOpenImage() {
    glWidget->inputImg = QFileDialog::getOpenFileName(this, tr("Open Image file..."), "", tr("Image Files (*.jpg *.png *.bmp)"));
    if (glWidget->inputImg.isEmpty()) return;

    glWidget->clearGeometry();
    glWidget->loadImage(glWidget->inputImg.toUtf8().constData());

    setWindowTitle("PhotoTo3D - " + glWidget->inputImg);
}

void Client::onClearSilhouette() {
    glWidget->clearSilhouette();
}

void Client::onLoadSilhouette() {
    glWidget->inputCtr = QFileDialog::getOpenFileName(this, tr("Open contour file..."), "",
                                                      tr("Contour Files (*.ctr)"));
    if (glWidget->inputCtr.isEmpty()) return;

    glWidget->loadSilhouette(glWidget->inputCtr.toUtf8().constData());
}

void Client::onSaveSilhouette() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save contour file..."), "", tr("Contour Files (*.ctr)"));
    if (filename.isEmpty()) return;

    glWidget->saveSilhouette(filename.toUtf8().constData());
}

void Client::onOpenCGA() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open CGA file..."), "", tr("CGA Files (*.xml)"));
    if (filename.isEmpty()) return;

    glWidget->loadCGA(filename.toUtf8().data());
}

void Client::onSaveCGA() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save CGA file..."), "", tr("CGA Files (*.xml)"));
    if (filename.isEmpty()) return;

    glWidget->saveCGA(filename.toUtf8().constData());
}

void Client::onSaveOBJ() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save OBJ file..."), "", tr("OBJ Files (*.obj)"));
    if (filename.isEmpty()) return;

    glWidget->saveOBJ(filename.toUtf8().constData());
}

void Client::onLoadOBJ() {
    glWidget->inputObj = QFileDialog::getOpenFileName(this, tr("Open object file..."), "", tr("Object Files (*.obj)"));
    if (glWidget->inputObj.isEmpty()) return;

    glWidget->loadOBJ(glWidget->inputObj.toUtf8().constData());
}

void Client::onUndo() {
    glWidget->undo();
}

void Client::onBuildingReconstruction() {
    // Call Submit Job
    char s[10];
    BuildingReconstructionDialog dlg;
    if (dlg.exec()) {
        QFile file(glWidget->inputPrm);

        file.open(QIODevice::WriteOnly);

        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("Photo2Bldg");

        xmlWriter.writeStartElement("BuildRec");

        // Mass
        xmlWriter.writeTextElement("checkBoxMassGrammarAutomaticDetection", dlg.ui.checkBoxMassGrammarAutomaticDetection->isChecked() ? "true": "false");
        sprintf(s, "%d", dlg.ui.spinBoxMassGrammarId->value());
        xmlWriter.writeTextElement("spinBoxMassGrammarId", s);

        // Facade
        xmlWriter.writeTextElement("checkBoxFacadeGrammarAutomaticDetection", dlg.ui.checkBoxFacadeGrammarAutomaticDetection->isChecked() ? "true": "false");
        sprintf(s, "%d", dlg.ui.spinBoxFacadeGrammarId->value());
        xmlWriter.writeTextElement("spinBoxFacadeGrammarId", s);
        xmlWriter.writeTextElement("checkBoxAdjustContrast", dlg.ui.checkBoxAdjustContrast->isChecked() ? "true" : "false");
        xmlWriter.writeTextElement("checkBoxUseMultipleColors", dlg.ui.checkBoxUseMultipleColors->isChecked() ? "true" : "false");

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();

        file.close();

        glWidget->execProcess();
//        glWidget->massReconstruction(
//                dlg.ui.checkBoxMassGrammarAutomaticDetection->isChecked(),
//                dlg.ui.spinBoxMassGrammarId->value() - 1,
//                227, 25, -40, 0, -70, -20, -10, 10, 20, 90, -0.8, 0.8, -0.8, 0.8, -15, 15, -15, 15, 1, true, 3000, 0);
//        glWidget->grammar_type = GLWidget3D::GRAMMAR_TYPE_FACADE;
//        glWidget->facadeReconstruction(
//                dlg.ui.checkBoxFacadeGrammarAutomaticDetection->isChecked(),
//                dlg.ui.spinBoxFacadeGrammarId->value() - 1,
//                dlg.ui.checkBoxAdjustContrast->isChecked(),
//                dlg.ui.checkBoxUseMultipleColors->isChecked());
//
//        glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
//        glWidget->clearBackground();
//        glWidget->clearSilhouette();
//        glWidget->update();
    }
}

void Client::onMassReconstruction() {
    char s[10];
    MassReconstructionDialog dlg;
    if (dlg.exec()) {
        QFile file(glWidget->inputPrm);

        file.open(QIODevice::WriteOnly);

        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("Photo2Bldg");

        xmlWriter.writeStartElement("MassRec");

        xmlWriter.writeTextElement("checkBoxAutomaticRecognition", dlg.ui.checkBoxAutomaticRecognition->isChecked() ? "true": "false");
        sprintf(s, "%d", dlg.ui.spinBoxGrammarId->value());
        xmlWriter.writeTextElement("spinBoxGrammarId", s);
        xmlWriter.writeTextElement("lineEditImageSize", dlg.ui.lineEditImageSize->text());
        xmlWriter.writeTextElement("lineEditCameraDistance", dlg.ui.lineEditCameraDistance->text());
        xmlWriter.writeTextElement("lineEditXrotMin", dlg.ui.lineEditXrotMin->text());
        xmlWriter.writeTextElement("lineEditXrotMax", dlg.ui.lineEditXrotMax->text());
        xmlWriter.writeTextElement("lineEditYrotMin", dlg.ui.lineEditYrotMin->text());
        xmlWriter.writeTextElement("lineEditYrotMax", dlg.ui.lineEditYrotMax->text());
        xmlWriter.writeTextElement("lineEditZrotMin", dlg.ui.lineEditZrotMin->text());
        xmlWriter.writeTextElement("lineEditZrotMax", dlg.ui.lineEditZrotMax->text());
        xmlWriter.writeTextElement("lineEditFovMin", dlg.ui.lineEditFovMin->text());
        xmlWriter.writeTextElement("lineEditFovMax", dlg.ui.lineEditFovMax->text());
        xmlWriter.writeTextElement("lineEditOXMin", dlg.ui.lineEditOXMin->text());
        xmlWriter.writeTextElement("lineEditOXMax", dlg.ui.lineEditOXMax->text());
        xmlWriter.writeTextElement("lineEditOYMin", dlg.ui.lineEditOYMin->text());
        xmlWriter.writeTextElement("lineEditOYMax", dlg.ui.lineEditOYMax->text());
        xmlWriter.writeTextElement("lineEditXMin", dlg.ui.lineEditXMin->text());
        xmlWriter.writeTextElement("lineEditXMax", dlg.ui.lineEditXMax->text());
        xmlWriter.writeTextElement("lineEditYMin", dlg.ui.lineEditYMin->text());
        xmlWriter.writeTextElement("lineEditYMax", dlg.ui.lineEditYMax->text());

        xmlWriter.writeTextElement("radioButtonSilhouetteLine8", dlg.ui.radioButtonSilhouetteLine8->isChecked() ? "true": "false");
        xmlWriter.writeTextElement("checkBoxRefinement", dlg.ui.checkBoxRefinement->isChecked() ? "true": "false");
        xmlWriter.writeTextElement("lineEditIterations", dlg.ui.lineEditIterations->text());
        xmlWriter.writeTextElement("radioButtonRefinementBobyqa", dlg.ui.radioButtonRefinementBobyqa->isChecked() ? "true": "false");


        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();

        file.close();
        glWidget->execProcess();
//        bool automaticRecognition = dlg.ui.checkBoxAutomaticRecognition->isChecked();
//        int grammarId = dlg.ui.spinBoxGrammarId->value() - 1;
//        int image_size = dlg.ui.lineEditImageSize->text().toInt();
//        float cameraDistanceBase = dlg.ui.lineEditCameraDistance->text().toFloat();
//        float xrotMin = dlg.ui.lineEditXrotMin->text().toFloat();
//        float xrotMax = dlg.ui.lineEditXrotMax->text().toFloat();
//        float yrotMin = dlg.ui.lineEditYrotMin->text().toFloat();
//        float yrotMax = dlg.ui.lineEditYrotMax->text().toFloat();
//        float zrotMin = dlg.ui.lineEditZrotMin->text().toFloat();
//        float zrotMax = dlg.ui.lineEditZrotMax->text().toFloat();
//        float fovMin = dlg.ui.lineEditFovMin->text().toFloat();
//        float fovMax = dlg.ui.lineEditFovMax->text().toFloat();
//        float oxMin = dlg.ui.lineEditOXMin->text().toFloat();
//        float oxMax = dlg.ui.lineEditOXMax->text().toFloat();
//        float oyMin = dlg.ui.lineEditOYMin->text().toFloat();
//        float oyMax = dlg.ui.lineEditOYMax->text().toFloat();
//        float xMin = dlg.ui.lineEditXMin->text().toFloat();
//        float xMax = dlg.ui.lineEditXMax->text().toFloat();
//        float yMin = dlg.ui.lineEditYMin->text().toFloat();
//        float yMax = dlg.ui.lineEditYMax->text().toFloat();
//        int silhouette_line_type = dlg.ui.radioButtonSilhouetteLine8->isChecked() ? 0 : 1;
//        bool refinement = dlg.ui.checkBoxRefinement->isChecked();
//        int maxIters = dlg.ui.lineEditIterations->text().toInt();
//        int refinement_method = dlg.ui.radioButtonRefinementBobyqa->isChecked() ? 0 : 1;
//
//        glWidget->massReconstruction(automaticRecognition, grammarId, image_size, cameraDistanceBase, xrotMin, xrotMax, yrotMin, yrotMax, zrotMin, zrotMax, fovMin, fovMax, oxMin, oxMax, oyMin, oyMax, xMin, xMax, yMin, yMax, silhouette_line_type, refinement, maxIters, refinement_method);
    }
}

void Client::onFacadeReconstruction() {
    char s[10];
    MassReconstructionDialog dlg;
    if (dlg.exec()) {
        QFile file(glWidget->inputPrm);

        file.open(QIODevice::ReadWrite);

        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("Photo2Bldg");

        xmlWriter.writeStartElement("FacadeRec");

        xmlWriter.writeTextElement("checkBoxAutomaticRecognition", dlg.ui.checkBoxAutomaticRecognition->isChecked() ? "true": "false");
        sprintf(s, "%d", dlg.ui.spinBoxGrammarId->value());
        xmlWriter.writeTextElement("spinBoxGrammarId", s);
        xmlWriter.writeTextElement("lineEditImageSize", dlg.ui.lineEditImageSize->text());
        xmlWriter.writeTextElement("lineEditCameraDistance", dlg.ui.lineEditCameraDistance->text());
        xmlWriter.writeTextElement("lineEditXrotMin", dlg.ui.lineEditXrotMin->text());
        xmlWriter.writeTextElement("lineEditXrotMax", dlg.ui.lineEditXrotMax->text());
        xmlWriter.writeTextElement("lineEditYrotMin", dlg.ui.lineEditYrotMin->text());
        xmlWriter.writeTextElement("lineEditYrotMax", dlg.ui.lineEditYrotMax->text());
        xmlWriter.writeTextElement("lineEditZrotMin", dlg.ui.lineEditZrotMin->text());
        xmlWriter.writeTextElement("lineEditZrotMax", dlg.ui.lineEditZrotMax->text());
        xmlWriter.writeTextElement("lineEditFovMin", dlg.ui.lineEditFovMin->text());
        xmlWriter.writeTextElement("lineEditFovMax", dlg.ui.lineEditFovMax->text());
        xmlWriter.writeTextElement("lineEditOXMin", dlg.ui.lineEditOXMin->text());
        xmlWriter.writeTextElement("lineEditOXMax", dlg.ui.lineEditOXMax->text());
        xmlWriter.writeTextElement("lineEditOYMin", dlg.ui.lineEditOYMin->text());
        xmlWriter.writeTextElement("lineEditOYMax", dlg.ui.lineEditOYMax->text());
        xmlWriter.writeTextElement("lineEditXMin", dlg.ui.lineEditXMin->text());
        xmlWriter.writeTextElement("lineEditXMax", dlg.ui.lineEditXMax->text());
        xmlWriter.writeTextElement("lineEditYMin", dlg.ui.lineEditYMin->text());
        xmlWriter.writeTextElement("lineEditYMax", dlg.ui.lineEditYMax->text());

        xmlWriter.writeTextElement("radioButtonSilhouetteLine8", dlg.ui.radioButtonSilhouetteLine8->isChecked() ? "true": "false");
        xmlWriter.writeTextElement("checkBoxRefinement", dlg.ui.checkBoxRefinement->isChecked() ? "true": "false");
        xmlWriter.writeTextElement("lineEditIterations", dlg.ui.lineEditIterations->text());
        xmlWriter.writeTextElement("radioButtonRefinementBobyqa", dlg.ui.radioButtonRefinementBobyqa->isChecked() ? "true": "false");


        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
        glWidget->execProcess();
    }

//    glWidget->grammar_type = GLWidget3D::GRAMMAR_TYPE_FACADE;

//    glWidget->facadeReconstruction(true, 0, true, false);
//    glWidget->update();
}
//
//void Client::onOption() {
//    OptionDialog dlg;
//    dlg.setSilhouetteWidth(glWidget->silhouetteWidth);
//    dlg.setSilhouetteColor(glWidget->silhouetteColor);
//    dlg.setMaterial(glWidget->grammar_ids["material"]);
//
//    if (dlg.exec()) {
//        glWidget->silhouetteWidth = dlg.getSilhouetteWidth();
//        glWidget->silhouetteColor = dlg.getSilhouetteColor();
//        glWidget->grammar_ids["material"] = dlg.getMaterial();
//        glWidget->updateGeometry();
//        glWidget->update();
//    }
//}
//
//void Client::onRenderingChanged() {
//    if (ui.actionLineRendering->isChecked()) {
//        glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
//    }
//    else if (ui.actionHatchingRendering->isChecked()) {
//        glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_HATCHING;
//    }
//    else if (ui.actionSSAORendering->isChecked()) {
//        glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
//    }
//    glWidget->update();
//}
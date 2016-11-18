#include "MassReconstructionDialog.h"

MassReconstructionDialog::MassReconstructionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	groupSilhouetteLine = new QButtonGroup(this);
	groupSilhouetteLine->addButton(ui.radioButtonSilhouetteLine8);
	groupSilhouetteLine->addButton(ui.radioButtonSilhouetteLineAA);

	groupRefinement = new QButtonGroup(this);
	groupRefinement->addButton(ui.radioButtonRefinementBobyqa);
	groupRefinement->addButton(ui.radioButtonRefinementRandom);

	ui.checkBoxAutomaticRecognition->setChecked(false);
	ui.lineEditGrammarSnippet->setText("1");
	ui.lineEditGrammarSnippet->setEnabled(!ui.checkBoxAutomaticRecognition->isChecked());
	ui.lineEditImageSize->setText("227");
	ui.lineEditCameraDistance->setText("25");
	// all
	ui.lineEditXrotMin->setText("-40");
	ui.lineEditXrotMax->setText("0");
	ui.lineEditYrotMin->setText("-70");
	ui.lineEditYrotMax->setText("-20");
	ui.lineEditZrotMin->setText("-10");
	ui.lineEditZrotMax->setText("10");
	ui.lineEditFovMin->setText("20");
	ui.lineEditFovMax->setText("90");
	ui.lineEditOXMin->setText("-0.8");
	ui.lineEditOXMax->setText("0.8");
	ui.lineEditOYMin->setText("-0.8");
	ui.lineEditOYMax->setText("0.8");
	ui.lineEditXMin->setText("-15");
	ui.lineEditXMax->setText("15");
	ui.lineEditYMin->setText("-15");
	ui.lineEditYMax->setText("15");
	ui.radioButtonSilhouetteLine8->setChecked(false);
	ui.radioButtonSilhouetteLineAA->setChecked(true);
	ui.checkBoxImageBlur->setChecked(false);
	ui.lineEditImageBlurSize->setEnabled(false);
	ui.lineEditImageBlurSize->setText("5");
	ui.checkBoxRefinement->setChecked(false);
	ui.lineEditIterations->setText("3000");
	ui.lineEditIterations->setEnabled(ui.checkBoxRefinement->isChecked());
	ui.radioButtonRefinementBobyqa->setChecked(true);
	ui.radioButtonRefinementBobyqa->setEnabled(ui.checkBoxRefinement->isChecked());
	ui.radioButtonRefinementRandom->setChecked(false);
	ui.radioButtonRefinementRandom->setEnabled(ui.checkBoxRefinement->isChecked());

	connect(ui.checkBoxAutomaticRecognition, SIGNAL(clicked()), this, SLOT(onAutomaticRecognition()));
	connect(ui.checkBoxImageBlur, SIGNAL(clicked()), this, SLOT(onImageBlur()));
	connect(ui.checkBoxRefinement, SIGNAL(clicked()), this, SLOT(onRefinement()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

MassReconstructionDialog::~MassReconstructionDialog() {
}

void MassReconstructionDialog::onAutomaticRecognition() {
	ui.lineEditGrammarSnippet->setEnabled(ui.checkBoxAutomaticRecognition->isChecked());
}

void MassReconstructionDialog::onImageBlur() {
	ui.lineEditImageBlurSize->setEnabled(ui.checkBoxImageBlur->isChecked());
}

void MassReconstructionDialog::onRefinement() {
	ui.lineEditIterations->setEnabled(ui.checkBoxRefinement->isChecked());
	ui.radioButtonRefinementBobyqa->setEnabled(ui.checkBoxRefinement->isChecked());
	ui.radioButtonRefinementRandom->setEnabled(ui.checkBoxRefinement->isChecked());
}

void MassReconstructionDialog::onOK() {
	accept();
}

void MassReconstructionDialog::onCancel() {
	reject();
}

#include "MassReconstructionDialog.h"

MassReconstructionDialog::MassReconstructionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	groupSilhouetteLine = new QButtonGroup(this);
	groupSilhouetteLine->addButton(ui.radioButtonSilhouetteLine8);
	groupSilhouetteLine->addButton(ui.radioButtonSilhouetteLineAA);

	groupRefinement = new QButtonGroup(this);
	groupRefinement->addButton(ui.radioButtonRefinementBobyqa);
	groupRefinement->addButton(ui.radioButtonRefinementRandom);

	ui.checkBoxAutomaticRecognition->setChecked(true);
	ui.spinBoxGrammarId->setRange(1, 10);
	ui.spinBoxGrammarId->setValue(6);
	ui.spinBoxGrammarId->setEnabled(!ui.checkBoxAutomaticRecognition->isChecked());
	ui.lineEditImageSize->setText("227");
	ui.lineEditCameraDistance->setText("25");
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
	ui.checkBoxRefinement->setChecked(true);
	ui.lineEditIterations->setText("3000");
	ui.lineEditIterations->setEnabled(ui.checkBoxRefinement->isChecked());
	ui.radioButtonRefinementBobyqa->setChecked(true);
	ui.radioButtonRefinementBobyqa->setEnabled(ui.checkBoxRefinement->isChecked());
	ui.radioButtonRefinementRandom->setChecked(false);
	ui.radioButtonRefinementRandom->setEnabled(ui.checkBoxRefinement->isChecked());

	connect(ui.checkBoxAutomaticRecognition, SIGNAL(clicked()), this, SLOT(onAutomaticRecognition()));
	connect(ui.checkBoxRefinement, SIGNAL(clicked()), this, SLOT(onRefinement()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

MassReconstructionDialog::~MassReconstructionDialog() {
}

void MassReconstructionDialog::onAutomaticRecognition() {
	ui.spinBoxGrammarId->setEnabled(!ui.checkBoxAutomaticRecognition->isChecked());
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

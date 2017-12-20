#include "BuildingReconstructionDialog.h"

BuildingReconstructionDialog::BuildingReconstructionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.checkBoxMassGrammarAutomaticDetection->setChecked(true);
	ui.spinBoxMassGrammarId->setRange(1, 10);
	ui.spinBoxMassGrammarId->setValue(6);
	ui.spinBoxMassGrammarId->setEnabled(!ui.checkBoxMassGrammarAutomaticDetection->isChecked());
	ui.checkBoxFacadeGrammarAutomaticDetection->setChecked(true);
	ui.spinBoxFacadeGrammarId->setRange(1, 10);
	ui.spinBoxFacadeGrammarId->setValue(1);
	ui.spinBoxFacadeGrammarId->setEnabled(!ui.checkBoxFacadeGrammarAutomaticDetection->isChecked());
	ui.checkBoxAdjustContrast->setChecked(true);

	connect(ui.checkBoxMassGrammarAutomaticDetection, SIGNAL(clicked()), this, SLOT(onMassGrammarAutomaticDetection()));
	connect(ui.checkBoxFacadeGrammarAutomaticDetection, SIGNAL(clicked()), this, SLOT(onFacadeGrammarAutomaticDetection()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

BuildingReconstructionDialog::~BuildingReconstructionDialog() {
}

void BuildingReconstructionDialog::onMassGrammarAutomaticDetection() {
	ui.spinBoxMassGrammarId->setEnabled(!ui.checkBoxMassGrammarAutomaticDetection->isChecked());
}

void BuildingReconstructionDialog::onFacadeGrammarAutomaticDetection() {
	ui.spinBoxFacadeGrammarId->setEnabled(!ui.checkBoxFacadeGrammarAutomaticDetection->isChecked());
}

void BuildingReconstructionDialog::onOK() {
	accept();
}

void BuildingReconstructionDialog::onCancel() {
	reject();
}

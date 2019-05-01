#ifndef BUILDINGRECONSTRUCTIONDIALOG_H
#define BUILDINGRECONSTRUCTIONDIALOG_H

#include <QDialog>
#include "ui_BuildingReconstructionDialog.h"

class BuildingReconstructionDialog : public QDialog {
	Q_OBJECT

public:
	Ui::BuildingReconstructionDialog ui;

public:
	BuildingReconstructionDialog(QWidget *parent = 0);
	~BuildingReconstructionDialog();

public slots:
	void onMassGrammarAutomaticDetection();
	void onFacadeGrammarAutomaticDetection();
	void onOK();
	void onCancel();
};

#endif // BUILDINGRECONSTRUCTIONDIALOG_H

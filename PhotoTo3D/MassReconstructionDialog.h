#ifndef MASSRECONSTRUCTIONDIALOG_H
#define MASSRECONSTRUCTIONDIALOG_H

#include <QDialog>
#include "ui_MassReconstructionDialog.h"

class MassReconstructionDialog : public QDialog
{
	Q_OBJECT

private:
	QButtonGroup* groupSilhouetteLine;
	QButtonGroup* groupRefinement;

public:
	MassReconstructionDialog(QWidget *parent = 0);
	~MassReconstructionDialog();
	
	public slots:
	void onAutomaticRecognition();
	void onImageBlur();
	void onRefinement();
	void onOK();
	void onCancel();

public:
	Ui::MassReconstructionDialog ui;
};

#endif // MASSRECONSTRUCTIONDIALOG_H

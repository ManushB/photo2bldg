#include "OptionDialog.h"

OptionDialog::OptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OptionDialog::~OptionDialog() {
}

void OptionDialog::setSilhouetteWidth(int silhouetteWidth) {
	ui.lineEditSilhouetteWidth->setText(QString::number(silhouetteWidth));
}

int OptionDialog::getSilhouetteWidth() {
	return ui.lineEditSilhouetteWidth->text().toInt();
}

void OptionDialog::setSilhouetteColor(const QColor& silhouetteColor) {
	ui.lineEditSilhouetteRed->setText(QString::number(silhouetteColor.red()));
	ui.lineEditSilhouetteGreen->setText(QString::number(silhouetteColor.green()));
	ui.lineEditSilhouetteBlue->setText(QString::number(silhouetteColor.blue()));
}

QColor OptionDialog::getSilhouetteColor() {
	return QColor(ui.lineEditSilhouetteRed->text().toInt(), ui.lineEditSilhouetteGreen->text().toInt(), ui.lineEditSilhouetteBlue->text().toInt());
}

void OptionDialog::onOK() {
	accept();
}

void OptionDialog::onCancel() {
	reject();
}

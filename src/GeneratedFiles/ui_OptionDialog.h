/********************************************************************************
** Form generated from reading UI file 'OptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONDIALOG_H
#define UI_OPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_OptionDialog
{
public:
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLineEdit *lineEditSilhouetteGreen;
    QLineEdit *lineEditSilhouetteBlue;
    QLineEdit *lineEditSilhouetteRed;
    QLabel *label_19;
    QLineEdit *lineEditSilhouetteWidth;
    QLabel *label_20;
    QLineEdit *lineEditMaterial;

    void setupUi(QDialog *OptionDialog)
    {
        if (OptionDialog->objectName().isEmpty())
            OptionDialog->setObjectName(QStringLiteral("OptionDialog"));
        OptionDialog->resize(323, 151);
        pushButtonOK = new QPushButton(OptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(50, 110, 91, 31));
        pushButtonCancel = new QPushButton(OptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(190, 110, 91, 31));
        label_15 = new QLabel(OptionDialog);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(210, 40, 16, 16));
        label_16 = new QLabel(OptionDialog);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(260, 40, 16, 16));
        label_17 = new QLabel(OptionDialog);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(160, 40, 16, 16));
        label_18 = new QLabel(OptionDialog);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(20, 40, 121, 16));
        lineEditSilhouetteGreen = new QLineEdit(OptionDialog);
        lineEditSilhouetteGreen->setObjectName(QStringLiteral("lineEditSilhouetteGreen"));
        lineEditSilhouetteGreen->setGeometry(QRect(220, 40, 31, 20));
        lineEditSilhouetteBlue = new QLineEdit(OptionDialog);
        lineEditSilhouetteBlue->setObjectName(QStringLiteral("lineEditSilhouetteBlue"));
        lineEditSilhouetteBlue->setGeometry(QRect(270, 40, 31, 20));
        lineEditSilhouetteRed = new QLineEdit(OptionDialog);
        lineEditSilhouetteRed->setObjectName(QStringLiteral("lineEditSilhouetteRed"));
        lineEditSilhouetteRed->setGeometry(QRect(170, 40, 31, 20));
        label_19 = new QLabel(OptionDialog);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(20, 10, 101, 16));
        lineEditSilhouetteWidth = new QLineEdit(OptionDialog);
        lineEditSilhouetteWidth->setObjectName(QStringLiteral("lineEditSilhouetteWidth"));
        lineEditSilhouetteWidth->setGeometry(QRect(160, 10, 71, 20));
        label_20 = new QLabel(OptionDialog);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(20, 70, 101, 16));
        lineEditMaterial = new QLineEdit(OptionDialog);
        lineEditMaterial->setObjectName(QStringLiteral("lineEditMaterial"));
        lineEditMaterial->setGeometry(QRect(160, 70, 71, 20));
        QWidget::setTabOrder(lineEditSilhouetteWidth, lineEditSilhouetteRed);
        QWidget::setTabOrder(lineEditSilhouetteRed, lineEditSilhouetteGreen);
        QWidget::setTabOrder(lineEditSilhouetteGreen, lineEditSilhouetteBlue);
        QWidget::setTabOrder(lineEditSilhouetteBlue, pushButtonOK);
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(OptionDialog);

        QMetaObject::connectSlotsByName(OptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OptionDialog)
    {
        OptionDialog->setWindowTitle(QApplication::translate("OptionDialog", "OptionDialog", 0));
        pushButtonOK->setText(QApplication::translate("OptionDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("OptionDialog", "Cancel", 0));
        label_15->setText(QApplication::translate("OptionDialog", "G", 0));
        label_16->setText(QApplication::translate("OptionDialog", "B", 0));
        label_17->setText(QApplication::translate("OptionDialog", "R", 0));
        label_18->setText(QApplication::translate("OptionDialog", "Silhouette color:", 0));
        label_19->setText(QApplication::translate("OptionDialog", "Silhouette width:", 0));
        label_20->setText(QApplication::translate("OptionDialog", "Material:", 0));
    } // retranslateUi

};

namespace Ui {
    class OptionDialog: public Ui_OptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONDIALOG_H

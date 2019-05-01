/********************************************************************************
** Form generated from reading UI file 'BuildingReconstructionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BUILDINGRECONSTRUCTIONDIALOG_H
#define UI_BUILDINGRECONSTRUCTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_BuildingReconstructionDialog
{
public:
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QGroupBox *groupBox;
    QCheckBox *checkBoxAdjustContrast;
    QCheckBox *checkBoxUseMultipleColors;
    QLabel *label_2;
    QCheckBox *checkBoxFacadeGrammarAutomaticDetection;
    QSpinBox *spinBoxFacadeGrammarId;
    QGroupBox *groupBox_2;
    QSpinBox *spinBoxMassGrammarId;
    QLabel *label;
    QCheckBox *checkBoxMassGrammarAutomaticDetection;

    void setupUi(QDialog *BuildingReconstructionDialog)
    {
        if (BuildingReconstructionDialog->objectName().isEmpty())
            BuildingReconstructionDialog->setObjectName(QStringLiteral("BuildingReconstructionDialog"));
        BuildingReconstructionDialog->resize(345, 262);
        pushButtonOK = new QPushButton(BuildingReconstructionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(60, 220, 91, 31));
        pushButtonCancel = new QPushButton(BuildingReconstructionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(200, 220, 91, 31));
        groupBox = new QGroupBox(BuildingReconstructionDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 90, 321, 111));
        checkBoxAdjustContrast = new QCheckBox(groupBox);
        checkBoxAdjustContrast->setObjectName(QStringLiteral("checkBoxAdjustContrast"));
        checkBoxAdjustContrast->setGeometry(QRect(20, 60, 151, 17));
        checkBoxUseMultipleColors = new QCheckBox(groupBox);
        checkBoxUseMultipleColors->setObjectName(QStringLiteral("checkBoxUseMultipleColors"));
        checkBoxUseMultipleColors->setGeometry(QRect(20, 80, 151, 17));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 20, 61, 21));
        checkBoxFacadeGrammarAutomaticDetection = new QCheckBox(groupBox);
        checkBoxFacadeGrammarAutomaticDetection->setObjectName(QStringLiteral("checkBoxFacadeGrammarAutomaticDetection"));
        checkBoxFacadeGrammarAutomaticDetection->setGeometry(QRect(100, 20, 121, 21));
        spinBoxFacadeGrammarId = new QSpinBox(groupBox);
        spinBoxFacadeGrammarId->setObjectName(QStringLiteral("spinBoxFacadeGrammarId"));
        spinBoxFacadeGrammarId->setGeometry(QRect(240, 20, 42, 22));
        groupBox_2 = new QGroupBox(BuildingReconstructionDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 10, 321, 61));
        spinBoxMassGrammarId = new QSpinBox(groupBox_2);
        spinBoxMassGrammarId->setObjectName(QStringLiteral("spinBoxMassGrammarId"));
        spinBoxMassGrammarId->setGeometry(QRect(240, 20, 42, 22));
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 61, 21));
        checkBoxMassGrammarAutomaticDetection = new QCheckBox(groupBox_2);
        checkBoxMassGrammarAutomaticDetection->setObjectName(QStringLiteral("checkBoxMassGrammarAutomaticDetection"));
        checkBoxMassGrammarAutomaticDetection->setGeometry(QRect(100, 20, 121, 21));

        retranslateUi(BuildingReconstructionDialog);

        QMetaObject::connectSlotsByName(BuildingReconstructionDialog);
    } // setupUi

    void retranslateUi(QDialog *BuildingReconstructionDialog)
    {
        BuildingReconstructionDialog->setWindowTitle(QApplication::translate("BuildingReconstructionDialog", "BuildingReconstructionDialog", 0));
        pushButtonOK->setText(QApplication::translate("BuildingReconstructionDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("BuildingReconstructionDialog", "Cancel", 0));
        groupBox->setTitle(QApplication::translate("BuildingReconstructionDialog", "Facade", 0));
        checkBoxAdjustContrast->setText(QApplication::translate("BuildingReconstructionDialog", "Adjust contrast", 0));
        checkBoxUseMultipleColors->setText(QApplication::translate("BuildingReconstructionDialog", "Use multiple facade colors", 0));
        label_2->setText(QApplication::translate("BuildingReconstructionDialog", "Grammar#", 0));
        checkBoxFacadeGrammarAutomaticDetection->setText(QApplication::translate("BuildingReconstructionDialog", "Automatic detection", 0));
        groupBox_2->setTitle(QApplication::translate("BuildingReconstructionDialog", "Mass", 0));
        label->setText(QApplication::translate("BuildingReconstructionDialog", "Grammar#", 0));
        checkBoxMassGrammarAutomaticDetection->setText(QApplication::translate("BuildingReconstructionDialog", "Automatic detection", 0));
    } // retranslateUi

};

namespace Ui {
    class BuildingReconstructionDialog: public Ui_BuildingReconstructionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BUILDINGRECONSTRUCTIONDIALOG_H

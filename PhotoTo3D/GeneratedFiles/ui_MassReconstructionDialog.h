/********************************************************************************
** Form generated from reading UI file 'MassReconstructionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MASSRECONSTRUCTIONDIALOG_H
#define UI_MASSRECONSTRUCTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_MassReconstructionDialog
{
public:
    QLineEdit *lineEditGrammarSnippet;
    QLabel *label_13;
    QLineEdit *lineEditXMax;
    QLabel *label_16;
    QLineEdit *lineEditOXMin;
    QLabel *label_15;
    QLabel *label_26;
    QLineEdit *lineEditOYMin;
    QRadioButton *radioButtonSilhouetteLineAA;
    QLabel *label_9;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_25;
    QLineEdit *lineEditImageSize;
    QLineEdit *lineEditOYMax;
    QLineEdit *lineEditIterations;
    QLineEdit *lineEditYrotMin;
    QLineEdit *lineEditFovMin;
    QLineEdit *lineEditZrotMin;
    QLabel *label_10;
    QLabel *label_23;
    QLabel *label_31;
    QLineEdit *lineEditXrotMax;
    QLineEdit *lineEditYMin;
    QLineEdit *lineEditZrotMax;
    QCheckBox *checkBoxRefinement;
    QPushButton *pushButtonOK;
    QRadioButton *radioButtonRefinementRandom;
    QLineEdit *lineEditXrotMin;
    QLabel *label_24;
    QLabel *label_29;
    QLabel *label_20;
    QLabel *label_6;
    QLabel *label_27;
    QLineEdit *lineEditYrotMax;
    QLineEdit *lineEditCameraDistance;
    QLineEdit *lineEditFovMax;
    QLabel *label_30;
    QLabel *label_11;
    QRadioButton *radioButtonRefinementBobyqa;
    QLabel *label_14;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_28;
    QPushButton *pushButtonCancel;
    QLabel *label;
    QLineEdit *lineEditOXMax;
    QLineEdit *lineEditXMin;
    QCheckBox *checkBoxAutomaticRecognition;
    QLineEdit *lineEditYMax;
    QRadioButton *radioButtonSilhouetteLine8;

    void setupUi(QDialog *MassReconstructionDialog)
    {
        if (MassReconstructionDialog->objectName().isEmpty())
            MassReconstructionDialog->setObjectName(QStringLiteral("MassReconstructionDialog"));
        MassReconstructionDialog->resize(582, 493);
        lineEditGrammarSnippet = new QLineEdit(MassReconstructionDialog);
        lineEditGrammarSnippet->setObjectName(QStringLiteral("lineEditGrammarSnippet"));
        lineEditGrammarSnippet->setGeometry(QRect(250, 10, 31, 20));
        label_13 = new QLabel(MassReconstructionDialog);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(173, 110, 16, 20));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXMax = new QLineEdit(MassReconstructionDialog);
        lineEditXMax->setObjectName(QStringLiteral("lineEditXMax"));
        lineEditXMax->setGeometry(QRect(198, 290, 61, 20));
        label_16 = new QLabel(MassReconstructionDialog);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(290, 10, 41, 20));
        label_16->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        lineEditOXMin = new QLineEdit(MassReconstructionDialog);
        lineEditOXMin->setObjectName(QStringLiteral("lineEditOXMin"));
        lineEditOXMin->setGeometry(QRect(108, 230, 61, 20));
        label_15 = new QLabel(MassReconstructionDialog);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(10, 10, 91, 20));
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_26 = new QLabel(MassReconstructionDialog);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setGeometry(QRect(28, 230, 71, 16));
        label_26->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditOYMin = new QLineEdit(MassReconstructionDialog);
        lineEditOYMin->setObjectName(QStringLiteral("lineEditOYMin"));
        lineEditOYMin->setGeometry(QRect(108, 260, 61, 20));
        radioButtonSilhouetteLineAA = new QRadioButton(MassReconstructionDialog);
        radioButtonSilhouetteLineAA->setObjectName(QStringLiteral("radioButtonSilhouetteLineAA"));
        radioButtonSilhouetteLineAA->setGeometry(QRect(200, 360, 82, 17));
        label_9 = new QLabel(MassReconstructionDialog);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(30, 170, 71, 16));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_21 = new QLabel(MassReconstructionDialog);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(20, 50, 81, 16));
        label_21->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_22 = new QLabel(MassReconstructionDialog);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(28, 290, 71, 16));
        label_22->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_25 = new QLabel(MassReconstructionDialog);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setGeometry(QRect(170, 320, 16, 20));
        label_25->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditImageSize = new QLineEdit(MassReconstructionDialog);
        lineEditImageSize->setObjectName(QStringLiteral("lineEditImageSize"));
        lineEditImageSize->setGeometry(QRect(110, 50, 61, 20));
        lineEditOYMax = new QLineEdit(MassReconstructionDialog);
        lineEditOYMax->setObjectName(QStringLiteral("lineEditOYMax"));
        lineEditOYMax->setGeometry(QRect(198, 260, 61, 20));
        lineEditIterations = new QLineEdit(MassReconstructionDialog);
        lineEditIterations->setObjectName(QStringLiteral("lineEditIterations"));
        lineEditIterations->setGeometry(QRect(280, 390, 91, 20));
        lineEditYrotMin = new QLineEdit(MassReconstructionDialog);
        lineEditYrotMin->setObjectName(QStringLiteral("lineEditYrotMin"));
        lineEditYrotMin->setGeometry(QRect(110, 140, 61, 20));
        lineEditFovMin = new QLineEdit(MassReconstructionDialog);
        lineEditFovMin->setObjectName(QStringLiteral("lineEditFovMin"));
        lineEditFovMin->setGeometry(QRect(110, 200, 61, 20));
        lineEditZrotMin = new QLineEdit(MassReconstructionDialog);
        lineEditZrotMin->setObjectName(QStringLiteral("lineEditZrotMin"));
        lineEditZrotMin->setGeometry(QRect(110, 170, 61, 20));
        label_10 = new QLabel(MassReconstructionDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(30, 200, 71, 16));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_23 = new QLabel(MassReconstructionDialog);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(28, 320, 71, 16));
        label_23->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_31 = new QLabel(MassReconstructionDialog);
        label_31->setObjectName(QStringLiteral("label_31"));
        label_31->setGeometry(QRect(30, 360, 71, 16));
        label_31->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXrotMax = new QLineEdit(MassReconstructionDialog);
        lineEditXrotMax->setObjectName(QStringLiteral("lineEditXrotMax"));
        lineEditXrotMax->setGeometry(QRect(200, 110, 61, 20));
        lineEditYMin = new QLineEdit(MassReconstructionDialog);
        lineEditYMin->setObjectName(QStringLiteral("lineEditYMin"));
        lineEditYMin->setGeometry(QRect(108, 320, 61, 20));
        lineEditZrotMax = new QLineEdit(MassReconstructionDialog);
        lineEditZrotMax->setObjectName(QStringLiteral("lineEditZrotMax"));
        lineEditZrotMax->setGeometry(QRect(200, 170, 61, 20));
        checkBoxRefinement = new QCheckBox(MassReconstructionDialog);
        checkBoxRefinement->setObjectName(QStringLiteral("checkBoxRefinement"));
        checkBoxRefinement->setGeometry(QRect(110, 390, 91, 17));
        pushButtonOK = new QPushButton(MassReconstructionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(370, 450, 91, 31));
        radioButtonRefinementRandom = new QRadioButton(MassReconstructionDialog);
        radioButtonRefinementRandom->setObjectName(QStringLiteral("radioButtonRefinementRandom"));
        radioButtonRefinementRandom->setGeometry(QRect(200, 420, 82, 17));
        lineEditXrotMin = new QLineEdit(MassReconstructionDialog);
        lineEditXrotMin->setObjectName(QStringLiteral("lineEditXrotMin"));
        lineEditXrotMin->setGeometry(QRect(110, 110, 61, 20));
        label_24 = new QLabel(MassReconstructionDialog);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(170, 290, 16, 20));
        label_24->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_29 = new QLabel(MassReconstructionDialog);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setGeometry(QRect(170, 260, 16, 20));
        label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_20 = new QLabel(MassReconstructionDialog);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(173, 170, 16, 20));
        label_20->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(MassReconstructionDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 110, 71, 16));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_27 = new QLabel(MassReconstructionDialog);
        label_27->setObjectName(QStringLiteral("label_27"));
        label_27->setGeometry(QRect(170, 230, 16, 20));
        label_27->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditYrotMax = new QLineEdit(MassReconstructionDialog);
        lineEditYrotMax->setObjectName(QStringLiteral("lineEditYrotMax"));
        lineEditYrotMax->setGeometry(QRect(200, 140, 61, 20));
        lineEditCameraDistance = new QLineEdit(MassReconstructionDialog);
        lineEditCameraDistance->setObjectName(QStringLiteral("lineEditCameraDistance"));
        lineEditCameraDistance->setGeometry(QRect(110, 80, 61, 20));
        lineEditFovMax = new QLineEdit(MassReconstructionDialog);
        lineEditFovMax->setObjectName(QStringLiteral("lineEditFovMax"));
        lineEditFovMax->setGeometry(QRect(200, 200, 61, 20));
        label_30 = new QLabel(MassReconstructionDialog);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setGeometry(QRect(30, 390, 71, 16));
        label_30->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_11 = new QLabel(MassReconstructionDialog);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(173, 200, 16, 20));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        radioButtonRefinementBobyqa = new QRadioButton(MassReconstructionDialog);
        radioButtonRefinementBobyqa->setObjectName(QStringLiteral("radioButtonRefinementBobyqa"));
        radioButtonRefinementBobyqa->setGeometry(QRect(110, 420, 82, 17));
        label_14 = new QLabel(MassReconstructionDialog);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(173, 140, 16, 20));
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_7 = new QLabel(MassReconstructionDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 80, 91, 20));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_8 = new QLabel(MassReconstructionDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(30, 140, 71, 16));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_28 = new QLabel(MassReconstructionDialog);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setGeometry(QRect(28, 260, 71, 16));
        label_28->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButtonCancel = new QPushButton(MassReconstructionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(470, 450, 91, 31));
        label = new QLabel(MassReconstructionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(210, 390, 71, 20));
        lineEditOXMax = new QLineEdit(MassReconstructionDialog);
        lineEditOXMax->setObjectName(QStringLiteral("lineEditOXMax"));
        lineEditOXMax->setGeometry(QRect(198, 230, 61, 20));
        lineEditXMin = new QLineEdit(MassReconstructionDialog);
        lineEditXMin->setObjectName(QStringLiteral("lineEditXMin"));
        lineEditXMin->setGeometry(QRect(108, 290, 61, 20));
        checkBoxAutomaticRecognition = new QCheckBox(MassReconstructionDialog);
        checkBoxAutomaticRecognition->setObjectName(QStringLiteral("checkBoxAutomaticRecognition"));
        checkBoxAutomaticRecognition->setGeometry(QRect(110, 10, 131, 17));
        lineEditYMax = new QLineEdit(MassReconstructionDialog);
        lineEditYMax->setObjectName(QStringLiteral("lineEditYMax"));
        lineEditYMax->setGeometry(QRect(198, 320, 61, 20));
        radioButtonSilhouetteLine8 = new QRadioButton(MassReconstructionDialog);
        radioButtonSilhouetteLine8->setObjectName(QStringLiteral("radioButtonSilhouetteLine8"));
        radioButtonSilhouetteLine8->setGeometry(QRect(110, 360, 82, 17));
        QWidget::setTabOrder(checkBoxAutomaticRecognition, lineEditGrammarSnippet);
        QWidget::setTabOrder(lineEditGrammarSnippet, lineEditImageSize);
        QWidget::setTabOrder(lineEditImageSize, lineEditCameraDistance);
        QWidget::setTabOrder(lineEditCameraDistance, lineEditXrotMin);
        QWidget::setTabOrder(lineEditXrotMin, lineEditXrotMax);
        QWidget::setTabOrder(lineEditXrotMax, lineEditYrotMin);
        QWidget::setTabOrder(lineEditYrotMin, lineEditYrotMax);
        QWidget::setTabOrder(lineEditYrotMax, lineEditZrotMin);
        QWidget::setTabOrder(lineEditZrotMin, lineEditZrotMax);
        QWidget::setTabOrder(lineEditZrotMax, lineEditFovMin);
        QWidget::setTabOrder(lineEditFovMin, lineEditFovMax);
        QWidget::setTabOrder(lineEditFovMax, lineEditOXMin);
        QWidget::setTabOrder(lineEditOXMin, lineEditOXMax);
        QWidget::setTabOrder(lineEditOXMax, lineEditOYMin);
        QWidget::setTabOrder(lineEditOYMin, lineEditOYMax);
        QWidget::setTabOrder(lineEditOYMax, lineEditXMin);
        QWidget::setTabOrder(lineEditXMin, lineEditXMax);
        QWidget::setTabOrder(lineEditXMax, lineEditYMin);
        QWidget::setTabOrder(lineEditYMin, lineEditYMax);
        QWidget::setTabOrder(lineEditYMax, radioButtonSilhouetteLine8);
        QWidget::setTabOrder(radioButtonSilhouetteLine8, radioButtonSilhouetteLineAA);
        QWidget::setTabOrder(radioButtonSilhouetteLineAA, checkBoxRefinement);
        QWidget::setTabOrder(checkBoxRefinement, lineEditIterations);
        QWidget::setTabOrder(lineEditIterations, radioButtonRefinementBobyqa);
        QWidget::setTabOrder(radioButtonRefinementBobyqa, radioButtonRefinementRandom);
        QWidget::setTabOrder(radioButtonRefinementRandom, pushButtonOK);
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(MassReconstructionDialog);

        QMetaObject::connectSlotsByName(MassReconstructionDialog);
    } // setupUi

    void retranslateUi(QDialog *MassReconstructionDialog)
    {
        MassReconstructionDialog->setWindowTitle(QApplication::translate("MassReconstructionDialog", "MassReconstructionDialog", 0));
        label_13->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_16->setText(QApplication::translate("MassReconstructionDialog", "(1 - 10)", 0));
        label_15->setText(QApplication::translate("MassReconstructionDialog", "Grammar snippet:", 0));
        label_26->setText(QApplication::translate("MassReconstructionDialog", "O x:", 0));
        radioButtonSilhouetteLineAA->setText(QApplication::translate("MassReconstructionDialog", "Line AA", 0));
        label_9->setText(QApplication::translate("MassReconstructionDialog", "Zrot:", 0));
        label_21->setText(QApplication::translate("MassReconstructionDialog", "Image size:", 0));
        label_22->setText(QApplication::translate("MassReconstructionDialog", "X pos:", 0));
        label_25->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_10->setText(QApplication::translate("MassReconstructionDialog", "Field of view:", 0));
        label_23->setText(QApplication::translate("MassReconstructionDialog", "Y pos:", 0));
        label_31->setText(QApplication::translate("MassReconstructionDialog", "Silhouette:", 0));
        checkBoxRefinement->setText(QApplication::translate("MassReconstructionDialog", "Yes", 0));
        pushButtonOK->setText(QApplication::translate("MassReconstructionDialog", "OK", 0));
        radioButtonRefinementRandom->setText(QApplication::translate("MassReconstructionDialog", "Random walk", 0));
        label_24->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_29->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_20->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_6->setText(QApplication::translate("MassReconstructionDialog", "Xrot:", 0));
        label_27->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_30->setText(QApplication::translate("MassReconstructionDialog", "Refinement:", 0));
        label_11->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        radioButtonRefinementBobyqa->setText(QApplication::translate("MassReconstructionDialog", "Bobyqa", 0));
        label_14->setText(QApplication::translate("MassReconstructionDialog", "~", 0));
        label_7->setText(QApplication::translate("MassReconstructionDialog", "Camera distance:", 0));
        label_8->setText(QApplication::translate("MassReconstructionDialog", "Yrot:", 0));
        label_28->setText(QApplication::translate("MassReconstructionDialog", "O y:", 0));
        pushButtonCancel->setText(QApplication::translate("MassReconstructionDialog", "Cancel", 0));
        label->setText(QApplication::translate("MassReconstructionDialog", "#Iterations:", 0));
        checkBoxAutomaticRecognition->setText(QApplication::translate("MassReconstructionDialog", "Automatic recognition", 0));
        radioButtonSilhouetteLine8->setText(QApplication::translate("MassReconstructionDialog", "Line 8", 0));
    } // retranslateUi

};

namespace Ui {
    class MassReconstructionDialog: public Ui_MassReconstructionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MASSRECONSTRUCTIONDIALOG_H

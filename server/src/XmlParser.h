//
// Created by root on 9/19/19.
//

#ifndef XMLPARSER_H
#define XMLPARSER_H
#include <QXmlStreamReader>
#include <QFile>

struct BuildRec{
    bool checkBoxMassGrammarAutomaticDetection;
    int spinBoxMassGrammarId;

    bool checkBoxFacadeGrammarAutomaticDetection;
    int spinBoxFacadeGrammarId;
    bool checkBoxAdjustContrast;
    bool checkBoxUseMultipleColors;
};
struct MassRec{
    bool checkBoxAutomaticDetection;
    int spinBoxGrammarId;
    int lineEditImageSize;
    float lineEditCameraDistance;
    float lineEditXrotMin;
    float lineEditXrotMax;
    float lineEditYrotMin;
    float lineEditYrotMax;
    float lineEditZrotMin;
    float lineEditZrotMax;
    float lineEditFovMin;
    float lineEditFovMax;
    float lineEditOXMin;
    float lineEditOXMax;
    float lineEditOYMin;
    float lineEditOYMax;
    float lineEditXMin;
    float lineEditXMax;
    float lineEditYMin;
    float lineEditYMax;
    bool radioButtonSilhouetteLine8;
    bool checkBoxRefinement;
    int lineEditIterations;
    bool radioButtonRefinementBobyqa;
};

class XmlParser {

public:
    const QString BUILDING_RECONSTRUCTION = "BuildRec";
    const QString MASS_RECONSTRUCTION = "MassRec";
    const QString FACADE_RECONSTRUCTION = "FacadeRec";

    QXmlStreamReader r;
    QString function;

    BuildRec buildRec;
    MassRec massRec;

    void readFile(QString file);
    BuildRec processBuildRec();
    MassRec processMassRec();
};

#endif //XMLPARSER_H

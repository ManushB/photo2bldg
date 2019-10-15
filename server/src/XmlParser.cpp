#include "XmlParser.h"
#include <iostream>

void XmlParser::readFile(QString pFile) {
    QFile xmlFile(pFile);
    xmlFile.open(QIODevice::ReadOnly);
    r.setDevice(&xmlFile);
    if (r.readNextStartElement() && r.name() == "Photo2Bldg") {
        r.readNextStartElement();
        function = r.name().toString();
        if (function == BUILDING_RECONSTRUCTION) {
            buildRec = processBuildRec();
        } else if (function == MASS_RECONSTRUCTION) {
            massRec = processMassRec();
        } else if (function == FACADE_RECONSTRUCTION) {
            massRec = processMassRec();
        }
    }
}

BuildRec XmlParser::processBuildRec(){
    bool mGAD, fGAD, aC, uMC;
    int mgID, fgID;

    while(r.readNextStartElement()){
        if (r.name() == "checkBoxMassGrammarAutomaticDetection") {
            mGAD = r.readElementText() == "true";
        } else if (r.name() == "spinBoxMassGrammarId") {
            mgID = r.readElementText().toInt();
        } else if (r.name() == "checkBoxFacadeGrammarAutomaticDetection") {
            fGAD = r.readElementText() == "true";
        } else if (r.name() == "spinBoxFacadeGrammarId"){
            fgID = r.readElementText().toInt();
        } else if (r.name() == "checkBoxAdjustContrast") {
            aC = r.readElementText() == "true";
        } else if (r.name() == "checkBoxUseMultipleColors") {
            uMC = r.readElementText() == "true";
        }
    }

    return BuildRec{
            .checkBoxMassGrammarAutomaticDetection =  mGAD,
            .spinBoxMassGrammarId =  mgID,
            .checkBoxFacadeGrammarAutomaticDetection =  fGAD,
            .spinBoxFacadeGrammarId = fgID,
            .checkBoxAdjustContrast = aC,
            .checkBoxUseMultipleColors = uMC
    };
}

MassRec XmlParser::processMassRec(){
    bool aD, sL8, rF, rFB;
    int gID, iSz, it;
    float cD, xRMin, xRMax, yRMin, yRMax, zRMin, zRMax, fMin, fMax, oxMin, oxMax, oyMin, oyMax, xMin, xMax, yMin, yMax;

    while(r.readNextStartElement()){
        QString val = r.readElementText();
        if (r.name() == "checkBoxAutomaticDetection") {
            aD = val == "true";
        } else if (r.name() == "spinBoxGrammarId") {
            gID = val.toInt();
        } else if (r.name() == "lineEditImageSize") {
            iSz = val.toUtf8().toInt();
        } else if (r.name() == "lineEditCameraDistance"){
            cD = val.toFloat();
        } else if (r.name() == "lineEditXrotMin") {
            xRMin = val.toFloat();
        } else if (r.name() == "lineEditXrotMax") {
            xRMax = val.toFloat();
        } else if (r.name() == "lineEditYrotMin") {
            yRMin = val.toFloat();
        } else if (r.name() == "lineEditYrotMax") {
            yRMax = val.toFloat();
        } else if (r.name() == "lineEditZrotMin") {
            zRMin = val.toFloat();
        } else if (r.name() == "lineEditZrotMax") {
            zRMax = val.toFloat();
        } else if (r.name() == "lineEditFovMin") {
            fMin = val.toFloat();
        } else if (r.name() == "lineEditFovMax") {
            fMax = val.toFloat();
        } else if (r.name() == "lineEditOXMin") {
            oxMin = val.toFloat();
        } else if (r.name() == "lineEditOXMax") {
            oxMax = val.toFloat();
        } else if (r.name() == "lineEditOYMin") {
            oyMin = val.toFloat();
        } else if (r.name() == "lineEditOYMax") {
            oyMax = val.toFloat();
        } else if (r.name() == "lineEditXMin") {
            xMin = val.toFloat();
        } else if (r.name() == "lineEditXMax") {
            xMax = val.toFloat();
        } else if (r.name() == "lineEditYMin") {
            yMin = val.toFloat();
        } else if (r.name() == "lineEditYMax") {
            yMax = val.toFloat();
        } else if (r.name() == "radioButtonSilhouetteLine8") {
            sL8 = val == "true";
        } else if (r.name() == "checkBoxRefinement") {
            rF = val == "true";
        } else if (r.name() == "lineEditIterations") {
            it = val.toInt();
        } else if (r.name() == "radioButtonRefinementBobyqa") {
            rFB = val == "true";
        }
    }

    return MassRec{
            .checkBoxAutomaticDetection = aD,
            .spinBoxGrammarId = gID,
            .lineEditImageSize = iSz,
            .lineEditCameraDistance = cD,
            .lineEditXrotMin = xRMin,
            .lineEditXrotMax = xRMax,
            .lineEditYrotMin = yRMin,
            .lineEditYrotMax = yRMax,
            .lineEditZrotMin = zRMin,
            .lineEditZrotMax = zRMax,
            .lineEditFovMin = fMin,
            .lineEditFovMax = fMax,
            .lineEditOXMin = oxMin,
            .lineEditOXMax = oxMax,
            .lineEditOYMin = oyMin,
            .lineEditOYMax = oyMax,
            .lineEditXMin = xMin,
            .lineEditXMax = xMax,
            .lineEditYMin = yMin,
            .lineEditYMax = yMax,
            .radioButtonSilhouetteLine8 = sL8,
            .checkBoxRefinement = rF,
            .lineEditIterations = it,
            .radioButtonRefinementBobyqa = rFB
    };

}

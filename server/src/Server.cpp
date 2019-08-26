//#include "Server.h"
//#include "MassReconstruction.h"
//#include <QtGui/QColor>
//#include <QtGui/QImage>
//
//void Server::openImage(char* filename) {
//    if (filename.isEmpty()) return;
//
//    QImage temp;
//    temp.load(filename);
//    imageOrig = QImage(std::max(temp.width(), temp.height()), std::max(temp.width(), temp.height()), QImage::Format_RGB32);
//    imageOrig.fill(QColor(255, 255, 255));
//
//}
//
//
//void Server::onBuildingReconstruction(char* params) {
////    glWidget->massReconstruction(
////            dlg.ui.checkBoxMassGrammarAutomaticDetection->isChecked(),
////            dlg.ui.spinBoxMassGrammarId->value() - 1,
////            227, 25, -40, 0, -70, -20, -10, 10, 20, 90, -0.8, 0.8, -0.8, 0.8, -15, 15, -15, 15, 1, true, 3000, 0);
//    glWidget->massReconstruction(params[0], params[1] - 1,
//            227, 25, -40, 0, -70, -20, -10, 10, 20, 90, -0.8, 0.8, -0.8, 0.8, -15, 15, -15, 15, 1, true, 3000, 0);
//    glWidget->grammar_type = GL3D::GRAMMAR_TYPE_FACADE;
////    glWidget->facadeReconstruction(
////            dlg.ui.checkBoxFacadeGrammarAutomaticDetection->isChecked(),
////            dlg.ui.spinBoxFacadeGrammarId->value() - 1,
////            dlg.ui.checkBoxAdjustContrast->isChecked(),
////            dlg.ui.checkBoxUseMultipleColors->isChecked());
//    glWidget->facadeReconstruction(params[2], params[3], params[4], params[5]);
//
////    glWidget->renderManager.renderingMode = RenderManager::RENDERING_MODE_SSAO;
//    return
//}
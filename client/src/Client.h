#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include "ui_Client.h"
#include "GLWidget3D.h"

class Client : public QMainWindow {
Q_OBJECT

public:
    Ui::ClientClass ui;
    GLWidget3D* glWidget;

public:
    Client(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);

public slots:
    void onClearBackground();
    void onOpenImage();
    void onClearSilhouette();
    void onLoadSilhouette();
    void onSaveSilhouette();
    void onOpenCGA();
    void onSaveCGA();
    void onLoadOBJ();
    void onSaveOBJ();
    void onUndo();
    void onBuildingReconstruction();
    void onMassReconstruction();
    void onFacadeReconstruction();
//    void onOption();
//    void onRenderingChanged();
};

#endif //CLIENT_H
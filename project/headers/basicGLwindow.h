#ifndef __BASIC__GL__WINDOW__
#define __BASIC__GL__WINDOW__

#include "basicwindow.h"


class MainWindow;
class BasicGLWidget;
class QLabel;
class QCheckBox;

class BasicGLWindow : public BasicWindow
{
	Q_OBJECT

public:
    BasicGLWindow(QString name = "BasicGLWindow");
    ~BasicGLWindow();

public slots:
void SLOT_MoveSceneCheckbox(int val);
void SLOT_MoveCameraCheckbox(int val);
void SLOT_UpdateFPS(float FPS);

signals:

protected:
    void createBoxScene();

    // Keyboard and mouse interaction
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

	Ui::BasicWindow m_ui;
	BasicGLWidget* m_glWidget;
    QLabel* m_fpsLabel;
    QCheckBox* m_moveSceneCheckbox;
    QCheckBox* m_moveCameraCheckbox;

    //Input
    QPoint m_mouseLastPos;
    bool m_movingCamera;
};
#endif
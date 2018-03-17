#ifndef __BASIC__GL__WINDOW__
#define __BASIC__GL__WINDOW__

#include "basicwindow.h"

class MainWindow;
class BasicGLWidget;

class BasicGLWindow : public BasicWindow
{
	Q_OBJECT

public:
    BasicGLWindow(QString name = "BasicGLWindow");
    ~BasicGLWindow();

protected:
    void createBoxScene();

    // Keyboard and mouse interaction
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    // FPS
    void showFps();

	Ui::BasicWindow m_ui;
	BasicGLWidget* m_glWidget;

    //Input
    QPoint m_mouseLastPos;

    // FPS
    bool m_showFps;
};
#endif
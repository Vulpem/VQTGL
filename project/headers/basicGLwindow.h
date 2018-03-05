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
	Ui::BasicWindow m_ui;
	BasicGLWidget* m_glWidget;
};
#endif
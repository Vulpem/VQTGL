#ifndef __TESTING__GL__WINDOW__
#define __TESTING__GL__WINDOW__

#include "basicGLwindow.h"

class MainWindow;
class BasicGLWidget;

class TestingGLWindow : public BasicGLWindow
{
	Q_OBJECT

public:
    TestingGLWindow(QString name = "Testing GL Window");
    ~TestingGLWindow();
};
#endif
#include <QWidget>
#include "ui_phongwindow.h"
#include "basicwindow.h"

class MainWindow;
class PhongGLWidget;

class PhongWindow : public BasicWindow
{
	Q_OBJECT

public:
	PhongWindow(QString name = "BasicGLWindow");
	~PhongWindow();

private slots:
	void loadModel();

private:
	Ui::PhongWindow m_ui;
	MainWindow* m_mainWindow;
	PhongGLWidget* m_glWidget;

	QVBoxLayout* layoutFrame;
};

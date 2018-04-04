#include <QWidget>
#include "ui_texturingwindow.h"
#include "basicwindow.h"

class MainWindow;
class TexturingGLWidget;

class TexturingWindow : public BasicWindow
{
	Q_OBJECT

public:
	TexturingWindow(QString name = "TexturingWindow");
	~TexturingWindow();

private slots:
	void loadTex1();
	void deleteTex1();
	void loadTex2();
	void deleteTex2();
	
private:

	void initGUI();

	Ui::TexturingWindow m_ui;
	MainWindow* m_mainWindow;
	TexturingGLWidget* m_glWidget;
	
	QString m_filenameTex1;
	QString m_filenameTex2;

};

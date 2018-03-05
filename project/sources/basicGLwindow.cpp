#include "../headers/basicglwidget.h"
#include "../headers/basicGLwindow.h"
#include <qlayout.h>

BasicGLWindow::BasicGLWindow(QString name)
    : BasicWindow(name)
{
    m_ui.setupUi(this);

	// Insert the m_glWidget in the GUI
	m_glWidget = new BasicGLWidget;
	QVBoxLayout* layoutFrame = new QVBoxLayout(m_ui.qGLFrame);
	layoutFrame->setMargin(0);
	layoutFrame->addWidget(m_glWidget);
	m_glWidget->show();

	show();
}

BasicGLWindow::~BasicGLWindow()
{
	if (m_glWidget != nullptr) {
		delete m_glWidget;
		m_glWidget = nullptr;
	}
}
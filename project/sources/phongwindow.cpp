#include "../headers/phongglwidget.h"
#include "../headers/phongwindow.h"
#include "../headers/mainwindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>


PhongWindow::PhongWindow(QString name)
{
	m_ui.setupUi(this);

	// Insert the m_glWidget in the GUI
	m_glWidget = new PhongGLWidget("./project/models/Patricio.obj", false);
	layoutFrame = new QVBoxLayout(m_ui.qGLFrame);
	layoutFrame->setMargin(0);
	layoutFrame->addWidget(m_glWidget);
	m_glWidget->show();
	
	connect(m_ui.qUndockButton, SIGNAL(clicked()), this, SLOT(dockUndock()));
	connect(m_ui.qLoadModelButton, SIGNAL(clicked()), this, SLOT(loadModel()));
}

PhongWindow::~PhongWindow()
{
	if (m_glWidget != nullptr) {
		delete m_glWidget;
		m_glWidget = nullptr;
	}
		
}

void PhongWindow::loadModel() 
{
	QString filename = QFileDialog::getOpenFileName((QWidget*)this, tr("Load Model"),
		"./project/models/", tr("3D Models (*.obj)"));
	if (filename.size() != 0)
	{
		// We delete the glWidget and create another one to restart the GLContext
		// Otherwise, the painter does not work and the fps are not shown
		if (m_glWidget != nullptr) {
			delete m_glWidget;
			m_glWidget = nullptr;
		}

		m_glWidget = new PhongGLWidget(filename, true);
		layoutFrame->addWidget(m_glWidget);
		m_glWidget->show();
	}
}

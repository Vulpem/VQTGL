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

    createBoxScene();

	show();
}

BasicGLWindow::~BasicGLWindow()
{
	if (m_glWidget != nullptr) {
		delete m_glWidget;
		m_glWidget = nullptr;
	}
}

void BasicGLWindow::createBoxScene()
{
    std::vector<Vertex> vertices;
    std::vector<uint> indices = {
        0, 2, 1,
        1, 2, 3
    };

    vertices.push_back({
        { -10.f, +10.f, 0.f },
        { 0.f,0.f,-1.f },
        { 0.f,0.f },
        { 0.f, 1.f, 0.f }
        });
    vertices.push_back({
        { +10.f, +10.f, 0.f },
        { 0.f,0.f,-1.f },
        { 1.f,0.f },
        { 0.f, 1.f, 1.f }
        });
    vertices.push_back({
        { -10.f, -10.f, -0.f },
        { 0.f,0.f,-1.f },
        { 0.f,1.f },
        { 0.f, 0.f, 1.f }
        });
    vertices.push_back({
        { +10.f, -10.f, -0.f },
        { 0.f,0.f,-1.f },
        { 1.f,1.f },
        { 1.f, 0.f, 0.f }
        });

    MeshPtr p = m_glWidget->AddMesh(vertices, indices);
    p->m_position.setZ(-40);
}

void BasicGLWindow::keyPressEvent(QKeyEvent * event)
{
}

void BasicGLWindow::mousePressEvent(QMouseEvent * event)
{
    m_mouseLastPos = event->pos();
}

void BasicGLWindow::mouseMoveEvent(QMouseEvent * event)
{
    int dx = event->x() - m_mouseLastPos.x();
    int dy = event->y() - m_mouseLastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        m_glWidget->RotateAll(QVector3D(8.f * dy, 8.f * dx, 0.f));
    }
    else if (event->buttons() & Qt::RightButton) {
        m_glWidget->RotateAll(QVector3D(8.f * dy, 0.f, 8.f * dx));
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        m_glWidget->TranslateAll(QVector3D(dx / 4.f, -dy / 4.f, 0.f));
    }
    m_mouseLastPos = event->pos();
}

void BasicGLWindow::mouseReleaseEvent(QMouseEvent * event)
{
}

void BasicGLWindow::wheelEvent(QWheelEvent * event)
{
    const int degrees = event->delta() / 8;
    if (degrees)
        m_glWidget->TranslateAll(QVector3D(0.f, 0.f, degrees / 10.f));
}

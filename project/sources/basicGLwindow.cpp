#include "../headers/basicglwidget.h"
#include "../headers/basicGLwindow.h"
#include <qlayout.h>

#include <iostream>

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

    // FPS
    m_showFps = false;
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
    switch (event->key()) {
    case Qt::Key_B:
        // Change the background color
        std::cout << "-- AGEn message --: Change background color" << std::endl;
        m_glWidget->changeBackgroundColor(QColor::fromRgb(rand() % 255, rand() % 255, rand() % 255));
        break;
    case Qt::Key_F:
        // Enable/Disable frames per second
        m_showFps = !m_showFps;

        // TO DO: Show or hide the FPS information

        break;
    case Qt::Key_H:
        // Show the help message
        std::cout << "-- AGEn message --: Help" << std::endl;
        std::cout << std::endl;
        std::cout << "Keys used in the application:" << std::endl;
        std::cout << std::endl;
        std::cout << "-B:  change background color" << std::endl;
        std::cout << "-F:  show frames per second (fps)" << std::endl;
        std::cout << "-H:  show this help" << std::endl;
        std::cout << "-R:  reset the camera parameters" << std::endl;
        std::cout << "-F5: reload shaders" << std::endl;
        std::cout << std::endl;
        std::cout << "IMPORTANT: the focus must be set to the glwidget to work" << std::endl;
        std::cout << std::endl;
        break;
    case Qt::Key_R:
        // Reset the camera and scene parameters
        std::cout << "-- AGEn message --: Reset camera" << std::endl;
        m_glWidget->ResetCamera();
        m_glWidget->ResetScene();
        break;
    case Qt::Key_F5:
        // Reload shaders
        std::cout << "-- AGEn message --: Reload shaders" << std::endl;
        m_glWidget->reloadShaders();
        break;
    default:
        event->ignore();
        break;
    }
}

void BasicGLWindow::showFps()
{
    //TODO
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

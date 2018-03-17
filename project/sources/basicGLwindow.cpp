#include "../headers/basicglwidget.h"
#include "../headers/basicGLwindow.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qinputdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <iostream>

BasicGLWindow::BasicGLWindow(QString name)
    : BasicWindow(name)
{
    m_ui.setupUi(this);

    m_glWidget = new BasicGLWidget;

    QVBoxLayout* layoutInputs = new QVBoxLayout();
	// Insert the m_glWidget in the GUI
	QHBoxLayout* layoutFrame = new QHBoxLayout(m_ui.qGLFrame);
	layoutFrame->setMargin(0);
	layoutFrame->addWidget(m_glWidget);
    layoutFrame->addLayout(layoutInputs);

    QLabel* fps = new QLabel("Fps:");
    layoutInputs->addWidget(fps);
    m_fpsLabel = new QLabel("00");
    layoutInputs->addWidget(m_fpsLabel);

    QLabel* title = new QLabel("Controls:");
    layoutInputs->addWidget(title);

    m_moveSceneCheckbox = new QCheckBox("Move Scene");
    layoutInputs->addWidget(m_moveSceneCheckbox);

    m_moveCameraCheckbox = new QCheckBox("Move Camera");
    layoutInputs->addWidget(m_moveCameraCheckbox);

    layoutInputs->addStretch(1);


    connect(m_moveCameraCheckbox, &QCheckBox::stateChanged, this, &BasicGLWindow::SLOT_MoveCameraCheckbox);
    connect(m_moveSceneCheckbox, &QCheckBox::stateChanged, this, &BasicGLWindow::SLOT_MoveSceneCheckbox);
    connect(m_glWidget, &BasicGLWidget::UpdatedFPS, this, &BasicGLWindow::SLOT_UpdateFPS);
    m_moveSceneCheckbox->setCheckState(Qt::CheckState::Checked);

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

void BasicGLWindow::SLOT_MoveSceneCheckbox(int val)
{
    m_movingCamera = !val;
    if (m_movingCamera)
    {
        m_moveCameraCheckbox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        m_moveCameraCheckbox->setCheckState(Qt::CheckState::Unchecked);
    }
}

void BasicGLWindow::SLOT_MoveCameraCheckbox(int val)
{
    m_movingCamera = val;
    if (m_movingCamera)
    {
        m_moveSceneCheckbox->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        m_moveSceneCheckbox->setCheckState(Qt::CheckState::Checked);
    }
}

void BasicGLWindow::SLOT_UpdateFPS(float FPS)
{
    m_fpsLabel->setText(QString::number(FPS));
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
    case Qt::Key_W:
        m_glWidget->TranslateCamera(m_glWidget->GetCameraForward());
        break;
    case Qt::Key_S:
        m_glWidget->TranslateCamera(-m_glWidget->GetCameraForward());
        break;
    case Qt::Key_D:
        m_glWidget->TranslateCamera(m_glWidget->GetCameraRight());
        break;
    case Qt::Key_A:
        m_glWidget->TranslateCamera(-m_glWidget->GetCameraRight());
        break;
    case Qt::Key_Q:
        m_glWidget->TranslateCamera(m_glWidget->GetCameraUp());
        break;
    case Qt::Key_E:
        m_glWidget->TranslateCamera(-m_glWidget->GetCameraUp());
        break;
    default:
        event->ignore();
        break;
    }
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
        if(m_movingCamera)
            m_glWidget->RotateCamera(QVector3D(dy, dx, 0.f));
        else
            m_glWidget->RotateAll(QVector3D(dy, dx, 0.f));
    }

    else if (event->buttons() & Qt::RightButton) {
        if(!m_movingCamera)
            m_glWidget->RotateAll(QVector3D(dy, 0.f, dx));
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        if (m_movingCamera)
            m_glWidget->TranslateCamera(QVector3D(-dx, -dy, 0.f));
        else
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
    {
        m_glWidget->TranslateAll(QVector3D(0.f, 0.f, degrees / 10.f));
    }
}

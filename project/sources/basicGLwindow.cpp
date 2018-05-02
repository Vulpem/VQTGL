#include "../headers/basicGLwindow.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qinputdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qspinbox.h>
#include <iostream>

BasicGLWindow::BasicGLWindow(QString name)
    : BasicWindow(name)
    , m_inputMovement(InputMovement::FPScamera)
    , m_whatToDraw(BasicGLWidget::WhatToDraw::finalImage)
{
    m_ui.setupUi(this);

    m_glWidget = new BasicGLWidget("./project/models/mech.OBJ");
    m_glWidget->m_whatToDraw = m_whatToDraw;
    m_glWidget->m_SSAORadius = m_ui.SSAORadius->value();

	m_glWidgetContainer = new QVBoxLayout(m_ui.qGLFrame);
	m_glWidgetContainer->setMargin(0);
	m_glWidgetContainer->addWidget(m_glWidget);
	m_glWidget->show();


    QGraphicsScene *scene = new QGraphicsScene();
    scene->clear();
    scene->addText("(Empty)");

    m_ui.qTex1View->resetMatrix();
    m_ui.qTex1View->setScene(scene);
    m_ui.qTex1View->show();

    m_ui.qTex2View->resetMatrix();
    m_ui.qTex2View->setScene(scene);
    m_ui.qTex2View->show();


	connect(m_ui.qLoadModelButton, &QPushButton::clicked, this, &BasicGLWindow::SLOT_LoadModel);
	connect(m_ui.MoveControlsComboBox, &QComboBox::currentTextChanged, this, &BasicGLWindow::SLOT_ChangedInputMovement);
    connect(m_ui.WhatToRender, &QComboBox::currentTextChanged, this, &BasicGLWindow::SLOT_ChangedWhatToDraw);
    connect(m_ui.SSAORadius, SIGNAL(valueChanged(double)), this, SLOT(SLOT_ChangedSSAORadius(double)));

	connect(m_glWidget, &BasicGLWidget::UpdatedFPS, this, &BasicGLWindow::SLOT_UpdateFPS);

    connect(m_ui.qLoadTex1Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_LoadTexture);
    connect(m_ui.qDeleteTex1Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_UnloadTexture);
    connect(m_ui.qLoadTex2Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_LoadTexture2);
    connect(m_ui.qDeleteTex2Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_UnloadTexture2);

	show();
}

BasicGLWindow::~BasicGLWindow()
{
	if (m_glWidget != nullptr) {
		delete m_glWidget;
		m_glWidget = nullptr;
	}
}

void BasicGLWindow::SLOT_ChangedInputMovement(QString val)
{
	if (val == "Camera")
	{
		m_inputMovement = InputMovement::FPScamera;
	}
	else if (val == "Scene")
	{
		m_inputMovement = InputMovement::scene;
	}
}

void BasicGLWindow::SLOT_ChangedWhatToDraw(QString val)
{
    if (val == "Final Image")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::finalImage;
    }
    else if (val == "Simple render")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::simpleRender;
    }
    else if (val == "Depth")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::depth;
    }
    else if (val == "Normals")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::normals;
    }
    else if (val == "Ambient Occlusion")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::ambientOcclusion;
    }
    else if (val == "Position")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::position;
    }
    else if (val == "Blurred AO")
    {
        m_whatToDraw = BasicGLWidget::WhatToDraw::smoothAO;
    }
    m_glWidget->m_whatToDraw = m_whatToDraw;
    m_glWidget->update();
}

void BasicGLWindow::SLOT_UpdateFPS(float FPS)
{
    //m_fpsLabel->setText(QString::number(FPS));
}

void BasicGLWindow::SLOT_LoadModel()
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

		m_glWidget = new BasicGLWidget(filename);
        m_glWidget->m_whatToDraw = m_whatToDraw;
        m_glWidget->m_SSAORadius = m_ui.SSAORadius->value();
		m_glWidgetContainer->addWidget(m_glWidget);
		m_glWidget->show();
	}
}

void BasicGLWindow::SLOT_LoadTexture()
{
    m_filenameTex1 = QFileDialog::getOpenFileName(this, tr("Load Texture"),
        "./project/textures", tr("*.png;;*.jpg *.jpeg;;*.bmp;;*.gif;;*.pbm *.pgm *.ppm;;*.xbm *.xpm;;All files (*.*)"));

    if (m_filenameTex1.size() != 0) {
        m_glWidget->LoadTexture(m_filenameTex1, 0);

        QGraphicsScene* texture = new QGraphicsScene();
        texture->addPixmap(m_filenameTex1);
        m_ui.qTex1View->setScene(texture);
        m_ui.qTex1View->fitInView(texture->itemsBoundingRect());
        m_ui.qTex1View->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        m_ui.qTex1View->show();
    }
}

void BasicGLWindow::SLOT_UnloadTexture()
{
    m_glWidget->UnloadTexture(0);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->clear();
    scene->addText("(Empty)");

    m_ui.qTex1View->resetMatrix();
    m_ui.qTex1View->setScene(scene);
    m_ui.qTex1View->show();

    m_filenameTex1.clear();
}

void BasicGLWindow::SLOT_LoadTexture2()
{
    m_filenameTex2 = QFileDialog::getOpenFileName(this, tr("Load Texture"),
        "./project/textures", tr("*.png;;*.jpg *.jpeg;;*.bmp;;*.gif;;*.pbm *.pgm *.ppm;;*.xbm *.xpm;;All files (*.*)"));

    if (m_filenameTex2.size() != 0) {
        m_glWidget->LoadTexture(m_filenameTex2, 1);

        QGraphicsScene* texture = new QGraphicsScene();
        texture->addPixmap(m_filenameTex2);
        m_ui.qTex2View->setScene(texture);
        m_ui.qTex2View->fitInView(texture->itemsBoundingRect());
        m_ui.qTex2View->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        m_ui.qTex2View->show();
    }
}

void BasicGLWindow::SLOT_UnloadTexture2()
{
    m_glWidget->UnloadTexture(1);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->clear();
    scene->addText("(Empty)");

    m_ui.qTex2View->resetMatrix();
    m_ui.qTex2View->setScene(scene);
    m_ui.qTex2View->show();

    m_filenameTex2.clear();
}

void BasicGLWindow::SLOT_ChangedSSAORadius(double val)
{
    m_glWidget->m_SSAORadius = val;
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
        if(m_inputMovement == InputMovement::FPScamera)
            m_glWidget->RotateCamera(QVector3D(-dy, -dx, 0.f));
        else
            m_glWidget->RotateAll(QVector3D(dy, dx, 0.f));
    }

    else if (event->buttons() & Qt::RightButton) {
        if(m_inputMovement != InputMovement::FPScamera)
            m_glWidget->RotateAll(QVector3D(dy, 0.f, dx));
    }
    else if (event->buttons() & Qt::MiddleButton)
    {
        if (m_inputMovement == InputMovement::FPScamera)
            m_glWidget->TranslateCamera(m_glWidget->GetCameraRight() * -dx + m_glWidget->GetCameraUp() * dy);
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
		if (m_inputMovement == InputMovement::FPScamera)
		{
			m_glWidget->TranslateCamera(m_glWidget->GetCameraForward() * degrees / 10.f);
		}
		else
		{
			m_glWidget->TranslateAll(QVector3D(0.f, 0.f, degrees / 10.f));
		}
    }
}

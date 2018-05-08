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

	InitSSAOGUI();

	initRaytracingGUI();

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
	m_glWidget->update();
}

void BasicGLWindow::SLOT_ChangedSSAOSlider(int val)
{
	m_ui.SSAORadius->setValue((float)val / 100000.f);
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
    QPoint pos = event->pos();
    if (IsOverGLWidget(pos))
    {
        m_mouseLastPos = event->pos();
        m_moving = true;
    }
}

void BasicGLWindow::mouseMoveEvent(QMouseEvent * event)
{
    QPoint pos = event->pos();
    if (m_moving)
    {
        int dx = event->x() - m_mouseLastPos.x();
        int dy = event->y() - m_mouseLastPos.y();
        if (event->buttons() & Qt::LeftButton) {
            if (m_inputMovement == InputMovement::FPScamera)
                m_glWidget->RotateCamera(QVector3D(-dy / 3.f, -dx / 3.f, 0.f));
            else
                m_glWidget->RotateAll(QVector3D(dy, dx, 0.f));
        }

        else if (event->buttons() & Qt::RightButton) {
            if (m_inputMovement != InputMovement::FPScamera)
                m_glWidget->RotateAll(QVector3D(dy / 3.f, 0.f, dx / 3.f));
        }
        else if (event->buttons() & Qt::MiddleButton)
        {
            if (m_inputMovement == InputMovement::FPScamera)
                m_glWidget->TranslateCamera(m_glWidget->GetCameraRight() * -dx + m_glWidget->GetCameraUp() * dy);
            else
                m_glWidget->TranslateAll(QVector3D(dx /7.f, -dy / 7.f, 0.f));
        }
    }
    m_mouseLastPos = event->pos();
}

void BasicGLWindow::mouseReleaseEvent(QMouseEvent * event)
{
    m_moving = false;
}

void BasicGLWindow::wheelEvent(QWheelEvent * event)
{
    QPoint pos = event->pos();
    if (IsOverGLWidget(pos))
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
}

bool BasicGLWindow::IsOverGLWidget(QPoint pos)
{
    const QPoint min = m_glWidget->pos();
    const QPoint max = QPoint(min.x() + m_glWidget->width(), min.y() + m_glWidget->height());
    return ( pos.x() > min.x() && pos.y() > min.y() && pos.x() < max.x() && pos.y() < max.y());
}


// Raytracing

void BasicGLWindow::InitSSAOGUI()
{
	m_glWidget = new BasicGLWidget("./project/models/mech.obj");
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
	connect(m_ui.SSAOSlider, SIGNAL(valueChanged(int)), this, SLOT(SLOT_ChangedSSAOSlider(int)));


	connect(m_glWidget, &BasicGLWidget::UpdatedFPS, this, &BasicGLWindow::SLOT_UpdateFPS);

	connect(m_ui.qLoadTex1Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_LoadTexture);
	connect(m_ui.qDeleteTex1Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_UnloadTexture);
	connect(m_ui.qLoadTex2Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_LoadTexture2);
	connect(m_ui.qDeleteTex2Button, &QPushButton::clicked, this, &BasicGLWindow::SLOT_UnloadTexture2);
}


void BasicGLWindow::render(const std::vector<Sphere> &spheres)
{
	m_width = m_ui.qRayTracingView->width() - 2;
	m_height = m_ui.qRayTracingView->height() - 2;

	glm::vec3 *image = new glm::vec3[m_width * m_height], *pixel = image;
	float invWidth = 1 / float(m_width), invHeight = 1 / float(m_height);
	float fov = 30, aspectratio = m_width / float(m_height);
	float angle = tan(PI * 0.5 * fov / 180.);

	int progress = 0;
	int numPixels = m_width * m_height;

	// Trace rays
	for (unsigned y = 0; y < m_height; ++y) {
		for (unsigned x = 0; x < m_width; ++x, ++pixel) {
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			glm::vec3 rayDir(xx, yy, -1);
			rayDir = glm::normalize(rayDir);
			glm::vec3 rayOrig(0.0f, 0.0f, 0.0f);
			*pixel = traceRay(rayOrig, rayDir, spheres, 0);

			progress++;
			emit renderingProgress((int)((float)progress / (float)numPixels * 100));
		}
	}

	QImage img(m_width, m_height, QImage::Format_ARGB32);

	int id_img = 0;
	for (unsigned j = 0; j < m_height; ++j) {
		for (unsigned i = 0; i < m_width; ++i) {
			QColor col(MIN(255, image[id_img].x * 255),
				MIN(255, image[id_img].y * 255),
				MIN(255, image[id_img].z * 255));
			img.setPixelColor(i, j, col);
			id_img++;
		}
	}

	QGraphicsScene* imgView = new QGraphicsScene();
	imgView->addPixmap(QPixmap::fromImage(img));
	m_ui.qRayTracingView->setScene(imgView);
	m_ui.qRayTracingView->show();

	delete[] image;
}

void BasicGLWindow::SLOT_raytraceScene() {
	std::vector<Sphere> spheres;

	// Lights
	spheres.push_back(Sphere(glm::vec3(10.0f, 20.0f, 0.0f), 2, glm::vec3(0.0f, 0.0f, 0.0f), false, 0.0f, 0.0f, 2.0f, glm::vec3(1.0f, 1.0f, 1.0f)));
	spheres.push_back(Sphere(glm::vec3(-10.0f, 20.0f, 0.0f), 2, glm::vec3(0.0f, 0.0f, 0.0f), false, 0.0f, 0.0f, 2.0f, glm::vec3(1.0f, 1.0f, 1.0f)));
	spheres.push_back(Sphere(glm::vec3(0.0f, 10.0f, 0.0f), 2, glm::vec3(0.0f, 0.0f, 0.0f), false, 0.0f, 0.0f, 2.0f, glm::vec3(1.0f, 1.0f, 1.0f)));

	// Spheres of the scene
	spheres.push_back(Sphere(glm::vec3(0.0, -10004, -30), 10000, glm::vec3(0.0f, 0.2f, 0.5f), false, 0.0, 0.0));
	spheres.push_back(Sphere(glm::vec3(0.0f, 0.0f, -20.0f), 2, glm::vec3(1.0f, 1.0f, 1.0f), true, 0.9f, 1.1f));
	spheres.push_back(Sphere(glm::vec3(4.0f, 0.0f, -32.5f), 4, glm::vec3(0.0f, 0.5f, 0.0f), true, 0.0f, 0.0f));
	spheres.push_back(Sphere(glm::vec3(-5.0f, 0.0f, -35.0f), 3, glm::vec3(0.5f, 0.5f, 0.5f), true, 0.0f, 0.0f));
	spheres.push_back(Sphere(glm::vec3(-4.5f, -1.0f, -19.0f), 1.5f, glm::vec3(0.5f, 0.1f, 0.0f), true, 0.0f, 0.0f));

	render(spheres);
}

void BasicGLWindow::initRaytracingGUI()
{
	m_width = m_ui.qRayTracingView->width() - 2;
	m_height = m_ui.qRayTracingView->height() - 2;
	background_color = glm::vec3(1.0f, 1.0f, 1.0f);

	connect(m_ui.qRenderButton, SIGNAL(clicked()), this, SLOT(SLOT_raytraceScene()));
	connect(this, SIGNAL(renderingProgress(int)), m_ui.qProgressBar, SLOT(setValue(int)));

	QGraphicsScene *scene = new QGraphicsScene();
	scene->clear();
	scene->addText("(Empty)");

	m_ui.qRayTracingView->resetMatrix();
	m_ui.qRayTracingView->setScene(scene);
	m_ui.qRayTracingView->show();
}

glm::vec3 BasicGLWindow::traceRay(
	const glm::vec3 &rayOrig,
	const glm::vec3 &rayDir,
	const std::vector<Sphere> &spheres,
	const int &depth)
{
	glm::vec3 color = glm::vec3(1.f, 0.f, 1.f);
	Intersection hit;
	std::for_each(spheres.begin(), spheres.end(), [=, &hit](const Sphere& s)
	{
		Intersection newHit;
		newHit = intersection(s, rayOrig, rayDir);
		if(newHit.intersected && newHit.distHit < hit.distHit) { hit = newHit; }
	} );

	if (hit.intersected)
	{
		color = hit.colorHit;
	}
	// TO DO
	return color;
}

BasicGLWindow::Intersection BasicGLWindow::intersection(
	const Sphere &sphere,
	const glm::vec3 &rayOrig,
	const glm::vec3 &rayDir) {

	float inter0 = INFINITY;
	float inter1 = INFINITY;

	Intersection ret;

	if (sphere.intersect(rayOrig, rayDir, inter0, inter1))
	{
		if (inter0 < 0)
			inter0 = inter1;

		ret.intersected = true;
		ret.distHit = inter0;
		ret.posHit = rayOrig + rayDir * inter0;
		ret.normalHit = ret.posHit - sphere.getCenter();
		ret.normalHit = glm::normalize(ret.normalHit);

		// If the normal and the view direction are not opposite to each other
		// reverse the normal direction. That also means we are inside the sphere so set
		// the inside bool to true.
		ret.isInside = false;
		float dotProd = glm::dot(rayDir, ret.normalHit);

		if (dotProd > 0) {
			ret.normalHit = -ret.normalHit;
			ret.isInside = true;
		}

		ret.colorHit = sphere.getSurfaceColor();
	}
	return ret;
}

glm::vec3 BasicGLWindow::blendReflRefrColors(
	const Sphere* sphere,
	const glm::vec3 &raydir,
	const glm::vec3 &normalHit,
	const glm::vec3 &reflColor,
	const glm::vec3 &refrColor) {

	float facingRatio = -glm::dot(raydir, normalHit);
	float fresnel = 0.5f + pow(1 - facingRatio, 3) * 0.5;

	glm::vec3 blendedColor = (reflColor * fresnel + refrColor * (1 - fresnel) * sphere->transparencyFactor())*sphere->getSurfaceColor();
	return blendedColor;
}
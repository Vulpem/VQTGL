#include "../headers/basicglwidget.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QColorDialog>
#include <QPainter>
#include <math.h>

#include <gl\GL.h>

#include <iostream>
#include <gl\GLU.h>



Vertex::Vertex(float3 position, float3 normal, float2 UVs, float3 col)
    : pos(position)
    , norm(normal)
    , UV(UVs)
    , color(col)
{}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices)
    : m_position(0.f, 0.f, 0.f)
    , m_scale(1.f, 1.f, 1.f)
    , m_rotation(0.f, 0.f, 0.f)
    , m_dataBuf(QOpenGLBuffer::Type::VertexBuffer)
    , m_indicesBuf(QOpenGLBuffer::Type::IndexBuffer)
{
    m_numIndices = indices.size();

    m_indicesBuf.create();
    m_indicesBuf.bind();
    m_indicesBuf.allocate(indices.data(), sizeof(uint) * m_numIndices);

    m_dataBuf.create();
    m_dataBuf.bind();
    m_dataBuf.allocate(vertices.data(), sizeof(Vertex) * vertices.size());
}

Mesh::~Mesh()
{
    m_dataBuf.destroy();
    m_dataBuf.destroy();
}

QMatrix4x4 Mesh::GetTransform()
{
    QMatrix4x4 ret;
	ret.setToIdentity();
	ret.translate(m_position);
	ret.rotate(QQuaternion::fromEulerAngles(m_rotation));
    ret.scale(m_scale);
    return ret;
}


BasicGLWidget::BasicGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
	// To receive key events
	setFocusPolicy(Qt::StrongFocus);

	// Attributes initialization
	// Screen
	m_width = 500;
	m_height = 500;
	
	// Scene
	m_sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	m_sceneRadius = 50.0f;
	m_bgColor = Qt::black;
	m_backFaceCulling = false;

	// Shaders
	m_program = nullptr;

	// FPS
	m_showFps = false;
	m_frameCount = 0;
	m_FPS = 0;
}

BasicGLWidget::~BasicGLWidget()
{
    cleanup();
}

QSize BasicGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize BasicGLWidget::sizeHint() const
{
    return QSize(m_width, m_height);
}

MeshPtr BasicGLWidget::AddMesh(const std::vector<Vertex>& vertices, const std::vector<uint>& indices)
{
    MeshPtr p = std::make_shared<Mesh>(vertices, indices);
    m_meshes.push_back(p);
    return AddMesh(p);
}

MeshPtr BasicGLWidget::AddMesh(MeshPtr mesh)
{
    m_meshes.push_back(mesh);
    update();
    return mesh;
}

void BasicGLWidget::RotateAll(QVector3D rotationEuler)
{
    for (auto mesh : m_meshes)
    {
        mesh->m_rotation += rotationEuler;
    }
    update();
}

void BasicGLWidget::SetRotationAll(QVector3D rotationEuler)
{
    for (auto mesh : m_meshes)
    {
        mesh->m_rotation = rotationEuler;
    }
    update();
}

void BasicGLWidget::TranslateAll(QVector3D translation)
{
    for (auto mesh : m_meshes)
    {
        mesh->m_position += translation;
    }
    update();
}

void BasicGLWidget::SetTranslationAll(QVector3D position)
{
    for (auto mesh : m_meshes)
    {
        mesh->m_position = position;
    }
    update();
}

void BasicGLWidget::ResetScene()
{
    SetRotationAll(QVector3D(0.f, 0.f, 0.f));
    SetTranslationAll(QVector3D(0.f, 0.f, -40.f));
}

std::vector<MeshPtr> BasicGLWidget::GetMeshes()
{
    return m_meshes;
}

QVector3D BasicGLWidget::GetCameraPosition()
{
    return m_cameraPosition;
}

void BasicGLWidget::SetCameraPosition(QVector3D position)
{
    m_cameraPosition = position;
    viewTransform();
}

void BasicGLWidget::TranslateCamera(QVector3D translation)
{
    SetCameraPosition(m_cameraPosition + translation);
}

QVector3D BasicGLWidget::GetCameraRotation()
{
    return m_cameraRotation;
}

void BasicGLWidget::SetCameraRotation(QVector3D rotation)
{
    m_cameraRotation = rotation;
    viewTransform();
}

void BasicGLWidget::RotateCamera(QVector3D rotation)
{
    SetCameraRotation(m_cameraRotation + rotation);
}

void BasicGLWidget::cleanup()
{
	makeCurrent();
	
	if (m_program == nullptr)
        return;
    
	delete m_program;
    m_program = 0;
    
	doneCurrent();
}

void BasicGLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &BasicGLWidget::cleanup);
    initializeOpenGLFunctions();
 	loadShaders();
	projectionTransform();
	viewTransform();

    computeBBoxScene();
}

void BasicGLWidget::paintGL()
{
	// FPS computation
	computeFps();
	
	// Paint the scene
	glClearColor(m_bgColor.red() / 255.0f, m_bgColor.green() / 255.0f, m_bgColor.blue() / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	if (m_backFaceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	

	//Quad drawing
	m_program->bind();

    for (auto mesh : m_meshes)
    {
        mesh->m_dataBuf.bind();
        mesh->m_indicesBuf.bind();

        glVertexAttribPointer(m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)0);
        glEnableVertexAttribArray(m_vertexLoc);

        glVertexAttribPointer(m_normalLoc, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
        glEnableVertexAttribArray(m_normalLoc);

        glVertexAttribPointer(m_UVLoc, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
        glEnableVertexAttribArray(m_UVLoc);

        glVertexAttribPointer(m_colorLoc, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(8 * sizeof(float)));
        glEnableVertexAttribArray(m_colorLoc);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Apply the geometric transforms to the scene (position/orientation)
        meshTransform(mesh);

        glDrawElements(GL_TRIANGLES, (GLint)mesh->GetNIndices(), GL_UNSIGNED_INT, (void*)0);
    }

	m_program->release();

	if (m_showFps)
		showFps();
}

void BasicGLWidget::resizeGL(int w, int h)
{
	m_width = w;
	m_height = h;
	
	glViewport(0, 0, m_width, m_height);
	m_ar = (float)m_width / (float)m_height;
	
	// We do this if we want to preserve the initial fov when resizing
	if (m_ar < 1.0f) {
		m_fov = 2.0f*atan(tan(m_fovIni / 2.0f) / m_ar) + m_radsZoom;
	}
	else {
		m_fov = m_fovIni + m_radsZoom;
	}

	// After modifying the parameters, we update the camera projection
	projectionTransform();
}

void BasicGLWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_B:
			// Change the background color
			std::cout << "-- AGEn message --: Change background color" << std::endl;
			changeBackgroundColor(QColor::fromRgb(rand() % 255, rand() % 255, rand() % 255));
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
			ResetCamera();
            ResetScene();
			break;
		case Qt::Key_F5: 
			// Reload shaders
			std::cout << "-- AGEn message --: Reload shaders" << std::endl;
			reloadShaders();
			break;
		default:
			event->ignore();
			break;
	}
}

void BasicGLWidget::loadShaders()
{
	std::cout << "Loading Shaders: \n";

	// Declaration of the shaders
	QOpenGLShader vs(QOpenGLShader::Vertex, this);
	QOpenGLShader fs(QOpenGLShader::Fragment, this);
	QOpenGLShader geom(QOpenGLShader::Geometry, this);

	// Load and compile the shaders
	vs.compileSourceFile("./project/shaders/basicgl.vert");
	fs.compileSourceFile("./project/shaders/basicgl.frag");
	geom.compileSourceFile("./project/shaders/basicgl.geom");

	// Create the program
	m_program = new QOpenGLShaderProgram;

	// Add the shaders
	m_program->addShader(&fs);
	m_program->addShader(&vs);
	m_program->addShader(&geom);

	// Link the program
	m_program->link();

	// Bind the program (we are gonna use this program)
	m_program->bind();

	// Get the attribs locations of the vertex shader
	m_vertexLoc = glGetAttribLocation(m_program->programId(), "vertex");
	m_normalLoc = glGetAttribLocation(m_program->programId(), "normal");
	m_UVLoc = glGetAttribLocation(m_program->programId(), "UV");
	m_colorLoc = glGetAttribLocation(m_program->programId(), "color");

	std::cout << "	Attribute locations \n";
	std::cout << "		vertex:		" << m_vertexLoc << "\n";
	std::cout << "		normal:		" << m_normalLoc << "\n";
	std::cout << "		UVs:		" << m_UVLoc << "\n";
	std::cout << "		color:		" << m_colorLoc << "\n";

	// Get the uniforms locations of the vertex shader
	m_projLoc = m_program->uniformLocation("projTransform");
	m_viewLoc = m_program->uniformLocation("viewTransform");
	m_transLoc = m_program->uniformLocation("sceneTransform");

	std::cout << "	Uniform locations \n";
	std::cout << "		projection transform:		" << m_projLoc << "\n";
	std::cout << "		view transform:			" << m_viewLoc << "\n";
	std::cout << "		scene transform:		" << m_transLoc << "\n";

	m_program->release();
}

void BasicGLWidget::reloadShaders()
{
	if (m_program == nullptr)
		return;
	makeCurrent();
	delete m_program;
	m_program = 0;
	loadShaders();
	update();
}

void BasicGLWidget::projectionTransform()
{
    m_program->bind();

	// Set the camera type
	QMatrix4x4 proj;
	proj.setToIdentity();
	
	// TO DO: Set the camera parameters 
	m_zNear = m_sceneRadius;
	m_zFar = 3.0f * m_sceneRadius;

	proj.perspective(45.0f, GLfloat(m_width) / m_height, 0.01f, 100.0f);

	// Send the matrix to the shader
	m_program->setUniformValue(m_projLoc, proj);

    m_program->release();
}

void BasicGLWidget::ResetCamera()
{
    SetCameraRotation(QVector3D(0.f, 0.f, 0.f));
    SetCameraPosition(QVector3D(0.f, 0.f, 0.f));
}

void BasicGLWidget::viewTransform()
{
    m_program->bind();

	// Set the camera position
	QMatrix4x4 view;
	view.setToIdentity();
	view.translate(m_cameraPosition);
    view.translate(0.f,0.f,-2.f);
    view.rotate(QQuaternion::fromEulerAngles(m_cameraRotation));

	// Send the matrix to the shader
	m_program->setUniformValue(m_viewLoc, view);

    m_program->release();
}

void BasicGLWidget::changeBackgroundColor(QColor color)
{
	m_bgColor = color;
	update();
}

void BasicGLWidget::computeBBoxScene()
{
	// Right now we have just a quad of 20x20x0
	m_sceneRadius = sqrt(20 * 20 + 20 * 20)/2.0f;
	m_sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BasicGLWidget::meshTransform(MeshPtr mesh)
{
	// Send the matrix to the shader
	m_program->setUniformValue(m_transLoc, mesh->GetTransform());
}

void BasicGLWidget::computeFps() 
{
	if (m_frameCount == 0)
	{
		m_FPSTimer.start();
	}

	if (m_FPSTimer.elapsed()/1000.f >= 1.f)
	{
		m_FPS = m_frameCount;
		m_frameCount = 0;
		m_FPSTimer.restart();
	}

	m_frameCount++;
}

void BasicGLWidget::showFps()
{
	
	// TO DO: Show the FPS

}

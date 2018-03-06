#include "../headers/basicglwidget.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QColorDialog>
#include <QPainter>
#include <math.h>

#include <iostream>

struct float3
{
    float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    float x, y, z;
};

struct float2
{
    float2(float _x, float _y) : x(_x), y(_y) {}
    float x, y;
};

struct Vertex
{
    Vertex(float3 position, float3 normal = { 0.f,0.f,-1.f }, float2 UVs = { 0.f,0.f }, float3 col = { 1.f,1.f,1.f }) : pos(position), norm(normal), UV(UVs), color(col) {}
    float3 pos;
    float3 norm;
    float2 UV;
    float3 color;
};

BasicGLWidget::BasicGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
	// To receive key events
	setFocusPolicy(Qt::StrongFocus);

	// Attributes initialization
	// Screen
	m_width = 500;
	m_height = 500;

    //Camera

    /*m_ar;
    m_fov;
    m_fovIni;
    m_zNear;
    m_zFar;
    m_radsZoom;
    m_xPan;
    m_yPan;*/
	
	// Scene
	m_sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	m_sceneRadius = 50.0f;
	m_bgColor = Qt::black;
	m_backFaceCulling = false;

    // Mouse
    /*m_xClick;
    m_yClick;
    m_xRot;
    m_yRot;*/

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

void BasicGLWidget::cleanup()
{
	makeCurrent();
	
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &m_buf_data);
	glDeleteBuffers(1, &m_buf_indices);
	
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
	createBuffersScene();
	computeBBoxScene();
	projectionTransform();
	viewTransform();
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
	
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	
	m_program->bind();

    glBindBuffer(GL_ARRAY_BUFFER, m_buf_data);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buf_indices);

    glVertexAttribPointer(m_vertexLoc, 3, GL_FLOAT, false, 11 * sizeof(float), (GLvoid*)0);
    glVertexAttribPointer(m_normalLoc, 3, GL_FLOAT, false, 11 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glVertexAttribPointer(m_UVLoc,     2, GL_FLOAT, false, 11 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    glVertexAttribPointer(m_colorLoc,  3, GL_FLOAT, false, 11 * sizeof(float), (GLvoid*)(8 * sizeof(float)));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Apply the geometric transforms to the scene (position/orientation)
	sceneTransform();

    glDrawElements(GL_TRIANGLES,m_nIndices, GL_UNSIGNED_INT, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
			resetCamera();
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

void BasicGLWidget::mousePressEvent(QMouseEvent *event)
{

	// TO DO: Rotation of the scene and PAN

}

void BasicGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	
	// TO DO: Rotation of the scene and PAN

}

void BasicGLWidget::mouseReleaseEvent(QMouseEvent *event)
{

	// TO DO: Rotation of the scene and PAN

}

void BasicGLWidget::wheelEvent(QWheelEvent* event)
{

	// TO DO: Change the fov of the camera to zoom in and out	

}

void BasicGLWidget::loadShaders()
{
	// Declaration of the shaders
	QOpenGLShader vs(QOpenGLShader::Vertex, this);
	QOpenGLShader fs(QOpenGLShader::Fragment, this);

	// Load and compile the shaders
	vs.compileSourceFile("./shaders/basicgl.vert");
	fs.compileSourceFile("./shaders/basicgl.frag");

	// Create the program
	m_program = new QOpenGLShaderProgram;

	// Add the shaders
	m_program->addShader(&fs);
	m_program->addShader(&vs);

	// Link the program
	m_program->link();

	// Bind the program (we are gonna use this program)
	m_program->bind();

	uint a = m_program->programId();

	// Get the attribs locations of the vertex shader
	m_vertexLoc = 1;// glGetAttribLocation(m_program->programId(), "vertex");
	m_normalLoc = 2;// glGetAttribLocation(m_program->programId(), "normal");
	m_UVLoc = 3;// glGetAttribLocation(m_program->programId(), "UV");
	m_colorLoc = 4;// glGetAttribLocation(m_program->programId(), "color");

	// Get the uniforms locations of the vertex shader
	m_transLoc = glGetUniformLocation(m_program->programId(), "sceneTransform");
	m_projLoc = glGetUniformLocation(m_program->programId(), "projTransform");
	m_viewLoc = glGetUniformLocation(m_program->programId(), "viewTransform");
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
	// Set the camera type
	glm::mat4 proj(1.0f);
	

	// TO DO: Set the camera parameters 
	

	// Send the matrix to the shader
	glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, &proj[0][0]);

}

void BasicGLWidget::resetCamera()
{

	// TO DO: Reset the camera/view parameters

}

void BasicGLWidget::viewTransform()
{
	// Set the camera position
	glm::mat4 view(1.0f);


	// TO DO: Camera placement and PAN
	

	// Send the matrix to the shader
	glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, &view[0][0]);
}

void BasicGLWidget::changeBackgroundColor(QColor color)
{
	m_bgColor = color;
	update();
}

void BasicGLWidget::createBuffersScene()
{
    // TO DO: Create the buffers, initialize VAO, VBOs, etc.
    std::vector<Vertex> vertices;
    std::vector<uint> indices = {
        0, 2, 1,
        1, 2, 3
    };

    vertices.push_back({
        {-10.f, +10.f, 0.f},
        {0.f,0.f,-1.f},
        {0.f,0.f},
        {0.f, 1.f, 0.f}
        });
    vertices.push_back({
        { +10.f, +10.f, 0.f },
        { 0.f,0.f,-1.f },
        { 1.f,0.f },
        { 0.f, 1.f, 1.f }
        });
    vertices.push_back({
        { -10.f, -10.f, 0.f },
        { 0.f,0.f,-1.f },
        { 0.f,1.f },
        { 0.f, 0.f, 1.f }
        });
    vertices.push_back({
        { +10.f, -10.f, 0.f },
        { 0.f,0.f,-1.f },
        { 1.f,1.f },
        { 1.f, 0.f, 0.f }
        });

    m_nIndices = indices.size();

    glGenBuffers(1, &m_buf_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buf_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_nIndices, indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_buf_data);
    glBindBuffer(GL_ARRAY_BUFFER, m_buf_data);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BasicGLWidget::computeBBoxScene()
{
	// Right now we have just a quad of 20x20x0
	m_sceneRadius = sqrt(20 * 20 + 20 * 20)/2.0f;
	m_sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
}

void BasicGLWidget::sceneTransform()
{
    glm::mat4 geomTransform(1.0f);

	// TO DO: Rotations of the scene


	// Send the matrix to the shader
	glUniformMatrix4fv(m_transLoc, 1, GL_FALSE, &geomTransform[0][0]);
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
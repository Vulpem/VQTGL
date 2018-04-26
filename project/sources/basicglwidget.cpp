#include "../headers/basicglwidget.h"
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QColorDialog>
#include <QPainter>
#include <math.h>
#include <qopenglcontext.h>

#include <gl\GL.h>

#include <iostream>
#include <gl\GLU.h>

#include "basicglwidget.h"


Mesh::Mesh(BasicGLWidget& owner)
    : m_position(0.f, 0.f, 0.f)
    , m_scale(1.f, 1.f, 1.f)
    , m_rotation(0.f, 0.f, 0.f)
	, m_modelFilename("")
    , m_VBOModelVerts(QOpenGLBuffer::Type::VertexBuffer)
    , m_VBOModelNorms(QOpenGLBuffer::Type::VertexBuffer)
    , m_VBOModelUVs(QOpenGLBuffer::Type::VertexBuffer)
	, m_VBOModelMatAmb(QOpenGLBuffer::Type::VertexBuffer)
	, m_VBOModelMatDiff(QOpenGLBuffer::Type::VertexBuffer)
	, m_VBOModelMatSpec(QOpenGLBuffer::Type::VertexBuffer)
	, m_VBOModelMatShin(QOpenGLBuffer::Type::VertexBuffer)
	, m_owner(owner)
{
}

Mesh::~Mesh()
{
	m_owner.makeCurrent();
	//glDisableVertexAttribArray(0);
	m_VBOModelVerts.destroy();
	m_VBOModelNorms.destroy();
	m_VBOModelMatAmb.destroy();
	m_VBOModelMatDiff.destroy();
	m_VBOModelMatSpec.destroy();
	m_VBOModelMatShin.destroy();
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

void Mesh::Load(QString filename)
{
	m_modelFilename = filename;
	// Load the OBJ model - BEFORE creating the buffers!
	m_model.load(filename.toStdString());

	m_owner.makeCurrent();
	// VBO Vertices
	m_VBOModelVerts.create();
	m_VBOModelVerts.bind();
	m_VBOModelVerts.allocate(m_model.VBO_vertices(), sizeof(GLfloat)*m_model.faces().size() * 3 * 3);

	// VBO Normals
	m_VBOModelNorms.create();
	m_VBOModelNorms.bind();
	m_VBOModelNorms.allocate(m_model.VBO_normals(), sizeof(GLfloat)*m_model.faces().size() * 3 * 3);

    //VBO UVs
    m_VBOModelUVs.create();
    m_VBOModelUVs.bind();
    m_VBOModelUVs.allocate(m_model.VBO_UVs(), sizeof(GLfloat)*m_model.faces().size() * 2 * 3);

	// Instead of colors, we pass the materials 
	// VBO Ambient component
	m_VBOModelMatAmb.create();
	m_VBOModelMatAmb.bind();
	m_VBOModelMatAmb.allocate(m_model.VBO_matamb(), sizeof(GLfloat)*m_model.faces().size() * 3 * 3);

	// VBO Diffuse component
	m_VBOModelMatDiff.create();
	m_VBOModelMatDiff.bind();
	m_VBOModelMatDiff.allocate(m_model.VBO_matdiff(), sizeof(GLfloat)*m_model.faces().size() * 3 * 3);

	// VBO Specular component
	m_VBOModelMatSpec.create();
	m_VBOModelMatSpec.bind();
	m_VBOModelMatSpec.allocate(m_model.VBO_matspec(), sizeof(GLfloat)*m_model.faces().size() * 3 * 3);

	// VBO Shininess component
	m_VBOModelMatShin.create();
	m_VBOModelMatShin.bind();
	m_VBOModelMatShin.allocate(m_model.VBO_matshin(), sizeof(GLfloat)*m_model.faces().size() * 3);
}

void Mesh::LoadTexture(QString filename, int n)
{
    m_textures[n] = TexturePtr(new QOpenGLTexture(QImage(filename).mirrored()));
    m_textures[n]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_textures[n]->setMagnificationFilter(QOpenGLTexture::Linear);
}

void Mesh::UnloadTexture(int n)
{
	m_owner.makeCurrent();
    auto tex = m_textures.find(n);
    if (tex != m_textures.end())
    {
        m_textures.erase(tex);
    }
}




BasicGLWidget::BasicGLWidget(QString modelFilename, QWidget *parent) : QOpenGLWidget(parent)
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

    //FPS
    m_frameCount = 0;
    m_FPS = 0;

	m_modelFilename = modelFilename;
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

MeshPtr BasicGLWidget::LoadModel(QString modelFilename)
{
	MeshPtr p = std::make_shared<Mesh>(*this);
	p->Load(modelFilename);
	m_meshes.push_back(p);
	return p;
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
    SetTranslationAll(QVector3D(0.f, 0.f, -10.f));
}

std::vector<MeshPtr> BasicGLWidget::GetMeshes()
{
    return m_meshes;
}

void BasicGLWidget::LoadTexture(QString filename, int n)
{
    m_meshes.front()->LoadTexture(filename, n);
    update();
}

void BasicGLWidget::UnloadTexture(int n)
{
    m_meshes.front()->UnloadTexture(n);
    update();
}

QVector3D BasicGLWidget::GetCameraPosition()
{
    return m_cameraPosition;
}

void BasicGLWidget::SetCameraPosition(QVector3D position)
{
    m_cameraPosition = position;
    viewTransform();
    update();
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
    update();
}

void BasicGLWidget::RotateCamera(QVector3D rotation)
{
    SetCameraRotation(m_cameraRotation + rotation);
}

QVector3D BasicGLWidget::GetCameraForward()
{
    QMatrix4x4 view;
    view.rotate(QQuaternion::fromEulerAngles(m_cameraRotation));
    return QVector3D(0.f, 0.f, -1.f) * view.inverted();
}

QVector3D BasicGLWidget::GetCameraRight()
{
    QMatrix4x4 view;
    view.rotate(QQuaternion::fromEulerAngles(m_cameraRotation));
    return QVector3D(1.f, 0.f, 0.f) *  view.inverted();
}

QVector3D BasicGLWidget::GetCameraUp()
{
    QMatrix4x4 view;
    view.rotate(QQuaternion::fromEulerAngles(m_cameraRotation));
    return QVector3D(0.f, 1.f, 0.f) *  view.inverted();
}

void BasicGLWidget::cleanup()
{
	makeCurrent();
	
	if (m_program == nullptr)
        return;
    
	delete m_program;
    m_program = 0;
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
	if (m_modelFilename != "")
	{
		LoadModel(m_modelFilename);
	}
	projectionTransform();
	viewTransform();
    computeBBoxScene();
	initFBO();
}

void BasicGLWidget::initFBO()
{
	makeCurrent();

	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

	m_fbo = new QOpenGLFramebufferObject(m_width, m_height, format);
	m_fbo->addColorAttachment(m_width, m_height);
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

	m_fbo->bind();
	GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(3, bufs);

	//TODO remove
	m_fbo->bindDefault();

    const Mesh& mesh = *m_meshes.front();
	for (int n = 0; n < 2; n++)
	{
		auto tex = mesh.m_textures.find(n);
		if (tex != mesh.m_textures.end())
		{
			tex->second->bind(10 + n);
			glUniform1i(m_texLoc[n], 10 + n);
			glUniform1i(m_texLoaded[n], 1);
		}
		else
		{
			glUniform1i(m_texLoaded[n], 0);
		}
	}

	glUniform3f(m_lightColLoc, 1.f, 1.f, 1.f);

    for (auto mesh : m_meshes)
    {
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelVerts.bufferId());
		glVertexAttribPointer(m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_vertexLoc);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelNorms.bufferId());
		glVertexAttribPointer(m_normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_normalLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelUVs.bufferId());
        glVertexAttribPointer(m_UVLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_UVLoc);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatAmb.bufferId());
		glVertexAttribPointer(m_matAmbLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_matAmbLoc);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatDiff.bufferId());
		glVertexAttribPointer(m_matDiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_matDiffLoc);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatSpec.bufferId());
		glVertexAttribPointer(m_matSpecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_matSpecLoc);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatShin.bufferId());
		glVertexAttribPointer(m_matShinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(m_matShinLoc);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Apply the geometric transforms to the scene (position/orientation)
        meshTransform(mesh);

		glDrawArrays(GL_TRIANGLES, 0, mesh->m_model.faces().size() *3);
    }

	m_fbo->bindDefault();	

    m_program->release();
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

void BasicGLWidget::loadShaders()
{
	makeCurrent();
	std::cout << "Loading Shaders: \n";

	// Declaration of the shaders
	QOpenGLShader vs(QOpenGLShader::Vertex, this);
	QOpenGLShader fs(QOpenGLShader::Fragment, this);
	QOpenGLShader geom(QOpenGLShader::Geometry, this);

	// Load and compile the shaders
	vs.compileSourceFile("./project/shaders/BasicGLWindow/sceneRender.vert");
	fs.compileSourceFile("./project/shaders/BasicGLWindow/sceneRender.frag");
	geom.compileSourceFile("./project/shaders/BasicGLWindow/sceneRender.geom");

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
	m_vertexLoc = m_program->attributeLocation("vertex");
	m_normalLoc = m_program->attributeLocation("normal");
	m_UVLoc = m_program->attributeLocation("UV");

	m_matAmbLoc = m_program->attributeLocation("matamb");
    m_matDiffLoc = m_program->attributeLocation("matdiff");
	m_matSpecLoc = m_program->attributeLocation("matspec");
	m_matShinLoc = m_program->attributeLocation("matshin");

	std::cout << "	Attribute locations \n";
	std::cout << "		vertex:		" << m_vertexLoc << "\n";
	std::cout << "		normal:		" << m_normalLoc << "\n";
	std::cout << "		UVs:		" << m_UVLoc << "\n";
	std::cout << "		ambient: 	" << m_matAmbLoc << "\n";
	std::cout << "		diffuse:	" << m_matDiffLoc << "\n";
	std::cout << "		specular:	" << m_matSpecLoc << "\n";
	std::cout << "		shinyness:	" << m_matShinLoc << "\n";

	// Get the uniforms locations of the vertex shader
	m_projLoc = m_program->uniformLocation("projTransform");
	m_viewLoc = m_program->uniformLocation("viewTransform");
	m_transLoc = m_program->uniformLocation("sceneTransform");

	m_lightPosLoc = m_program->uniformLocation("lightPos");
	m_lightColLoc = m_program->uniformLocation("lightCol");

    m_texLoc[0] = m_program->uniformLocation("tex1Texture");
	m_texLoc[1] = m_program->uniformLocation("tex2Texture");
    m_texLoaded[0] = m_program->uniformLocation("tex1Loaded");
	m_texLoaded[1] = m_program->uniformLocation("tex2Loaded");

	std::cout << "	Uniform locations \n";
	std::cout << "		projection transform:   " << m_projLoc << "\n";
	std::cout << "		view transform:         " << m_viewLoc << "\n";
	std::cout << "		scene transform:        " << m_transLoc << "\n";
	std::cout << "		light position:         " << m_lightPosLoc << "\n";
	std::cout << "		light color:            " << m_lightColLoc << "\n";
	std::cout << "		texture 1:              " << m_texLoc[0] << "\n";
	std::cout << "		texture 2:              " << m_texLoc[1] << "\n";
	std::cout << "		is texture 1 loaded:    " << m_texLoaded[0] << "\n";
	std::cout << "		is texture 2 loaded:    " << m_texLoaded[1] << "\n";

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

float BasicGLWidget::GetFPS()
{
    return m_FPS;
}

void BasicGLWidget::projectionTransform()
{
	makeCurrent();

    m_program->bind();

	// Set the camera type
	QMatrix4x4 proj;
	proj.setToIdentity();
	
	// TO DO: Set the camera parameters 
	m_zNear = 0.02f;
	m_zFar = 1000.f;

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
	makeCurrent();
    m_program->bind();

	// Send the matrix to the shader
	m_program->setUniformValue(m_viewLoc, GetViewMatrix());

    m_program->release();
}

QMatrix4x4 BasicGLWidget::GetViewMatrix()
{
    QMatrix4x4 view;
	view.translate(m_cameraPosition);
    view.rotate(QQuaternion::fromEulerAngles(m_cameraRotation));
    return view.inverted();
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
	makeCurrent();
	// Send the matrix to the shader
	m_program->setUniformValue(m_transLoc, mesh->GetTransform());
}

void BasicGLWidget::computeFps()
{
    if (m_frameCount == 0)
    {
        m_FPSTimer.start();
    }

    if (m_FPSTimer.elapsed() / 1000.f >= 1.f)
    {
        m_FPS = m_frameCount;
        m_frameCount = 0;
        m_FPSTimer.restart();
    }
    m_frameCount++;
    emit(UpdatedFPS(m_FPS));
}

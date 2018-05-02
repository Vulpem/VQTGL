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




BasicGLWidget::BasicGLWidget(QString modelFilename, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_fbo(nullptr)
    , m_SSAORadius(0.1f)
{
	// To receive key events
	setFocusPolicy(Qt::StrongFocus);

	// Attributes initialization
	// Screen
	m_width = width();
	m_height = height();

    
	
	// Scene
	m_sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	m_sceneRadius = 50.0f;
	m_bgColor = Qt::black;
	m_backFaceCulling = false;

	// Shaders
	m_programs.sceneRender.m_program = nullptr;

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
	
	if (m_programs.sceneRender.m_program == nullptr)
        return;
    
	delete m_programs.sceneRender.m_program;
	m_programs.sceneRender.m_program = 0;
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

    if (m_fbo != nullptr)
    {
        delete m_fbo;
        m_fbo = nullptr;
    }

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    

    m_fbo = new QOpenGLFramebufferObject(m_width, m_height, format);
    m_fbo->addColorAttachment(m_width, m_height, GL_RGB16F);
    m_fbo->addColorAttachment(m_width, m_height, GL_RGB16F);
    m_fbo->addColorAttachment(m_width, m_height);

    float vertices[] = {
        -1.f, 1.f, 0.f,
        -1.f, -1.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, -1.f, 0.f,
    };

    m_planeVertices.create();
    m_planeVertices.bind();
    m_planeVertices.allocate(vertices, sizeof(float) * 12);

    const int randomImageSize = 64;

    srand(time(NULL));
    QImage randImage = QImage(randomImageSize,randomImageSize, QImage::Format::Format_RGB16);
    for (int y = 0; y < randomImageSize; y++)
    {
        for (int x = 0; x < randomImageSize; x++)
        {
            randImage.setPixel(x, y, qRgb(rand() % 256, rand() % 256, rand() % 256));
        }
    }
    m_randomTexture = new QOpenGLTexture(randImage);

    const int kernelSize = 32;
    std::vector<float> kernel(kernelSize * 3);
    srand(time(NULL));

    for (int i = 0; i < kernelSize; ++i)
    {
        kernel[i * 3 + 0] = (float)rand() / (float)RAND_MAX;
        kernel[i * 3 + 1] = (float)rand() / (float)RAND_MAX;
        kernel[i * 3 + 2] = (float)rand() / (float)RAND_MAX;

        //Reescale it, so points are more focused near the center
        float scale = float(i) / float(kernelSize);
        scale = scale * scale;
        scale = 0.1f * (1 - scale) + 1.f * scale;

        const float length = sqrt(
            (kernel[i * 3 + 0] * kernel[i * 3 + 0])
            + (kernel[i * 3 + 1] * kernel[i * 3 + 1])
            + (kernel[i * 3 + 2] * kernel[i * 3 + 2]));
        kernel[i * 3 + 0] = (kernel[i * 3 + 0] / length) * scale;
        kernel[i * 3 + 1] = (kernel[i * 3 + 1] / length) * scale;
        kernel[i * 3 + 2] = (kernel[i * 3 + 2] / length) * scale;
    }

    m_programs.ssao.m_program->bind();

    glUniform3fv(m_programs.ssao.m_kernelsLoc, kernelSize, kernel.data());

    m_programs.ssao.m_program->release();

}

void BasicGLWidget::paintGL()
{
    // FPS computation
    computeFps();

    glClearColor(0.f, 0.f,0.f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_fbo->bind();
    GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, bufs);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PaintToFBOFill();
    PaintSSAO();
    PaintScene();

    m_fbo->bindDefault();
    PaintToScreen();	
}

void BasicGLWidget::PaintToFBOFill()
{
    m_programs.fboFill.m_program->bind();
    GLenum bufs[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(2, bufs);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const Mesh& mesh = *m_meshes.front();
    for (auto mesh : m_meshes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelVerts.bufferId());
        glVertexAttribPointer(m_programs.fboFill.m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.fboFill.m_vertexLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelNorms.bufferId());
        glVertexAttribPointer(m_programs.fboFill.m_normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.fboFill.m_normalLoc);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Apply the geometric transforms to the scene (position/orientation)
        m_programs.fboFill.m_program->setUniformValue(m_programs.fboFill.m_transLoc, mesh->GetTransform());

        glDrawArrays(GL_TRIANGLES, 0, mesh->m_model.faces().size() * 3);
    }

    m_programs.fboFill.m_program->release();
}

void BasicGLWidget::PaintScene()
{
    m_programs.sceneRender.m_program->bind();
    GLenum bufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, bufs); 
    glClearColor(m_bgColor.red() / 255.0f, m_bgColor.green() / 255.0f, m_bgColor.blue() / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Mesh& mesh = *m_meshes.front();
    for (int n = 0; n < 2; n++)
    {
        auto tex = mesh.m_textures.find(n);
        if (tex != mesh.m_textures.end())
        {
            tex->second->bind(10 + n);
            glUniform1i(m_programs.sceneRender.m_texLoc[n], 10 + n);
            glUniform1i(m_programs.sceneRender.m_texLoaded[n], 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            glUniform1i(m_programs.sceneRender.m_texLoaded[n], 0);
        }
    }

    glUniform3f(m_programs.sceneRender.m_lightColLoc, 1.f, 1.f, 1.f);

    glUniform1i(m_programs.sceneRender.m_useSSAO, (m_whatToDraw == WhatToDraw::finalImage));

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_fbo->textures()[3]);
    glUniform1i(m_programs.planeRender.m_SSAOLoc, 6);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    for (auto mesh : m_meshes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelVerts.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_vertexLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelNorms.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_normalLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelUVs.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_UVLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_UVLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatAmb.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_matAmbLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_matAmbLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatDiff.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_matDiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_matDiffLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatSpec.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_matSpecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_matSpecLoc);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VBOModelMatShin.bufferId());
        glVertexAttribPointer(m_programs.sceneRender.m_matShinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(m_programs.sceneRender.m_matShinLoc);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Apply the geometric transforms to the scene (position/orientation)
        m_programs.sceneRender.m_program->setUniformValue(m_programs.sceneRender.m_transLoc, mesh->GetTransform());

        glDrawArrays(GL_TRIANGLES, 0, mesh->m_model.faces().size() * 3);
    }

    m_programs.sceneRender.m_program->release();
}

void BasicGLWidget::PaintSSAO()
{
    m_programs.ssao.m_program->bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    GLenum bufs[] = { GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(1, bufs);

    const QVector<GLuint> textures = m_fbo->textures();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glUniform1i(m_programs.ssao.m_depthTexLoc, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glUniform1i(m_programs.ssao.m_normalsTexLoc, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture->textureId());
    glUniform1i(m_programs.ssao.m_randomTexLoc, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUniform2f(m_programs.ssao.m_screenSize, m_width, m_height);

    glBindBuffer(GL_ARRAY_BUFFER, m_planeVertices.bufferId());
    glVertexAttribPointer(m_programs.ssao.m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(m_programs.ssao.m_vertexLoc);

    glUniform1f(m_programs.ssao.m_radiusLoc, m_SSAORadius);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_programs.ssao.m_program->release();
}

void BasicGLWidget::PaintToScreen()
{
    makeCurrent();

    m_programs.planeRender.m_program->bind();

    const QVector<GLuint> textures = m_fbo->textures();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(m_programs.planeRender.m_diffuseTexLoc, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glUniform1i(m_programs.planeRender.m_depthTexLoc, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glUniform1i(m_programs.planeRender.m_normalsTexLoc, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glUniform1i(m_programs.planeRender.m_SSAOLoc, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUniform1i(m_programs.planeRender.m_whatToDrawLoc, static_cast<int>(m_whatToDraw));

    glUniform2f(m_programs.planeRender.m_screenSize, m_width, m_height);

    glBindBuffer(GL_ARRAY_BUFFER, m_planeVertices.bufferId());
    glVertexAttribPointer(m_programs.planeRender.m_vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(m_programs.planeRender.m_vertexLoc);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_programs.planeRender.m_program->release();
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

    initFBO();
}

void BasicGLWidget::loadShaders()
{
	makeCurrent();
	std::cout << "Loading Shaders: \n";
    {
        std::cout << "\n\n	Compiling \"FBO Fill\" shader \n";
        // Declaration of the shaders
        QOpenGLShader vs(QOpenGLShader::Vertex, this);
        QOpenGLShader fs(QOpenGLShader::Fragment, this);
        QOpenGLShader geom(QOpenGLShader::Geometry, this);

        // Load and compile the shaders
        vs.compileSourceFile("./project/shaders/BasicGLWindow/FBOfill.vert");
        fs.compileSourceFile("./project/shaders/BasicGLWindow/FBOfill.frag");
        geom.compileSourceFile("./project/shaders/BasicGLWindow/FBOfill.geom");

        // Create the program
        m_programs.fboFill.m_program = new QOpenGLShaderProgram;

        // Add the shaders
        m_programs.fboFill.m_program->addShader(&fs);
        m_programs.fboFill.m_program->addShader(&vs);
        m_programs.fboFill.m_program->addShader(&geom);

        // Link the program
        m_programs.fboFill.m_program->link();

        // Bind the program (we are gonna use this program)
        m_programs.fboFill.m_program->bind();

        // Get the attribs locations of the vertex shader
        m_programs.fboFill.m_vertexLoc = m_programs.fboFill.m_program->attributeLocation("vertex");
        m_programs.fboFill.m_normalLoc = m_programs.fboFill.m_program->attributeLocation("normal");

        std::cout << "  Attribute locations \n";
        std::cout << "      vertex:                 " << m_programs.fboFill.m_vertexLoc << "\n";
        std::cout << "      normal:                 " << m_programs.fboFill.m_normalLoc << "\n";

        // Get the uniforms locations of the vertex shader
        m_programs.fboFill.m_projLoc = m_programs.fboFill.m_program->uniformLocation("projTransform");
        m_programs.fboFill.m_viewLoc = m_programs.fboFill.m_program->uniformLocation("viewTransform");
        m_programs.fboFill.m_transLoc = m_programs.fboFill.m_program->uniformLocation("sceneTransform");

        std::cout << "  Uniform locations \n";
        std::cout << "      projection transform:   " << m_programs.fboFill.m_projLoc << "\n";
        std::cout << "      view transform:         " << m_programs.fboFill.m_viewLoc << "\n";
        std::cout << "      scene transform:        " << m_programs.fboFill.m_transLoc << "\n";

        m_programs.fboFill.m_program->release();
    }

    {
        std::cout << "\n\n	Compiling \"Scene render\" shader \n";
        // Declaration of the shaders
        QOpenGLShader vs(QOpenGLShader::Vertex, this);
        QOpenGLShader fs(QOpenGLShader::Fragment, this);
        QOpenGLShader geom(QOpenGLShader::Geometry, this);

        // Load and compile the shaders
        vs.compileSourceFile("./project/shaders/BasicGLWindow/sceneRender.vert");
        fs.compileSourceFile("./project/shaders/BasicGLWindow/sceneRender.frag");
        geom.compileSourceFile("./project/shaders/BasicGLWindow/sceneRender.geom");

        // Create the program
        m_programs.sceneRender.m_program = new QOpenGLShaderProgram;

        // Add the shaders
        m_programs.sceneRender.m_program->addShader(&fs);
        m_programs.sceneRender.m_program->addShader(&vs);
        m_programs.sceneRender.m_program->addShader(&geom);

        // Link the program
        m_programs.sceneRender.m_program->link();

        // Bind the program (we are gonna use this program)
        m_programs.sceneRender.m_program->bind();

        // Get the attribs locations of the vertex shader
        m_programs.sceneRender.m_vertexLoc = m_programs.sceneRender.m_program->attributeLocation("vertex");
        m_programs.sceneRender.m_normalLoc = m_programs.sceneRender.m_program->attributeLocation("normal");
        m_programs.sceneRender.m_UVLoc = m_programs.sceneRender.m_program->attributeLocation("UV");

        m_programs.sceneRender.m_matAmbLoc = m_programs.sceneRender.m_program->attributeLocation("matamb");
        m_programs.sceneRender.m_matDiffLoc = m_programs.sceneRender.m_program->attributeLocation("matdiff");
        m_programs.sceneRender.m_matSpecLoc = m_programs.sceneRender.m_program->attributeLocation("matspec");
        m_programs.sceneRender.m_matShinLoc = m_programs.sceneRender.m_program->attributeLocation("matshin");

        std::cout << "  Attribute locations \n";
        std::cout << "      vertex:                 " << m_programs.sceneRender.m_vertexLoc << "\n";
        std::cout << "      normal:                 " << m_programs.sceneRender.m_normalLoc << "\n";
        std::cout << "      UVs:                    " << m_programs.sceneRender.m_UVLoc << "\n";
        std::cout << "      ambient:                " << m_programs.sceneRender.m_matAmbLoc << "\n";
        std::cout << "      diffuse:                " << m_programs.sceneRender.m_matDiffLoc << "\n";
        std::cout << "      specular:               " << m_programs.sceneRender.m_matSpecLoc << "\n";
        std::cout << "      shinyness:              " << m_programs.sceneRender.m_matShinLoc << "\n";

        // Get the uniforms locations of the vertex shader
        m_programs.sceneRender.m_projLoc = m_programs.sceneRender.m_program->uniformLocation("projTransform");
        m_programs.sceneRender.m_viewLoc = m_programs.sceneRender.m_program->uniformLocation("viewTransform");
        m_programs.sceneRender.m_transLoc = m_programs.sceneRender.m_program->uniformLocation("sceneTransform");

        m_programs.sceneRender.m_lightPosLoc = m_programs.sceneRender.m_program->uniformLocation("lightPos");
        m_programs.sceneRender.m_lightColLoc = m_programs.sceneRender.m_program->uniformLocation("lightCol");

        m_programs.sceneRender.m_texLoc[0] = m_programs.sceneRender.m_program->uniformLocation("tex1Texture");
        m_programs.sceneRender.m_texLoc[1] = m_programs.sceneRender.m_program->uniformLocation("tex2Texture");
        m_programs.sceneRender.m_texLoaded[0] = m_programs.sceneRender.m_program->uniformLocation("tex1Loaded");
        m_programs.sceneRender.m_texLoaded[1] = m_programs.sceneRender.m_program->uniformLocation("tex2Loaded");
        m_programs.sceneRender.m_useSSAO = m_programs.sceneRender.m_program->uniformLocation("useSSAO");
        m_programs.sceneRender.m_SSAOLoc = m_programs.sceneRender.m_program->uniformLocation("SSAOTex");

        std::cout << "  Uniform locations \n";
        std::cout << "      projection transform:   " << m_programs.sceneRender.m_projLoc << "\n";
        std::cout << "      view transform:         " << m_programs.sceneRender.m_viewLoc << "\n";
        std::cout << "      scene transform:        " << m_programs.sceneRender.m_transLoc << "\n";
        std::cout << "      light position:         " << m_programs.sceneRender.m_lightPosLoc << "\n";
        std::cout << "      light color:            " << m_programs.sceneRender.m_lightColLoc << "\n";
        std::cout << "      texture 1:              " << m_programs.sceneRender.m_texLoc[0] << "\n";
        std::cout << "      texture 2:              " << m_programs.sceneRender.m_texLoc[1] << "\n";
        std::cout << "      is texture 1 loaded:    " << m_programs.sceneRender.m_texLoaded[0] << "\n";
        std::cout << "      is texture 2 loaded:    " << m_programs.sceneRender.m_texLoaded[1] << "\n";
        std::cout << "      use SSAO:               " << m_programs.sceneRender.m_transLoc << "\n";
        std::cout << "      SSAO texture:           " << m_programs.sceneRender.m_SSAOLoc << "\n";

        m_programs.sceneRender.m_program->release();
    }

    {
        std::cout << "\n\n	Compiling \"Plane render\" shader \n";

        // Declaration of the shaders
        QOpenGLShader vs(QOpenGLShader::Vertex, this);
        QOpenGLShader fs(QOpenGLShader::Fragment, this);

        // Load and compile the shaders
        vs.compileSourceFile("./project/shaders/BasicGLWindow/planeRender.vert");
        fs.compileSourceFile("./project/shaders/BasicGLWindow/planeRender.frag");

        // Create the program
        m_programs.planeRender.m_program = new QOpenGLShaderProgram;

        // Add the shaders
        m_programs.planeRender.m_program->addShader(&fs);
        m_programs.planeRender.m_program->addShader(&vs);

        // Link the program
        m_programs.planeRender.m_program->link();

        // Bind the program (we are gonna use this program)
        m_programs.planeRender.m_program->bind();

        // Get the attribs locations of the vertex shader
        m_programs.planeRender.m_vertexLoc = m_programs.planeRender.m_program->attributeLocation("vertex");

        std::cout << "  Attribute locations \n";
        std::cout << "      vertex:                 " << m_programs.planeRender.m_vertexLoc << "\n";

        // Get the uniforms locations of the vertex shader
        m_programs.planeRender.m_diffuseTexLoc = m_programs.planeRender.m_program->uniformLocation("diffuseTex");
        m_programs.planeRender.m_depthTexLoc = m_programs.planeRender.m_program->uniformLocation("depthTex");
        m_programs.planeRender.m_normalsTexLoc = m_programs.planeRender.m_program->uniformLocation("normalsTex");
        m_programs.planeRender.m_whatToDrawLoc = m_programs.planeRender.m_program->uniformLocation("whatToDraw");
        m_programs.planeRender.m_farPlaneLoc = m_programs.planeRender.m_program->uniformLocation("farPlane");
        m_programs.planeRender.m_nearPlaneLoc = m_programs.planeRender.m_program->uniformLocation("nearPlane");
        m_programs.planeRender.m_screenSize = m_programs.planeRender.m_program->uniformLocation("screenResolution");
        m_programs.planeRender.m_SSAOLoc = m_programs.planeRender.m_program->uniformLocation("SSAOTex");


        std::cout << "  Uniform locations \n";
        std::cout << "      Diffuse texture:        " << m_programs.planeRender.m_diffuseTexLoc << "\n";
        std::cout << "      Depth texture           " << m_programs.planeRender.m_depthTexLoc << "\n";
        std::cout << "      Normals texture:        " << m_programs.planeRender.m_normalsTexLoc << "\n";
        std::cout << "      SSAO texture:           " << m_programs.planeRender.m_SSAOLoc << "\n";
        std::cout << "      What to draw:           " << m_programs.planeRender.m_whatToDrawLoc << "\n";
        std::cout << "      far plane:              " << m_programs.planeRender.m_farPlaneLoc << "\n";
        std::cout << "      near plane:             " << m_programs.planeRender.m_nearPlaneLoc << "\n";
        std::cout << "      screen resolution:      " << m_programs.planeRender.m_screenSize << "\n";

        m_programs.planeRender.m_program->release();
    }

    {
        std::cout << "\n\n	Compiling \"SSAO\" shader \n";

        // Declaration of the shaders
        QOpenGLShader vs(QOpenGLShader::Vertex, this);
        QOpenGLShader fs(QOpenGLShader::Fragment, this);

        // Load and compile the shaders
        vs.compileSourceFile("./project/shaders/BasicGLWindow/SSAO.vert");
        fs.compileSourceFile("./project/shaders/BasicGLWindow/SSAO.frag");

        // Create the program
        m_programs.ssao.m_program = new QOpenGLShaderProgram;

        // Add the shaders
        m_programs.ssao.m_program->addShader(&fs);
        m_programs.ssao.m_program->addShader(&vs);

        // Link the program
        m_programs.ssao.m_program->link();

        // Bind the program (we are gonna use this program)
        m_programs.ssao.m_program->bind();

        // Get the attribs locations of the vertex shader
        m_programs.ssao.m_vertexLoc = m_programs.ssao.m_program->attributeLocation("vertex");

        std::cout << "  Attribute locations \n";
        std::cout << "      vertex:                 " << m_programs.ssao.m_vertexLoc << "\n";

        // Get the uniforms locations of the vertex shader
        m_programs.ssao.m_depthTexLoc = m_programs.ssao.m_program->uniformLocation("depthTex");
        m_programs.ssao.m_normalsTexLoc = m_programs.ssao.m_program->uniformLocation("normalsTex");
        m_programs.ssao.m_randomTexLoc = m_programs.ssao.m_program->uniformLocation("randomTex");
        m_programs.ssao.m_projectionMat = m_programs.ssao.m_program->uniformLocation("projectionMat");
        m_programs.ssao.m_kernelsLoc = m_programs.ssao.m_program->uniformLocation("kernel");
        m_programs.ssao.m_screenSize = m_programs.ssao.m_program->uniformLocation("screenResolution");
        m_programs.ssao.m_radiusLoc = m_programs.ssao.m_program->uniformLocation("sampleRadius");


        std::cout << "  Uniform locations \n";
        std::cout << "      Depth texture           " << m_programs.ssao.m_depthTexLoc << "\n";
        std::cout << "      Normals texture:        " << m_programs.ssao.m_normalsTexLoc << "\n";
        std::cout << "      Random texture:         " << m_programs.ssao.m_randomTexLoc << "\n";
        std::cout << "      projection matrix:      " << m_programs.ssao.m_projectionMat << "\n";
        std::cout << "      kernels:                " << m_programs.ssao.m_kernelsLoc << "\n";
        std::cout << "      screen resolution:      " << m_programs.ssao.m_screenSize << "\n";
        std::cout << "      Sample radius:          " << m_programs.ssao.m_radiusLoc << "\n";

        m_programs.ssao.m_program->release();
    }
}

void BasicGLWidget::reloadShaders()
{
	if (m_programs.sceneRender.m_program == nullptr)
		return;
	makeCurrent();
	delete m_programs.sceneRender.m_program;
	m_programs.sceneRender.m_program = 0;
	loadShaders();
	update();
}

float BasicGLWidget::GetFPS()
{
    return m_FPS;
}

void BasicGLWidget::projectionTransform()
{
    // Set the camera type
    QMatrix4x4 proj;
    proj.setToIdentity();

    // TO DO: Set the camera parameters 
    m_zNear = 0.01f;
    m_zFar = 100.f;

    proj.perspective(45.0f, GLfloat(m_width) / m_height, m_zNear, m_zFar);

    // Send the matrix to the shader
    m_programs.fboFill.m_program->bind();
    m_programs.fboFill.m_program->setUniformValue(m_programs.fboFill.m_projLoc, proj);

    m_programs.sceneRender.m_program->bind();
    m_programs.sceneRender.m_program->setUniformValue(m_programs.sceneRender.m_projLoc, proj);

    m_programs.ssao.m_program->bind();
    m_programs.ssao.m_program->setUniformValue(m_programs.ssao.m_projectionMat, proj);

    m_programs.planeRender.m_program->bind();
    m_programs.planeRender.m_program->setUniformValue(m_programs.planeRender.m_farPlaneLoc, m_zFar);
    m_programs.planeRender.m_program->setUniformValue(m_programs.planeRender.m_nearPlaneLoc, m_zNear);
    m_programs.planeRender.m_program->release();
}

void BasicGLWidget::ResetCamera()
{
    SetCameraRotation(QVector3D(0.f, 0.f, 0.f));
    SetCameraPosition(QVector3D(0.f, 0.f, 0.f));
}

void BasicGLWidget::viewTransform()
{
    QMatrix4x4 view = GetViewMatrix();
	m_programs.sceneRender.m_program->bind();
	m_programs.sceneRender.m_program->setUniformValue(m_programs.sceneRender.m_viewLoc, view);

    m_programs.fboFill.m_program->bind();
    m_programs.fboFill.m_program->setUniformValue(m_programs.fboFill.m_viewLoc, view);
    m_programs.fboFill.m_program->release();
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

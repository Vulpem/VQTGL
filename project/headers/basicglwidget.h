#ifndef BASICGLWIDGET_H
#define BASICGLWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTime>
#include <QWheelEvent>
#include <qtimer.h>

#include <memory>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

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
    Vertex(float3 position, float3 normal = { 0.f,0.f,-1.f }, float2 UVs = { 0.f,0.f }, float3 col = { 1.f,1.f,1.f });
    float3 pos;
    float3 norm;
    float2 UV;
    float3 color;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint> indices);
    ~Mesh();

    QMatrix4x4 GetTransform();

    QVector3D m_position;
    QVector3D m_scale;
    QQuaternion m_rotation;

    QOpenGLBuffer m_dataBuf;
    QOpenGLBuffer m_indicesBuf;
    uint m_numIndices;
};

typedef std::shared_ptr<Mesh> MeshPtr;

class BasicGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    BasicGLWidget(QWidget *parent = 0);
    ~BasicGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void cleanup();

signals:

protected:
    void initializeGL() override;
	void paintGL() override;
    void resizeGL(int width, int height) override;
	
	// Keyboard and mouse interaction
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
    // Shaders
	void loadShaders();
	void reloadShaders();

	// Camera
	void projectionTransform(); // Type of camera
	void resetCamera();
	void viewTransform(); // Position of the camera

	// Scene
	void changeBackgroundColor(QColor color);
	void createBoxScene();
	void computeBBoxScene();
	void meshTransform(MeshPtr mesh); // Position and orientation of the scene

	// FPS
	void computeFps();
	void showFps();
	
	/* Attributes */
	// Screen
	int m_width;
	int m_height;
	
	// Camera
	float m_ar;
	float m_fov;
	float m_fovIni;
	float m_zNear;
	float m_zFar;
	float m_radsZoom;
	float m_xPan;
	float m_yPan;

    std::vector<MeshPtr> m_meshes;

	// Scene
	glm::vec3 m_sceneCenter;
	float m_sceneRadius;

	QColor m_bgColor;
	bool m_backFaceCulling = false;

	// Mouse
	int m_xClick;
	int m_yClick;
	float m_xRot;
	float m_yRot;

	// Shaders
    QOpenGLShaderProgram *m_program;
	GLuint m_transLoc, m_projLoc, m_viewLoc;
	GLuint m_vertexLoc, m_normalLoc, m_UVLoc, m_colorLoc;

	// FPS
	bool m_showFps;
	uint m_FPS;
	uint m_frameCount;
	QTime m_FPSTimer;
	
};

#endif

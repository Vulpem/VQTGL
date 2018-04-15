#ifndef BASICGLWIDGET_H
#define BASICGLWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <qimage.h>
#include <qopengltexture.h>
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


typedef std::unique_ptr<QOpenGLTexture> TexturePtr;

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
	uint GetNIndices() { return m_numIndices; }
    void LoadTexture(QString filename, int n = -1);
    void UnloadTexture(int n);

    QVector3D m_position;
    QVector3D m_scale;
    QVector3D m_rotation;

    QOpenGLBuffer m_dataBuf;
    QOpenGLBuffer m_indicesBuf;
    std::map<int, TexturePtr> m_textures;
private:
    uint m_numIndices;
};

typedef std::shared_ptr<Mesh> MeshPtr;

class BasicGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    BasicGLWidget(QString modelFilename = "", QWidget *parent = 0);
    ~BasicGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    //Meshes
	MeshPtr LoadModel(QString modelFilename);
    MeshPtr AddMesh(const std::vector<Vertex>& vertices, const std::vector<uint>& indices);
    MeshPtr AddMesh(MeshPtr mesh);
    std::vector<MeshPtr> GetMeshes();

    void LoadTexture(QString filename, int n = -1);
    void UnloadTexture(int n);

    //Scene
    void RotateAll(QVector3D rotationEuler);
    void SetRotationAll(QVector3D rotationEuler);
    void TranslateAll(QVector3D translation);
    void SetTranslationAll(QVector3D position);
    void ResetScene();

    void changeBackgroundColor(QColor color);

    //Camera
    void ResetCamera();

    QVector3D GetCameraPosition();
    void SetCameraPosition(QVector3D position);
    void TranslateCamera(QVector3D translation);

    QVector3D GetCameraRotation();
    void SetCameraRotation(QVector3D rotation);
    void RotateCamera(QVector3D rotation);

    QVector3D GetCameraForward();
    QVector3D GetCameraRight();
    QVector3D GetCameraUp();

    //Shaders
    void reloadShaders();

    //FPS
    float GetFPS();

public slots:
    void cleanup();

signals:
    void UpdatedFPS(float fps);

protected:
    void initializeGL() override;
	void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    // Shaders
	void loadShaders();

	// Camera
	void projectionTransform(); // Type of camera
	void viewTransform(); // Position of the camera
    QMatrix4x4 GetViewMatrix();

	// Scene
	void computeBBoxScene();
	void meshTransform(MeshPtr mesh); // Position and orientation of the scene
    void computeFps();

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

    QVector3D m_cameraPosition;
    QVector3D m_cameraRotation;

    std::vector<MeshPtr> m_meshes;

	// Scene
	glm::vec3 m_sceneCenter;
	float m_sceneRadius;

	QColor m_bgColor;
	bool m_backFaceCulling = false;

	// Shaders
    QOpenGLShaderProgram *m_program;
	GLuint m_transLoc, m_projLoc, m_viewLoc;
	GLuint m_vertexLoc, m_normalLoc, m_UVLoc, m_colorLoc;

    // FPS
    uint m_FPS;
    uint m_frameCount;
    QTime m_FPSTimer;
};

#endif

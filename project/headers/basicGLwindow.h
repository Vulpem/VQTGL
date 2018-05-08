#ifndef __BASIC__GL__WINDOW__
#define __BASIC__GL__WINDOW__

#include "basicwindow.h"
#include "ui_BasicGLWindow.h"
#include "basicglwidget.h"

#include "glm\glm.hpp"
#include "definitions.h"
#include "sphere.h"

class MainWindow;
class BasicGLWidget;
class QLabel;
class QCheckBox;

class BasicGLWindow : public BasicWindow
{
	Q_OBJECT

	enum class InputMovement
	{
        scene,
		FPScamera		
	};

public:
    BasicGLWindow(QString name = "BasicGLWindow");
    ~BasicGLWindow();

public slots:
//SSAO  -----------------------------------------------------------
void SLOT_ChangedInputMovement(QString val);
void SLOT_ChangedWhatToDraw(QString val);
void SLOT_UpdateFPS(float FPS);
void SLOT_LoadModel();
void SLOT_LoadTexture();
void SLOT_UnloadTexture();
void SLOT_LoadTexture2();
void SLOT_UnloadTexture2();
void SLOT_ChangedSSAORadius(double val);
void SLOT_ChangedSSAOSlider(int val);

// RayTracing -----------------------------------------------------------
void SLOT_raytraceScene();

signals:
//Raytracing -----------------------------------------------------------
void renderingProgress(int);

protected:
    // General -----------------------------------------------------------
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    bool IsOverGLWidget(QPoint pos);

	// SSAO -----------------------------------------------------------
	Ui::BasicGLWindow m_ui;
	BasicGLWidget* m_glWidget;
	QVBoxLayout* m_glWidgetContainer;

    QString m_filenameTex1;
    QString m_filenameTex2;
    bool m_moving = false;

	//RayTracing -----------------------------------------------------------
	private slots:

signals:

private:
	// General -----------------------------------------------------------

	//Input
	QPoint m_mouseLastPos;
	InputMovement m_inputMovement;

	BasicGLWidget::WhatToDraw m_whatToDraw;

	// SSAO  -----------------------------------------------------------
	void InitSSAOGUI();

	// Ray Tracing -----------------------------------------------------------
	struct Intersection
	{
		bool intersected = false;
		float distHit = FLT_MAX;
		glm::vec3 posHit = glm::vec3(0.f,0.f,0.f);
		glm::vec3 normalHit = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 colorHit = glm::vec3(0.f, 0.f, 0.f);
		bool isInside = false;
	};

	void initRaytracingGUI();

	void render(const std::vector<Sphere> &spheres);

	glm::vec3 traceRay(
		const glm::vec3 &rayOrig,
		const glm::vec3 &rayDir,
		const std::vector<Sphere> &spheres,
		const int &depth);

	Intersection intersection(
		const glm::vec3 &rayOrig,
		const glm::vec3 &rayDir,
		const std::vector<Sphere> &spheres,
		float epsilon = 0.f);

	Intersection intersection(
		const Sphere &sphere,
		const glm::vec3& rayOrig,
		const glm::vec3 &rayDir,
		float epsilon = 0.f);

	glm::vec3 blendReflRefrColors(
		const Sphere* sphere,
		const glm::vec3 &rayDir,
		const glm::vec3 &normalHit,
		const glm::vec3 &reflColor,
		const glm::vec3 &refrColor);

	// Screen
	int m_width;
	int m_height;
	glm::vec3 background_color;
};
#endif
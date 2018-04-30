#ifndef __BASIC__GL__WINDOW__
#define __BASIC__GL__WINDOW__

#include "basicwindow.h"
#include "ui_BasicGLWindow.h"
#include "basicglwidget.h"

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
void SLOT_ChangedInputMovement(QString val);
void SLOT_ChangedWhatToDraw(QString val);
void SLOT_UpdateFPS(float FPS);
void SLOT_LoadModel();
void SLOT_LoadTexture();
void SLOT_UnloadTexture();
void SLOT_LoadTexture2();
void SLOT_UnloadTexture2();

signals:

protected:
    // Keyboard and mouse interaction
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

	Ui::BasicGLWindow m_ui;
	BasicGLWidget* m_glWidget;
	QVBoxLayout* m_glWidgetContainer;

    QString m_filenameTex1;
    QString m_filenameTex2;

    //Input
    QPoint m_mouseLastPos;
	InputMovement m_inputMovement;

    BasicGLWidget::WhatToDraw m_whatToDraw;
};
#endif
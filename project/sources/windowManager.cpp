#include "glwidget.h"
#include "windowManager.h"
#include "helloQT.h"
#include "basicGLwindow.h"
#include "phongwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qwindow.h>

ListedWindow::ListedWindow(QLayout* layout, BasicWindow* window)
    : m_window(window)
{
    m_groupBox = new QGroupBox(window->GetName(), this);
    QHBoxLayout* boxLayout = new QHBoxLayout();

    QPushButton* focus = new QPushButton("Focus", this);
    focus->setMinimumSize(QSize(60, 28));
    boxLayout->addWidget(focus);

    QPushButton* close = new QPushButton("Close", this);
    close->setMinimumSize(QSize(60, 28));
    boxLayout->addWidget(close);

    m_groupBox->setLayout(boxLayout);
    layout->addWidget(m_groupBox);

    connect(focus, &QPushButton::clicked, this, &ListedWindow::Focus);
    connect(close, &QPushButton::clicked, this, &ListedWindow::Close);

    connect(m_window, &BasicWindow::Closed, this, &ListedWindow::Close);
}

ListedWindow::~ListedWindow()
{
    m_groupBox->deleteLater();
    m_window->close();
}

QString ListedWindow::GetName()
{
    return m_window->GetName();
}

void ListedWindow::Focus()
{
    m_window->activateWindow();
}

void ListedWindow::Close()
{
    this->deleteLater();
}



WindowManager::WindowManager()
{
    m_layout = new QVBoxLayout();
    QVBoxLayout* v = new QVBoxLayout();
    v->addLayout(m_layout);
    v->addStretch();
    setMinimumSize(QSize(250, 350));
	setLayout(v);
}

void WindowManager::AddWindow(WindowTypes win)
{
    BasicWindow* window = nullptr;

    switch (win)
    {
    case WindowTypes::Hello_QT:
        window = new HelloQT(); break;
    case WindowTypes::Basic_Window:
        window = new BasicWindow(); break;
    case WindowTypes::Basic_GL:
        window = new BasicGLWindow(); break;
	case WindowTypes::Patri_Window:
		window = new PhongWindow(); break;
    }

    if (window != nullptr)
    {
        ListedWindow* w = new ListedWindow(m_layout, window);
        connect(this, &WindowManager::RequestedCloseAll, w, &ListedWindow::Close);
    }
}

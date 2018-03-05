#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#include <QWidget>
#include <qlayout.h>

class QSlider;
class QPushButton;

class GLWidget;
class BasicWindow;
class QGroupBox;

enum class WindowTypes
{
    Hello_QT = 0,
    Basic_Window,
    Basic_GL,
    Testing_GL
};

class ListedWindow : public QWidget
{
    Q_OBJECT
public:
    ListedWindow(QLayout* layout, BasicWindow* window);
    ~ListedWindow();

    QString GetName();
private:
    BasicWindow * m_window;
    QGroupBox* m_groupBox;

    public slots:
    void Focus();
    void Close();
};

class WindowManager : public QWidget
{
	Q_OBJECT

public:
	WindowManager();
    void AddWindow(WindowTypes win);

public slots:
void CloseAll() { emit(RequestedCloseAll()); }
signals:
    void RequestedCloseAll();

private:
    QVBoxLayout *m_layout;
};

#endif
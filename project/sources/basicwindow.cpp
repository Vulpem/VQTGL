#include "../headers/basicwindow.h"

BasicWindow::BasicWindow(QString name)
    : m_name(name)
{
	setWindowTitle(name);
    show();
}
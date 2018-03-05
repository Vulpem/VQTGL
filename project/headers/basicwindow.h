#ifndef __BASIC__WINDOW__
#define __BASIC__WINDOW__
#include <QWidget>

#include "ui_basicwindow.h"

class BasicWindow : public QWidget
{
	Q_OBJECT

public:
	BasicWindow(QString name = "Unnamed");
    QString GetName() { return m_name; }

protected:
    QString m_name;

    void BasicWindow::closeEvent(QCloseEvent *event) override
    {
        emit(Closed());
    }

public slots:
signals:
    void Closed();
};
#endif
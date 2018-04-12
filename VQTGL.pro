TEMPLATE      = app

HEADERS       = project/headers/glwidget.h \
                project/headers/helloQT.h \
                project/headers/mainwindow.h \
                project/headers/logo.h \
                project/headers/basicglwidget.h \
                project/headers/basicwindow.h \
                project/headers/windowManager.h \
                project/headers/basicGLwindow.h \
                project/headers/model.h \
                project/headers/phongglwidget.h \
                project/headers/phongwindow.h \
                project/headers/definitions.h \
		project/headers/texturingglwidget.h \
		project/headers/texturingwindow.h

SOURCES       = project/main.cpp \
                project/sources/glwidget.cpp \
                project/sources/helloQT.cpp \
                project/sources/mainwindow.cpp \
                project/sources/logo.cpp \
                project/sources/basicglwidget.cpp \
                project/sources/basicwindow.cpp \
                project/sources/windowManager.cpp \
                project/sources/basicGLwindow.cpp \
                project/sources/model.cpp \
                project/sources/phongglwidget.cpp \
                project/sources/phongwindow.cpp \
                project/sources/texturingglwidget.cpp \
                project/sources/texturingwindow.cpp

FORMS         = project/forms/basicwindow.ui \
	        project/forms/phongwindow.ui \
		project/forms/texturingwindow.ui \
		project/forms/BasicGLWindow.ui

QT           += widgets
QT           += opengl

CONFIG	     += console

INCLUDEPATH  += ./project \
                ./project/libs \
                ./project/headers \
                ./project/sources

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/VQTGL
INSTALLS += target
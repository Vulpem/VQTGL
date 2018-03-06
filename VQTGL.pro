TEMPLATE      = app

HEADERS       = project/headers/glwidget.h \
                project/headers/helloQT.h \
                project/headers/mainwindow.h \
                project/headers/logo.h \
                project/headers/basicglwidget.h \
                project/headers/basicwindow.h \
                project/headers/windowManager.h \
                project/headers/basicGLwindow.h \
                project/headers/testingGLwindow.h

SOURCES       = project/main.cpp \
                project/sources/glwidget.cpp \
                project/sources/helloQT.cpp \
                project/sources/mainwindow.cpp \
                project/sources/logo.cpp \
                project/sources/basicglwidget.cpp \
                project/sources/basicwindow.cpp \
                project/sources/windowManager.cpp \
                project/sources/basicGLwindow.cpp \
                project/sources/testingGLwindow.cpp

FORMS         = project/forms/basicwindow.ui

QT           += widgets

CONFIG	     += console

INCLUDEPATH  += ./project \
                ./project/libs \
                ./project/headers \
                ./project/sources

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/VQTGL
INSTALLS += target
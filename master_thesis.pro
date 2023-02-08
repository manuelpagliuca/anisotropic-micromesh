QT       += core gui opengl openglwidgets widgets

CONFIG += c++17 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/dependencies/GLM

SOURCES += \
    GLWidget.cpp \
    main.cpp \
    BoundingBox.cpp \
    Mainwindow.cpp \
    Mesh.cpp \
    Shader.cpp \
    Main.cpp \
    Trackball.cpp

HEADERS += \
    BoundingBox.h \
    GLWidget.h \
    Mainwindow.h \
    Mesh.h \
    Shader.h \
    Trackball.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    shaders/shader.frag \
    shaders/shader.vert

LIBS += opengl32.lib user32.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


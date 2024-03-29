QT       += core gui opengl openglwidgets widgets

CONFIG += c++17 console
TARGET = "anisotropic_micromesh"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/dependencies

SOURCES += \
  Sources\CLI.cpp \
  Sources\GLWidget.cpp \
  Sources\Line.cpp \
  Sources\MeshDebug.cpp \
  Sources\MeshTest.cpp \
  Sources\MeshUtils.cpp \
  Sources\Ray.cpp \
  Sources\Utility.cpp \
  Sources\BoundingBox.cpp \
  Sources\Mainwindow.cpp \
  Sources\Mesh.cpp \
  Sources\Shader.cpp \
  Sources\Main.cpp \
  Sources\Trackball.cpp

HEADERS += \
  Sources\BoundingBox.h \
  Sources\Debug.h \
  Sources\Edge.h \
  Sources\Face.h \
  Sources\GLWidget.h \
  Sources\Line.h \
  Sources\Mainwindow.h \
  Sources\Mesh.h \
  Sources\Ray.h \
  Sources\Shader.h \
  Sources\Trackball.h \
  Sources\Utility.h \
  Sources\Vertex.h

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


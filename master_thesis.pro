QT       += core gui opengl openglwidgets widgets

CONFIG += c++17 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/dependencies

Release:DESTDIR = Release
Release:OBJECTS_DIR = Release/.obj
Release:MOC_DIR = Release/.moc
Release:RCC_DIR = Release/.rcc
Release:UI_DIR = Release/.ui

Debug:DESTDIR = Debug
Debug:OBJECTS_DIR = Debug/.obj
Debug:MOC_DIR = Debug/.moc
Debug:RCC_DIR = Debug/.rcc
Debug:UI_DIR = Debug/.ui

SOURCES += \
  Sources/GLWidget.cpp \
  Sources/MeshDebug.cpp \
  Sources/MeshTest.cpp \
  Sources/MeshUtils.cpp \
  Sources/Ray.cpp \
  Sources/Utility.cpp \
  Sources/BoundingBox.cpp \
  Sources/Mainwindow.cpp \
  Sources/Mesh.cpp \
  Sources/Shader.cpp \
  Sources/Main.cpp \
  Sources/Trackball.cpp

HEADERS += \
  Headers/BoundingBox.h \
  Headers/Debug.h \
  Headers/Edge.h \
  Headers/Face.h \
  Headers/GLWidget.h \
  Headers/Mainwindow.h \
  Headers/Mesh.h \
  Headers/Ray.h \
  Headers/Shader.h \
  Headers/Trackball.h \
  Headers/Utility.h \
  Headers/Vertex.h

FORMS += \
  Mainwindow.ui

DISTFILES += \
  Shaders/shader.frag \
  Shaders/shader.vert

LIBS += opengl32.lib user32.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


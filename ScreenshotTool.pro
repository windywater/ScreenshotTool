#-------------------------------------------------
#
# Project created by QtCreator 2015-05-20T21:47:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScreenshotTool
TEMPLATE = app

UI_DIR += forms
RCC_DIR += qrc
MOC_DIR += moc
OBJECTS_DIR += obj

SOURCES += main.cpp\
        MainWidget.cpp \
    ZoomInWindow.cpp \
    ScreenshotService.cpp \
    FrameView.cpp \
    Block.cpp \
    MainTool.cpp

HEADERS  += MainWidget.h \
    ZoomInWindow.h \
    ScreenshotService.h \
    FrameView.h \
    Region.h \
    Block.h \
    MainTool.h

FORMS    += MainWidget.ui \
    ZoomInWindow.ui

#-------------------------------------------------
#
# Project created by QtCreator 2016-11-12T08:49:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WFRobotStudio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    commonfunctions.cpp \
    myopenglwidget2d.cpp \
    myopenglwidget3d.cpp \
    CmyDxfFilter.cpp \
    dl_dxf.cpp \
    dl_writer_ascii.cpp \
    GraphtoPath.cpp \
    globalvariables.cpp \
    myinputdialog.cpp \
    pathposturesetdialog.cpp \
    drawcoordinatewidget3d.cpp \
    programpreivwsetupdlg.cpp \
    pathinofoutpointsetdlg.cpp

HEADERS  += mainwindow.h \
    commonfunctions.h \
    myopenglwidget2d.h \
    myopenglwidget3d.h \
    CmyDxfFilter.h \
    dl_attributes.h \
    dl_codes.h \
    dl_creationadapter.h \
    dl_creationinterface.h \
    dl_dxf.h \
    dl_entities.h \
    dl_exception.h \
    dl_extrusion.h \
    dl_global.h \
    dl_writer.h \
    dl_writer_ascii.h \
    GraphtoPath.h \
    globalvariables.h \
    myinputdialog.h \
    pathposturesetdialog.h \
    drawcoordinatewidget3d.h \
    programpreivwsetupdlg.h \
    graphtopathdatastruct.h \
    pathinofoutpointsetdlg.h

FORMS    += mainwindow.ui \
    myinputdialog.ui \
    pathposturesetdialog.ui \
    programpreivwsetupdlg.ui \
    pathinofoutpointsetdlg.ui

CONFIG +=console
include("C:/Qt/qtpropertybrowser/src/qtpropertybrowser.pri")
#include("G:/Qt/qtpropertybrowser/src/qtpropertybrowser.pri")


INCLUDEPATH += "C:\Program Files\boost_1_61_0"

RESOURCES += \
    resource.qrc
RC_FILE =app.rc

DISTFILES += \
    app.rc

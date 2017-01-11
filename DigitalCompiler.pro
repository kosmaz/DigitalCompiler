#-------------------------------------------------
#
# Project created by QtCreator 2015-03-24T23:36:24
#
#-------------------------------------------------
DEPLOYMENT_PLUGIN
QT+=core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DigitalCompiler
TEMPLATE = app


CONFIG += c++11

RC_ICONS+=C:/SoftwareProjects/Qt_projects/projects/project_IV/frank_et_moi/images/ICONS/book.ico
RC_LANG+=English(US/UK)
RC_CODEPAGE+=****
VERSION+=0.1.0.0
QMAKE_TARGET_COMPANY+=-k0$m@3- Inc.
QMAKE_TARGET_DESCRIPTION+=Digital Compiler
QMAKE_TARGET_COPYRIGHT+=Licensed under the GPL License
QMAKE_TARGET_PRODUCT+=Digital Compiler

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    resources.qrc

HEADERS += \
    src/Bank.h \
    src/mainwindow.h \
    src/questionbank.h \
    src/ui_mainwindow.h

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/questionbank.cpp

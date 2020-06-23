#-------------------------------------------------
#
# Project created by QtCreator 2020-06-16T16:35:15.615Z
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = FunDisplay
TEMPLATE = lib

DEFINES += FUNDISPLAY_LIBRARY

CONFIG += c++11 plugin
CONFIG -= debug_and_release_target

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=         fundisplay.cpp         fundisplaywidget.cpp         fundisplaycontrols.cpp

HEADERS +=         fundisplay.h         fundisplaywidget.h         fundisplaycontrols.h

FORMS +=         fundisplaycontrols.ui

DISTFILES += \
    images/hobbits.jpeg \
    images/hobbits.png \
    images/odns.jpeg \
    images/one-does-not-simply.png

RESOURCES += \
    images.qrc

LIBS += -L$$OUT_PWD/../../../hobbits-core/ -lhobbits-core

INCLUDEPATH += $$PWD/../../../hobbits-core
DEPENDPATH += $$PWD/../../../hobbits-core

unix:!mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,'$$ORIGIN/../../lib:$$ORIGIN'"
}

mac {
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,'@executable_path/../Frameworks'"
}

unix {
    target.path = $$(HOME)/.local/share/hobbits/plugins/displays
    INSTALLS += target
}

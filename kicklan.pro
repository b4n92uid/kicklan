#-------------------------------------------------
#
# Project created by QtCreator 2017-01-27T23:33:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include('kicklan.pri');

TARGET = kicklan
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS TINS_STATIC WIN32

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $${LIBTINS_INCLUDE_DIR} $${WPCAP_INCLUDE_DIR} $${BOOST_INCLUDE_DIR}

LIBS += -L$${WPCAP_LIB_DIR} -L$${LIBTINS_LIB_DIR}

LIBS += -ltins -lwpcap -lIphlpapi -lWs2_32

Release:DESTDIR = ../release
Debug:DESTDIR = ../debug

SOURCES += main.cpp\
        mainwindow.cpp \
    ArpSpoofing.cpp \
    StatsModel.cpp \
    SnifferWorker.cpp \
    LanScannerTask.cpp

HEADERS  += mainwindow.h \
    ArpSpoofing.h \
    StatsModel.h \
    SnifferWorker.h \
    LanScannerTask.h

FORMS    += \
    MainWindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    kicklan.pri

#-------------------------------------------------
#
# Project created by QtCreator 2021-08-14T10:52:41
#
#-------------------------------------------------

QT       += core gui websockets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EasyChat
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        client.cpp \
        clientthread.cpp \
        common.cpp \
        controller.cpp \
        filelisttablewidget.cpp \
        filewidget.cpp \
        inputmessagewidget.cpp \
        logindialog.cpp \
        main.cpp \
        mainchatwidget.cpp \
        maintabwidget.cpp \
        mainwindow.cpp \
        messagewidget.cpp \
        monitor.cpp \
        mynetworkcontroller.cpp \
        mytablewidget.cpp \
        registerdlg.cpp \
        settingdlg.cpp \
        tools.cpp \
        userlistwidget.cpp

HEADERS += \
        client.h \
        clientthread.h \
        common.h \
        controller.h \
        filelisttablewidget.h \
        filewidget.h \
        inputmessagewidget.h \
        logindialog.h \
        mainchatwidget.h \
        maintabwidget.h \
        mainwindow.h \
        messagewidget.h \
        monitor.h \
        mynetworkcontroller.h \
        mytablewidget.h \
        registerdlg.h \
        settingdlg.h \
        tools.h \
        userlistwidget.h

FORMS += \
        filelisttablewidget.ui \
        filewidget.ui \
        inputmessagewidget.ui \
        logindialog.ui \
        mainchatwidget.ui \
        maintabwidget.ui \
        mainwindow.ui \
        messagewidget.ui \
        registerdlg.ui \
        settingdlg.ui \
        userlistwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    logo.ico \
    logo.rc

RC_FILE += logo.rc

LIBS += -lws2_32 -ldbghelp

msvc{
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}



TEMPLATE = app
TARGET = canalinux

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    broadcasterworkerthread.cpp \
    devicelist.cpp \
    devicelistcombobox.cpp \
    main.cpp \
    mainwindow.cpp \
    monitorworkerthread.cpp

HEADERS += \
    broadcasterworkerthread.h \
    devicelist.h \
    devicelistcombobox.h \
    mainwindow.h \
    monitorworkerthread.h

FORMS += \
    mainwindow.ui

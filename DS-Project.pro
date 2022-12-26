QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/graph.cpp \
    src/main.cpp \
    src/paintwidget.cpp \
    src/widget.cpp \

HEADERS += \
    src/include/graph.h \
    src/include/paintwidget.h \
    src/include/widget.h \

FORMS += \
    widget.ui

VERSION = 1.0.0.0

QMAKE_TARGET_COMPANY = "qwq"

QMAKE_TARGET_PRODUCT = "Traffic Advisory System Demo"

QMAKE_TARGET_DESCRIPTION = "qwq"

QMAKE_TARGET_COPYRIGHT = "Copyright 2022-2026 AkashiNeko. All rights reserved."

RC_ICONS = icon/icon.ico

RC_LANG = 0x0004

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


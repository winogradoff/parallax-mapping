QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = parallax_mapping
TEMPLATE = app

SOURCES += main.cpp\
        widget.cpp \
    glwidget.cpp \
    widget_slots.cpp

HEADERS  += widget.h \
    glwidget.h

FORMS    += widget.ui

RESOURCES += \
    resources.qrc

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vinograd_parallax_mapping_3
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    glwidget.cpp \
    widget_slots.cpp

HEADERS  += widget.h \
    glwidget.h

FORMS    += widget.ui

OTHER_FILES += \
    blinn.vert

RESOURCES += \
    resources.qrc

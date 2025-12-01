QT += core gui widgets multimedia multimediawidgets

CONFIG += c++11

TARGET = Tomeo
TEMPLATE = app

SOURCES += \
    the_button.cpp \
    the_player.cpp \
    tomeo.cpp \
    home.cpp \
    record.cpp

HEADERS += \
    the_button.h \
    the_player.h \
    home.h\
    record.h

RESOURCES += mima.qrc

MOC_DIR = moc
UI_DIR = ui
OBJECTS_DIR = obj


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

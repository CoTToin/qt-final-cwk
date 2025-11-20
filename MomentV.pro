QT += core gui multimedia multimediawidgets widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = MomentV
TEMPLATE = app

SOURCES += \
    main.cpp \
    the_button.cpp \
    core/ThePlayer.cpp \
    core/VideoManager.cpp \
    widgets/MainPage.cpp \
    widgets/RecordPage.cpp \
    widgets/ProfilePage.cpp \
    widgets/FriendsPage.cpp

HEADERS += \
    the_button.h \
    core/ThePlayer.h \
    core/VideoManager.h \
    widgets/MainPage.h \
    widgets/RecordPage.h \
    widgets/ProfilePage.h \
    widgets/FriendsPage.h

RESOURCES += resources/resources.qrc

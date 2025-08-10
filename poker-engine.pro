QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client/clientgamewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    server/cards.cpp \
    server/evaluate.cpp \
    server/game.cpp \
    server/player.cpp \
    server/server.cpp \
    client/client.cpp \
    server/serverworker.cpp

HEADERS += \
    client/clientgamewindow.hpp \
    server/cards.hpp \
    server/evaluate.hpp \
    server/game.hpp \
    mainwindow.hpp \
    server/player.hpp \
    server/server.hpp \
    client/client.hpp \
    server/serverworker.hpp

FORMS += \
    mainwindow.ui

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

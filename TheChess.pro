TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    piecesmodel.cpp \
    piece.cpp \
    boardmodel.cpp \
    elapsedtimer.cpp

RESOURCES += qml.qrc

HEADERS += \
    boardmodel.h \
    piecesmodel.h \
    piece.h \
    elapsedtimer.h

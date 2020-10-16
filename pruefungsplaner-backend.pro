QT -= gui
QT += websockets

CONFIG += c++17 console
CONFIG -= app_bundle

include($$PWD/libs/pruefungsplaner-datamodel/pruefungsplaner-datamodel.pri)
include($$PWD/libs/security-provider/client/client.pri)
include($$PWD/libs/qt-jsonrpc-server/qt-jsonrpc-server.pri)
INCLUDEPATH += $$PWD/libs/jwt-cpp/include

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        src/backendservice.cpp \
        src/main.cpp

HEADERS += \
        src/QtJsonTraits.h \
        src/backendservice.h

LIBS += -lcrypto

test{
    include($$PWD/libs/gtest/gtest_dependency.pri)

    QT *= testlib
    TEMPLATE = app
    TARGET = pruefungsplaner-backend-tests
    INCLUDEPATH *= $$PWD/src

    CONFIG *= thread
    LIBS *= -lgtest -lgtest_main

    SOURCES -= src/main.cpp
    SOURCES += tests/qthelper.cpp \
            tests/backendservicetest.cpp
}
else{
    TEMPLATE = app
    TARGET = pruefungsplaner-backend
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

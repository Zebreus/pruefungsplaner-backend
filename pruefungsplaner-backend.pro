QT -= gui
QT += websockets

CONFIG += c++17 console
CONFIG -= app_bundle

include($$PWD/libs/pruefungsplaner-datamodel/pruefungsplaner-datamodel.pri)
include($$PWD/libs/security-provider/client/client.pri)
include($$PWD/libs/security-provider/security-provider.pri)
include($$PWD/libs/qt-jsonrpc-server/qt-jsonrpc-server.pri)
INCLUDEPATH += $$PWD/libs/jwt-cpp/include
INCLUDEPATH += $$PWD/libs/cpptoml/include

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        src/backendservice.cpp \
        src/configuration.cpp \
        src/main.cpp

HEADERS += \
        src/QtJsonTraits.h \
        src/backendservice.h \
        src/configuration.h

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

unix{
    # Install executable
    target.path = /usr/bin

    # Install default config file
    config.path = /etc/$${TARGET}/
    config.files = res/config.toml

    # Create data directory
    datadir.path = /usr/share/$${TARGET}
    datadir.extra = " "
    datadir.uninstall = " "

    # Create directory for keys
    keys.path = $${datadir.path}/keys
    keys.extra = " "
    keys.uninstall = " "

    # Create directory for storage
    storage.path = $${datadir.path}/data
    storage.extra = " "
    storage.uninstall = " "
}

!isEmpty(target.path): INSTALLS += target
!isEmpty(config.path): INSTALLS += config
!isEmpty(keys.path): INSTALLS += keys
!isEmpty(storage.path): INSTALLS += storage
!isEmpty(datadir.path): INSTALLS += datadir

DEFINES += DEFAULT_CONFIG_PATH=\"\\\"$${config.path}\\\"\" \
           DEFAULT_KEYS_PATH=\"\\\"$${keys.path}\\\"\"\
           DEFAULT_STORAGE_PATH=\"\\\"$${storage.path}\\\"\"

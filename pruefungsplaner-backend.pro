QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/main.cpp \
        src/dataModel/day.cpp \
        src/dataModel/group.cpp \
        src/dataModel/module.cpp \
        src/dataModel/plan.cpp \
        src/dataModel/semester.cpp \
        src/dataModel/serializabledataobject.cpp \
        src/dataModel/timeslot.cpp \
        src/dataModel/week.cpp

HEADERS += \
        qt-jsonrpc-server/src/jsonrpcserver.h \
        qt-jsonrpc-server/src/jsonrpcconnection.h \
        src/dataModel/day.h \
        src/dataModel/group.h \
        src/dataModel/module.h \
        src/dataModel/plan.h \
        src/dataModel/semester.h \
        src/dataModel/serializabledataobject.h \
        src/dataModel/timeslot.h \
        src/dataModel/week.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

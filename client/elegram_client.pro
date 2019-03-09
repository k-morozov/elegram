QT += quick qml network gui
CONFIG += c++14
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

RESOURCES += qml.qrc
LIBS += -lprotobuf
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
TARGET = ElegramClient

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD/imports

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    protobuf/messages.pb.h \
    src/jobs/ijob.h \
    src/jobs/jobs.h \
    src/jobs/jobsutils.h \
    src/backend.h \
    src/message_processing/messagefactory.h \
    src/message_processing/messageparser.h \
    src/model/chatslist.h \
    src/model/chatsmodel.h \
    src/model/contactslist.h \
    src/model/contactsmodel.h \
    src/model/messageslist.h \
    src/model/messagesmodel.h \
    src/network/network_session.h \
    src/backend_facade.h

SOURCES += \
    protobuf/messages.pb.cc \
    src/jobs/jobs.cpp \
    src/jobs/jobsutils.cpp \
    src/backend.cpp \
    src/message_processing/messagefactory.cpp \
    src/message_processing/messageparser.cpp \
    src/model/chatslist.cpp \
    src/model/chatsmodel.cpp \
    src/model/contactslist.cpp \
    src/model/contactsmodel.cpp \
    src/model/messageslist.cpp \
    src/model/messagesmodel.cpp \
    src/network/network_session.cpp \
    src/backend_facade.cpp \
    src/main.cpp

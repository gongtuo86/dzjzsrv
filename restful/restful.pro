#-------------------------------------------------
#
# Project created by QtCreator 2023-04-23T10:01:09
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = dzjzwebrsrv
CONFIG   += console debug warn_off thread
CONFIG   -= app_bundle

MOC_DIR = $(RUNHOME)/tmp/dzjzsrv/restful/moc
OBJECTS_DIR = $(RUNHOME)/tmp/dzjzsrv/restful/obj
UI_DIR = $(RUNHOME)/tmp/dzjzsrv/restful/ui
RCC_DIR = $(RUNHOME)/tmp/dzjzsrv/restful/rcc

QMAKE_CXXFLAGS += -std=c++0x

QMAKE_LFLAGS += -Wl,-rpath,$(RUNHOME)/lib -Wl,-rpath,$(RUNHOME)/lib

TEMPLATE = app

HEADERS += \
            src/controller/mycontroller.hpp \
            src/websocket/WSListener.hpp \
            src/appcomponent.hpp \
            src/commonutils.h

SOURCES += \
            src/app.cpp \
            src/websocket/WSListener.cpp
 

DESTDIR = $$(RUNHOME)/bin

INCLUDEPATH +=  \
                $(RUNHOME_INC)/include \
                $(RUNHOME_INC)/smmi/component/dfjson/include \
                $(RUNHOME_INC)/dzjzsrv/dzjzprot \
                ./include \
                ./src \

# oatpp-websocket和oatpp顺序不能颠倒，依赖顺序为从右往左，所以oatpp-websocket必须在前面
LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -L$$PWD/lib -ldfnet -ldflogger -loatpp-websocket -loatpp -ldzjzprot -ldfjson -liconv -lexdb



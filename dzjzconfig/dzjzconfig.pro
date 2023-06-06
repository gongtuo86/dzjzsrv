#-------------------------------------------------
#
# Project created by QtCreator 2023-05-09T11:14:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug warn_off thread

TARGET = dzjzconfig
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

QMAKE_LFLAGS += -Wl,-rpath,$(RUNHOME)/lib -Wl,-rpath,$(RUNHOME)/lib

DEFINES += DF8003S
DESTDIR = $$(RUNHOME)/bin

MOC_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzconfig/moc
OBJECTS_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzconfig/obj
UI_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzconfig/ui
RCC_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzconfig/rcc

INCLUDEPATH +=  \
                $(RUNHOME_INC)/include \
                $(RUNHOME_INC)/smmi/component/dfjson/include

LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -lexdb -ldfnet -ldorbase -ldflogger -ldfjson


HEADERS  += mainwindow.h \
            operationdelegate.h \
            dmDelegate.h \
            commonformdialog.h \
            dto.h \
            dbmanager.h \
            deviceDelegate.h \
            jsontreedialog.h \
            dzjzconfigutil.h
      
SOURCES += main.cpp\
        mainwindow.cpp \
        operationdelegate.cpp \
        commonformdialog.cpp \
        dbmanager.cpp \
        deviceDelegate.cpp \
        jsontreedialog.cpp

FORMS    += mainwindow.ui \
            jsontreedialog.ui

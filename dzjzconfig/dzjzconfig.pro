#-------------------------------------------------
#
# Project created by QtCreator 2023-05-09T11:14:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug warn_off thread console

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
                $(RUNHOME_INC)/smmi/component/dfjson/include \
                $(RUNHOME_INC)/dzjzsrv/dzjzprot \
                $(RUNHOME_INC)/smmi/tools/alarmapplogin4/alarmapplogin \
                $(RUNHOME_INC)/include/mmi/interface

LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -lexdb -ldfnet -ldorbase -ldflogger -ldfjson -ldzjzprot -lalarmapplogin4


HEADERS  += mainwindow.h \
            operationdelegate.h \
            dmDelegate.h \
            commonformdialog.h \
            dto.h \
            dbmanager.h \
            jsontreedialog.h \
            dzjzconfigutil.h \
            callRunnable.h \
            dzjzfileparser.h

      
SOURCES += main.cpp\
        mainwindow.cpp \
        operationdelegate.cpp \
        commonformdialog.cpp \
        dbmanager.cpp \
        jsontreedialog.cpp \
        callRunnable.cpp \
        dzjzfileparser.cpp

FORMS    += mainwindow.ui \
            jsontreedialog.ui

RESOURCES += \
    theme/light/lightstyle.qrc \
    theme/dark/darkstyle.qrc \
    theme/green/greenstyle.qrc


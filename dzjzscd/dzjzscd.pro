QT += core
QT -= gui
# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug warn_off dll plugin thread

MOC_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzscd/moc
OBJECTS_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzscd/obj
UI_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzscd/ui
RCC_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzscd/rcc

QMAKE_CXXFLAGS += -std=c++0x

QMAKE_LFLAGS += -Wl,-rpath,$(RUNHOME)/lib -Wl,-rpath,$(RUNHOME)/lib

DEFINES += DZJZSCD_DLL DF8003S

win32{
    DEFINES += WIN32 _AFXDLL
    DEFINES -= _USRDLL
}

TEMPLATE = lib
TARGET = dzjzscd
DESTDIR = $$(RUNHOME)/lib

INCLUDEPATH +=  \
                $(RUNHOME_INC)/include \
                $(RUNHOME_INC)/scada/include \
                $(RUNHOME_INC)/smmi/component/dfjson/include \
                ../include

LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -lexdb -ldfnet -ldorbase -ldflogger -lscd_dll -ldfjson

HEADERS += \
            dzjzcommon.h \
            dzjzrounditem.h  \
            dzjzevent.h \
            dzjzcalctool.h \
            dzjzactionstatis.h \
            dzjzinspect.h \
            dzjzfilegenerator.h \
            dzjzupload.h

SOURCES	+= \
            dzjzcommon.cpp \
            dzjzrounditem.cpp \
            dzjzevent.cpp \
            dzjzcalctool.cpp \
            dzjzactionstatis.cpp \
            dzjzinspect.cpp \
            dzjzfilegenerator.cpp \



  





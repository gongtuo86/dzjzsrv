QT       += core
QT       -= gui
# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug warn_off dll plugin thread

MOC_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzprot/moc
OBJECTS_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzprot/obj
UI_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzprot/ui
RCC_DIR = $$(RUNHOME)/tmp/dzjzsrv/dzjzprot/rcc

QMAKE_CXXFLAGS += -std=c++0x

QMAKE_LFLAGS += -Wl,-rpath,$(RUNHOME)/lib -Wl,-rpath,$(RUNHOME)/lib

DEFINES += EPROTMMICTRL_DLL DF8003S

win32{
    DEFINES += WIN32 _AFXDLL
    DEFINES -= _USRDLL
}

TEMPLATE = lib
TARGET = dzjzprot
DESTDIR = $$(RUNHOME)/lib

INCLUDEPATH +=  \
                $(RUNHOME_INC)/include \
                $(RUNHOME_INC)/smmi/component/dfjson/include \
                $(RUNHOME_INC)/scada/include \
                ../dzjzscd \
                ../include

LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -lexdb -ldfnet -ldorbase -ldflogger -ldfjson -ldzjzscd

SOURCES	+= \
    dzjzprot.cpp \
    protapp.cpp \
    fixvaluesrv.cpp \
    protdb.cpp \
    protocol.cpp \
    net103_sh.cxx

HEADERS += \
    dzjzprot.h \
    protapp.h \
    fixvaluesrv.h \
    protdb.h \
    protdef.h \
    protocol.h \
    net103_sh.h

QT       += core
QT       -= gui
# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug warn_off thread console

MOC_DIR = $$(RUNHOME)/tmp/prot/dzjzscdsrv/moc
OBJECTS_DIR = $$(RUNHOME)/tmp/prot/dzjzscdsrv/obj
UI_DIR = $$(RUNHOME)/tmp/prot/dzjzscdsrv/ui
RCC_DIR = $$(RUNHOME)/tmp/prot/dzjzscdsrv/rcc

QMAKE_CXXFLAGS += -std=c++0x

QMAKE_LFLAGS += -Wl,-rpath,$(RUNHOME)/lib -Wl,-rpath,$(RUNHOME)/lib

DEFINES += DF8003S

win32{
    DEFINES += WIN32 _AFXDLL
    DEFINES -= _USRDLL
}

TEMPLATE = app
TARGET = DZJZ_Server
DESTDIR = $$(RUNHOME)/bin

INCLUDEPATH +=  \
                $(RUNHOME_INC)/include \
                $(RUNHOME_INC)/scada/include \
                $(RUNHOME_INC)/smmi/component/dfjson/include \
                ../include \
                ../dzjzscd

LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -ldfnet -ldflogger -ldzjzscd -ldorbase -lexdb -lscd_dll -ldfjson

SOURCES +=  \
          main.cpp \

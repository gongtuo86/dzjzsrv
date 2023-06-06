QT       += core network
QT       -= gui
# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug warn_off thread console

MOC_DIR = $$(RUNHOME)/tmp/prot/dzjzprotsrv/moc
OBJECTS_DIR = $$(RUNHOME)/tmp/prot/dzjzprotsrv/obj
UI_DIR = $$(RUNHOME)/tmp/prot/dzjzprotsrv/ui
RCC_DIR = $$(RUNHOME)/tmp/prot/dzjzprotsrv/rcc

QMAKE_CXXFLAGS += -std=c++0x

QMAKE_LFLAGS += -Wl,-rpath,$(RUNHOME)/lib -Wl,-rpath,$(RUNHOME)/lib

DEFINES += DF8003S

win32{
    DEFINES += WIN32 _AFXDLL
    DEFINES -= _USRDLL
}

TEMPLATE = app
TARGET = dzjzprotsrv
DESTDIR = $$(RUNHOME)/bin

INCLUDEPATH +=  \
                $(RUNHOME_INC)/include \
                $(RUNHOME_INC)/smmi/component/dfjson/include \
                ../dzjzprot
                

LIBS += -L$(RUNHOME)/bin -L$(RUNHOME)/lib -ldfnet -ldzjzprot -ldflogger

SOURCES +=  \
          main.cpp \

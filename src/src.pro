TEMPLATE = lib

CONFIG += link_pkgconfig plugin
CONFIG -= qt

test {
	CONFIG += staticlib
}

TARGET = latex-equations
INCLUDEPATH += . 

PKGCONFIG += gtk+-3.0
PKGCONFIG += glib-2.0
PKGCONFIG += evolution-mail-3.0

QMAKE_CXXFLAGS += -fPIC

DEFINES += GETTEXT_PACKAGE='\\"latex-equations\\"'
DESTDIR = ../lib

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
target.files = ${DESTDIR}/${TARGET}.so
INSTALLS += target

debug {
	OBJECTS_DIR = .obj.debug
}

release {
	OBJECTS_DIR = .obj.release
}

# Input
HEADERS += m-calendar-ui.h \
           m-mail-ui.h \
           m-msg-composer-extension.h \
           m-shell-view-extension.h \
           m-utils.h \
           latex-converter.h

SOURCES += \
           latex-equations-module.c \
           m-calendar-ui.c \
           m-mail-ui.c \
           m-msg-composer-extension.c \
           m-shell-view-extension.c \
           m-utils.c \
           latex-converter.cpp


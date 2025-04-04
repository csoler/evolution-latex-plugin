TEMPLATE = lib

CONFIG += link_pkgconfig plugin
CONFIG -= qt

test {
	CONFIG += staticlib
}

TARGET = example-module
INCLUDEPATH += . 

PKGCONFIG += gtk+-3.0
PKGCONFIG += glib-2.0
PKGCONFIG += evolution-mail-3.0

DEFINES += GETTEXT_PACKAGE='\\"example-module\\"'
DESTDIR = ../lib

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
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
           m-utils.h

SOURCES += example-module.c \
           m-calendar-ui.c \
           m-mail-ui.c \
           m-msg-composer-extension.c \
           m-shell-view-extension.c \
           m-utils.c

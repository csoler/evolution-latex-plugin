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

INSTALLED_FILES = $$DESTDIR/lib$${TARGET}.so 

target2.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/plugins/
target2.files = latex-equations.eplug

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
target.files = $$INSTALLED_FILES

INSTALLS = target target2

message("Target.path = " $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/)
message("Target.files = " $$INSTALLED_FILES)

debug {
	OBJECTS_DIR = .obj.debug
}

release {
	OBJECTS_DIR = .obj.release
}

# Input
HEADERS += \
           m-msg-composer-extension.h \
           latex-converter.h

SOURCES += \
           latex-equations-module.c \
           m-msg-composer-extension.c \
           latex-converter.cpp


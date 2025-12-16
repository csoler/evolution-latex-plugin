TEMPLATE = lib

CONFIG += link_pkgconfig plugin
CONFIG -= qt

test {
	CONFIG += staticlib
}

TARGET = latex-equations
INCLUDEPATH += . 

QMAKE_CXXFLAGS += -fPIC 

PKGCONFIG += glib-2.0
PKGCONFIG += gtk+-3.0

EDS_VERSION = $$system(pkg-config --modversion libedataserver-1.2)

message("EDS Version: $$EDS_VERSION")

PKGCONFIG += evolution-mail-3.0

DEFINES += GETTEXT_PACKAGE='\\"latex-equations\\"'
DESTDIR = ../lib

target2.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/plugins/
target2.files = latex-equations.eplug

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
target.files = $$DESTDIR/lib$${TARGET}.so 

INSTALLS = target target2

debug {
	OBJECTS_DIR = .obj.debug
}

release {
	OBJECTS_DIR = .obj.release
}

HEADERS += latex-converter.h
SOURCES += latex-converter.cpp

greaterThan(EDS_VERSION,3.50.0) {
	HEADERS += latex-equations-extension-gtk4.h 
	SOURCES += latex-equations-extension-gtk4.c 
} else {
	HEADERS += latex-equations-extension-gtk3.h 
	SOURCES += latex-equations-extension-gtk3.c 
}

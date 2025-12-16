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

greaterThan(EDS_VERSION,3.50.0) {
} else {

# Input
	HEADERS += \
			m-msg-composer-extension.h \
			latex-converter.h

	SOURCES += \
			m-msg-composer-extension.c \
			latex-converter.cpp

}

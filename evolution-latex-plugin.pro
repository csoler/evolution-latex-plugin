TEMPLATE = subdirs
SUBDIRS += src
CONFIG += ordered

test {
	SUBDIRS += test
}

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
target.files = lib/liblatex-equations.so
INSTALLS += target


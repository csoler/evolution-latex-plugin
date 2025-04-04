TEMPLATE = subdirs
SUBDIRS += src
CONFIG += ordered

test {
	SUBDIRS += test
}

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
target.files = lib/libexample-module.so
INSTALLS += target


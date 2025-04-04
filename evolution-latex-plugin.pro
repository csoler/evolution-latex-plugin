TEMPLATE = subdirs
SUBDIRS += src

target.path = $$(HOME)/.local/share/evolution/modules/lib/evolution/modules/
target.files = lib/libexample-module.so
INSTALLS += target


TEMPLATE = app

CONFIG -= qt
LIBS += -lpng

INCLUDEPATH += ../src

SOURCES = test.cpp \
          ../src/latex-converter.cpp

HEADERS = ../src/latex-converter.h
DESTDIR = ../bin




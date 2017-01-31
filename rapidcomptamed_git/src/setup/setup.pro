# Adding a general build configuration
# (c) Pierre-Marie Desombre, Eric MAEKER, 2008-2009
# BSD revised Licence

TEMPLATE = app
TARGET = setup

# always include the config.pri __after__ the TARGET's definition
include(../../buildspecs/config.pri)

# redefine the TARGET's name (this can be moved in the config.pri file)
TARGET = $$BINARY_NAME

# define destination path
DESTDIR = $$BUID_BINARY_PATH

DEPENDPATH += $$PWD

# Input
HEADERS += wiz1.h page1.h
FORMS   += page1.ui
SOURCES += main.cpp wiz1.cpp page1.cpp

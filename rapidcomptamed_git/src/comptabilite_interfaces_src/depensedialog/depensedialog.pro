# Adding a general build configuration
# (c) Pierre-Marie Desombre, Eric MAEKER, 2008-2009
# BSD revised Licence

TEMPLATE = app
TARGET = depensedialog

message("qmake depense.pro")

# always include the config.pri __after__ the TARGET's definition
include(../../../buildspecs/config.pri)

# redefine the TARGET's name (this can be moved in the config.pri file)
TARGET = $$BINARY_NAME

# define destination path
DESTDIR = $$BUILD_BINARY_PATH

DEPENDPATH += $$PWD

# include common code
include($$SOURCES_ROOT_PATH/src/common/common.pri)

# include qprintereasy code
include($$SOURCES_ROOT_PATH/src/common/print/src.pri)

#include configure class
include($$SOURCES_ROOT_PATH/src/common/configure/configure.pri)

# Input
HEADERS += depensedialog.h \
           treeviewdelegate.h \
           analyse.h \
           depensetools.h 

FORMS +=   depensedialog.ui  \
           analysedialog.ui 

SOURCES +=  main.cpp \
            depensedialog.cpp \
            treeviewdelegate.cpp \
            analyse.cpp \
            depensetools.cpp 

QT += sql 

win32 {
CONFIG += qt windows release
}



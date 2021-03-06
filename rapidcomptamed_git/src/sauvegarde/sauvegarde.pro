######################################################################
# Automatically generated by qmake (2.01a) mar. janv. 20 20:24:50 2009
######################################################################

TEMPLATE = app
TARGET = sauvegarde

message("qmake sauvegarde.pro")

# always include the config.pri __after__ the TARGET's definition
include(../../buildspecs/config.pri)

# redefine the TARGET's name (this can be moved in the config.pri file)
TARGET = $$BINARY_NAME

# define destination path
DESTDIR = $$BUILD_BINARY_PATH

DEPENDPATH += $$PWD

# include common code
include($$SOURCES_ROOT_PATH/src/common/common.pri)

#include configure code
include($$SOURCES_ROOT_PATH/src/common/configure/configure.pri)

# Input
HEADERS += connexions.h recherche_mysqldump.h  sauvedialog.h  recup.h connect.h findFileSauve.h
FORMS += recherche_mysqldump.ui  sauvedialog.ui RecupDialog.ui ConnectDialog.ui
SOURCES += main.cpp recherche_mysqldump.cpp sauvedialog.cpp recup.cpp connect.cpp findFileSauve.cpp
QT += sql
win32 {
CONFIG += qt windows release #console
}

# Adding a general build configuration
# (c) Pierre-Marie Desombre, Eric MAEKER, 2008-2009
# BSD revised Licence

TEMPLATE = app
TARGET = recettedialog

message("qmake recettedialog.pro")

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

#include ccam code
#include($$SOURCES_ROOT_PATH/src/ccam/ccam.pri)

#include configure class
include($$SOURCES_ROOT_PATH/src/common/configure/configure.pri)

# Input
HEADERS +=  modifierrecette.h \
            recettedialog.h \
            rafraichirdialog.h \
            customModel.h \
            pondere.h \
            rapidmodel.h \
            recettesconstants.h \
            ../parametrages/parametrages.h \
            ../../recettestriees/etatdesdus/etatdesdus.h\
            ../../recettestriees/etatdesdus/tableau.h

FORMS += recettedialog.ui \
         modifierrecette.ui \
         rafraichirdialog.ui \
         pondere.ui\
         ../parametrages/ParametragesDialog.ui \
         ../../recettestriees/etatdesdus/etatdesdusdialog.ui\
         ../../recettestriees/etatdesdus/tableaudialog.ui\

SOURCES += main.cpp \
           modifierrecette.cpp \
           recettedialog.cpp \
           rafraichirdialog.cpp \
           customModel.cpp \
           pondere.cpp\
           rapidmodel.cpp \
           ../parametrages/parametrages.cpp \
           ../../recettestriees/etatdesdus/etatdesdus.cpp\
           ../../recettestriees/etatdesdus/tableau.cpp

QT += sql \
      xml \
      qt3support

#win32 {
#CONFIG += qt windows release console
#}



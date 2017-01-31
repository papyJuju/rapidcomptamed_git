# Adding a general build configuration
# (c) Pierre-Marie Desombre, Eric MAEKER, 2008-2009
# BSD revised Licence

TEMPLATE = app
TARGET = depensestriees

# always include the config.pri __after__ the TARGET's definition
include(../../../buildspecs/config.pri)

# redefine the TARGET's name (this can be moved in the config.pri file)
TARGET = $$BINARY_NAME

# define destination path
DESTDIR = $$BUID_BINARY_PATH

DEPENDPATH += $$PWD

# include common code
include($$SOURCES_ROOT_PATH/src/common/common.pri)

# Input
HEADERS += depensestriees.h \
           ../etatstries/etatstries.h \
           ../../common/completeBase.h \
           ../../common/creationtable.h \
           ../../common/configure/utilisateur.h \
           ../../common/configure/modifutil.h \
           ../../common/configure/mdp.h

SOURCES +=  main.cpp \
            depensestriees.cpp \
	    ../etatstries/etatstries.cpp \
	    ../../common/completeBase.cpp \
	    ../../common/creationtable.cpp \
	    ../../common/configure/utilisateur.cpp \
            ../../common/configure/modifutil.cpp \
            ../../common/configure/mdp.cpp 

FORMS += ../etatstries/etatstries.ui \
           ../../common/configure/utilisateurdialog.ui \
           ../../common/configure/modifUtil.ui \
           ../../common/configure/MdpDialog.ui

QT += sql

INCLUDEPATH += $${PWD}
#INCLUDEPATH += $${PWD}/../../catdoc/catdoclib/
#DEPENDPATH += $${PWD}/../../catdoc/catdoclib/

#file:///home/debianpmd/rapidcomptamed/catdoc/catdoclib/libcatdoclib.a

#LIBS += -L$${PWD}/../../catdoc/catdoclib/ -llibcatdoclib

HEADERS += $${PWD}/parseCCAM.h \
           $${PWD}/ccambasethread.h \
           $${PWD}/parseconstants.h \
           $${PWD}/findFile.h
SOURCES += $${PWD}/parseCCAM.cpp \
           $${PWD}/ccambasethread.cpp \
           $${PWD}/findFile.cpp
FORMS +=  $${PWD}/parse.ui



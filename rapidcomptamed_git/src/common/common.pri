# Common code includer
# (c) Eric MAEKER, 2009
# BSD revised Licence

INCLUDEPATH += $${PWD}

HEADERS += \
    $${PWD}/databasemanager.h \
    $${PWD}/settings.h \
    $${PWD}/icore.h \
    $${PWD}/core.h \
    $${PWD}/commandlineparser.h \
    $${PWD}/test.h \
    $${PWD}/constants.h \
    $${PWD}/position.h \
    $${PWD}/tablesdiagrams.h \
    $${PWD}/version.h \    
    $${PWD}/xmlparser.h 
    
SOURCES += \
    $${PWD}/databasemanager.cpp \
    $${PWD}/settings.cpp \
    $${PWD}/core.cpp \
    $${PWD}/position.cpp \
    $${PWD}/test.cpp \
    $${PWD}/xmlparser.cpp 


QT += sql xml

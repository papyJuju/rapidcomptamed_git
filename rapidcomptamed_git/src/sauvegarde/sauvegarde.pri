#project by Pierre-Marie Desombre
#2014
#pm.desombre@gmail.com

INCLUDEPATH += $${PWD}

HEADERS += $${PWD}/connexions.h \
           $${PWD}/recherche_mysqldump.h  \
           $${PWD}/sauvedialog.h  \
           $${PWD}/recup.h \
           $${PWD}/connect.h \
           $${PWD}/findFileSauve.h
FORMS += $${PWD}/recherche_mysqldump.ui  \
         $${PWD}/sauvedialog.ui \
         $${PWD}/RecupDialog.ui \
         $${PWD}/ConnectDialog.ui
SOURCES += $${PWD}/recherche_mysqldump.cpp \
           $${PWD}/sauvedialog.cpp \
           $${PWD}/recup.cpp \
           $${PWD}/connect.cpp \
           $${PWD}/findFileSauve.cpp

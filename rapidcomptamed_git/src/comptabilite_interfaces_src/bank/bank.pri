#project by Pierre-Marie Desombre
#2012
#pm.desombre@gmail.com

INCLUDEPATH += $${PWD}

HEADERS += $${PWD}/deposit.h \
           $${PWD}/controlbalance.h \
           $${PWD}/validdialog.h \
           $${PWD}/movementschecking.h \
           $${PWD}/checkdelegate.h \
           $${PWD}/receiptschecking.h \
           $${PWD}/bankMainWindow.h

FORMS += $${PWD}/depositwidget.ui\
         $${PWD}/balancedialog.ui \
         $${PWD}/validDialog.ui \
         $${PWD}/movementsChecking.ui \
         $${PWD}/receiptsChecking.ui \
         $${PWD}/mainWindowBank.ui


SOURCES += $${PWD}/deposit.cpp \
           $${PWD}/controlbalance.cpp \
           $${PWD}/validdialog.cpp \
           $${PWD}/movementschecking.cpp \
           $${PWD}/checkdelegate.cpp \
           $${PWD}/receiptschecking.cpp \
           $${PWD}/bankMainWindow.cpp


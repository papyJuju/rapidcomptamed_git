QT *= xml
QT *= sql
INCLUDEPATH += $${PWD}
SOURCES += $${PWD}/qprintereasy.cpp \
   $${PWD}/qtextdocumentextra.cpp \
    $${PWD}/headingUserParam/headingParam.cpp
FORMS += $${PWD}/headingUserParam/headingParamDialog.ui
HEADERS += $${PWD}/qprintereasy.h \
    $${PWD}/qprintereasy_global.h \
    $${PWD}/qtextdocumentextra.h \
    $${PWD}/headingUserParam/headingParam.h
message(src.pri path $${PWD}/headingUserParam/headingParamDialog.ui)


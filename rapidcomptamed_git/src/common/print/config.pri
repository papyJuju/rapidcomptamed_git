# define some paths
PACKAGE_PWD	      = $${PWD}/../

# prepare package name // TARGET __MUST BE__ defined BEFORE this file is included
PACKAGE_TARGET      = $$quote($${TARGET})

# package destdir
PACKAGE_DESTDIR     = $${PACKAGE_PWD}/bin

# temporary path for building
PACKAGE_BUILD_PATH  = $${PACKAGE_PWD}/build

# build mode
CONFIG     += qt warn_on thread x11 windows
mac:CONFIG *= x86 ppc

# define build paths
CONFIG( debug, debug|release ) {
  #Debug
  message( Building $${PACKAGE_TARGET} in DEBUG )
  CONFIG      += console
  mac:CONFIG  -= ppc
  unix:PACKAGE_TARGET   = $$quote($$join(PACKAGE_TARGET,,,_debug))
  else:PACKAGE_TARGET   = $$quote($$join(PACKAGE_TARGET,,,_d))
  DEFINES  *= QPRINTEREASY_DEBUG
} else {
  #Release
  message( Building in RELEASE )
  DEFINES   *= QPRINTEREASY_RELEASE
}

# define objects, ui, rcc building path
unix:OBJECTS_DIR   = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/unix
win32:OBJECTS_DIR  = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/win32
mac:OBJECTS_DIR    = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/mac
UI_DIR	           = $${PACKAGE_BUILD_PATH}/.ui
MOC_DIR	           = $${PACKAGE_BUILD_PATH}/$${TARGET}/.moc
RCC_DIR	           = $${PACKAGE_BUILD_PATH}/$${TARGET}/.rcc

# define some usefull values
QMAKE_TARGET_COMPANY       = "QPrinterEasy Team"
QMAKE_TARGET_PRODUCT       = $${TARGET}
QMAKE_TARGET_DESCRIPTION   = "Some description"
QMAKE_TARGET_COPYRIGHT     = "Copyright (C) 2009 QPrinterEasy team"
PACKAGE_DOMAIN             = "http://code.google.com/p/qprintereasy"

# define variable for source code
DEFINES	*= "PACKAGE_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\"" \
           "PACKAGE_VERSION=\"\\\"$${PACKAGE_VERSION}\\\"\"" \
           "PACKAGE_DOMAIN=\"\\\"$${PACKAGE_DOMAIN}\\\"\"" \
           "PACKAGE_COPYRIGHTS=\"\\\"$${QMAKE_TARGET_COPYRIGHT}\\\"\""

TARGET   = $${PACKAGE_TARGET}
DESTDIR  = $${PACKAGE_DESTDIR}
INCLUDEPATH  = $${PWD}

# General build configuration
# (c) Eric MAEKER, 2009
# BSD revised Licence

# check that TARGET is defined -> otherwise stop the build process with an error
isEmpty(TARGET):error(You must define a TARGET when including config.pri)

# define some paths related to applications and sources
# assuming this file is placed in the root path of the sources
SOURCES_ROOT_PATH = $$PWD/../
SOURCES_BUILD_PATH = $${SOURCES_ROOT_PATH}/build
SOURCES_RESOURCES = $${SOURCES_ROOT_PATH}/resources

# define some paths related to the binaries
BUILD_BINARY_PATH = $${SOURCES_ROOT_PATH}/bin

message(building $$TARGET in $$SOURCES_BUILD_PATH)

# redefine binary target in debug mode add 'd' or '_debug'
BINARY_TARGET = $$quote($${TARGET})
CONFIG( debug, debug|release ) {
   unix:BINARY_TARGET = $$quote($$join(BINARY_TARGET,,,_debug))
   else:BINARY_TARGET = $$quote($$join(BINARY_TARGET,,,d))
}
# make the binary name available in c++ code using macro definition
DEFINES	*= "BINARY_NAME=\"\\\"$${BINARY_TARGET}\\\"\""

# define building path
unix:OBJECTS_DIR = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/unix
win32:OBJECTS_DIR = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/win32
mac:OBJECTS_DIR = $${SOURCES_BUILD_PATH}/$${BINARY_TARGET}/.obj/mac
UI_DIR	 = $${SOURCES_BUILD_PATH}/$${TARGET}/.ui
MOC_DIR	 = $${SOURCES_BUILD_PATH}/$${TARGET}/.moc
RCC_DIR	 = $${SOURCES_BUILD_PATH}/$${TARGET}/.rcc

# Define include path from /src
INCLUDEPATH += $${SOURCES_ROOT_PATH}/src

# Define DEBUG/RELEASE macros
CONFIG(debug, debug|release) {
  message( Building $${BINARY_TARGET} in DEBUG )
  DEFINES  *= DEBUG
} else {
  message( Building $${BINARY_TARGET} in RELEASE )
  DEFINES  *= RELEASE
  # no warning from application
#  DEFINES  *= QT_NO_DEBUG_OUTPUT \
#              QT_NO_WARNING_OUTPUT \
#              QT_NO_DEBUG
}

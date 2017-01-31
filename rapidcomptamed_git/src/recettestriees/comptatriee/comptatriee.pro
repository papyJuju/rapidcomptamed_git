# Adding a general build configuration
# (c) Pierre-Marie Desombre, Eric MAEKER, 2008-2009
# BSD revised Licence
TEMPLATE = app
TARGET = RapidComptaMed

message("qmake comptatriee.pro")

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

#include bank code
include($$SOURCES_ROOT_PATH/src/comptabilite_interfaces_src/bank/bank.pri)

#include bankcontrol code
include($$SOURCES_ROOT_PATH/src/comptabilite_interfaces_src/bankcontrols/bankcontrol.pri)

#include facturation
include($$SOURCES_ROOT_PATH/src/comptabilite_interfaces_src/facturation/facturation.pri)

#include parseCCAM code
include($$SOURCES_ROOT_PATH/src/parse_CCAM/parse_CCAM.pri)

#include sauvegarde code
include($$SOURCES_ROOT_PATH/src/sauvegarde/sauvegarde.pri)

#include alertes code
include($$SOURCES_ROOT_PATH/src/alertes/alertes.pri)

#include export_fiscal code
include($$SOURCES_ROOT_PATH/src/recettestriees/export_fiscal/exportfiscal.pri)

# Input
HEADERS += comptatrieemainwindow.h \
           ../grand_livre/grandlivre.h \
           ../grand_livre/myThread.h \
           ../recettestriees/recettestriees.h \
           ../depensestriees/depensestriees.h \
           ../etatstries/etatstries.h \
           ../etatstries/etatsconstants.h \
           ../etatdesdus/etatdesdus.h \
           ../etatdesdus/tableau.h \
           ../frenchTax/frenchTax.h\
           ../../notes/notes.h \
           ../../setup/wiz1.h \
           ../../setup/page1.h \
           ../../comptabilite_interfaces_src/immobilisations/immobilisations.h \
           ../../comptabilite_interfaces_src/immobilisations/message.h \
           ../../comptabilite_interfaces_src/immobilisations/NouvelleImmobilisation.h \
           ../../comptabilite_interfaces_src/parametrages/parametrages.h \
           ../../comptabilite_interfaces_src/depensedialog/depensedialog.h \
           ../../comptabilite_interfaces_src/depensedialog/analyse.h \
           ../../comptabilite_interfaces_src/depensedialog/treeviewdelegate.h \
           ../../comptabilite_interfaces_src/depensedialog/depensetools.h \
           ../../comptabilite_interfaces_src/recettedialog/recettedialog.h \
           ../../comptabilite_interfaces_src/recettedialog/modifierrecette.h \
           ../../comptabilite_interfaces_src/recettedialog/rafraichirdialog.h \
           ../../comptabilite_interfaces_src/recettedialog/customModel.h \
           ../../comptabilite_interfaces_src/recettedialog/pondere.h \
           ../../comptabilite_interfaces_src/recettedialog/rapidmodel.h \
           ../../comptabilite_interfaces_src/recettedialog/recettesconstants.h

FORMS +=   comptatrieemainwindow.ui \
           ../grand_livre/grandlivredialog.ui \
           ../etatstries/etatstries.ui \
           #../etatstries/PratIdDialog.ui \
           ../etatdesdus/etatdesdusdialog.ui \
           ../etatdesdus/tableaudialog.ui \
           ../frenchTax/frenchTaxDialog.ui \
           ../../notes/Notes.ui \
           ../../setup/page1.ui \
           ../../comptabilite_interfaces_src/immobilisations/immobilisations.ui \
           ../../comptabilite_interfaces_src/immobilisations/NouvelleImmobilisation.ui \
           ../../comptabilite_interfaces_src/parametrages/ParametragesDialog.ui \
           ../../comptabilite_interfaces_src/depensedialog/depensedialog.ui  \
           ../../comptabilite_interfaces_src/depensedialog/analysedialog.ui \
           ../../comptabilite_interfaces_src/recettedialog/rafraichirdialog.ui \
           ../../comptabilite_interfaces_src/recettedialog/modifierrecette.ui \
           ../../comptabilite_interfaces_src/recettedialog/recettedialog.ui \
           ../../comptabilite_interfaces_src/recettedialog/pondere.ui 

SOURCES += comptatrieemainwindow.cpp \
           ../grand_livre/grandlivre.cpp \
           ../grand_livre/myThread.cpp \
           ../recettestriees/recettestriees.cpp \
           ../depensestriees/depensestriees.cpp \
           ../etatstries/etatstries.cpp \
           #../etatstries/pratid.cpp \
           ../etatdesdus/etatdesdus.cpp \
           ../etatdesdus/tableau.cpp \
           ../frenchTax/frenchTax.cpp\
           ../../notes/notes.cpp\
           ../../setup/wiz1.cpp \
           ../../setup/page1.cpp \
           ../../comptabilite_interfaces_src/immobilisations/immobilisations.cpp \
           ../../comptabilite_interfaces_src/immobilisations/message.cpp \
           ../../comptabilite_interfaces_src/immobilisations/NouvelleImmobilisation.cpp \
           ../../comptabilite_interfaces_src/parametrages/parametrages.cpp \
           ../../comptabilite_interfaces_src/depensedialog/depensedialog.cpp \
           ../../comptabilite_interfaces_src/depensedialog/analyse.cpp \
           ../../comptabilite_interfaces_src/depensedialog/treeviewdelegate.cpp \
           ../../comptabilite_interfaces_src/depensedialog/depensetools.cpp \
           ../../comptabilite_interfaces_src/recettedialog/recettedialog.cpp \
           ../../comptabilite_interfaces_src/recettedialog/modifierrecette.cpp \
           ../../comptabilite_interfaces_src/recettedialog/rafraichirdialog.cpp \
           ../../comptabilite_interfaces_src/recettedialog/customModel.cpp \
           ../../comptabilite_interfaces_src/recettedialog/pondere.cpp \
           ../../comptabilite_interfaces_src/recettedialog/rapidmodel.cpp \
           main.cpp 
           
QT += xml sql widgets

win32 {
CONFIG += qt windows release 
}          

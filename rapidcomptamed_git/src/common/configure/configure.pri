INCLUDEPATH += $${PWD}
include(../../ccam/ccam.pri)
# Input
HEADERS += $${PWD}/mdp.h \
           $${PWD}/utilisateur.h \
           $${PWD}/modifutil.h \
            $${PWD}/sitesdialog.h \
            $${PWD}/payeursdialog.h \
            $${PWD}/actesdispo.h \
            $${PWD}/pourcentages.h \
            $${PWD}/parametredialog.h \
            $${PWD}/automatisme.h \
            $${PWD}/vision.h \
            $${PWD}/help.h \
            $${PWD}/thesaurus.h \
            $${PWD}/util.h \
            $${PWD}/pratid.h \
            $${PWD}/passdialog.h \
            $${PWD}/comptatorapidcompta.h \
            $${PWD}/delegate.h \
            $${PWD}/diversblob.h \
            $${PWD}/tokens.h \
            $${PWD}/nameindex.h \
            $${PWD}/iotools.h \
            $${PWD}/rapport.h \
            $${PWD}/tools.h \
            $${PWD}/configdatas.h \
            $${PWD}/configmedintuxini.h \
            $${PWD}/message_alerte.h \
            $${PWD}/medintux/constants_medintux.h \
            $${PWD}/medintux/connexion_medintux.h             

FORMS +=   $${PWD}/utilisateurdialog.ui \
           $${PWD}/MdpDialog.ui \
           $${PWD}/modifUtil.ui \
           $${PWD}/sitesdialog.ui \
           $${PWD}/payeursdialog.ui \
           $${PWD}/actesdispodialog.ui \
           $${PWD}/pourcentagesdialog.ui \
           $${PWD}/parametredialog.ui \
           $${PWD}/automatisme.ui \
           $${PWD}/rapport.ui \
           $${PWD}/helpForm.ui \
           $${PWD}/thesaurusWidget.ui \
           $${PWD}/PratIdDialog.ui \
           $${PWD}/PassDialog.ui \
           $${PWD}/comptatorapidcompta.ui \
           $${PWD}/nameindex.ui \
           $${PWD}/configmedintuxini.ui \
           $${PWD}/configwidget.ui \
           $${PWD}/vision.ui

SOURCES += $${PWD}/mdp.cpp \
           $${PWD}/utilisateur.cpp \
           $${PWD}/modifutil.cpp \
            $${PWD}/sitesdialog.cpp \
           $${PWD}/payeursdialog.cpp \
           $${PWD}/actesdispo.cpp \
           $${PWD}/pourcentages.cpp \
           $${PWD}/parametredialog.cpp \
           $${PWD}/automatisme.cpp \
           $${PWD}/vision.cpp \
           $${PWD}/help.cpp \
           $${PWD}/thesaurus.cpp \
           $${PWD}/util.cpp \
           $${PWD}/pratid.cpp \
           $${PWD}/passdialog.cpp \
           $${PWD}/comptatorapidcompta.cpp \
           $${PWD}/delegate.cpp \
           $${PWD}/tokens.cpp \
           $${PWD}/nameindex.cpp \
           $${PWD}/iotools.cpp \
           $${PWD}/rapport.cpp \
           $${PWD}/tools.cpp \
           $${PWD}/configmedintuxini.cpp \
           $${PWD}/configdatas.cpp \
           $${PWD}/message_alerte.cpp \
           $${PWD}/medintux/connexion_medintux.cpp



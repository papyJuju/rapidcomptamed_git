#ifndef NAMEINDEX_H
#define NAMEINDEX_H

#include "ui_nameindex.h"
#include <common/configure/medintux/connexion_medintux.h>
#include <QSqlTableModel>
#include <QDialog>

namespace NameIndexDiagramSpace
{
    enum NameindexHeaders
    {
        ID_INDEX=0,
        NAME,
        SURNAME,
        GUID,
        DIVERS,
        NameindexHeadersMaxParam
        };
};

class NamesDialog:public QDialog,public Ui::NamesDialog
{
    Q_OBJECT
    public:
         NamesDialog(QWidget * parent = 0, QSqlDatabase db = QSqlDatabase(),const QString & programname = QString());
        ~NamesDialog ();
        QString getName();
    private slots:
        void textchanged(const QString & texts);
        void recordName();
        void deleteName();
    private:
        QSqlTableModel *m_tableModel;
        QSqlDatabase m_dbin;
        QSqlDatabase m_dbMedintux;
        MedintuxConnexionSpace::ConnexionMedintux * m_connexion;
        QString m_name;
        QString m_programName;
};  

#endif


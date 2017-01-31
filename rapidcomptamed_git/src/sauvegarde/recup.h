#ifndef RECUP_H
#define RECUP_H

#include "ui_RecupDialog.h"
#include "findFileSauve.h"
#include <QWidget>
#include <QSqlDatabase>

class recup : public QWidget, public Ui::RecupDialog
{
    Q_OBJECT
    public :
        recup(QObject * parent = 0);
        ~recup();
    private slots :
        void setWhatsThisMode();
        void wait();
        void restaurer();
        void drtuxselected(const QString & text);
    private :
        QSqlDatabase m_dbdrtux;
        QSqlDatabase m_dbcompta;
        SauvegardeSpace::findFileSauve *find;
        QString m_backupFile;
        QString m_backupFilePath;
        QString m_host,m_user,m_pass,m_port;
};

#endif 

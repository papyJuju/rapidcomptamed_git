#ifndef COMPTATORAPIDCOMPTA_H
#define COMPTATORAPIDCOMPTA_H

#include "ui_comptatorapidcompta.h"
#include <QWidget>
#include <QSqlDatabase>

class ComptaToRapid:public QWidget,public Ui::ComptatorapidcomptaWidget
{
    Q_OBJECT
    public:
        ComptaToRapid (QWidget * parent = 0);
        ~ComptaToRapid ();
        
    private slots:
        void getBaseToInsert(QListWidgetItem *);
        void run();
    private:
        void getBasesToInsert();
        bool connectOldBase(const QString & basechoosen);
        bool runCopyBase();
        bool renameSqliteBase();
        QString m_pass;
        QStringList m_oldtables;
        QHash<QString,QString> m_hasholdfields;
        QString m_basechoosen;
        QSqlDatabase m_driverbase;
};

#endif


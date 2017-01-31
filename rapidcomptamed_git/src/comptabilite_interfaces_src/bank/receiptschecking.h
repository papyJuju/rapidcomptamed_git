#ifndef RECEIPTSCHECKING_H
#define RECEIPTSCHECKING_H

#include "ui_receiptsChecking.h"

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QHash>

class ReceiptsChecking:public QWidget,public Ui::receiptsCheckingWidget
{
    Q_OBJECT
    enum ReceipsHeaders
    {
    ID_HONO = 0,
    ID_USR,
    ID_DRTUX_USR,
    PATIENT,
    ID_SITE,
    ID_PAYEURS,
    GUID,
    PRATICIEN,
    DATE,
    ACTE,
    ACTECLAIR,
    REMARQUE,
    ESP,
    CHQ,
    CB,
    VIR,
    DAF,
    AUTRE,
    DU,
    DU_PAR,
    VALIDE,
    TRACABILITE,
    ReceipsHeaders_MaxParam
    };
    enum Users{
        ID_USER = 0,
        USER_NAME,
        LOGIN        
        };
    public:
        ReceiptsChecking(QWidget * parent = 0);
        ~ReceiptsChecking();
    private slots :
        void getReceiptsUnchecked(const QString & login);
        void recordCheckedReceipts();
        void revertCheckedReceipts();
        void recordDetail();
    private:
        QDate getFirstDate(const QString & id);
        QSqlDatabase m_accountancyDb;
        QSqlTableModel *m_model;   
        QHash<QString,QString> m_hashLoginId;     
};

#endif


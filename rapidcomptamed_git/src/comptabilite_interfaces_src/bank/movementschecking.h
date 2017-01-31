#ifndef MOVEMENTSCHECKING_H
#define MOVEMENTSCHECKING_H

#include "ui_movementsChecking.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QHash>

class MovementsChecking:public QWidget,public Ui::movementsCheckingWidget
{
    Q_OBJECT
    enum MouvementsHeaders
    {
        ID_MOUVEMENT = 0,
        ID_MVT_DISPO,
        ID_USR,
        ID_COMPTE,
        TYPE,
        LIBELLE,
        DATE,
        DATE_VALEUR,
        MONTANT,
        REMARQUE,
        VALIDE,
        TRACABILITE,
        VALIDATION,
        DETAIL,
        MouvementsHeaders_MaxParam
        };
    enum Users{
        ID_USER = 0,
        USER_NAME,
        LOGIN        
        };
    public:
        MovementsChecking(QWidget * parent = 0);
        ~MovementsChecking();
    private slots :
        void getMovementsUnchecked(const QString & login);
        void recordCheckedMovements();
        void revertCheckedMovements();
    private:
        QDate getFirstDate(const QString & id);
        QSqlDatabase m_accountancyDb;
        QSqlTableModel *m_model;   
        QHash<QString,QString> m_hashLoginId;     
};

#endif


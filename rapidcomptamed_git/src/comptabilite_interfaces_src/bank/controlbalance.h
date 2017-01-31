#ifndef CONTROLBALANCE_H
#define CONTROLBALANCE_H

#include "ui_balancedialog.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QDate>

class ControlBalance : public QDialog,Ui::BalanceDialog{
    Q_OBJECT;
    public:
        ControlBalance(QDate & dateBegin,
                       QDate & dateEnd,
                       QWidget * parent = 0,
                       double  lastBalance = 0.00,
                       double  balance = 0.00,
                       double  cashDeposits = 0.00,
                       double  checksDeposits = 0.00,
                       double  visaDeposits = 0.00,
                       double  othersDeposits = 0.00,
                       QString  idCompte = "");
        ~ControlBalance();
    private:
        enum paymentTypes{
            CASH = 0,
            CHECKS,
            VISA,
            VIR,
            IDHONO,
            paymentTypes_MaxParam
            };
        void analyse();
        QSqlDatabase m_db;
        QDate m_dateBegin;
        QDate m_dateEnd;
        double m_lastBalance;
        double m_balance;
        double m_cashDeposits;
        double m_checksDeposits;
        double m_visaDeposits;
        double m_othersDeposits;
        QString m_id_compte;
};

#endif


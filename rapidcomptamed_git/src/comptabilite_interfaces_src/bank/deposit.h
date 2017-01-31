#ifndef DEPOSIT_H
#define DEPOSIT_H
#include "ui_depositwidget.h"
#include <QWidget>
#include <QHash>
#include <QSqlDatabase>

class Deposits:public QWidget,public Ui::DepositWidget{
    Q_OBJECT
    enum inputsWidgets{
        CASH_EDIT = 0,
        CHECKS_EDIT,
        VISA_EDIT,
        OTHER_EDIT,
        inputsWidgets_MaxParam
        };
    enum comptesBancaires{
        IDCOMPTE = 0,
        IDUSER_CB,
        LIBELLE_CB,
        TITULAIRE_CB,
        NOM_BANQUE_CB,
        comptesBancaires_MaxParam
        };
    public:
        Deposits(QWidget * parent = 0);
        ~Deposits();
    private slots:
        void cancel();
        void recordInBank();
        void showBalanceDialog();
        void getBalance(const QString & bank);
        void setBalanceChanged();
        void setNewBalance(double balance);
        void cancel(int index);
        void remplirComptesBancaires(const QString&);
    private:
        QString idUsrChosen();
        QString idBankChosen();
        QDate lastValidatedDate();
        QHash  <int,QDoubleSpinBox*> m_hashEdits;
        QSqlDatabase m_db;
        QHash<QString,QString> m_hashUsers;//id user,login
        QHash<QString,QString> m_hashBanks;
        QString m_prefBankLibelle;
        QString m_prefLogin;
        QString m_prefUserLogin;
        int m_prefId;
        double m_lastBalance;
        double m_balance;
        double m_lastCashValue;
        double m_lastChecksValue;
        double m_lastVisaValue;
        double m_lastOtherValue;
};

#endif


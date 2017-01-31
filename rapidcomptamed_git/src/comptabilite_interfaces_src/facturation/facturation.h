#ifndef FACTURATION_H
#define FACTURATION_H

#include "ui_facturation.h"
#include "facturemodel.h"
#include "facturationIO.h"

#include <QWidget>
#include <QSqlTableModel>


class Facture : public QWidget, public Ui::FacturationWidget
{
    Q_OBJECT
    public:
        Facture (QWidget * parent = 0);
        ~Facture ();
    private slots:
        void addrow();
        void delRow();
        void newBill();
        //void setValueToValueDelegate(const QModelIndex & topLeft, const QModelIndex & bottomRight );
        void record();
        void deleteBill();
        void printBill();
        void setFactureFromId(const QString & factureid);
    private:
        QStringList setFacturesIdComboBox();
        void setTableView();
        void setdatedelegate();
        void setvaleurdelegate();
        void setmodepaymentdelegate();
        void setbuttondelegate();
        void setusersdelegate();
        void setstatutpayementdelegate();
        void setpatientdelegate();
        void setsitedelegate();
        void setpayeurdelegate();
        QString documentToPrint();
        QSqlDatabase m_db;
        QSqlTableModel *m_modelseances;
        QSqlTableModel *m_modelhonoraires;
        FactureModel *m_modelfacture;
        Ui::FacturationWidget *ui;
        FacturationIO * m_io;
        QString m_programName; // nom du programme appelant
        QString m_nomRecupere; // patient name
        QString m_prenomRecupere;//patient surname
        QString m_dateOfBirth;
        QString m_guidEntre;    // uid patient history file
        QString m_tarif;        // price
        QString m_codeTarif;    // price code
        QString m_user;         //user from calling program
        QString m_prefuserid;
};

#endif


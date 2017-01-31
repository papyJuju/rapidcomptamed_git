#ifndef FACTURATIONIO_H
#define FACTURATIONIO_H

#include "facturemodel.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStandardItemModel>

/*
<tab>1<tab>2<tab>...<br/>
<tab>1<tab>2<tab>...<br/>
*/

class FacturationIO:public QObject
{
    Q_OBJECT
    enum DatesEnum
    {
        ROW_FACTURE=0,
        DATE_OPERATION_INROW,
        DATE_PAIEMENT_INROW,
        DATE_VALEUR_INROW,
        DATE_VALIDATION_INROW,
        DatesEnumMaxPAram
        };
    public:
        FacturationIO(QObject *parent = 0);
        ~FacturationIO();
        double getValueFromCcamAct(const QString & act);
        bool record(FactureModel *model,const QString & idfacture, const QString & programmeName);
        bool deleteBill(const QString & factureid);
        QStringList listOfFacturesId();
        FactureModel * getModelFromFactureId(const QString & factureid);
        //QVariant insertNewUser(int rowinbill,int rowfacturation, const QString & iduser);
    private:
        int getrowoffacturation(QSqlTableModel & modelfacturation,const QString & factureid);
        QString getFactureOfId(const QString & factureid);
        QString getStringToRecordFromFactureModel(FactureModel *model);
        bool recordValidatesActsInHonoraires(FactureModel *model,const QString programmeName);
        bool insertIntoHonoraires(QString  idusr, QString  idsite, QString  idpayeur, QString  nomprenom,
                                   QString  guid,  QString  praticien,  QString  date,  QString  act,
                                  QString payment , QString value);
        QSqlDatabase m_db;
        QString m_rowseparator;
        QString m_fieldseparator;
        QHash<int,QString> m_hashtypepayment;
};  

#endif


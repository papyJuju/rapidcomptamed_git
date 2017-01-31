#include "ccambasethread.h"
#include "parseconstants.h"

#include <common/constants.h>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>

#include <QDebug>

using namespace Common;
using namespace ParseConstants;

FillCcamBase::FillCcamBase(QObject * parent)
{
    Q_UNUSED(parent);
    m_model = new QStandardItemModel;
}

FillCcamBase::~FillCcamBase(){}

void FillCcamBase::run()
{
    m_mutex.lock();
    fillCcamDatabase();
    m_mutex.unlock();
}

void FillCcamBase::fillCcamDatabase()
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlTableModel model(this,db);
    model.setTable("ccam");    
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.select();
    int rows = model.rowCount();
    for (int row = 0; row < m_model->rowCount(); ++row) 
    {
         if (!model.insertRows(rows+row,1))
         {
               qWarning() << __FILE__ << QString::number(__LINE__) << "ccam insertRows error :" << model.lastError().text() ;
             }
         //qDebug() << __FILE__ << QString::number(__LINE__) << " model row =" << QString::number(model.rowCount()) ;
         int lastId = lastCCAMid().toInt(); 
         QString codeString;
         for (int col = 0; col < itemsCCAMbaseMaxParam; ++col)
         {
             QVariant data = QVariant();
             switch(col){
                 case IDCCAM :
                     data = lastId+1;
                     //qDebug() << __FILE__ << QString::number(__LINE__) << " id data =" << data.toString() ;
                     break;
                 case CODE_CCAM :
                     data = m_model->data(m_model->index(row,MODEL_CODE),Qt::DisplayRole);
                     codeString = data.toString();
                     break;
                 case ABSTRACT :
                     data = m_model->data(m_model->index(row,MODEL_TEXT),Qt::DisplayRole);
                     break;
                 case TYPE :
                     data = QVariant("ccam");
                     break;
                 case AMOUNT :
                     data = m_model->data(m_model->index(row,MODEL_PRICE),Qt::DisplayRole);
                     if (data == "Non pris en charge")
                     {
                           data = QVariant(0.00);
                         }
                     data = QVariant(data.toString().replace(",","."));
                     //qDebug() << __FILE__ << QString::number(__LINE__) << " data price =" <<  data.toString();
                     break;
                 case REIMBURSMENT_CCAM :
                     data = QVariant("70");
                     break;
                 case DATE :
                     data = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
                     break;  
                 case OTHERS :
                     data = m_model->data(m_model->index(row,MODEL_BLOB),Qt::DisplayRole);
                     break;                                                                                  
                 default :
                     break;    
                 }
             if (!model.setData(model.index(rows+row,col),data,Qt::EditRole))
             {
                   qWarning() << __FILE__ << QString::number(__LINE__) << "ccam model error :" << model.lastError().text() ;
                 } 
                 if (!model.submitAll())
                 {
                       qWarning() << __FILE__ << QString::number(__LINE__) << "ccam sub model submit error" ;
                 }
             }
            if (!model.submitAll())
            {
                qWarning() << __FILE__ << QString::number(__LINE__) << "ccam model submit error" ;
                }
            QString rowString = QString::number(row)+":"+codeString;                 
            recupDataForParseBySignal(rowString);
         }
}

QVariant FillCcamBase::lastCCAMid()
{
    int lastId = 0;
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlQuery q(db);
    QString req = QString("SELECT %1 FROM %2").arg("id_ccam","ccam");
    if (!q.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "ccam id search = " << q.lastError().text() ;
        }
    QStringList list;
    while (q.next())
    {
        int number = q.value(0).toInt();
        if (number > lastId)
        {
              lastId = number;
            }
        }
    //qDebug() << __FILE__ << QString::number(__LINE__) << " list last =" << QString::number(lastId);
    return QVariant(lastId);
        
}

void FillCcamBase::getStandardItemModel(QStandardItemModel * model)
{
    m_model = model;
}

void FillCcamBase::recupDataForParseBySignal(const QString& info)
{
    emit outThread(info);
}


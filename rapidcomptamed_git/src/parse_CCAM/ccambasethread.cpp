#include "ccambasethread.h"
#include "parseconstants.h"

#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QDate>

#include <QDebug>

using namespace Common;
using namespace Constants;
using namespace ParseConstants;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

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
    /*    enum ModelItems
    {
        MODEL_CODE = 0,
        MODEL_TEXT,
        MODEL_PRICE,
        MODEL_BLOB,
        ModelItems_MaxParam 
        };*/
    if (!copyCcamBase())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to save ccam" ;
        }
    else
    {
        if (!truncateBase())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "unable to truncate ccam" ;
            }
        }
    for (int row = 0; row < m_model->rowCount(); ++row) 
    {
        QString idccam;
        QString code = m_model->data(m_model->index(row,MODEL_CODE),Qt::DisplayRole).toString();
        QString newvalue = m_model->data(m_model->index(row,MODEL_PRICE),Qt::DisplayRole).toString();
        QSqlQuery qyCode(db);
        QString codeReq = QString("select %1 from %2 where %3 = '%4'").arg("id_ccam","ccam","code",code);
        if (!qyCode.exec(codeReq))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << qyCode.lastError().text() ;
            }
        while (qyCode.next())
        {
            idccam = qyCode.value(0).toString();
            }
        if (idccam.isEmpty())
        {
              if (!insertNewCode(code,row))
              {
                    qWarning() << __FILE__ << QString::number(__LINE__) << "unable to insert code = " << code;
                  }
            }
        else
        {
            updateCode(idccam,newvalue,row);
            }
        QString rowString = QString::number(row)+":"+code;                 
        recupDataForParseBySignal(rowString);
        }//for
}

bool FillCcamBase::insertNewCode(QString code,int row)
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QStringList listofheaders;
    listofheaders << "code" << "abstract" << "type" << "amount" << "reimbursment" << "date" << "others";
    QString ccamheaders = listofheaders.join(",");
    QString values;
    QStringList valueslist;
    /*Full texts 	id_ccam 	code 	abstract 	type 	amount 	reimbursment 	date 	others*/
    for (int col = CODE_CCAM; col < itemsCCAMbaseMaxParam ; ++col)
    {
          QString value;
          switch(col){
              case CODE_CCAM :
                  value = m_model->data(m_model->index(row,MODEL_CODE),Qt::DisplayRole).toString();
                  value = "'"+value+"'";
                  break;
              case ABSTRACT :
                  value = m_model->data(m_model->index(row,MODEL_TEXT),Qt::DisplayRole).toString();
                  value = manageSqlDefects(value);
                  value = "\""+value+"\"";
                  break;
              case TYPE :
                  value = "'ccam'";
                  break;
              case AMOUNT :
                   value = m_model->data(m_model->index(row,MODEL_PRICE),Qt::DisplayRole).toString();
                   if (value.contains("Non pris en charge"))
                   {
                       value = "0";
                       }
                   if (value.isEmpty())
                   {
                       value = "0";
                       }
                   if (value.contains(","))
                   {
                       value.replace(",",".");
                       }
                   value = "'"+value+"'";
                   break;
              case REIMBURSMENT_CCAM:
                   value = "70";
                   value = "'"+value+"'";
                   break;
              case DATE:
                   value = QDate::currentDate().toString("yyyy-MM-dd");
                   value = "'"+value+"'";
                   break;
              case OTHERS:
                   value = m_model->data(m_model->index(row,MODEL_BLOB),Qt::DisplayRole).toString();
                   value = manageSqlDefects(value);
                   value = "\""+value+"\"";
                   break;
              default :
                  break;
              }
          valueslist << value;
        }
    values = valueslist.join(",");
    QSqlQuery qy(db);
    QString newCodeReq = QString("insert into %1 (%2) values(%3)").arg("ccam",ccamheaders,values);
    if (!qy.exec(newCodeReq))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
          qDebug() << __FILE__ << QString::number(__LINE__) << qy.lastQuery() ;
          return false;
        }
    return true;
}

bool FillCcamBase::updateCode(QString idccam,QString newvalue,int row)
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlQuery qy(db);
    QString updateReq = QString("update %1 set %2 = '%3' where %4 = '%5'").arg("ccam","amount",newvalue,"id_ccam",idccam);
    if (!qy.exec(updateReq))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
          return false;
        }
    return true;
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

bool FillCcamBase::copyCcamBase()
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlDatabase driverbase = QSqlDatabase::database(Constants::DB_FIRST_CONNECTION);
    if (!driverbase.isOpen())
    {
        driverbase = QSqlDatabase :: addDatabase(Constants::MYSQL,Constants::DB_FIRST_CONNECTION);
        driverbase.setHostName(settings()->value(S_COMPTA_DB_HOST).toString());
        driverbase.setDatabaseName("mysql");
        driverbase.setUserName(settings()->value(S_COMPTA_DB_LOGIN).toString());
        driverbase.setPassword(settings()->value(S_COMPTA_DB_PASSWORD).toString());
        if (!driverbase.open())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open mysql" ;
            }
        }
    QString createSauveCcamBases = QString("create database if not exists %1").arg("sauveccam");
    QSqlQuery qyCreateSauveCcam(driverbase);
    if (!qyCreateSauveCcam.exec(createSauveCcamBases))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qyCreateSauveCcam.lastError().text() ;
          return false;
        }
    QSqlDatabase sauveccambase = QSqlDatabase::addDatabase(Constants::MYSQL,"sauveccam");
    sauveccambase = QSqlDatabase::addDatabase(Constants::MYSQL,"sauveccam");
    sauveccambase.setHostName(settings()->value(S_COMPTA_DB_HOST).toString());
    sauveccambase.setDatabaseName("sauveccam");
    sauveccambase.setUserName(settings()->value(S_COMPTA_DB_LOGIN).toString());
    sauveccambase.setPassword(settings()->value(S_COMPTA_DB_PASSWORD).toString());
    if (!sauveccambase.open())
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open sauveccambase" ;
        }
    QString newccam = QString("%1%2").arg("ccam",QDateTime::currentDateTime().toString("dd-MM-yyyy-hh-mm"));
    QString reqCreateNewCcam = "CREATE TABLE `sauveccam`.`"+newccam+"` ("
                               "`id_ccam` int( 20 ) unsigned NOT NULL AUTO_INCREMENT ,"
                               "`code` varchar( 50 ) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL ,"
                               "`abstract` varchar( 200 ) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL ,"
                               "`type` varchar( 50 ) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL ,"
                               "`amount` double NOT NULL ,"
                               "`reimbursment` double DEFAULT NULL ,"
                               "`date` date NOT NULL ,"
                               "`others` blob,"
                               "PRIMARY KEY ( `id_ccam` ));";
                               //) ENGINE = InnoDB DEFAULT CHARSET = latin1;";
    QSqlQuery qyCreateNewCcam(sauveccambase);
    if (!qyCreateNewCcam.exec(reqCreateNewCcam))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qyCreateNewCcam.lastError().text() ;
          return false;
        }
    //SET SQL_MODE = 'NO_AUTO_VALUE_ON_ZERO';
    QString reqCopyCcam = QString("INSERT INTO `sauveccam`.`%1` SELECT * FROM `%2`.`ccam` ;").arg(newccam,DB_ACCOUNTANCY);
    QSqlQuery qyCopy(driverbase);
    if (!qyCopy.exec(reqCopyCcam))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qyCopy.lastError().text() ;
          return false;
        }
    return true;
}

bool FillCcamBase::truncateBase()
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlQuery qyTruncate(db);
    QString reqTruncate = QString("truncate table ccam");
    if (!qyTruncate.exec(reqTruncate))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qyTruncate.lastError().text() ;
          return false;
        }
    return true;
}

QString FillCcamBase::manageSqlDefects(const QString & value)
{
    QString text = value;
    text.replace("'","\'");
    text.replace("et/ou","et ou");
    text.replace(",","");
    return text;
}

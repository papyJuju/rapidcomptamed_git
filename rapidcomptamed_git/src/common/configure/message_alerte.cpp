#include "message_alerte.h"
#include <common/constants.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QDate>

using namespace MessageSpace;
using namespace Common;
using namespace Constants;

MessageAlerte::MessageAlerte(QObject *parent)
{
    Q_UNUSED(parent);
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
}

MessageAlerte::~MessageAlerte(){}

QString MessageAlerte::messagesAvailables()
{
    enum {ALERTE=0,DATE};
    QString alerte = "alertes";
    QString date = "date";
    QString currentdate = QDate::currentDate().toString("yyyy-MM-dd");
    int yes = 0;
    QString req = QString("select %1,%2 from %3 where %2 <= '%4'")
    .arg(alerte,date,TABLE_ALERTES,currentdate);
    req+= QString(" and %1 = '%2'").arg("valid",yes);
    QStringList list;
    QSqlQuery qy(m_db);
    if (!qy.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        QString alertefound = qy.value(ALERTE).toString();
        QString datealerte = qy.value(DATE).toString();
        QString result = alertefound+":"+datealerte;
        list << result;
        }
    return list.join("\n");
}

QString MessageAlerte::messagesAvailablesAndNotRead()
{
    enum {ALERTE=0,DATE};
    QString alerte = "alertes";
    QString date = "date";
    QString currentdate = QDate::currentDate().toString("yyyy-MM-dd");
    QString isread = "isread";
    QString valid = "valid";
    QString yes = "0";
    QString no = "1";
    QString req = QString("select %1,%2 from %3 where %2 <= '%4'")
    .arg(alerte,date,TABLE_ALERTES,currentdate);
    req+= QString(" and %1 = '%2'").arg(valid,yes);
    req+= QString(" and %1 = '%2'").arg(isread,no);
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__)<< req;
    QStringList list;
    QSqlQuery qy(m_db);
    if (!qy.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        QString alertefound = qy.value(ALERTE).toString();
        QString datealerte = qy.value(DATE).toString();
        QString result = alertefound+":"+datealerte;
        list << result;
        }
    return list.join("\n");
}

bool MessageAlerte::messageAreAvailable()
{
    bool b = true;
    QString result = messagesAvailables();
    if (!result.isEmpty())
    {
        showMessage();
        }
    else
    {
        b = false;
        }
    return b;
}

bool MessageAlerte::messageAreAvailableAndNotRead()//for recettes
{
    bool b = true;
    QString result = messagesAvailablesAndNotRead();
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << result;
    if (!result.isEmpty())
    {
        int mess = showMessageNotRead();
        if (mess==QMessageBox::Ok)
        {
            if (!modifyAlertesToRead())
            {
                QMessageBox::warning(0,tr("Attention"),tr("Impossible de noter les messages comme lus."),QMessageBox::Ok);
                }
            }
        }
    else
    {
        b = false;
        }
    return b;
}

int MessageAlerte::showMessage()
{
    QByteArray messagearray = messagesAvailables().toUtf8();
    int ret = QMessageBox::information
                     (0,
                     tr("Attention"),
                     tr(messagearray.data()),
                     QMessageBox::Ok | QMessageBox::Cancel,
                     QMessageBox::Ok);
    return ret;
}

int MessageAlerte::showMessageNotRead()
{
    QByteArray messagearray = messagesAvailablesAndNotRead().toUtf8();
    int ret = QMessageBox::information
                     (0,
                     tr("Attention"),
                     tr(messagearray.data()),
                     QMessageBox::Ok | QMessageBox::Cancel,
                     QMessageBox::Ok);
    return ret;
}

bool MessageAlerte::modifyAlertesToRead()
{
    bool b = true;
    enum {ALERTE=0,DATE,ID_ALERTES};
    QString alerte = "alertes";
    QString date = "date";
    QString id = "id_alertes";
    QString isread = "isread";
    QString valid = "valid";
    QString currentdate = QDate::currentDate().toString("yyyy-MM-dd");
    QString yes = "0";
    QString no = "1";
    QString req = QString("select %1,%2,%3 from %4 where %2 <= '%5'")
    .arg(alerte,date,id,TABLE_ALERTES,currentdate);
    req+= QString(" and %1 = '%2'").arg(valid,yes);
    req+= QString(" and %1 = '%2'").arg(isread,no);
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << req;
    QStringList listofid;
    QSqlQuery qy(m_db);
    if (!qy.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        QString idvalue = qy.value(ID_ALERTES).toString();
        qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << idvalue;
        listofid << idvalue;
        }
    foreach(QString idread,listofid)
    {
        QSqlQuery query(m_db);
        QString reqid = QString("update %1 set %3='%4' where %5='%6'").arg(alerte,isread,"0",id,idread);
        qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << reqid;
        if (!query.exec(reqid))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
            b=false;            
            }
        }
     return b;
}






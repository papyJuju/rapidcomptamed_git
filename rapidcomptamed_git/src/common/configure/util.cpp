#include "util.h"
#include <common/constants.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QDebug>

using namespace Common;
using namespace Constants;
Security * Security::m_singleton = 0;

Security * Security::instance(QObject * parent)
{
    if(!m_singleton)
        m_singleton = new Security(parent);
    return m_singleton;
}

Security::Security(QObject * parent){}

Security::~Security(){}

bool Security::isUserLocked(QString user)
{
    bool success = false;
    QString serrure;
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlQuery q(db);
    QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                 .arg("serrure","utilisateurs","login",user);
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    while (q.next())
    {
    	serrure = q.value(0).toString();
        }
    if (serrure.contains("close"))
    {
    	  success = true;
        }    
    return success;
}



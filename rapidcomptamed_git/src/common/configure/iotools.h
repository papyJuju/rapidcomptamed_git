#ifndef IOTOOLS_H
#define IOTOOLS_H

#include <QSqlDatabase>
#include <QMap>
#include <QObject>


class Iotools:public QObject
{
    Q_OBJECT
    public:
         Iotools(QObject * parent = 0);
         ~Iotools ();
         double getValueFromAct(const QString & act);
         QString getIdFromUserLogin(const QString login);
         QString getIdFromSiteLogin(const QString login);
         QString getIdFromPayeurLogin(const QString login);
         QString getGuidFromName(const QString totalname,const QString programName);
         QString newGuidOfNameIndex();
         QMap<QString,QString> getSitesFromDatabase();
         QMap<QString,QString> getPayeursFromDatabase();
         bool isReturnedTrue();
    private:
        QString getbasefromact(const QString & act);
        QSqlDatabase m_db;
        bool m_iotoolsreturn;
        void configureConnexionMedintux();
};  

#endif


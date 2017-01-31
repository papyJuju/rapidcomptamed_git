#ifndef DEPENSETOOLS_H
#define DEPENSETOOLS_H

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QSqlTableModel>

class Depensetools : public QObject
{
    Q_OBJECT
    public:
        Depensetools (QObject * parent = 0);
        ~Depensetools ();
        QMap<int,QString> getLibelleFromDatabase();
    private:
        QSqlDatabase m_db;
        QSqlTableModel * m_modelmouvdispo;

};

#endif


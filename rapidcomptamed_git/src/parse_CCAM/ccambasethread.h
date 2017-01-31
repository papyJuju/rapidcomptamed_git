#ifndef CCAMBASETHREAD_H
#define CCAMBASETHREAD_H

#include <QThread>
#include <QMutex>
#include <QSqlDatabase>
#include <QStandardItemModel>

class FillCcamBase : public QThread
{
    Q_OBJECT
    public :
        FillCcamBase(QObject * parent = 0);
        ~FillCcamBase();
        QMutex m_mutex;
        void getStandardItemModel(QStandardItemModel *model);
    private :
        void run();
        void fillCcamDatabase();
        QVariant lastCCAMid();
        void recupDataForParseBySignal(const QString& info);
        bool insertNewCode(QString code,int row);
        bool updateCode(QString idccam,QString newvalue,int row);
        bool copyCcamBase();
        bool truncateBase();
        QString manageSqlDefects(const QString & value);
        QStandardItemModel * m_model;
    signals:
        void outThread(const QString & info);
        
};

#endif


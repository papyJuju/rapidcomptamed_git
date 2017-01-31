#ifndef RAPIDMODEL_H
#define RAPIDMODEL_H

#include <QObject>

#include <QAbstractTableModel>
#include <QSqlError>
#include <QVector>
#include <QStringList>
#include <QSqlDatabase>


namespace RapidModelNamespace
{

class RapidModel:public QAbstractTableModel
{
    Q_OBJECT
    public:
        RapidModel (QObject * parent = 0);
        ~RapidModel ();
        int rowCount(const QModelIndex &parent ) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
        bool insertRows( int position, int count, const QModelIndex & parent = QModelIndex() );
        bool removeRows(int position, int count, const QModelIndex & parent = QModelIndex());
        bool submit();
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        bool setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
        QSqlError lastError();
        Qt::ItemFlags flags(const QModelIndex &index) const;
        void setRapidModelReady();
        void setRapidModelNotReady();
        bool clear();
    protected slots :
        void replaceValueInDatabase(const QModelIndex&,const QModelIndex&);
    private:        
        bool removeDatabaseRows(const QStringList & idlist);
        QVector<QList<QVariant> > * m_listsOfValuesbyRows;
        QStringList m_headersRows;
        QStringList m_headersColumns;
        QSqlDatabase m_db;
        bool m_ready;
};//class RapidModel

};//namespace RapidModelNamespace

#endif



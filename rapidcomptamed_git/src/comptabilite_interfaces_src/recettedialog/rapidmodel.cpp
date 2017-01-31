#include "rapidmodel.h"
#include <common/constants.h>

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>


using namespace RapidModelNamespace;
using namespace Common;
using namespace Constants;

RapidModel::RapidModel(QObject * parent)
{
    m_ready = false;
    m_listsOfValuesbyRows = new QVector<QList<QVariant> >;
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    connect(this,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(replaceValueInDatabase(const QModelIndex&,const QModelIndex&)));
    
}

RapidModel::~RapidModel()
{
    m_ready = false;
}

int RapidModel::rowCount(const QModelIndex &parent ) const 
{
        Q_UNUSED(parent);
        return m_listsOfValuesbyRows->size();
}

int RapidModel::columnCount(const QModelIndex &parent) const 
{
        Q_UNUSED(parent);
        return int(RapidModelEnum_MaxParam);
}

QModelIndex RapidModel::index ( int row, int column, const QModelIndex & parent) const
{
        return QAbstractTableModel::index(row,column,parent);
}

bool RapidModel::insertRows( int position, int count, const QModelIndex & parent)
{
        beginInsertRows(parent, position, position+count-1);
        for (int row=0; row < count; row++) {

            QList<QVariant> list;
            for (int j = 0; j < RapidModelEnum_MaxParam; ++j)
            {
                list << QVariant(0);
            }
            m_listsOfValuesbyRows -> append(list);
        }
        m_headersRows.append(QString::number(++position));
        endInsertRows();
        return true;

}

bool RapidModel::removeRows(int position, int count, const QModelIndex & parent)
{
        Q_UNUSED(parent);
        QStringList idlist; 
        for (int row=0; row < count; row++) {
            idlist << m_listsOfValuesbyRows->at(position+row)[RM_HONO].toString();
            qDebug() << __FILE__ << QString::number(__LINE__) << " idlist =" <<  idlist[row];
            } 
        beginRemoveRows(parent, position, position+count-1);
        //int rows = m_listsOfValuesbyRows->size();      
        m_listsOfValuesbyRows->remove(position,count);
        endRemoveRows();
        if (m_ready)
        {
              if (!removeDatabaseRows(idlist))
              {
                  return false;
                  }   
            }
             
        return true;
}

bool RapidModel::submit()
{
        return QAbstractTableModel::submit();
}


QVariant RapidModel::data(const QModelIndex &index, int role) const
{
        QVariant data;
        if (!index.isValid()) {
                qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
            return QVariant();
        }


        if (role==Qt::EditRole || role==Qt::DisplayRole) {
            int row = index.row();
            const QList<QVariant> & valuesListByRow = m_listsOfValuesbyRows->at(row);
            data = valuesListByRow[index.column()];
        }
        return data;
}

bool RapidModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
        bool ret = true;
        if (!index.isValid()) {
                qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
            return false;
        }

        if (role==Qt::EditRole) {
            QList<QVariant> list;
            int row = index.row();
            list = m_listsOfValuesbyRows->at(row);
            QString idhono = list[RM_HONO].toString();
            switch(index.column()){
            case RM_HONO :
                list.replace(RM_HONO,value);
                break;
            case RM_PATIENT :
                list.replace(RM_PATIENT,value);
                break;
            case RM_GUID :
                list.replace(RM_GUID,value);
                break;
            case RM_PRATICIEN :
                list.replace(RM_PRATICIEN,value);
                break;
            case RM_DATE :
                list.replace(RM_DATE,value);
                break;
            case RM_ACTE :
                list.replace(RM_ACTE,value);
                break;
            case RM_REMARQUE :
                list.replace(RM_REMARQUE,value);
                break;
            case RM_ESP :
                list.replace(RM_ESP,value);
                break;
            case RM_CHQ :
                list.replace(RM_CHQ,value);
                break;
            case RM_CB :
                list.replace(RM_CB,value);
                break;
            case RM_VIR :
                list.replace(RM_VIR,value);

                break;
            case RM_DAF :
                list.replace(RM_DAF,value);
                break;
            case RM_DU :
                list.replace(RM_DU,value);
                break;
            case RM_TRACABILITE :
                list.replace(RM_TRACABILITE,value);
                break;
            case RM_DIVERS :
                list.replace(RM_DIVERS,value);
                break;
            default :
                break;
            }
            m_listsOfValuesbyRows->replace(row,list);
            Q_EMIT dataChanged(index, index);
            ret = true;
        }
        return ret;
}

QVariant RapidModel::headerData(int section, Qt::Orientation orientation, int role) const
{
        if (role==Qt::DisplayRole) {
            if (orientation==Qt::Horizontal) {
                switch (section) {
            case RM_HONO : return m_headersColumns.value(RM_HONO);                
                break;
            case RM_PATIENT : return m_headersColumns.value(RM_PATIENT);                
                break;
            case RM_GUID :return m_headersColumns.value(RM_GUID);  
                break;
            case RM_PRATICIEN :return m_headersColumns.value(RM_PRATICIEN);  
                break;
            case RM_DATE :return m_headersColumns.value(RM_DATE);  
                break;
            case RM_ACTE :return m_headersColumns.value(RM_ACTE);  
                break;
            case RM_REMARQUE :return m_headersColumns.value(RM_REMARQUE);  
                break;
            case RM_ESP :return m_headersColumns.value(RM_ESP);  
                break;
            case RM_CHQ :return m_headersColumns.value(RM_CHQ);  
                break;
            case RM_CB :return m_headersColumns.value(RM_CB);  
                break;
            case RM_VIR :return m_headersColumns.value(RM_VIR);  
                break;
            case RM_DAF :return m_headersColumns.value(RM_DAF);  
                break;
            case RM_DU :return m_headersColumns.value(RM_DU);  
                break;
            case RM_TRACABILITE :return m_headersColumns.value(RM_TRACABILITE); 
                break; 
            case RM_DIVERS :return m_headersColumns.value(RM_DIVERS);  
                break;
            default:
                break;
                }
            }
            else if (orientation==Qt::Vertical) {
                return QVariant(m_headersRows[section]);
            }
        }
        return QVariant();
}

bool RapidModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
        if (role == Qt::EditRole||role == Qt::DisplayRole) {
            if (orientation == Qt::Vertical) {
                m_headersRows.insert(section,value.toString());
            }
            else if (orientation == Qt::Horizontal){
                m_headersColumns.insert(section,value.toString());
            }
        }

        else {
            return false;
        }

        Q_EMIT QAbstractTableModel::headerDataChanged(orientation, section, section) ;
        return true;
}

QSqlError RapidModel::lastError()
{
        return lastError();
}

Qt::ItemFlags RapidModel::flags(const QModelIndex &index) const
{
        if (      index.column()==RM_PATIENT
               //|| index.column()==RM_GUID
               || index.column()==RM_PRATICIEN
               || index.column()==RM_DATE
               || index.column()==RM_ACTE
               || index.column()==RM_REMARQUE
               || index.column()==RM_ESP
               || index.column()==RM_CHQ
               || index.column()==RM_CB
               || index.column()==RM_VIR
               || index.column()==RM_DAF
               || index.column()==RM_DU
               || index.column()==RM_TRACABILITE
               || index.column()==RM_DIVERS ) {
            return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
        } else {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
}

void RapidModel::replaceValueInDatabase(const QModelIndex & topleftindex,const QModelIndex & bottomrightindex)
{
   if (m_ready)
   {
        int row = topleftindex.row();
        int column = topleftindex.column();
        QString idhono = data(index(row,RM_HONO),Qt::DisplayRole).toString();
        QString value = data(index(row,column),Qt::DisplayRole).toString();
        if (value.contains("'"))
        {
              value.replace("'","\'");
            }
        QSqlQuery qy(m_db);
        QString req = QString("update %1 set %2 = '%3' where %4 = '%5'").arg("honoraires",modifRecettesColumnsHash().value(column),value,"id_hono",idhono);
        qDebug() << __FILE__ << QString::number(__LINE__) << " req =" << req ;
        if (!qy.exec(req))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() 
            << req;                             
            }
       }

}

void RapidModel::setRapidModelReady()
{
    m_ready = true;
}

void RapidModel::setRapidModelNotReady()
{
    m_ready = false;
}

bool RapidModel::clear()
{
    setRapidModelNotReady();
    int rows = rowCount(QModelIndex());
    m_headersRows.clear();
    return removeRows(0,rows,QModelIndex());
}

bool RapidModel::removeDatabaseRows(const QStringList & idlist)
{
    QSqlQuery qy(m_db);
    foreach(QString id,idlist){
        QString req = QString("delete from %1 where %2 = '%3'").arg("honoraires","id_hono",id);
        if (!qy.exec(req))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
            return false;          
            }
        }
    return true;
}

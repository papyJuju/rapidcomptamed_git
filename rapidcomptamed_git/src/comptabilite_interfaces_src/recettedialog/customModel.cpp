#include "customModel.h"
#include <common/tablesdiagrams.h>

using namespace TablesDiagramsHonoraires;


customModel::customModel(QObject * object, QSqlDatabase & db){
    m_model = new QSqlTableModel(object,db);
    m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
}

customModel::~customModel(){
    getDoublesRows.clear();
}

void customModel::setTable(const QString & tableName){
   m_model->setTable(tableName);
   m_tableName = tableName;
}

QString customModel::tableName() const
{
    QString tableName = m_tableName;
    return m_model->tableName() ;
}

QString customModel::filter() const
{
    return m_model->filter();
}

bool customModel::select()
{
    return m_model->select();
}

void customModel::setFilter(const QString &filter)
{
    m_model->setFilter(filter);
}

QSqlRecord customModel::record(int row) const{
    return m_model->record(row);
}

Qt::ItemFlags customModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


int customModel::rowCount(const QModelIndex &parent) const
{
    return m_model->rowCount(parent);
}

int customModel::columnCount(const QModelIndex &parent) const
{
    return m_model->columnCount(parent);
}


QVariant customModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int row = index.row();
    int column = index.column();
    QVariant data = m_model->data(index, role);
    QString guid = m_model->data(m_model->index(row,TablesDiagramsHonoraires::GUID),Qt::DisplayRole).toString();
    
    switch(role){
        case Qt::DisplayRole :
            return data;
            break;
        case Qt::BackgroundRole :
            if(getDoublesRows.contains(guid)){
                QColor color("blue");
                color.setAlpha(025);
                return color;
                }
            else if (getDoublesNames.contains(guid))
            {
            	  if (column == PATIENT)
            	  {
            	      QColor colorRed("red");
            	      colorRed.setAlpha(025);
            	      return colorRed;
            	      }
            	  else
            	  {
            	  	return QColor("white");
            	      }
                }
            else{
                return QColor("white");
                }
            break;
        default :
            return QVariant();
            break;
        }
}

bool customModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = m_model->setData(index, value, role);
    return ret;
}


QVariant customModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model->headerData(section,orientation,role);
}

bool customModel::insertRows(int row, int count, const QModelIndex &parent)
{
    //isDirty() = true;
   //m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    bool ret = m_model->insertRows(row, count, parent);
//    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    return ret;
}

bool customModel::removeRows(int row, int count, const QModelIndex &parent)
{
    //isDirty() = true;
    return m_model->removeRows(row, count, parent);
}

bool customModel::submit()
{
    if (m_model->submitAll()) {
        //isDirty() = false;
        return true;
    }
    return false;
}

void customModel::revert()
{
    //isDirty() = false;
    m_model->revert();
}

bool customModel::isDirty() const
{
    return m_isDirty;
}

void customModel::sort(int column, Qt::SortOrder order)

{ 
    m_model->setSort(column, order);
    m_model->select();
    emit layoutChanged();
}

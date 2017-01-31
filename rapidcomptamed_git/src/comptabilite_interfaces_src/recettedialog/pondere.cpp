#include "pondere.h"
#include <QDebug>
#include <QComboBox>

VariantModel::VariantModel(QObject * parent)
{

}

VariantModel::~VariantModel(){}

QModelIndex VariantModel::index ( int row, int column, const QModelIndex & parent  ) const
{
     if (!parent.child(row,column).isValid())
     {
     	  return createIndex(row, column);
         }
     else
     {
     	return parent.child(row,column);
         }
}

QModelIndex VariantModel::parent ( const QModelIndex & index  ) const
{
     return QModelIndex();
}

int VariantModel::rowCount(const QModelIndex &parent ) const {
     Q_UNUSED(parent);
     return m_itemsVector.size();
}

int VariantModel::columnCount(const QModelIndex &parent ) const {
     Q_UNUSED(parent);
     return int(COL_COUNT);
}

bool VariantModel::setData ( const QModelIndex & index, const QVariant & value, int role)
{
    int row = index.row();
    int column = index.column();
    QVector<QVariant> list;
    list = m_itemsVector.at(row);
    switch(column){
        case ACT :
            list.replace(ACT,value);
            break;
        case VALUE:
            list.replace(VALUE,value);
            break;
        case MODIFIER :
            list.replace(MODIFIER,value);
            break;
        default :
            break;    
        }
    m_itemsVector.replace(row,list);
    Q_EMIT dataChanged(index, index);
    return true;       
}

QVariant VariantModel::data(const QModelIndex &index, int role ) const
{
            QVariant data;
            if (!index.isValid()) {
                qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
                return QVariant();
                }
                
            
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
               int row = index.row();
               const QVector<QVariant> & valuesListByRow = m_itemsVector.at(row);
               data = valuesListByRow[index.column()];
            }
            return data;
 }

 bool VariantModel::insertRows( int position, int count, const QModelIndex & parent  )
 {
            beginInsertRows(parent, position, position+count-1);
            for (int row=0; row < count; row++) {
                
                QVector<QVariant> list;
                for (int j = 0; j < COL_COUNT; ++j)
                {
            	    list << QVariant(0);
            	    }
                m_itemsVector . append(list);
                }
            endInsertRows();
            return true;
}

QVariant VariantModel::headerData(int section, Qt::Orientation orientation, int role) const
{
        if (role==Qt::DisplayRole) {
            if (orientation==Qt::Horizontal) {
               return QVariant(m_headersColumns[section]);
                }
            else if (orientation==Qt::Vertical) {
                return QVariant(m_headersRows[section]);
            }
            else
            {
                qWarning() << __FILE__ << QString::number(__LINE__) << "there is a problem" ;
                }
        }
        return QVariant();
}

bool VariantModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role)
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
        Q_EMIT QAbstractItemModel::headerDataChanged(orientation, section, section) ;
        return true;
}


Qt::ItemFlags VariantModel::flags(const QModelIndex &index) const
{
   return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}


SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
     : QItemDelegate(parent)
 {
 }

 QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
 {
     QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
     editor->setMinimum(0.0);
     editor->setMaximum(1.0);
     editor->setSingleStep(0.1);

     return editor;
 }

 void SpinBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
 {
     double value = index.model()->data(index, Qt::DisplayRole).toDouble();

     QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
     spinBox->setValue(value);
 }

 void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
 {
     QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
     spinBox->interpretText();
     double value = spinBox->value();

     model->setData(index, value, Qt::EditRole);
 }

 void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }

PondereActes::PondereActes(QWidget * parent,QHash<QString,QString> hashOfActs)
{
    setupUi(this);
    QStringList listOfDatas;
    listOfDatas = hashOfActs.keys();
    m_model = new VariantModel(this);
    m_model->setHeaderData(ACT,Qt::Horizontal,"Acte",Qt::EditRole);
    m_model->setHeaderData(VALUE,Qt::Horizontal,"Valeur",Qt::EditRole);
    m_model->setHeaderData(MODIFIER,Qt::Horizontal,"Facteur",Qt::EditRole);    
    for (int row = 0; row < listOfDatas.size(); ++row)
    {
    	QString act = listOfDatas[row];
    	if (!act.isEmpty())
    	{
    	    QVariant half = QVariant(1.0);
    	    QString value = hashOfActs.value(act);
    	    m_model->insertRows(row,1,QModelIndex());
    	    m_model->setHeaderData(row,Qt::Vertical,QString::number(row),Qt::EditRole);
    	    m_model->setData(m_model->index(row,ACT),act,Qt::EditRole);
    	    m_model->setData(m_model->index(row,VALUE),value,Qt::EditRole); 
    	    m_model->setData(m_model->index(row,MODIFIER),half,Qt::EditRole);
    	    m_model->submit();
    	    }
    	}
    m_model->submit();
    tableView->setModel(m_model);
    tableView->setItemDelegateForColumn(MODIFIER,new SpinBoxDelegate);    
    connect(applyButton,SIGNAL(pressed()),this,SLOT(apply()));    
}

PondereActes::~PondereActes(){}

void PondereActes::apply()
{
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        double factor = m_model->data(m_model->index(row,MODIFIER),Qt::DisplayRole).toDouble();
        double value = m_model->data(m_model->index(row,VALUE),Qt::DisplayRole).toDouble();
        QString act = m_model->data(m_model->index(row,ACT),Qt::DisplayRole).toString();
        QString newact = act+"*"+QString::number(factor);
        if (factor == 1.00)
        {
              newact = act;
            }
        QString newvalue = QString::number(value*factor);
        m_model->setData(m_model->index(row,ACT),newact,Qt::EditRole);
        m_model->setData(m_model->index(row,VALUE),newvalue,Qt::EditRole);
        m_model->submit();
        }
}


QHash<QString,QString> PondereActes::getModifiedValues()
{
    QHash<QString,QString> hash;
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        QString code = m_model->data(m_model->index(row,ACT),Qt::DisplayRole).toString();
        QString value = m_model->data(m_model->index(row,VALUE),Qt::DisplayRole).toString();
        hash.insertMulti(code,value);
        }
    return hash;
}


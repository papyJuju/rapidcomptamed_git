#include "treeviewdelegate.h"
#include "depensetools.h"
#include <common/tablesdiagrams.h>

#include <QDebug>


using namespace TablesDiagramsMouvements;

DateEditTreeViewFirstDelegate::DateEditTreeViewFirstDelegate(QObject * parent)
{
    Q_UNUSED(parent);
}

DateEditTreeViewFirstDelegate::~DateEditTreeViewFirstDelegate(){}

QSize DateEditTreeViewFirstDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(40,25);
}

QWidget *DateEditTreeViewFirstDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (  index.column() == DATE_VALEUR || index.column() == DATE)
    {
    	  QDateEdit * dateEdit = new QDateEdit(parent);
    	  dateEdit->setDisplayFormat("yyyy-MM-dd");
    	  return dateEdit;
        }
          return QStyledItemDelegate::createEditor(parent, option, index);

     
}

 void DateEditTreeViewFirstDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QDate date;
     date = index.model()->data(index,Qt::DisplayRole).toDate();
     dateEdit->setDate(date);
 }
 
  void DateEditTreeViewFirstDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QDateEdit * dateEdit = static_cast<QDateEdit*>(editor);
     QString value = dateEdit->date().toString("yyyy-MM-dd");

     model->setData(index, value, Qt::EditRole);
     model->submit();
 }
 
 ComboLibelleDelegate::ComboLibelleDelegate(QObject * parent)
 {
     Q_UNUSED(parent);
     m_mapLibelle = getLibelleFromDatabase();
}

ComboLibelleDelegate::~ComboLibelleDelegate(){}

QSize ComboLibelleDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(40,50);
}

QWidget *ComboLibelleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox * combo = new QComboBox(parent);
    combo->setEditable(true);
    combo->setInsertPolicy(QComboBox::InsertAtTop);
    return combo;
}

void ComboLibelleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
     Q_UNUSED(index);
     QComboBox * combo = static_cast<QComboBox*>(editor);
     QMap<int,QString>::const_iterator it;
     for (it = m_mapLibelle.begin(); it != m_mapLibelle.end(); ++it)
     {
         QString item = it.value();
         combo->addItem(item);
         }
}
 
void ComboLibelleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
{
     enum {KEYNAME=0,VALUE};
     QComboBox * combo = static_cast<QComboBox*>(editor);
     QString value = combo->currentText(); 
     model->setData(index,value, Qt::EditRole);
}

QMap<int,QString> ComboLibelleDelegate::getLibelleFromDatabase()
{
    QMap<int,QString> map;
    Depensetools deptools(this);
    map = deptools.getLibelleFromDatabase();
    return map;
}


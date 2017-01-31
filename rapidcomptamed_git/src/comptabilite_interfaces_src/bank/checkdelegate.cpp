#include "checkdelegate.h"


CheckBoxDelegate::CheckBoxDelegate(QObject * parent)
{
}

CheckBoxDelegate::~CheckBoxDelegate(){}

QSize CheckBoxDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(40,25);
}

QWidget *CheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (  index.column() == VALIDATION )
    {
    	  QCheckBox * checkBox = new QCheckBox(parent);
    	  checkBox->setCheckable(true);
    	  return checkBox;
        }
          return QStyledItemDelegate::createEditor(parent, option, index);

     
}

 void CheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
 {
     QCheckBox * checkBox = static_cast<QCheckBox*>(editor);
     QVariant data = index.model()->data(index,Qt::DisplayRole);
     if (data.toInt() == 1)
     {
     	  checkBox->setChecked(true);
         }
     else
     {
     	  checkBox->setChecked(false);
         }
 }
 
  void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                      const QModelIndex &index) const
 {
     QCheckBox * checkBox = static_cast<QCheckBox*>(editor);
     int value = 0 ;
     if (checkBox->isChecked())
     {
     	  value = 1;
         }
     model->setData(index, value, Qt::EditRole);
 }


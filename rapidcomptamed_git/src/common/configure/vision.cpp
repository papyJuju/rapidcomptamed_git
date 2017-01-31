#include <QtGui>
#include <QtCore>
#include <QtSql>
#include "vision.h"
#include "delegate.h"
#include "../constants.h"
using namespace Common;
using namespace Constants;
vision::vision(QString  base , QList<int>  listInt, int diverscolumn, QWidget * parent)
{
  setupUi(this);
  m_diverscolumn = diverscolumn;
  QList<int>    list             = listInt;
  setAttribute                     (Qt::WA_DeleteOnClose);
  QPoint parentPosition = parent->pos();
  move(parentPosition);
  QSqlDatabase db                = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  m_modelVision                  = new QSqlTableModel(this,db);
  m_modelVision                 -> setTable(base);
  m_modelVision                 -> setEditStrategy(QSqlTableModel::OnFieldChange);
  m_modelVision                 -> select();
  
  tableView                     -> setModel(m_modelVision);
  tableView                     -> setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView                     -> setSelectionMode(QAbstractItemView::SingleSelection);
  for(int i = 0 ; i < list.size() ; i ++)
  {
    int j                       =  list[i];
    tableView                   -> setColumnHidden(j,true);
    }
  tableView -> setItemDelegateForColumn(diverscolumn,new ComboDelegate(this,base));
  tableView                     -> horizontalHeader()-> setStretchLastSection ( true );
  tableView                     -> show();
  openEditorOnDivers();
  connect(deleteButton,  SIGNAL(pressed()),this,SLOT(effacer()));
  connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
  connect(m_modelVision,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),this,SLOT(refresh(const QModelIndex&,const QModelIndex&)));
}

vision::~vision(){}

void vision::effacer(){
  QModelIndex index                     = tableView->QAbstractItemView::currentIndex();
  int i                                 = index.row();
  m_modelVision                        -> removeRows(i,1,QModelIndex());
}

void vision::openEditorOnDivers()
{
    for (int r = 0; r < m_modelVision->rowCount(); ++r)
    {
          QModelIndex index = m_modelVision->index(r,m_diverscolumn);
          tableView->openPersistentEditor(index);
        }
}

void vision::refresh(const QModelIndex& indexbegin,const QModelIndex& indexend)
{
    Q_UNUSED(indexbegin);
    Q_UNUSED(indexend);
    openEditorOnDivers();
}


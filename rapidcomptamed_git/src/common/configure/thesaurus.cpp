#include "thesaurus.h"
#include <common/constants.h>
#include <common/position.h>
using namespace Common;
using namespace Constants;
thesaurusModify::thesaurusModify(QWidget * parent):QWidget(parent){
    setupUi(this);
    Position *position = new Position;
    position->centralPosition(this,width(),height());
    setAutoFillBackground(true);
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    m_model = new QSqlTableModel(this,m_db);
    m_model->setTable("thesaurus");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    tableView->setModel(m_model);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    tableView->horizontalHeader()->setStretchLastSection ( true );
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(okButton,SIGNAL(pressed()),this,SLOT(recordAndClose()));
    connect(addButton,SIGNAL(pressed()),this,SLOT(addRow()));
    connect(deleteButton,SIGNAL(pressed()),this,SLOT(deleteRow()));
}

thesaurusModify::~thesaurusModify(){}

void thesaurusModify::addRow(){
    int rows = m_model->rowCount();
    if (!m_model->insertRows(rows,1,QModelIndex()))
    {
    	  QMessageBox::warning(0,trUtf8("Error"),trUtf8("Unable to create row"),QMessageBox::Ok);
        }
    m_model->submitAll();
}

void thesaurusModify::deleteRow(){
    QModelIndex index = tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()){
        QMessageBox::warning(0,tr("Error"),tr("Please select a line to delete."),QMessageBox::Ok);
        return;
        }
    int row = index.row();
    if (!m_model->removeRows(row,1,QModelIndex()))
    {
    	  QMessageBox::warning(0,trUtf8("Error"),trUtf8("Unable to delete row"),QMessageBox::Ok);
        }
    m_model->submitAll();
}

void thesaurusModify::recordAndClose(){
    if (m_model->submitAll())
    {
    	  QMessageBox::information(0,trUtf8("Information"),trUtf8("Record success."),QMessageBox::Ok);
    	  close();
        }
    
}

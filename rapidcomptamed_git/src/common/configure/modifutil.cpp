
#include "modifutil.h"
#include "../constants.h"
#include <common/icore.h>
#include <common/position.h>

#include <QtGui>
#include <QtCore>
#include <QtSql>

static inline Common::Position *position() {return Common::ICore::instance()->position();}
using namespace Common;
using namespace Constants;
modifutil::modifutil(QWidget* parent):QWidget(parent){
  setupUi(this);
  setWindowFlags   (Qt::WindowStaysOnTopHint);
  //setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);
  setWindowFlags(Qt::Window);
  //position()->centralPosition(this,width(),height());
  m_dbUtil                     = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  m_modelUtil                  = new QSqlTableModel(this,m_dbUtil);
  m_modelUtil                 -> setTable("comptes_bancaires");
  m_modelUtil                 -> select();
  tableViewUtil               -> setModel(m_modelUtil);
  tableViewUtil               -> setSelectionBehavior(QAbstractItemView::SelectRows);
  tableViewUtil               -> setSelectionMode(QAbstractItemView::SingleSelection);
  tableViewUtil               -> horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableViewUtil               -> verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
  tableViewUtil               -> resizeColumnsToContents();
    
  connect(pushButton  , SIGNAL(pressed()),this,SLOT(enregistrerUtil()));
  connect(pushButton_2, SIGNAL(pressed()),this,SLOT(close()));
  connect(pushButton_3, SIGNAL(pressed()),this,SLOT(effacerLigne()));
}

modifutil::~modifutil(){}

void modifutil::enregistrerUtil(){
  if(m_modelUtil->submitAll() == false){
      QMessageBox::warning(0,tr("Attention"),trUtf8("Enregistrement incertain,\nveuillez vérifier votre base de données."),QMessageBox::Ok);
  }
  else{
      QMessageBox::information(0,tr("Information"),trUtf8("L'enregistrement a été effectué.\nLa fenêtre va fermer."),QMessageBox::Ok);
      emit close();
  }
}

void modifutil::effacerLigne(){
  QString id;
  QModelIndex index              = tableViewUtil->QAbstractItemView::currentIndex();
  int i                          = index.row();
  QModelIndex indexUsr           = m_modelUtil->index(i,1,QModelIndex());
  int idUsr                      = m_modelUtil->data(indexUsr,Qt::DisplayRole).toInt();
  m_modelUtil                    ->removeRows(i,1,QModelIndex());
  qDebug() << "delete ="+QString::number(idUsr);
  QString requeteDelete          = "DELETE  FROM utilisateurs WHERE id_usr = "+QString::number(idUsr)+"";
  QSqlQuery queryDelete(m_dbUtil);
            
  if(m_modelUtil->submitAll() == false){
      QMessageBox::warning(0,tr("Attention"),trUtf8("Effacement incertain,\nveuillez vérifier votre base de données."),QMessageBox::Ok);
  }
  else{
      queryDelete          . exec(requeteDelete);
      QMessageBox::information(0,tr("Information"),trUtf8("L'effacement a été effectué.\nLa fenêtre va fermer."),QMessageBox::Ok);
      emit close();
  }
  
}

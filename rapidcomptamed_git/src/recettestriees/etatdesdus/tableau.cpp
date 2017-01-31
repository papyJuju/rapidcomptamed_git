#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "tableau.h"

tabledialog::tabledialog(QSqlTableModel *modeldialog){
  setupUi(this);
  m_model = modeldialog;
  m_model->select();
  tableView->setModel(m_model);
  tableView->show();

  m_texte=lineEdit->text();

  if(m_texte.isEmpty())
      textenonchange();
  //----------------------connect-----------------------------------------------------------------
  connect(lineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(textechange(const QString&)));
  connect(pushButton  ,SIGNAL(pressed()),this,SLOT(accept()));
  connect(pushButton_2,SIGNAL(pressed()),this,SLOT(reject()));
}

tabledialog::~tabledialog(){}

void tabledialog::textechange(const QString &m_texte){

//m_model = new QSqlTableModel;
//m_model->setTable("IndexNomPrenom");

  m_model->setFilter("name LIKE '"+m_texte+"%'");
  m_model->setSort(1,Qt::AscendingOrder);//on classe par ordre alphabétique

  m_model->select();

  m_proxymodel = new QSortFilterProxyModel;
  m_proxymodel->setSourceModel(m_model);
  tableView->setModel(m_proxymodel);
  tableView->setColumnHidden(0,true);//on cache la colonne ID_Primkey
  tableView->setColumnHidden(3,true);//on cache la colonne FchGnrl_IDDos
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->resizeColumnsToContents();
}
//
void tabledialog::textenonchange(){

//m_model = new QSqlTableModel;
//m_model->setTable("IndexNomPrenom");

  m_model->setSort(1,Qt::AscendingOrder);//on classe par ordre alphabétique

  m_model->select();

  m_proxymodel = new QSortFilterProxyModel;
  m_proxymodel->setSourceModel(m_model);

  tableView->setModel(m_proxymodel);
  tableView->setColumnHidden(0,true);//on cache la colonne id_index
  tableView->setColumnHidden(3,true);//on cache la colonne guid
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->resizeColumnsToContents();
}

QString tabledialog::recupPatient(){
  QModelIndex index = tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()){
        QMessageBox::warning(0,tr("Erreur"),trUtf8("vous avez oublié de sélectionner un patient."),QMessageBox::Ok);
    }
  int row           = index.row();
  QModelIndex guid  = m_proxymodel->index(row,3,QModelIndex());
  QString guidpatient     = guid.data(Qt::DisplayRole).toString();
  emit close();
  return guidpatient;
}

tableList::tableList(QStringList & list){
  setupUi(this);
  lineEdit->hide();
  QStandardItemModel *m = new QStandardItemModel;

  for(int i = 0; i< list.size();i++)
  {
      //qDebug() << __FILE__ << QString::number(__LINE__) << " list[i] = " << list[i];
     // int row = i;
     // int col = 0;
      const QString str = list[i];
      QStandardItem *item = new QStandardItem(str);

      m->appendRow(item);
  }
  const QVariant variant(trUtf8("Nom,prénom"));
  m->setHeaderData(0,Qt::Horizontal,variant);
  tableView->setModel(m);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->setSortingEnabled(true);
  tableView->show();
  connect(pushButton  ,SIGNAL(pressed()),this,SLOT(accept()));
  connect(pushButton_2,SIGNAL(pressed()),this,SLOT(reject()));

}

QString tableList::findPatientName(){
  QString patient;
    QModelIndex index = tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()){
        QMessageBox::warning(0,tr("Erreur"),trUtf8("vous avez oublié de sélectionner un patient."),QMessageBox::Ok);
    }
  //int row           = index.row();
  //QStandardItemModel *mod = tableView->model();
  patient = index.data (Qt::DisplayRole ).toString();// list[row];
  qDebug() << __FILE__ << QString::number(__LINE__) << " patient = "+patient;
  return patient;
}

tableList::~tableList(){}

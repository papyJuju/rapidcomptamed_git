#include "nameindex.h"
#include "iotools.h"
#include "tools.h"

#include <common/configure/medintux/constants_medintux.h>

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace NameIndexDiagramSpace;
using namespace MedintuxConnexionSpace;

NamesDialog::NamesDialog(QWidget * parent, QSqlDatabase db,const QString & programname )
{
    Q_UNUSED(parent);
    setupUi(this);
    m_programName = programname;
    m_dbin = QSqlDatabase(db);
    if (!m_programName.isEmpty())
    {
          recordButton->hide();
          deleteButton->hide();
          if (m_programName == "Medintux")
          {
                m_connexion = new ConnexionMedintux(this);
                QString drtuxbase = m_connexion->medintuxbasename();
                if (!m_connexion->connectToDrTux())
                {
                      qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect to " << drtuxbase ;
                    }                
                m_dbMedintux = QSqlDatabase::database(drtuxbase);
              }
        }
    textchanged("");
    connect(searchEdit,SIGNAL(textChanged(const QString &)),this,SLOT(textchanged(const QString&)));
    connect(recordButton,SIGNAL(pressed()),this,SLOT(recordName()));
    connect(deleteButton,SIGNAL(pressed()),this,SLOT(deleteName()));
}

NamesDialog::~NamesDialog(){}

void NamesDialog::textchanged(const QString & texts)
{
    QString text = texts;
    if(text.contains("'")){
       text.replace("'","''");
       }
    QString table;    
    if (m_programName == "Medintux")
    {
        QString indexnomprenom = "IndexNomPrenom";
        #ifdef Q_OS_WIN32
        indexnomprenom = "indexnomprenom";
        #endif
        m_tableModel = new QSqlTableModel(this,m_dbMedintux);
        m_tableModel->setTable(indexnomprenom);
        m_tableModel->setFilter("FchGnrl_NomDos LIKE '"+text+"%'");
        } 
    else
    {
        m_tableModel = new QSqlTableModel(this,m_dbin);
        m_tableModel->setTable("nameindex");
        m_tableModel->setFilter("name LIKE '"+text+"%'");
        }
    m_tableModel->setSort(1,Qt::AscendingOrder);//on classe par ordre alphabétique
    m_tableModel->select();
    tableView->setModel(m_tableModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void NamesDialog::recordName()
{
    enum NameEnum {NAME_NAMESDIALOG = 0,FIRSTNAME_NAMESDIALOG};
    QString totalname;
    QStringList namelist;
    totalname = searchEdit->text();
    QString separator;
    qDebug() << __FILE__ << QString::number(__LINE__) << " totalname =" << totalname ;
    if ((totalname.contains(QChar(054))==false)&& (totalname.contains(QChar(073))==false))
    {
          QMessageBox::warning(0,tr("Warning"),tr("Veuillez séparer le nom et le prénom par , ou ; "),QMessageBox::Ok);
          return;
        }
    else
    {
        if (totalname.contains(QChar(054)))
        {
              qDebug() << __FILE__ << QString::number(__LINE__) << " totalname contains =" << "," ;
              separator = ",";
            }
        else if (totalname.contains(QChar(073)))
        {
              separator = ";";
            }
        else
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << "no separator" ;
            return;
            }
        namelist = totalname.split(separator);
        }
    QSqlTableModel model(this,m_dbin);
    model.setTable("nameindex");
    model.select();
    int rows = model.rowCount();    
    if (!model.insertRows(rows,1))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << model.lastError().text() ;
        }
    Iotools iotool;
    QString guid = iotool.newGuidOfNameIndex();
    QString name = namelist[NAME_NAMESDIALOG];
    QString firstname = namelist[FIRSTNAME_NAMESDIALOG];
    QString divers;
    QStringList values;
    values<< QString() << name << firstname << guid << divers;    
    for ( int i  = 1; i < NameindexHeadersMaxParam; ++i )
    {
          if (!model.setData(model.index(rows,i),values[i],Qt::EditRole))
          {
                qWarning() << __FILE__ << QString::number(__LINE__) << model.lastError().text() ;
              }
        }
    if (!model.submit())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << model.lastError().text() ;
        }
    else
    {
        QMessageBox::information(0,tr("Information"),namelist.join(separator)+tr(" a été enregistré."),QMessageBox::Ok);
        textchanged("");
        }
        
}

void NamesDialog::deleteName()
{
    int row = tableView->currentIndex().row();
    QAbstractItemModel * model = tableView->model();
    QString id = model->data(model->index(row,ID_INDEX),Qt::DisplayRole).toString();
    QSqlQuery qy(m_dbin);
    QString req = QString("delete from %1 where %2 = '%3'").arg("nameindex","id_index",id);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to delete line" ;
          QMessageBox::warning(0,tr("Warning"),tr("Impossible de supprimer le nom."),QMessageBox::Ok);
        }
    else
    {
        QMessageBox::information(0,tr("Information"),tr("Le nom a été effacé."),QMessageBox::Ok);
        textchanged("");
        }
}


QString NamesDialog::getName()
{
    QString nameresult;
    QModelIndex index = tableView->currentIndex();
    int row = index.row();    
    QString name; 
    QString surname; 
    if (tableView->model())
    {
          QAbstractItemModel * model = tableView->model();
          name = model->data(model->index(row,NAME),Qt::DisplayRole).toString();
          surname = model->data(model->index(row,SURNAME),Qt::DisplayRole).toString();
        }
    nameresult = name+","+surname;
    return nameresult;
}

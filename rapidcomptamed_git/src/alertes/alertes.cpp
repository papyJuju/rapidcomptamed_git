#include "alertes.h"
#include <common/constants.h>
#include <common/configure/delegate.h>
#include <common/icore.h>
#include <common/settings.h>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>

using namespace Common;
using namespace Constants;
using namespace AlertesSpace;


static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

Alertes::Alertes(QWidget * parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    m_model = new QSqlTableModel(this,m_db);
    fillUsersBox();
    QString userpref = findUserPref();
    fillTable(userpref);
    connect(recordButton,SIGNAL(pressed()),this,SLOT(recordAlerte()));
    connect(deleteButton,SIGNAL(pressed()),this,SLOT(deleteAlerte()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(usersBox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(fillTable(const QString&)));
}

Alertes::~Alertes(){}

void Alertes::fillTable(const QString& user)
{
    enum UserInside {ID_USR=0,LOGIN};
    if (m_model->rowCount()>0)
    {
        m_model->clear();
        }
    QString id_usr = user.split(",")[ID_USR];
    QString filter = QString("%1='%2'").arg("id_usr",id_usr);
    m_model->setTable(Constants::TABLE_ALERTES);
    m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_model->setHeaderData(ALERTE_COLUMN,Qt::Horizontal,trUtf8("Alertes"));
    m_model->setHeaderData(DATE_ALERTE,Qt::Horizontal,trUtf8("Dates des alertes"));
    m_model->select();
    tableView->setModel(m_model);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setResizeMode(AlertesSpace::ID_USR_ALERTE,QHeaderView::Stretch);
    tableView->setItemDelegateForColumn(DATE_ALERTE,new DateDelegate(this)); 
    tableView->setItemDelegateForColumn(VALID_ALERTE,new ComboYesNoDelegate(this));
    tableView->setItemDelegateForColumn(ISREAD,new ComboYesNoDelegate(this));
    for (int r = 0; r < m_model->rowCount(); ++r)
    {
    	QModelIndex indexvalid = m_model->index(r,VALID_ALERTE);
    	QModelIndex indexisread = m_model->index(r,ISREAD);
    	tableView->openPersistentEditor(indexvalid);
    	tableView->openPersistentEditor(indexisread);
    }
}

void Alertes::fillUsersBox()
{
    enum UserInside {ID_USR=0,LOGIN};
    QString req = QString("select id_usr,login from utilisateurs");
    QSqlQuery qy(m_db);
    QStringList list;
    list << findUserPref();
    if (!qy.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while(qy.next())
    {
         QString login = qy.value(LOGIN).toString();
         QString id_usr = qy.value(ID_USR).toString();
         list << id_usr+","+login;
    }
    list.removeDuplicates();
    usersBox->addItems(list);
}

QString Alertes::findUserPref()
{
  QString userpref = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
  QString id = Common::ICore::instance()->settings()->value(S_ID_UTILISATEUR_CHOISI).toString();
  return id+","+userpref;
}

void Alertes::recordAlerte()
{
    enum AlertesInside {ALERTE_ID=0,ALERTE_NAME,DATE_DATE,ALERTES_ID_USR};
    enum UserInside {ID_USR=0,LOGIN};
    QString alerte = alerteEdit->text();
    QDate date = calendar->selectedDate();
    QString datestr = date.toString("yyyy-MM-dd");
    QString loginId = usersBox->currentText();
    QString id_usr = loginId.split(",")[ID_USR];
    QString yes = "0";
    QString no = "1";
    QSqlQuery qy(m_db);
    QString req = QString("insert into %1 (%2,%3,%4,%5,%6) ")
                  .arg(TABLE_ALERTES,"alertes","date","id_usr","valid","isread");
    req+= QString("values('%1','%2','%3','%4',%5)").arg(alerte,datestr,id_usr,yes,no);
    if (!qy.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    QString userpref = usersBox->currentText().split(",")[0];
    fillTable(userpref);
}

void Alertes::deleteAlerte()
{
    int row = tableView->currentIndex().row();
    if (m_model->removeRows(row,1))
    {
    	QMessageBox::information(0,tr("Information"),tr("La ligne a été effacée."),QMessageBox::Ok);
    	}
    else{
        QMessageBox::warning(0,tr("Attention"),tr("Impossible d'effacer la ligne."),QMessageBox::Ok);
        }
    	
}

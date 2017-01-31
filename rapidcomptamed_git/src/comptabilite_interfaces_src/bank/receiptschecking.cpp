#include "receiptschecking.h"
#include "checkdelegate.h"

#include <common/icore.h>
#include <common/constants.h>
#include <common/settings.h>
#include <common/configure/util.h>
#include <common/commandlineparser.h>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

using namespace Common;
using namespace Constants;
enum { WarnDebugMessage = true };

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}

ReceiptsChecking::ReceiptsChecking(QWidget * parent)
{
    setupUi(this);
    const QString alphaText = "<html><body><font size=3 color=red>Cette fonction est ALPHA</font></body></html>";
    labelAlpha->setText(alphaText);
    label->setText("");
    m_accountancyDb = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
   
    QString prefUser = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
    QString prefId = Common::ICore::instance()->settings()->value(S_ID_UTILISATEUR_CHOISI).toString();
    if (prefUser.isEmpty())
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Impossible de choisir un utilisateur préférentiel."),QMessageBox::Ok);
        }
    QDate dateBegin = getFirstDate(prefId);
    dateBeginEdit->setDate(dateBegin);
    dateEndEdit->setDate(QDate::currentDate());
    m_model = new QSqlTableModel(this,m_accountancyDb);
    m_model->setTable("honoraires");
    QStringList usersList;
    usersList << prefUser;
    
    QSqlQuery qUser(m_accountancyDb);
    QString reqUsers = QString("SELECT %1,%2,%3 FROM %4").arg("id_usr","nom_usr","login","utilisateurs");
    if (!qUser.exec(reqUsers))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qUser.lastError().text();
    	  
        }
    while (qUser.next())
    {
    	QString login = qUser.value(LOGIN).toString();
    	QString id = qUser.value(ID_USER).toString();
    	usersList << login;
    	m_hashLoginId.insert(login,id);
        }
    usersList.removeDuplicates();
    comboBoxUsers->addItems(usersList);
    getReceiptsUnchecked(prefUser);
    connect(comboBoxUsers,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(getReceiptsUnchecked(const QString &)));
    connect(recordButton,SIGNAL(pressed()),this,SLOT(recordCheckedReceipts()));
    connect(cancelButton,SIGNAL(pressed()),this,SLOT(revertCheckedReceipts()));
    connect(detailEdit,SIGNAL(returnPressed()),this,SLOT(recordDetail()));    
    connect(closeButton,SIGNAL(pressed()),this,SLOT(close()));    
}

ReceiptsChecking::~ReceiptsChecking(){}

void ReceiptsChecking::getReceiptsUnchecked(const QString & login)
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " in getReceiptsUnchecked, login =" <<  login;
    if (security()->isUserLocked(login))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Cet utilisateur est verrouillé !"),QMessageBox::Ok);
    	  return ;
        }
    QString id = m_hashLoginId.value(login);
    qDebug() << __FILE__ << QString::number(__LINE__) << " id =" << id ;
    QString dateBegin = dateBeginEdit->date().toString("yyyy-MM-dd");
    QString dateEnd = dateEndEdit->date().toString("yyyy-MM-dd");
    
    QString filter = QString(" %1 < '%2'  AND date BETWEEN '%3' AND '%4' AND %5 = '%6'")
                    .arg("valide","1",dateBegin,dateEnd,"id_usr",id);

    m_model->setFilter(filter);
    m_model->select();
    
    qDebug() << __FILE__ << QString::number(__LINE__) << " query = " << m_model->query().lastQuery()  ;

    tableView->setModel(m_model);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableView->setColumnHidden(ID_HONO,true);
    tableView->setColumnHidden(ID_USR,true);
    tableView->setColumnHidden(ID_DRTUX_USR,true);
    tableView->setColumnHidden(ID_SITE,true);
    tableView->setColumnHidden(GUID,true);
    tableView->setColumnHidden(ACTECLAIR,true);
    tableView->setColumnHidden(DU,true);
    tableView->setColumnHidden(DU_PAR,true);
    tableView->setColumnHidden(TRACABILITE,true);
    tableView->setItemDelegateForColumn(VALIDE,new CheckBoxDelegate(this));
    for (int r = 0; r < m_model->rowCount(); ++r)
    {
    	  tableView->openPersistentEditor(m_model->index(r,VALIDE));
        }

}

QDate ReceiptsChecking::getFirstDate(const QString & id)
{
    QDate date;
    QSqlQuery q(m_accountancyDb);
    QString req = QString("SELECT %1 FROM %2 WHERE %3 NOT LIKE '%4' AND %5 = '%6'")
                 .arg("date","honoraires","valide","1");
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text();
          }
    int n = 0;
    while (q.next())
    {
    	if (n == 0)
    	{
    		  date = q.value(0).toDate();
    	    }
    	++n;
        }
    return date;
}

void ReceiptsChecking::recordDetail()
{
    int row = tableView->currentIndex().row();
    QString detail = detailEdit->text().trimmed();
    m_model->setData(m_model->index(row,REMARQUE),detail,Qt::EditRole);
    m_model->submit();
}

void ReceiptsChecking::recordCheckedReceipts()
{
   if (! m_model->submitAll())
   {
   	  qWarning() << __FILE__ << QString::number(__LINE__) << m_model->query().lastError() ;
       }
}

void ReceiptsChecking::revertCheckedReceipts()
{
    m_model->revertAll();
}

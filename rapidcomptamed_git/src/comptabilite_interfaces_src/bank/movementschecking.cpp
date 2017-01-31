#include "movementschecking.h"
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

MovementsChecking::MovementsChecking(QWidget * parent)
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
    m_model->setTable("mouvements");
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
    getMovementsUnchecked(prefUser);
    connect(comboBoxUsers,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(getMovementsUnchecked(const QString &)));
    connect(recordButton,SIGNAL(pressed()),this,SLOT(recordCheckedMovements()));
    connect(cancelButton,SIGNAL(pressed()),this,SLOT(revertCheckedMovements()));
    
    connect(closeButton,SIGNAL(pressed()),this,SLOT(close()));    
}

MovementsChecking::~MovementsChecking(){}

void MovementsChecking::getMovementsUnchecked(const QString & login)
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " in getMovementsUnchecked, login =" <<  login;
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
                    .arg("validation","1",dateBegin,dateEnd,"id_usr",id);

    m_model->setFilter(filter);
    m_model->select();
    
    qDebug() << __FILE__ << QString::number(__LINE__) << " query = " << m_model->query().lastQuery()  ;

    
    tableView->setModel(m_model);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    tableView->setColumnHidden(ID_MOUVEMENT,true);
    tableView->setColumnHidden(ID_MVT_DISPO,true);
    tableView->setColumnHidden(ID_USR,true);
    tableView->setColumnHidden(ID_COMPTE,true);
    tableView->setColumnHidden(TYPE,true);
    tableView->setColumnHidden(VALIDE,true);
    tableView->setColumnHidden(TRACABILITE,true);
    tableView->setItemDelegateForColumn(VALIDATION,new CheckBoxDelegate(this));
    for (int r = 0; r < m_model->rowCount(); ++r)
    {
    	  tableView->openPersistentEditor(m_model->index(r,VALIDATION));
        }

}

QDate MovementsChecking::getFirstDate(const QString & id)
{
    QDate date;
    QSqlQuery q(m_accountancyDb);
    QString req = QString("SELECT %1 FROM %2 WHERE %3 NOT LIKE '%4' AND %5 = '%6'")
                 .arg("date","mouvements","validation","1");
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

void MovementsChecking::recordCheckedMovements()
{
   if (! m_model->submitAll())
   {
   	  qWarning() << __FILE__ << QString::number(__LINE__) << m_model->query().lastError() ;
       }
}

void MovementsChecking::revertCheckedMovements()
{
    m_model->revertAll();
}

#include "deposit.h"
#include "controlbalance.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/databasemanager.h>
#include <common/settings.h>
#include <common/constants.h>
#include <common/position.h>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace Common;
using namespace Constants;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
static inline Common::DatabaseManager *databaseManager(){return Common::ICore::instance()->databaseManager();}
static inline Common::Position *position() {return Common::ICore::instance()->position();}

Deposits::Deposits(QWidget * parent):QWidget(parent){
    setupUi(this);
    position()->centralPosition(this,width(),height());
    m_db  = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    balanceButton->hide();
    //balanceButton->setEnabled(false);
    //setAutoFillBackground(true);
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    otherLabel->setWordWrap(true);
    cashDoubleSpinBox->setRange(0.00,1000000000.00);
    checksDoubleSpinBox->setRange(0.00,1000000000.00);
    visaDoubleSpinBox->setRange(0.00,1000000000.00);
    otherDoubleSpinBox->setRange(0.00,1000000000.00);
    balanceDoubleSpinBox->setRange(-1000000000.00,1000000000.00);
    cashDoubleSpinBox->setDecimals(2);
    checksDoubleSpinBox->setDecimals(2);
    visaDoubleSpinBox->setDecimals(2);
    otherDoubleSpinBox->setDecimals(2);
    balanceDoubleSpinBox->setDecimals(2);
    beginDateEdit->setDate(lastValidatedDate());
    endDateEdit->setDate(QDate::currentDate());
    //bank accounts
    m_prefBankLibelle = settings()->value(Constants::S_COMPTE_PREFERENTIEL).toString();
    QSqlQuery qBanks(m_db);
    const QString reqBanks = QString("SELECT %1,%2 FROM %3").arg("id_compte","libelle","comptes_bancaires");
    if (!qBanks.exec(reqBanks))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qBanks.lastError().text() ;
        }
    while (qBanks.next())
    {
    	QString idBank = qBanks.value(0).toString();
    	QString libelle = qBanks.value(1).toString();
    	m_hashBanks.insert(idBank,libelle);
        }
        
    //pref users
    m_prefLogin = settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
    m_prefId = settings()->value(Constants::S_ID_UTILISATEUR_CHOISI).toInt();
    QSqlQuery qPrefUser(m_db);
    const QString reqPrefUser = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("login",
                                                                                 "utilisateurs",
                                                                                 "id_usr",
                                                                                 QString::number(m_prefId));
    if (!qPrefUser.exec(reqPrefUser))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qPrefUser.lastError().text() ;
        }
    while (qPrefUser.next())
    {
    	m_prefUserLogin = qPrefUser.value(0).toString();
        }
    //combo users
    QSqlQuery queryUsers(m_db);
    const QString reqUsers = QString("SELECT %1,%2 FROM %3").arg("login","id_usr","utilisateurs");
    if (!queryUsers.exec(reqUsers))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << queryUsers.lastError().text() ;
        }
    while (queryUsers.next())
    {
    	QString userLogin = queryUsers.value(0).toString();
    	QString userId = queryUsers.value(1).toString();
    	m_hashUsers.insert(userId,userLogin);
        }
    QStringList usersList;
    usersList = m_hashUsers.values();
    usersList.prepend(m_prefUserLogin);
    usersList.removeDuplicates();
    usersComboBox->addItems(usersList);
    remplirComptesBancaires(m_prefUserLogin);
    //beginDateEdit->setDisplayFormat("yyyy/MM/dd");
    m_hashEdits.insert(CASH_EDIT,cashDoubleSpinBox);
    m_hashEdits.insert(CHECKS_EDIT,checksDoubleSpinBox);
    m_hashEdits.insert(VISA_EDIT,visaDoubleSpinBox);
    m_hashEdits.insert(OTHER_EDIT,otherDoubleSpinBox);
    quitButton->setShortcut(QKeySequence::Close);
    //connect
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(cancelButton,SIGNAL(pressed()),this,SLOT(cancel()));
    connect(okButton,SIGNAL(pressed()),this,SLOT(recordInBank()));
    //connect(balanceButton,SIGNAL(pressed()),this,SLOT(showBalanceDialog()));
    connect(bankComboBox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(getBalance(const QString&)));
    connect(cashDoubleSpinBox,SIGNAL(editingFinished()),this,SLOT(setBalanceChanged()));
    connect(checksDoubleSpinBox,SIGNAL(editingFinished()),this,SLOT(setBalanceChanged()));
    connect(visaDoubleSpinBox,SIGNAL(editingFinished()),this,SLOT(setBalanceChanged()));
    connect(otherDoubleSpinBox,SIGNAL(editingFinished()),this,SLOT(setBalanceChanged()));
    connect(balanceDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setNewBalance(double)));
    connect(bankComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(cancel(int)));
    connect(usersComboBox,SIGNAL(highlighted(const QString &)),this,SLOT(remplirComptesBancaires(const QString &)));
    
    
}

Deposits::~Deposits(){}

void Deposits::cancel(){
    for (int i = 0; i < m_hashEdits.size(); i += 1)
    {
    	m_hashEdits.value(i)->setValue(0.00);
    	}
    commentEdit->setText("");
}

void Deposits::cancel(int index){
    Q_UNUSED(index);
    for (int i = 0; i < m_hashEdits.size(); i += 1)
    {
    	m_hashEdits.value(i)->setValue(0.00);
    	}
    commentEdit->setText("");
}

void Deposits::recordInBank(){
    QSqlQuery q(m_db);
    QString deposit = QString("%1+%2+%3+%4").arg(QString::number(cashDoubleSpinBox->value()),
                                                 QString::number(checksDoubleSpinBox->value()),
                                                 QString::number(visaDoubleSpinBox->value()),
                                                 QString::number(otherDoubleSpinBox->value()));
    const QString fields = QString("id_usr,id_compte,type_depot,date,periode_deb,periode_fin,blob_depot,remarque");
    const QString values = QString("'%1','%2','%3','%4','%5','%6','%7','%8'")
                                                             .arg(idUsrChosen(),
                                                                  idBankChosen(),
                                                                  "all",
                                                                  QDate::currentDate().toString("yyyy-MM-dd"),
                                                                  beginDateEdit->date().toString("yyyy-MM-dd"),
                                                                  endDateEdit->date().toString("yyyy-MM-dd"),
                                                                  deposit,//cash+checks+visa+others (like this)
                                                                  commentEdit->text());

    const QString req = QString("INSERT INTO %1 (%2) VALUES(%3)").arg("depots",fields,values);
    QSqlQuery qRapBanc(m_db);
    const QString reqRapBank = QString("INSERT INTO %1 (%2) VALUES('%3','%4')")
                               .arg("rapprochement_bancaire",
                                    "id_compte,solde",
                                    idBankChosen(),
                                    QString::number(balanceDoubleSpinBox->value()));
    if (!q.exec(req)||!qRapBanc.exec(reqRapBank))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to insert values,")
    	  +q.lastError().text()+"\n"+qRapBanc.lastError().text(),QMessageBox::Ok);
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "insert = " << q.lastQuery()
    	                                                                     << qRapBanc.lastQuery();
          }
    else
    {
    	QMessageBox::information(0,trUtf8("Information"),trUtf8("Values has been inserted."),QMessageBox::Ok);
        showBalanceDialog();
        }
     //balanceButton->setEnabled(true);   
}

QString Deposits::idUsrChosen(){
    QString login = usersComboBox->currentText();
    QString id = "0";
    id = m_hashUsers.key(login);
    return id;
}

QString Deposits::idBankChosen(){
    QString bank = bankComboBox->currentText();
    QString id = "0";
    id = m_hashBanks.key(bank);
    return id;
}

void Deposits::showBalanceDialog(){
    QString id_compte = m_hashBanks.key(bankComboBox->currentText());
    double cashDeposits = cashDoubleSpinBox->value();
    double checksDeposits = checksDoubleSpinBox->value();
    double visaDeposits = visaDoubleSpinBox->value();
    double othersDeposits = otherDoubleSpinBox->value();
    QDate dateBegin = beginDateEdit->date();
    QDate dateEnd = endDateEdit->date();
    ControlBalance *b = new ControlBalance(dateBegin,
                                           dateEnd,
                                           this,
                                           m_lastBalance,
                                           m_balance,
                                           cashDeposits,
                                           checksDeposits,
                                           visaDeposits,
                                           othersDeposits,
                                           id_compte);
    b->exec();
}

void Deposits::getBalance(const QString & bank){
    QString id_compte = m_hashBanks.key(bank);
    qDebug() << __FILE__ << QString::number(__LINE__) << " id_compte =" << id_compte ;
    QSqlQuery qBalance(m_db);
    const QString reqBalance = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("solde",
                                                                                "rapprochement_bancaire",
                                                                                "id_compte",
                                                                                id_compte);
    if (!qBalance.exec(reqBalance))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qBalance.lastError().text() ;
        }
    while (qBalance.next())
    {
    	double balance = qBalance.value(0).toDouble();
    	m_balance = balance;
    	m_lastBalance = balance;
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_balance =" << QString::number(m_balance) ;
    balanceDoubleSpinBox->setValue(m_balance);
}

void Deposits::setBalanceChanged(){
    //double lastValue = qobject_cast<QDoubleSpinBox*>(sender())->value() - 1;
    QDoubleSpinBox * spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int send = m_hashEdits.key(spinBox);
    double value = spinBox->value();
    double lastValue = 0.00;
    switch(send){
        case CASH_EDIT :
            qDebug() << __FILE__ << QString::number(__LINE__) << " in CASH_EDIT "  ;
            lastValue = m_lastCashValue;
            m_lastCashValue = value;
            break;
        case CHECKS_EDIT :
            qDebug() << __FILE__ << QString::number(__LINE__) << " in CHECKS_EDIT "  ;
            lastValue = m_lastChecksValue;
            m_lastChecksValue = value;
            break;
        case VISA_EDIT :
            qDebug() << __FILE__ << QString::number(__LINE__) << " in VISA_EDIT "  ;
            lastValue = m_lastVisaValue;
            m_lastVisaValue = value;
            break;
        case OTHER_EDIT :
            qDebug() << __FILE__ << QString::number(__LINE__) << " in OTHER_EDIT "  ;
            lastValue = m_lastOtherValue;
            m_lastOtherValue = value;
        default :
            break;    
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " lastValue =" << QString::number(lastValue) ;
    m_balance += (value - lastValue);
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_balance =" << QString::number(m_balance) ;
    balanceDoubleSpinBox->setValue(m_balance);
}

void Deposits::setNewBalance(double balance){
    m_balance = balance;
}

QDate Deposits::lastValidatedDate(){
   QDate lastDate;
   QSqlQuery qDate(m_db);
   const QString req = QString("SELECT %1 FROM %2  WHERE %3 = '%4' ORDER BY date").arg("date",
                                                                                      "honoraires",
                                                                                      "valide",
                                                                                      "1");
   if (!qDate.exec(req))
   {
   	  qWarning() << __FILE__ << QString::number(__LINE__) << qDate.lastError().text() ;
       }
   QList<QDate> listOfDates;
   while (qDate.next())
   {
   	lastDate = qDate.value(0).toDate();
   	listOfDates << lastDate;
   	//qDebug() << __FILE__ << QString::number(__LINE__) << " date =" << lastDate.toString("yyyy-MM-dd") ;
       }
   if (listOfDates.size()<1)
   {
   	  lastDate = QDate::currentDate();
       }
   return lastDate;
}

void Deposits::remplirComptesBancaires(const QString & login){
    bankComboBox->clear();
    QString idUsr = m_hashUsers.key(login);
    QSqlQuery q(m_db);
    QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                  .arg("id_compte,id_usr,libelle,titulaire,nom_banque",
                       "comptes_bancaires",
                       "id_usr",
                       idUsr);
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    QStringList libelleList;
    while (q.next())
    {
    	QString idCompte  = q.value(IDCOMPTE).toString();
    	QString idUsr     = q.value(IDUSER_CB).toString();
    	QString libelle   = q.value(LIBELLE_CB).toString();
    	QString titulaire = q.value(TITULAIRE_CB).toString();
    	QString nomBanque = q.value(NOM_BANQUE_CB).toString();
    	libelleList << libelle;
        }
    int j = 1;
    QHash<int,QString> hashLibelles;
    for (int i = 0; i < libelleList.size(); i += 1)
    {
    	if (libelleList[i]==m_prefBankLibelle)
    	{
    		hashLibelles.insert(0,libelleList[i]);  
    	    }
    	else
    	{
    		hashLibelles.insert(j,libelleList[i]);
    		j++;
    	    }
        }
    bankComboBox->addItems(hashLibelles.values());
    //balance
        if (hashLibelles.values().size()>0)
    {
    	  getBalance(hashLibelles.values()[0]);
        }
        
}

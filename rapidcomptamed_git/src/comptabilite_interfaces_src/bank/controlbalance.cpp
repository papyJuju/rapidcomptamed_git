#include "controlbalance.h"
#include "validdialog.h"
#include <common/constants.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

using namespace Common;
using namespace Constants;

ControlBalance::ControlBalance(QDate & dateBegin,
                               QDate & dateEnd,
                               QWidget * parent,
                               double  lastBalance,
                               double  balance,
                               double  cashDeposits,
                               double  checksDeposits,
                               double  visaDeposits,
                               double  othersDeposits,
                               QString idCompte):QDialog(parent){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in controlbalance " ;
    setupUi(this);
    m_db  = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    m_dateBegin = dateBegin;
    m_dateEnd = dateEnd;
    m_lastBalance = lastBalance;
    m_balance = balance;
    m_cashDeposits = cashDeposits;
    m_checksDeposits = checksDeposits;
    m_visaDeposits = visaDeposits;
    m_othersDeposits =othersDeposits;//vir
    m_id_compte = idCompte;
    analyse();
    connect(okButton,SIGNAL(pressed()),this,SLOT(accept()));
    connect(cancelButton,SIGNAL(pressed()),this,SLOT(reject()));
}

ControlBalance::~ControlBalance(){}

void ControlBalance::analyse(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in analyse " ;
    double sumOfNotValidatedHono = 0.00;
    double sumOfCash = 0.00;
    double sumOfChecks = 0.00;
    double sumOfVisa = 0.00;
    double sumOfVir = 0.00;//vir
    QHash<int,QString> hashOfIdHonoNotValidated;
    QSqlQuery qHonoNotValidated(m_db);
    const QString reqHono = QString("SELECT %1 FROM %2 WHERE %3 NOT LIKE '%4' AND %5 BETWEEN '%6' AND '%7'")
                                                                               .arg("esp,chq,cb,vir,id_hono",
                                                                                    "honoraires",
                                                                                    "valide",
                                                                                    "1",
                                                                                    "date",
                                                                                    m_dateBegin.toString("yyyy-MM-dd"),
                                                                                    m_dateEnd.toString("yyyy-MM-dd"));
    if (!qHonoNotValidated.exec(reqHono))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qHonoNotValidated.lastError().text() ;
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << " qHonoNotValidated =" << qHonoNotValidated.lastQuery() ;
    while (qHonoNotValidated.next())
    {
    	int type = 0;
    	double cash = qHonoNotValidated.value(CASH).toDouble();
    	if (cash > 0)
    	{
    		type = CASH;  
    	    }
    	qDebug() << __FILE__ << QString::number(__LINE__) << " cash =" << QString::number(cash) ;
    	double checks = qHonoNotValidated.value(CHECKS).toDouble();
    	if (checks > 0)
    	{
    		type = CHECKS;  
    	    }
    	double visa = qHonoNotValidated.value(VISA).toDouble();
    	if (visa > 0)
    	{
    		type = VISA;  
    	    }
    	double vir = qHonoNotValidated.value(VIR).toDouble();
    	if (vir > 0)
    	{
    		type = VIR;
    	    }
    	QString idHono = qHonoNotValidated.value(IDHONO).toString();
    	
    	qDebug() << __FILE__ << QString::number(__LINE__) << " idHono =" << idHono ;
    	sumOfCash += cash;
    	sumOfChecks += checks;
    	sumOfVisa += visa;
    	sumOfVir += vir;
    	sumOfNotValidatedHono += cash+checks+visa+vir;
    	hashOfIdHonoNotValidated.insertMulti(type,idHono) ;
        }
    double newBalanceOfAccount = m_lastBalance + sumOfNotValidatedHono;
    //tests
    qDebug() << __FILE__ << QString::number(__LINE__) << " newBalanceOfAccount =" << QString::number(newBalanceOfAccount) ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_balance =" << QString::number(m_balance) ;
    double differenceTotal = newBalanceOfAccount - m_balance;
    double diffCash = m_cashDeposits - sumOfCash;
    double diffChecks = m_checksDeposits - sumOfChecks;
    double diffVisa = m_visaDeposits - sumOfVisa;
    double diffOthers = m_othersDeposits - sumOfVir;
    
    //affichage
    const QString textOfAnalyse = "<html><body><font color = blue size = 3>"
          ""+trUtf8("Entre votre comptabilité et vos dépôts en banque, la différence totale est : ")+"<br/>"
          ""+trUtf8("Différence totale = ")+"</font><font color = red size = 3>"+QString::number(differenceTotal)+"</font><br/>"
          "<font color = blue size = 3>"+trUtf8("Différence espèces = ")+"</font>"+QString::number(diffCash)+"<br/>"
          "<font color = blue size = 3>"+trUtf8("Différence chèques = ")+"</font>"+QString::number(diffChecks)+"<br/>"
          "<font color = blue size = 3>"+trUtf8("Différence visa = ")+"</font>"+QString::number(diffVisa)+"<br/>"
          "<font color = blue size = 3>"+trUtf8("Différence virements = ")+"</font>"+QString::number(diffOthers)+"<br/>"
          "</body></html>";
    textEdit->setHtml(textOfAnalyse);    
    //insert into honoraire validation
    qDebug() << __FILE__ << QString::number(__LINE__) << " hashOfIdHonoNotValidated.size() =" << QString::number(hashOfIdHonoNotValidated.keys().size()) ;
    if (hashOfIdHonoNotValidated.keys().size()>1)
    {
    	QSqlQuery qHonoValidated(m_db);
        ValidDialog mess(this,m_dateBegin,m_dateEnd);
        if (mess.exec() == QDialog::Accepted)
        {
     	    QList<int> listOfReturns = mess.checksBoxesReturn();
     	    for (int i = 0; i < listOfReturns.size(); i += 1)
     	    {
     	    	QStringList listOfNotValPerType = hashOfIdHonoNotValidated.values(i);
     	    	for (int j = 0; j < listOfNotValPerType.size(); j += 1)
     	    	{
     	    		QString idHono = listOfNotValPerType[j];
     	    		const QString reqHonoValidated = QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'")
                                                    .arg("honoraires",
                                                         "valide",
                                                         "1",
                                                         "id_hono",
                                                         idHono);
                    if (!qHonoValidated.exec(reqHonoValidated))
                    {
         	        qWarning() << __FILE__ << QString::number(__LINE__) << qHonoValidated.lastError().text() ;
                        }
     	    	    }
     	        }
            }
         }
}



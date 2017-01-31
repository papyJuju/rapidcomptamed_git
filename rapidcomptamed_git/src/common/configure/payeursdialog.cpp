
#include "payeursdialog.h"
#include "vision.h"
#include "delegate.h"
#include "tokens.h"

#include <common/constants.h>
#include <common/tablesdiagrams.h>

#include <QDebug>

using namespace Common;
using namespace Constants;
using namespace PayeursDiagrams;
using namespace Tokens;

payeurs::payeurs(QWidget * parent)
{
    m_database	= QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    setupUi(this);
    setAttribute      ( Qt::WA_DeleteOnClose);
    QPoint parentPosition = parent->parentWidget()->pos();
    QPoint plusPoint(50,50);
    move(parentPosition+plusPoint);
    quitButton    ->setEnabled(true);
    connect(recordButton,  SIGNAL(pressed()),this,SLOT(enregistrepayeurs()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(modifyButton,SIGNAL(pressed()),this,SLOT(voir()));

}
payeurs::~payeurs(){}

void payeurs::enregistrepayeurs(){
  m_nompayeur 	               = nameDebtorEdit->text();
  m_adressepayeur              = textEdit->toPlainText();
  m_cdpostpayeur               = CPlineEdit->text();
  m_villepayeur                = townEdit->text();
  m_telpayeur 	               = phoneEdit->text();
  m_codeAMC                    = amcLineEdit->text();
  m_codeAMC = prepareInsertValuesInDiversBlob(m_codeAMC);
  m_nompayeur 	               . replace("'","''");
  m_adressepayeur              . replace("'","''");
  m_villepayeur                . replace("'","''");

  QSqlQuery query2(m_database);
  query2.exec("INSERT INTO payeurs (nom_payeur,adresse_payeur,ville_payeur,code_postal,telephone_payeur,divers) VALUES ("
		"'"+m_nompayeur+"',"
		"'"+m_adressepayeur+"',"
		"'"+m_villepayeur+"',"
		"'"+m_cdpostpayeur+"',"
		"'"+m_telpayeur+"',"
		"'"+m_codeAMC+"')");
		if(query2.numRowsAffected()== 1)
		{
			QMessageBox::information(0,tr("information"),trUtf8("Les informations\nont été intégrées."),QMessageBox::Ok);
			clearLineEdits();
			}
		else 
		{
			QMessageBox::warning(0,tr("attention"),trUtf8("Echec insertion !"),QMessageBox::Ok);
			}

}

void payeurs::clearLineEdits()
{
  nameDebtorEdit->setText("");
  textEdit->setPlainText("");
  CPlineEdit->setText("");
  townEdit->setText("");
  phoneEdit->setText("");
  amcLineEdit->setText("");
  nameDebtorEdit->setFocus();
}

void payeurs::voir(){
  QString         base         = "payeurs";
  QList<int>      listToHide         ;
                  listToHide        << ID_PRIMKEY << ID_PAYEURS ;
  vision         *v            = new vision(base , listToHide,DIVERS_PAYEURS,this);
                  v           -> show();
}

QString payeurs::prepareInsertValuesInDiversBlob(const QString & value)
{
    QString insertvalue;
    insertvalue = QString(DIVERSBLOBSBEGIN);
    insertvalue += QString(NUMERO_AMC_BEGIN)+value+QString(NUMERO_AMC_END);
    insertvalue += QString(DIVERSBLOBSEND);
    return insertvalue;
}


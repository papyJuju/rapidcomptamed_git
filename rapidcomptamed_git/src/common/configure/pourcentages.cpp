#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "pourcentages.h"
#include "../constants.h"
using namespace Common;
using namespace Constants;
pourcentage::pourcentage()
{
  setupUi(this);
  m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);

  connect(pushButton,SIGNAL(pressed()),this,SLOT(enregistrepourcentage()));
  connect(pushButton_2,SIGNAL(pressed()),this,SLOT(close()));
}

pourcentage::~pourcentage()
{

}

void pourcentage::enregistrepourcentage(){
  QString type 		= lineEdit->text();
  QString pourcentage 	= lineEdit_2->text();

  QSqlQuery query(m_db);
  query.exec("INSERT INTO pourcentages (type,pourcentage) "
	"VALUES("
		"'"+type+"',"
		"'"+pourcentage+"')");
		
		if(query.numRowsAffected()== 1){
			QMessageBox::information(0,tr("information"),trUtf8("Les informations\nont été intégrées."),QMessageBox::Ok);
			}
			else {
			QMessageBox::warning(0,tr("attention"),trUtf8("Echec insertion !"),QMessageBox::Ok);
			}
}

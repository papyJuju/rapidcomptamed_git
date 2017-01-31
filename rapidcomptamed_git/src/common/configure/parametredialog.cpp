#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "parametredialog.h"
#include "../constants.h"
#include "../settings.h"
#include "../icore.h"
using namespace Common;
using namespace Constants;
parametredialog::parametredialog()
{
 setupUi(this);
 setAttribute(Qt::WA_DeleteOnClose);
 QSqlDatabase accountancyDb 	 = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);

 QString utilpref        = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
 QString idPref          = Common::ICore::instance()->settings()->value(S_ID_UTILISATEUR_CHOISI).toString();
 QString actepref        = Common::ICore::instance()->settings()->value(S_ACTE_COURANT).toString();
 QString valeuractepref  = Common::ICore::instance()->settings()->value(S_VALEUR_ACTE_COURANT).toString();
 QString comptePref      = Common::ICore::instance()->settings()->value(S_COMPTE_PREFERENTIEL).toString();

 lineEdit_2->setText(actepref);
 lineEdit_3->setText(valeuractepref);
 /*if(CreateConnexion2(motdepassechoix) == false){
	QMessageBox msgdb;
	msgdb.setWindowTitle(QObject::trUtf8("Erreur ouverture de base 33"));
	msgdb.setText(QObject::trUtf8("Impossible de se connecter à la base "));
	msgdb.setIcon(QMessageBox::Critical);
	msgdb.exec();
	if(QMessageBox::Ok){
	emit close();
	}
 }*/
 QStringList listecombo;
 listecombo << utilpref+"="+idPref;
 comboBox->setEditable(1);
 comboBox->setInsertPolicy(QComboBox::NoInsert);
 QSqlQuery query(accountancyDb);
           query.exec("SELECT login,id_usr FROM utilisateurs");
   while(query.next()){
       QString ligne1 = query.value(0).toString();
       QString ligne2 = query.value(1).toString();
       QString ligne1_2 = ligne1+"="+ligne2;
       listecombo << ligne1_2;
   }
 if(listecombo.size() < 1){
     QMessageBox::warning(0,trUtf8("Impossible"),trUtf8("Vous ne pouvez pas choisir d'utilisateur préférentiel,\n"
                                                        "puisque vous n'avez pas configuré vos utilisateurs de la comptabilité."),
                                                        QMessageBox::Ok);
     emit close();
 }
 //listecombo . sort();
 listecombo.removeDuplicates();
 comboBox->addItems(listecombo);

 comboBox_2 -> setEditable(1);
 comboBox_2 -> setInsertPolicy(QComboBox::NoInsert);
 QString reqComptes_bancaires = "SELECT libelle, id_compte FROM comptes_bancaires";
 QStringList listComptes_bancaires;
 QSqlQuery qComptes_bancaires(accountancyDb);
           qComptes_bancaires.exec(reqComptes_bancaires);
           while(qComptes_bancaires.next()){
               QString str = qComptes_bancaires.value(0).toString();
               qDebug() << "str comptes ="+str;
               listComptes_bancaires << str;
           }
  if((listComptes_bancaires.size() == 0 ) == false){
      listComptes_bancaires.prepend(comptePref);
  }
  else{
      listComptes_bancaires << comptePref;
  }
  comboBox_2 -> addItems(listComptes_bancaires);

 connect(pushButton,SIGNAL(pressed()),this,SLOT(fonctionparametre()));
 connect(pushButton_2,SIGNAL(pressed()),this,SLOT(close()));
}

parametredialog::~parametredialog(){}

void parametredialog::fonctionparametre(){

  QString utilisateur = comboBox->lineEdit()->text();
  QString actePref = lineEdit_2->text();
  QString valeurActePref = lineEdit_3->text();
  QString comptePref = comboBox_2->currentText();
  QString utilPref = utilisateur.split("=")[0];
  QString idUsr = utilisateur.split("=")[1];
  //changement dans config.ini
  Common::ICore::instance()->settings()->setValue(S_UTILISATEUR_CHOISI,utilPref);
  Common::ICore::instance()->settings()->setValue(S_ID_UTILISATEUR_CHOISI,idUsr);
  Common::ICore::instance()->settings()->setValue(S_ACTE_COURANT,actePref);
  Common::ICore::instance()->settings()->setValue(S_VALEUR_ACTE_COURANT,valeurActePref);
  Common::ICore::instance()->settings()->setValue(S_COMPTE_PREFERENTIEL,comptePref);
  QMessageBox::information(0,trUtf8("Information"),trUtf8("Config.ini a été renseigné avec les nouvelles valeurs,\nelles "
                                                          "seront valables à la nouvelle ouverture de recettes.\nLa "
                                                          "fenêtre va fermer."),QMessageBox::Ok);
  emit close();

}

/*bool parametredialog::CreateConnexion2(QString)
{

QStringList liste;
QStringList hostuser;
QFile fichier(""+g_pathabsolu+"/comptabilite.ini");
if(fichier.open(QIODevice::ReadOnly | QIODevice::Text) == false){
QMessageBox::warning(0,"erreur",QObject::trUtf8("le fichier comptabilite.ini n'est pas accessible"),QMessageBox::Ok);}
QTextStream loire(&fichier);
while(!loire.atEnd()){
QString ligne = loire.readLine();
//QStringList listint = ligne.split("=");
hostuser << ""+ligne+"";
}
QString host = hostuser[6];
QStringList listehost = host.split("=");
host = listehost[1];
host.replace(" ","");
QString utilisateur = hostuser[4];
QStringList listeutil = utilisateur.split("=");
utilisateur = listeutil[1];
utilisateur.replace(" ","");

QString mapoule = motdepassechoix;
mapoule.replace(" ","");

if(mapoule == ""){

QSqlDatabase accountancyDb = QSqlDatabase :: addDatabase("QMYSQL");
accountancyDb.setHostName(""+host+"");
accountancyDb.setDatabaseName("comptabilite");
accountancyDb.setUserName(""+utilisateur+"");
accountancyDb.setPassword("");

if (!accountancyDb.open()) {
	QMessageBox::critical(0,"Erreur ouverture de base 117", accountancyDb.lastError().text());
	return false;
}

return true;}
else{
QStringList list;
QString lettre;
QString hellothatsme;
int chiffremot;
int chiffre;
QString mot;

chiffremot = mapoule.mid(4,1).toInt();
chiffre = mapoule.mid(3,1).toInt();
list << ""+mapoule.mid(10+chiffre,1)+"";
mot = list[0];

int j= 1;
for(int i=0; i< chiffremot-1 ;i++){
lettre = mapoule.mid(j*3+10+chiffre,1);
list  << ""+lettre+"";
j++;
}
QString motus;
int k = 1;
for(int i=0; i< chiffremot-1 ;i++){
motus += list[k];
k++;
}
hellothatsme = mot+motus;

QString motdepasse = mapoule;//hellothatsme;
motdepasse = motdepasse.replace(QRegExp("\\n"),"");
QSqlDatabase accountancyDb = QSqlDatabase :: addDatabase("QMYSQL");
accountancyDb.setHostName(""+host+"");
accountancyDb.setDatabaseName("comptabilite");
accountancyDb.setUserName(""+utilisateur+"");
accountancyDb.setPassword(""+motdepasse+"");

if (!accountancyDb.open()) {
	QMessageBox::critical(0,"Erreur ouverture de base 158", accountancyDb.lastError().text());
	return false;
}

return true;
}
}*/


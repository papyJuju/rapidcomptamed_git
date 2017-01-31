#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "creationtable.h"
#include "./configure/utilisateur.h"
#include "constants.h"
#include "icore.h"
#include "settings.h"
#include "./configure/mdp.h"
using namespace Common;
using namespace Constants;

bool createBaseAccountancy(){
  if(createConnexion3() == true && createbase() == true){
       createtable_actes_disponibles();
       createtable_ccam();
       createtable_comptes_bancaires();
       createtable_depots();
       createtable_honoraires();
       createtable_immobilisations();
       createtable_mouvements();
       createtable_mouvements_disponibles();
       createtable_paiements();
       createtable_payeurs();
       createtable_sites();
       createtable_utilisateurs();
       createtable_z_version();
       createtable_pourcentage();
       createtable_seances();
       createTable_indemnites_deplacement();
       createTable_thesaurus();
       return true;
  }
  else
     return false;
}
bool createConnexion3(){
     if(!Common::ICore::instance()->settings()->writeAllDefaults()){
         
         return false;
     }
     mdp * databaseData = new mdp;
         if(databaseData->exec() == QDialog::Accepted){
                 Common::ICore::instance()->settings()->setValue(Constants::S_COMPTA_DB_LOGIN,databaseData->mdpLogin());
                 Common::ICore::instance()->settings()->setValue(Constants::S_COMPTA_DB_PASSWORD,databaseData->mdpPassword());
                 Common::ICore::instance()->settings()->setValue(Constants::S_COMPTA_DB_HOST,databaseData->mdpHost());
    
         }
     QSqlDatabase db = QSqlDatabase :: addDatabase("QMYSQL","trois");
     db.setHostName(Common::ICore::instance()->settings()->value(S_COMPTA_DB_HOST).toString());
     db.setDatabaseName("mysql");
     db.setUserName(Common::ICore::instance()->settings()->value(S_COMPTA_DB_LOGIN).toString());
     db.setPassword(Common::ICore::instance()->settings()->value(S_COMPTA_DB_PASSWORD).toString());
     if(!db.open()){
     qDebug() << "la base trois ne peut être ouverte.";
     return false;}
  return true;
}
bool createbase(){
    QSqlDatabase db = QSqlDatabase::database("trois");
    QSqlQuery query(db);
    return query.exec("CREATE DATABASE comptabilite");
}

void createtable_actes_disponibles(){
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if(!db.isValid())
	{
        QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
          +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
  QString requete1;
  if (db.driverName() == QLatin1String(SQLITE)) {
  requete1 =  "CREATE TABLE IF NOT EXISTS	actes_disponibles ("
		"id_acte_dispo   integer primary key," 	 
		"nom_acte 	varchar(50) 		NOT NULL  			,"
		"desc_acte 	varchar(200) 	 	NOT NULL  			,"
		"type 		varchar(100) 	 	NOT NULL 			,"
		"montant_total 	double 						NOT NULL 			,"
		"montant_tiers 	double 						NOT NULL 			,"
		"date_effet	date						NOT NULL			);";
  }
  if (db.driverName() == QLatin1String(MYSQL)) {
  requete1 = "CREATE TABLE IF NOT EXISTS	actes_disponibles ("
		"id_acte_dispo  int(10)  	UNSIGNED  	       		NOT NULL  	 auto_increment ," 	 
		"nom_acte 	varchar(50) 	COLLATE utf8_unicode_ci 	NOT NULL  			,"
		"desc_acte 	varchar(200) 	COLLATE utf8_unicode_ci 	NOT NULL  			,"
		"type 		varchar(100) 	COLLATE utf8_unicode_ci 	NOT NULL 			,"
		"montant_total 	double 						NOT NULL 			,"
		"montant_tiers 	double 						NOT NULL 			,"
		"date_effet	date						NOT NULL			,"
		"PRIMARY KEY(id_acte_dispo));";
		}


  QSqlQuery query1(db);
  if(!query1.exec(requete1)){
      QMessageBox::warning(0,QObject::trUtf8("Erreur"),QObject::trUtf8("Problème de création de actes_disponibles")+
                                             "\n"+QString(__FILE__)+"\n"+QString::number(__LINE__),QMessageBox::Ok);
  }
  if(db.driverName() == QLatin1String(MYSQL)){
  //-------------------------remplissage par le dump-----------------------------
  QStringList listeActes;
  QString strList        = "";
  QString pathDumpActesDispo = Common::ICore::instance()->settings()->applicationResourcesPath()
    +"/dump/Dump_ActesDisponible.sql";
  QFile fileActesIni(pathDumpActesDispo);
    if(!fileActesIni.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,QObject::tr("Erreur"),
        QObject::trUtf8("Dump_ActesDisponible.sql non trouvé\npour : ")+pathDumpActesDispo+"\n"
          +QString(__FILE__),QMessageBox::Ok);
        }
    QTextStream streamActes(&fileActesIni);
    while(!streamActes.atEnd()){
        QString ligneActes = streamActes.readLine();
        strList           += ligneActes;
            if (ligneActes.endsWith(";") && (ligneActes.endsWith("*//;")) == false ){
                listeActes       << strList;
                strList          = "";
                }
        }
    qDebug() << "avant for";
    qDebug() << "somme listeActes ="+QString::number(listeActes.size());
    for(int i = 0; i < listeActes.size() ; i++){
        QSqlQuery query(db);
        query.exec(listeActes[i]);
    }
    }
    else if(db.driverName() == QLatin1String(SQLITE)) {
        QStringList listFields;
                    listFields << "id_acte_dispo" 
                                << "nom_acte" 
                                << "desc_acte" 
                                << "type" 
                                << "montant_total"  
                                << "montant_tiers" 
                                << "date_effet";
                                
        QString table = "actes_disponibles";
        QString csvFile =Common::ICore::instance()->settings()->applicationResourcesPath()
          +"/csv_files/"+table+".csv";
        if(!cvsParser(listFields,table, csvFile)){
            QMessageBox::warning(0,QObject::trUtf8("Erreur"),
              QObject::trUtf8("Pas de remplissage de table ")+table+
                                                    "\n"+QString(__FILE__)+"\n"+QString::number(__LINE__),
                                                    QMessageBox::Ok);
        }
    }
}

void createtable_ccam()
{
    QString req ;
    req =  "CREATE TABLE IF NOT EXISTS ccam ("
                    "id_ccam  int(20) UNSIGNED  NOT NULL  auto_increment ,"
                    "code varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "abstract varchar(200) COLLATE utf8_unicode_ci NULL,"
                    "type varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "amount double NOT NULL,"
                    "reimbursment double NULL,"
                    "date date NOT NULL,"
                    "others blob NULL,"
                    "PRIMARY KEY(id_ccam));";
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if(!db.open())
	{
         QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
            +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	    }
	QSqlQuery queryCcam(db);
	if (!queryCcam.exec(req))
	{
	      qWarning() << __FILE__ << QString::number(__LINE__) << queryCcam.lastError().text() ;
	    }
    
}
void createtable_comptes_bancaires(){
QString requete2;
requete2 = "CREATE TABLE IF NOT EXISTS	comptes_bancaires ("
		"id_compte	      int(10)		UNSIGNED 			NOT NULL	auto_increment 	,"	
		"id_usr 	      int(10) 		UNSIGNED 			NOT NULL 			,"
		"libelle 	      varchar(150) 	COLLATE utf8_unicode_ci  	NULL 				,"
		"titulaire 	      varchar(100) 	COLLATE utf8_unicode_ci 	NULL 				,"
		"nom_banque 	      varchar(50) 	COLLATE utf8_unicode_ci 	NULL 				,"
		"rib_code_banque      varchar(5) 	COLLATE utf8_unicode_ci 	NULL 				,"
		"rib_code_guichet     varchar(5) 	COLLATE utf8_unicode_ci  	NULL 				,"
		"rib_numcompte 	      varchar(11) 	COLLATE utf8_unicode_ci 	NULL 				,"	
		"rib_cle 	      varchar(2) 	COLLATE utf8_unicode_ci 	NULL 				,"	
		"solde_initial 	      double 						NULL				,"	
		"remarque 	      blob 		 	 			NULL 				,"
		"PRIMARY KEY(id_compte));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
         QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
            +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

  QSqlQuery query2(db);
  if(!query2.exec(requete2)){
      qWarning() << query2.lastError().text() << __FILE__ << QString::number(__LINE__);
  }
}
void createtable_depots(){
  QString requete3;
  requete3 = "CREATE TABLE IF NOT EXISTS	depots 	  ("
		"id_depot  	int(11)  	 	  				NOT NULL  	 auto_increment  ," 
		"id_usr 	int(11) 						NOT NULL 			 ," 
		"id_compte 	int(10) 		UNSIGNED 			NOT NULL 			," 
		"type_depot 	varchar(10) 		COLLATE utf8_unicode_ci		NULL 				," 
		"date 		date 							NOT NULL  			," 
		"periode_deb 	date 							NULL 	         		," 
		"periode_fin 	date 							NULL  			        ," 
		"blob_depot 	blob 			 				NULL 			        ," 
		"remarque 	text                    COLLATE utf8_unicode_ci         NULL 				," 
		"valide 	tinyint(4) 						NULL 			        ," 
		"PRIMARY KEY(id_depot));";
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  if(!db.open())
	{
  QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

  QSqlQuery query3(db);
  query3.exec(requete3);
}
void createtable_honoraires(){
QString requete4;
requete4 = "CREATE TABLE IF NOT EXISTS	honoraires	("		
		"id_hono  	int(11)  	 	UNSIGNED  			NOT NULL	 auto_increment  ,"	  
		"id_usr 	int(11) 						NOT NULL			 ,"
		"id_drtux_usr 	int(11) 						NULL 				 ,"
		"patient 	varchar(75) 		COLLATE utf8_unicode_ci		NOT NULL 			,"
		"id_site        varchar(10)    	        COLLATE utf8_unicode_ci         NULL                            ,"
		"id_payeurs     varchar(36)             COLLATE utf8_unicode_ci         NULL                            ,"
		"GUID 		varchar(36) 		COLLATE utf8_unicode_ci		NULL 				,"
		"praticien 	varchar(75) 		COLLATE utf8_unicode_ci 	NOT NULL			,"
		"date 		date 							NOT NULL 			,"
		"acte 		blob 		                        		NULL 				,"
		"acteclair      varchar(12)             COLLATE utf8_unicode_ci         NULL                            ,"
		"remarque 	blob 			 	 			NULL 				,"
		"esp 		double 							NOT NULL 			,"
		"chq 		double 							NOT NULL 			,"
		"cb 		double 							NOT NULL			,"
		"daf 		double 							NOT NULL 			,"
		"autre 		double 							NOT NULL 			,"
		"du 		double 							NOT NULL 			,"
		"du_par		varchar(100) 		COLLATE utf8_unicode_ci		NULL 			        ,"
		"valide 	tinyint(1) 						NULL				,"
		"tracabilite 	blob 			 				NULL				,"
		"PRIMARY KEY(id_hono));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query4(db);
query4.exec(requete4);
}
void createtable_immobilisations(){
QString requete5;
requete5 = "CREATE TABLE IF NOT EXISTS	immobilisations ("		
		"id_immob  	int(10)  	 	UNSIGNED  			NOT NULL  	 auto_increment  ,"	  
		"id_usr 	int(10) 		UNSIGNED 			NOT NULL 			,"
		"id_compte 	int(10) 		UNSIGNED 			NOT NULL 			,"
		"libelle 	varchar(150) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"date_service 	date 							NOT NULL			,"
		"duree 		int(11) 						NOT NULL 			,"
		"mode 		tinyint(1) 						NOT NULL			,"
		"valeur 	bigint(20) 		UNSIGNED 			NOT NULL			,"
		"montant_tva 	double 							NULL 				,"
		"valeur_residuelle bigint(20) 						NOT NULL			,"
		"resultat 	blob 			 				NOT NULL			,"
		"mouvements 	blob 			 	 			NULL 				,"
		"remarque 	text 			COLLATE utf8_unicode_ci	        NULL 				,"
		"tracabilite 	blob 			 	 			NULL				,"
		"PRIMARY KEY(id_immob));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query5(db);
query5.exec(requete5);
}
void createtable_mouvements(){
QString requete6;
requete6 = "CREATE TABLE IF NOT EXISTS	mouvements	("
		"id_mouvement  int(10) 			UNSIGNED   			NOT NULL    	 auto_increment ,"
		"id_mvt_dispo  int(11)             					NOT NULL    			,"
 		"id_usr        int(10) 			UNSIGNED      			NOT NULL                    	,"
		" id_compte    int(10) 			UNSIGNED    			NOT NULL 			,"
		" type         tinyint(3) 		UNSIGNED  			NOT NULL			,"
		" libelle      varchar(100)        	COLLATE utf8_unicode_ci 	NOT NULL			,"
		" date         date                					NOT NULL			,"
		" date_valeur  date                					NOT NULL 			,"
		" montant      double UNSIGNED     					NOT NULL			,"
 		"remarque      blob                 					NULL  				,"
		" valide       tinyint(4)          					NOT NULL			,"
		" tracabilite  blob               					NULL   				,"
		" validation   tinyint(1)         					NULL				,"
		" detail       varchar(100)        	COLLATE utf8_unicode_ci 	NULL  				,"
		"PRIMARY KEY(id_mouvement));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query6(db);
query6.exec(requete6);
    //-------------------------remplissage par le dump-----------------------------

  QStringList listeActes;
  QString strList        = "";
  QFile fileActesIni(Common::ICore::instance()->settings()->applicationResourcesPath()+"/dump/dump_mouvements_disponibles.sql");
    if(!fileActesIni.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("dump_mouvements_disponibles.sql non trouvé."),QMessageBox::Ok);
        }
    QTextStream streamActes(&fileActesIni);
    while(!streamActes.atEnd()){
        QString ligneActes = streamActes.readLine();
        strList           += ligneActes;
            if (ligneActes.endsWith(";") && (ligneActes.endsWith("*//;")) == false ){
                listeActes       << strList;
                strList          = "";
                }
        }

    for(int i = 0; i < listeActes.size() ; i++){
        QSqlQuery query(db);
        query.exec(listeActes[i]);
    }
}
void createtable_mouvements_disponibles(){
QString requete7;
requete7 = "CREATE TABLE IF NOT EXISTS	mouvements_disponibles ("
		"id_mvt_dispo  	int(10)  	 	UNSIGNED  			NOT NULL  	 auto_increment  ,"	  
		"id_mvt_parent 	int(11) 					 	NULL 				,"
		"type 		tinyint(3) 		UNSIGNED 			NOT NULL  			,"
		"libelle 	varchar(100) 		COLLATE utf8_unicode_ci 	NOT NULL   			,"
		"remarque 	blob 							NULL 				,"
		"PRIMARY KEY(id_mvt_dispo));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query7(db);
query7.exec(requete7);
}

void createtable_paiements(){
  /*QSqlDatabase db        = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
     if(!db.open()){
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
     }
  QStringList listePaiements;
  QString strList        = "";
  QFile filePaiementsIni(Common::ICore::instance()->settings()->applicationResourcesPath()+"/dump/dump_Paiements.sql");
    if(!filePaiementsIni.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("dump_paiements.sql non trouvé."),QMessageBox::Ok);
    }
    QTextStream streamPaiements(&filePaiementsIni);
    while(!streamPaiements.atEnd()){
        QString lignePaiements = streamPaiements.readLine();
        strList               += lignePaiements;*/
           // if (lignePaiements.endsWith(";") && (lignePaiements.endsWith("*//;")) == false ){
        /*       listePaiements       << strList;
                strList          = "";
            }
        }
    for(int i = 0; i < listePaiements.size() ; i++){
        QSqlQuery query(db);
        query.exec(listePaiements[i]);
    }*/
    QString requete7;
requete7 = "CREATE TABLE IF NOT EXISTS paiements ("
                    "id_paiement   int(11)  	 	UNSIGNED  NOT NULL  auto_increment ,"
                    "id_hono  	   int(11)  	 	UNSIGNED  NOT NULL ,"
                    "date  	   date  	 	  	  NOT NULL,"
                    "id_payeurs    bigint(20)  	 	  	  NOT NULL,"
                    "acte  	   blob  	 	          NULL  ,"
                    "esp  	   double  	 	  	  NOT  NULL,"
                    "chq  	   double  	 	  	  NOT  NULL,"
                    "cb  	   double  	 	  	  NOT  NULL,"
                    "daf  	   double  	 	  	  NOT  NULL,"
                    "autre  	   double  	 	  	  NOT  NULL,"
                    "du 	   double  	 	  	  NOT  NULL,"
                    "valide  	   tinyint(1)  	 	  	  NULL ,"
                    "PRIMARY KEY(id_paiement));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query7(db);
query7.exec(requete7);
}

void createtable_payeurs(){
QString requete8;
requete8 = "CREATE TABLE IF NOT EXISTS	payeurs	("
		"ID_Primkey  	bigint(20)  	 	  				NOT NULL  	 auto_increment  ,"	  
		"id_payeurs 	bigint(20) 						NOT NULL 			,"
		"nom_payeur 	varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"adresse_payeur varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"ville_payeur 	varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"code_postal 	varchar(5) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"telephone_payeur varchar(100) 		COLLATE utf8_unicode_ci 	NULL				,"
		"PRIMARY KEY(ID_Primkey));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query8(db);
query8.exec(requete8);
}
void createtable_sites(){
QString requete9;
requete9 = "CREATE TABLE IF NOT EXISTS	sites ("
		"ID_Primkey  	bigint(10)  	 	  				NOT NULL 	 auto_increment  ,"	  
		"id_site 	varchar(10) 		COLLATE utf8_unicode_ci 	NOT NULL			,"
		"site 		varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"adresse_site 	varchar(100) 		COLLATE utf8_unicode_ci  	NULL 				,"
		"cp_site 	varchar(5) 		COLLATE utf8_unicode_ci		NULL 				,"
		"ville_site 	varchar(100) 		COLLATE utf8_unicode_ci  	NULL 				,"
		"telsite 	varchar(50) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"faxsite 	varchar(50) 		COLLATE utf8_unicode_ci 	NULL 				,"
		"mailsite 	varchar(100) 		COLLATE utf8_unicode_ci 	NULL				,"
		"PRIMARY KEY(ID_Primkey));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query9(db);
query9.exec(requete9);
}
void createtable_utilisateurs(){
QString requete10;
requete10 = "CREATE TABLE IF NOT EXISTS	utilisateurs ("
		"id_usr  	int(10)  	 	UNSIGNED  			NOT NULL  	 auto_increment ," 	  
		"nom_usr 	varchar(150) 		COLLATE utf8_unicode_ci		NULL 				,"
		"login 	varchar(15) 			COLLATE utf8_unicode_ci		NOT NULL			,"
		"password 	varchar(50) 		COLLATE utf8_unicode_ci		NOT NULL			,"
		"id_drtux_usr 	int(11) 			 			NULL 				,"
		"PRIMARY KEY(id_usr));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

  QSqlQuery query10(db);
  if(query10.exec(requete10)) lancerutilisateurconfig();
}

void createtable_z_version(){
QString requete11;
requete11 = "CREATE TABLE IF NOT EXISTS	z_version  ("
		"infos_version  blob  	 		  		  	NULL				);";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

QSqlQuery query11(db);
query11.exec(requete11);
		
}

void createtable_pourcentage(){
QString requete12 = "CREATE TABLE IF NOT EXISTS	pourcentages ("
			"id_pourcent	int(10)		UNSIGNED			NOT NULL	auto_increment ,"
			"type		varchar(100)	COLLATE utf8_unicode_ci		NULL				,"
			"pourcentage	varchar(6)	COLLATE utf8_unicode_ci		NOT NULL			,"
			"PRIMARY KEY(id_pourcent));";
QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

			
QSqlQuery query12(db);
query12.exec(requete12);

  QStringList listePourcent;
  QString strList        = "";
  
  QFile fileActesIni(Common::ICore::instance()->settings()->applicationResourcesPath()+"/dump/dump_pourcentages.sql");
    if(!fileActesIni.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("dump_pourcentages.sql non trouvé."),QMessageBox::Ok);
        }
    QTextStream streamActes(&fileActesIni);
    while(!streamActes.atEnd()){
        QString ligneActes = streamActes.readLine();
        strList           += ligneActes;
            if (ligneActes.endsWith(";") && (ligneActes.endsWith("*//;")) == false ){
                listePourcent       << strList;
                strList          = "";
                }
        }
    qDebug() << "avant for";
    qDebug() << "somme listeActes ="+QString::number(listePourcent.size());
    for(int i = 0; i < listePourcent.size() ; i++){
        QSqlQuery query(db);
        query.exec(listePourcent[i]);
    }
}

void createtable_seances(){
  QString requete13 = "CREATE TABLE IF NOT EXISTS	seances  ("
			"id_paiement    int(11)			unsigned			NOT NULL     	auto_increment ,"
			"id_hono  	int(11)  	 	UNSIGNED  			NOT NULL	 		,"
			"date 		date 							NOT NULL 			,"
			"id_payeurs 	bigint(20) 						NOT NULL 			,"
		        "id_usr         varchar(10)                                             NULL                            ,"
                        "id_site        varchar(10)                                             NULL                            ,"
			"acte 		text                    COLLATE  utf8_unicode_ci	NULL 				,"
			"esp 		double 							NOT NULL 			,"
			"chq 		double 							NOT NULL 			,"
			"cb 		double 							NOT NULL			,"
			"daf 		double 							NOT NULL 			,"
			"autre 		double 							NOT NULL 			,"
			"du 		double 							NOT NULL 			,"
			"valide 	tinyint(1) 						NULL				,"
			"PRIMARY KEY(id_paiement));";
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}

  QSqlQuery query13(db);
  query13.exec(requete13);
}

void createtable_rapprochement_bancaire(){
  QString req14;
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
  if(db.driverName() == MYSQL){	
      req14 = "CREATE TABLE IF NOT EXISTS rapprochement_bancaire ("
                    "id_rap_bank   int(11) UNSIGNED  NOT NULL  auto_increment ,"
                    "id_compte     int(11) UNSIGNED  null,"
                    "solde         double NULL,"
                    "PRIMARY KEY(id_rap_bank));";
  }
  if(db.driverName() == SQLITE){
      req14 = "CREATE TABLE IF NOT EXISTS rapprochement_bancaire ("
                    "id_rap_bank INTEGER PRIMARY KEY,"
                    "id_compte     int(11)   null,"
                    "solde         double NULL);";
                    
  }                  
  QSqlQuery query14(db);
  query14.exec(req14);
}

void lancerutilisateurconfig(){
  QString str = "";
  utilisateur *machin = new utilisateur(0,str);
               machin-> show();
}

bool createTable_IndexName(){
  qDebug() << __FILE__ << QString::number(__LINE__) << "in createTable_IndexName()";
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  QString req;
  if (db.driverName() == QLatin1String(SQLITE)) {
          req = "CREATE TABLE IF NOT EXISTS  nameindex ("
                "id_index         INTEGER PRIMARY KEY,"
                "name             varchar(50)                NULL,"
                "surname          varchar(50)               NULL,"
                "guid             varchar(6)                NOT NULL);";
  }
  else if(db.driverName() == QLatin1String(MYSQL)){
         req = "CREATE TABLE IF NOT EXISTS  nameindex ("
                "id_index         int(6)                 unsigned                    NOT NULL auto_increment,"
                "name             varchar(50)            COLLATE  utf8_unicode_ci    NULL,"
                "surname          varchar(50)            COLLATE  utf8_unicode_ci    NULL,"
                "guid             varchar(6)                                         NOT NULL,"
                "PRIMARY KEY(id_index));";
  }
  
  if(!db.open() || !db.isOpen())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
  QSqlQuery queryCreateIndex(db);
            
            if(queryCreateIndex.exec(req) == true){
              QMessageBox::information(0,QObject::tr("Information"),QObject::trUtf8("Index of names has been created."),QMessageBox::Ok);
              return true;
            }
   QMessageBox::critical(0,QObject::tr("Critical"),QObject::trUtf8("Create nameindex, error =")+db.lastError().text(),QMessageBox::Ok);
   return false;        
}

bool cvsParser(QStringList & listFields,QString & table, QString & csvFile){
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
         QFile file(csvFile);
         if(!file.open(QIODevice::ReadOnly)){
             QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("Fichier.csv non trouvé."),QMessageBox::Ok);
         }
         QTextStream stream(&file);
                    stream.setCodec("UTF-8");//ISO 8859-1
         while(!stream.atEnd()){
             QString ligne = stream.readLine();
             if(ligne.isEmpty()){
                 break;
             }
                     ligne.replace("\"","'");
                     ligne.remove(QRegExp("[\n]"));
                     ligne.remove(QRegExp("[\r\n]"));
             QStringList listValues = ligne.split(";");
             QStringList fields;
             QStringList values;
             for(int a = 0 ; a < listValues.size() ; a++){
                 fields << listFields[a];
                 if(listValues[a] == ""){
                     listValues[a] = "''";
                 }
                 if(listValues[a].contains("Comptes d'attente")){
                              listValues[a].replace("Comptes d'attente","Comptes d''attente");
                 }
                 values << listValues[a];
             }
             QString fieldsStr = fields.join(",");
             QString valuesStr = values.join(",");
             QString insertStr = "INSERT INTO "+table+" ("+fieldsStr+") "
                                 "VALUES("+valuesStr+")";
             qDebug() << "INSERT INTO "+table+" ("+fieldsStr+") VALUES("+valuesStr+")";
             QSqlQuery insertQuery(db);
                       if(!insertQuery.exec(insertStr)){
                           qCritical() << "Erreur feedAccountancyDatabaseWithDefault pour table = "
                           +table+":"+insertQuery.lastError().text();
                           return false;
                       }
         }
     return true;
}
bool createTable_thesaurus()
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString req;
     if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
	+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
     if(db.driverName() == MYSQL){
         req = "CREATE TABLE IF NOT EXISTS	thesaurus ("
                    "id_thesaurus  int(10)       UNSIGNED NOT NULL auto_increment,"
                    "sequence 	    varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(100)  COLLATE utf8_unicode_ci NOT NULL,"
                    "PRIMARY KEY(id_thesaurus));";
                    }
     if(db.driverName() == SQLITE){
         req = "CREATE TABLE IF NOT EXISTS	thesaurus ("
                    "id_thesaurus  INTEGER PRIMARY KEY,"
                    "sequence 	    varchar(50)    NOT NULL,"
                    "type           varchar(100)   NOT NULL);";
     }
     qWarning() << __FILE__ << " " << QString::number(__LINE__) << " "<< req;
     QSqlQuery q(db);
     if(!q.exec(req)){
         QMessageBox::critical(0,QObject::tr("Critical"),q.lastError().text()+__FILE__+QString::number(__LINE__),
         QMessageBox::Ok);
         return false;
     }
     QStringList list;
                 list << "id_thesaurus"
                      << "sequence"
                      << "type";
     QString thesaurus = "thesaurus";
     create_defaults_in_tables(thesaurus,list);
     return true;        	
}

bool createTable_forfaits(){
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString req;
     if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
     if(db.driverName() == MYSQL){
         req = "CREATE TABLE IF NOT EXISTS	forfaits ("
                    "id_forfaits  int(10)       UNSIGNED NOT NULL auto_increment,"
                    "forfaits 	    varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "valeur 	    varchar(10)   COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(10)  COLLATE utf8_unicode_ci NOT NULL,"
                    "PRIMARY KEY(id_forfaits));";
                    }
     if(db.driverName() == SQLITE){
         req = "CREATE TABLE IF NOT EXISTS	forfaits ("
                    "id_forfaits  INTEGER PRIMARY KEY,"
                    "forfaits 	    varchar(50)    NOT NULL,"
                    "valeur 	    varchar(10)    NOT NULL,"
                    "type           varchar(10)    NOT NULL);";
     }

     QSqlQuery q(db);
     if(!q.exec(req)){
         QMessageBox::critical(0,QObject::tr("Critical"),q.lastError().text()+__FILE__
                               +QString::number(__LINE__),QMessageBox::Ok);
         return false;
     }
     QStringList list;
                 list << "id_forfaits"
                      << "forfaits"
                      << "valeur"
                      << "type";
     QString forfaits = "forfaits";               
     create_defaults_in_tables(forfaits,list);
     return true;  
}

bool createTable_informations(){
      QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString req;
     if (!db.open()) {
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
	  +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
     if(db.driverName() == MYSQL){
         req = "CREATE TABLE IF NOT EXISTS INFORMATIONS ("
                    "id_info int(6) UNSIGNED  NOT NULL  auto_increment ,"
                    "VERSION varchar(6) NULL,"
                    "PRIMARY KEY(id_info));";
                    }
     if(db.driverName() == SQLITE){
         req = "CREATE TABLE IF NOT EXISTS INFORMATIONS ("
                    "id_info integer primary key,"
                    "VERSION varchar(6) NULL);";
     }

     QSqlQuery q(db);
     if(!q.exec(req)){
         QMessageBox::critical(0,QObject::tr("Critical"),q.lastError().text()+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
         return false;
     }
     return true;
}

bool createTable_notes(){
      QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString req;
     if (!db.open()) {
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
	  +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
     if(db.driverName() == MYSQL){
         req = "CREATE TABLE IF NOT EXISTS notes ("
                    "id_notes int(6) UNSIGNED  NOT NULL  auto_increment ,"
                    "notes blob NULL,"
                    "PRIMARY KEY(id_notes));";
                    }
     if(db.driverName() == SQLITE){
         req = "CREATE TABLE IF NOT EXISTS notes ("
                    "id_notes integer primary key,"
                    "notes blob NULL);";
     }

     QSqlQuery q(db);
     if(!q.exec(req)){
         QMessageBox::critical(0,QObject::tr("Critical"),q.lastError().text()+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
         return false;
     }
     return true;
}

bool createTable_indemnites_deplacement(){
     QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString req;
     if(!db.open())
	{
	QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("La base comptabilite ne s'ouvre pas.")
	  +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
	}
     if(db.driverName() == MYSQL){
         req = "CREATE TABLE IF NOT EXISTS indemnites_deplacement ("
                    "id_IK int(6) UNSIGNED  NOT NULL  auto_increment ,"
                    "type varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "value double NOT  NULL,"
                    "abattement double NOT  NULL,"
                    "indemnites_forfait double NULL,"
                    "PRIMARY KEY(id_IK));";
                    }
     if(db.driverName() == SQLITE){
         req = "CREATE TABLE IF NOT EXISTS indemnites_deplacement ("
                    "id_IK integer primary key,"
                    "type varchar(50)  NULL,"
                    "value double NOT  NULL,"
                    "abattement double NOT  NULL,"
                    "indemnites_forfait double NULL);";
     }

     QSqlQuery q(db);
     if(!q.exec(req)){
         QMessageBox::critical(0,QObject::tr("Critical"),q.lastError().text()+__FILE__+QString::number(__LINE__),QMessageBox::Ok);
         return false;
     }
      QStringList list;
                 list << "id_IK"
                      << "type"
                      << "value"
                      << "abattement"
                      << "indemnites_forfait";
     QString indemnites = "indemnites_deplacement";               
     create_defaults_in_tables(indemnites,list);
     return true;
}

void create_defaults_in_tables(QString & table, QStringList & listFields){
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY); 
     //---------récupérer les champs---------------
  QString csvFile = Common::ICore::instance()->settings()->applicationResourcesPath()+"/csv_files/"+table+".csv";
         QFile file(csvFile);
         if(!file.open(QIODevice::ReadOnly)){
             QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("Fichier.csv non trouvé."),QMessageBox::Ok);
         }
         QTextStream stream(&file);
                    stream.setCodec("UTF-8");//ISO 8859-1
         while(!stream.atEnd()){
             QString ligne = stream.readLine();
             if(ligne.isEmpty()){
                 break;
             }
                     ligne.replace("\"","'");
                     ligne.remove(QRegExp("[\n]"));
                     ligne.remove(QRegExp("[\r\n]"));
             QStringList listValues = ligne.split(";");
             QStringList fields;
             QStringList values;
             for(int a = 0 ; a < listValues.size() ; a++){
                 fields << listFields[a];
                 if(listValues[a] == ""){
                     listValues[a] = "''";
                 }
                 if(listValues[a].contains("Comptes d'attente")){
                              listValues[a].replace("Comptes d'attente","Comptes d''attente");
                 }
                 values << listValues[a];
             }
             QString fieldsStr = fields.join(",");
             QString valuesStr = values.join(",");
             QString insertStr = "INSERT INTO "+table+" ("+fieldsStr+") "
                                 "VALUES("+valuesStr+")";
             qDebug() << "INSERT INTO "+table+" ("+fieldsStr+") VALUES("+valuesStr+")";
             QSqlQuery insertQuery(db);
                       if(!insertQuery.exec(insertStr)){
                           qCritical() << "Erreur feedAccountancyDatabaseWithDefault pour table = "
                           +table+":"+insertQuery.lastError().text();
                       }
             }
}

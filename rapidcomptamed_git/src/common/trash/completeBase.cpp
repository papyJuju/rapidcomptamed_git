#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "completeBase.h"
#include "creationtable.h"
#include "constants.h"
#include "settings.h"
#include "icore.h"
#include "commandlineparser.h"
#include "databasemanager.h"
using namespace Common;
using namespace Constants;

static inline QString dumpPath(){return Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump";}
static inline QString pixmapPath(){return Common::ICore::instance()->settings()->iconPath();}

void completeBaseCompta(QPushButton *button){
  int nbrOfTables = Common::ICore::instance()->settings()->value(S_COMPTA_NBR_OF_TABLES).toInt();
  if(Common::ICore::instance()->databaseManager()->isAnIndependantProgram()){
	    nbrOfTables++;
	}
  QSqlDatabase database = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  if(!database.isValid()){
	createbase();
	}
  QStringList listetablespresentes;
      QSqlQuery qListOfTables(database);
    QString base = QString(Constants::DB_ACCOUNTANCY);
    QString reqListOfTables;
    if(database.driverName() == "QMYSQL"){
        reqListOfTables = QString("show tables from %1;").arg(base);
    }
    if(database.driverName() == "QSQLITE"){
        reqListOfTables = QString("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << reqListOfTables;
    if(!qListOfTables.exec(reqListOfTables)){
        qWarning() << __FILE__ << " " << QString::number(__LINE__) << qListOfTables.lastError().text();
        }
    QString strListOfTables;
    while(qListOfTables.next()){
       QString s = qListOfTables.value(0).toString();
       listetablespresentes << s;
       qWarning() << __FILE__ << " " << QString::number(__LINE__) << s;
    }
QStringList listetables;
listetables 	<< "actes_disponibles"
        << "ccam"
		<< "comptes_bancaires"
		<< "depots"
		<< "honoraires"
		<< "immobilisations"
		<< "mouvements"
		<< "mouvements_disponibles"
		<< "paiements"
		<< "payeurs"
		<< "pourcentages"
		<< "seances"
		<< "sites"
		<< "utilisateurs"
		<< "rapprochement_bancaire"
		<< "z_version"
		<< "forfaits"
		<< "INFORMATIONS"
		<< "nameindex"
		<< "thesaurus"
		<< "indemnites_deplacement"
		<< "notes";

	for(int i = 0 ; i < listetables.size() ; i++){

		if(!listetablespresentes.contains(listetables[i],Qt::CaseInsensitive))
				{
 		qDebug() << __FILE__ << QString::number(__LINE__) 
 		<< "listetables[i]= "<< listetables[i];
		if   	(listetables[i] == "actes_disponibles")
			createtable_actes_disponibles();
		else if (listetables[i] == "ccam")
		    createtable_ccam();
		else if (listetables[i] == "comptes_bancaires")
			createtable_comptes_bancaires();
		else if (listetables[i] == "depots")
			createtable_depots();
		else if	(listetables[i] == "honoraires")
			createtable_honoraires();
		else if	(listetables[i] == "immobilisations")
			createtable_immobilisations();
		else if	(listetables[i] == "mouvements")
			createtable_mouvements();
		else if	(listetables[i] == "mouvements_disponibles")
			createtable_mouvements_disponibles();
	        else if (listetables[i] == "paiements")
	                createtable_paiements();
		else if	(listetables[i] == "payeurs")
			createtable_payeurs();
		else if	(listetables[i] == "sites")
			createtable_sites();
		else if	(listetables[i] == "utilisateurs")
			createtable_utilisateurs();
		else if	(listetables[i] == "z_version")
			createtable_z_version();
		else if (listetables[i] == "pourcentages")
			createtable_pourcentage();
		else if (listetables[i] == "seances")
			createtable_seances();
	        else if (listetables[i] == "rapprochement_bancaire")
			createtable_rapprochement_bancaire();
		else if (listetables[i] == "thesaurus"){
		      qDebug() << "to thesaurus "<< __FILE__ << QString::number(__LINE__);
		        createTable_thesaurus();}
		else if (listetables[i] == "forfaits")
		        createTable_forfaits();
        else if (listetables[i] == "INFORMATIONS")
		        createTable_informations();
        else if (listetables[i] == "nameindex")
		        createTable_IndexName();
		else if (listetables[i] == "indemnites_deplacement")
		        createTable_indemnites_deplacement();
		else if (listetables[i] == "notes")
		        createTable_notes();
		}
	}//boucle for
        QSqlQuery queryImmob(database);
                  queryImmob.exec(" ALTER TABLE immobilisations CHANGE remarque remarque TEXT CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL");
	QStringList nombredechamps;
	    QSqlQuery   query(database);
	    query.exec("SELECT * FROM actes_disponibles WHERE id_acte_dispo = 1");
	    QSqlRecord record = query.record();
	    int count         = record.count();
	    if(count < 7){
	        QString alter = "ALTER TABLE actes_disponibles ADD date_effet date NULL AFTER montant_tiers;";
	        QSqlQuery queryalter(database);
	        queryalter.exec(alter);
	        }
	QStringList NombreChampsHonoraires;
	    QSqlQuery   query2(database);
	    query2.exec("SELECT * FROM honoraires ");
	    QSqlRecord record2 = query2.record();
	    int count2         = record2.count();
	    if(count2 < 21){
	        QString alterhono = "ALTER TABLE honoraires ADD id_site varchar(10)  COLLATE utf8_unicode_ci NULL AFTER patient;";

	        QSqlQuery queryalterhono(database);
	        queryalterhono.exec(alterhono);
	        QString alterhono2= "ALTER TABLE honoraires ADD id_payeurs varchar(36) COLLATE utf8_unicode_ci NULL AFTER id_site;";
	        QSqlQuery queryalterhono2(database);
	        queryalterhono2.exec(alterhono2);
	        QString alterhono3= "ALTER TABLE honoraires ADD du_par	varchar(100) COLLATE utf8_unicode_ci NULL AFTER du;";
	        QSqlQuery queryalterhono3(database);
	        queryalterhono3.exec(alterhono3);
	        QString alterhono4 = "ALTER TABLE honoraires ADD acteclair varchar(12) COLLATE utf8_unicode_ci NULL AFTER acte;";
                QSqlQuery queryalterhono4(database);
	        queryalterhono4.exec(alterhono4);
	        }
	QStringList nbreChampsSeances;
        QSqlQuery queryChampsSeances(database);
                  queryChampsSeances.exec("SELECT * FROM seances WHERE id_paiement = 1");
            QSqlRecord recordSeances = queryChampsSeances.record();
            int countSeances = recordSeances.count();
            if(countSeances < 13){
               QSqlQuery queryAlterSeances1(database);
                         queryAlterSeances1.exec("ALTER TABLE seances ADD id_usr varchar(10) NULL AFTER id_payeurs;");
               QSqlQuery queryAlterSeances2(database);
                         queryAlterSeances2.exec("ALTER TABLE seances ADD id_site varchar(10) NULL AFTER id_usr;");
            }
        QStringList NombreListeMouvementsDisponibles;
        QSqlQuery  queryMvtsDisp(database);
                   queryMvtsDisp.exec("SELECT id_mvt_dispo FROM mouvements_disponibles");
                   while(queryMvtsDisp.next()){
                       QString str                        = queryMvtsDisp.value(0).toString();
                       NombreListeMouvementsDisponibles  <<  str;
                   }
                   if(NombreListeMouvementsDisponibles.size() < 40){
                         QStringList listeMD;
                         QString strList        = "";
                         QFile fileActesIniMD(dumpPath()+"/dump_mouvements_disponibles.sql");
                          if(!fileActesIniMD.open(QIODevice::ReadOnly)){
                                  QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("dump_mouvements_disponibles.sql "
                                                                                               "non trouvé."),QMessageBox::Ok);
                           }
                          QTextStream streamActesMD(&fileActesIniMD);
                            while(!streamActesMD.atEnd()){
                                streamActesMD.setCodec("ISO 8859-1");
                                QString ligneActes = streamActesMD.readLine();
                                qDebug() << "ligneActes= "+ligneActes;
                                strList           += ligneActes;
                                if (ligneActes.endsWith(";") && (ligneActes.endsWith("*//;")) == false ){
                                    listeMD       << strList;
                                    strList          = "";
                                }
                           }
                    if(database.driverName() == MYSQL){
                         for(int i = 0; i < listeMD.size() ; i++){
                             qDebug () << "listeMD ="+listeMD[i];
                             QSqlQuery queryMD(database);
                             queryMD.exec(listeMD[i]);
                         }
                      }
                    }
                    if(database.driverName() == SQLITE){
                        QString table = "mouvements_disponibles";
                        QStringList listFields;
                                    listFields << "id_mvt_dispo" <<	"id_mvt_parent" << "type" << "libelle" << "remarque";

                        QString csvFile = Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/csv_files/mouvements_disponibles.csv";
                        QFile file(csvFile);
                        if(!file.open(QIODevice::ReadOnly)){
                             QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("Fichier.csv non trouvé."),QMessageBox::Ok);
                        }
                        QTextStream stream(&file);
                                    stream.setCodec("UTF-8");//ISO 8859-1
                        while(!stream.atEnd()){
                            QString ligne = stream.readLine();
                            if(ligne == ""){
                                break;
                            }
                            // qDebug() << "ligne ="+ligne;
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
                       QSqlQuery insertQuery(database);
                                 insertQuery.exec(insertStr);
                       }
                     }
    //--mise à niveau mouvements---------------
                      QSqlQuery queryMouvements(database);
                             queryMouvements.exec("SELECT * FROM mouvements");
                   QSqlRecord recordMvts = queryMouvements.record();
	           int countMvts         = recordMvts.count();
                   if(countMvts < 14){
                     QSqlQuery queryM1(database);
                     queryM1.exec("ALTER TABLE mouvements ADD validation  tinyint(1)                             NULL AFTER tracabilite;");
                     QSqlQuery queryM2(database);
                     queryM2.exec("ALTER TABLE mouvements ADD detail      varchar(100)  COLLATE utf8_unicode_ci  NULL AFTER validation;");
                   }

    if (button != 0)
    {
    	  button -> setEnabled(false);
    	  button -> setIcon(QIcon(pixmapPath()+"/lock.xpm"));
        }
    QSqlQuery q(database);
    QString req;
    if(database.driverName() == "QMYSQL"){
        req = QString("show tables from %1;").arg(base);
    }
    if(database.driverName() == "QSQLITE"){
        req = QString("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << req;
    if(!q.exec(req)){
        qWarning() << __FILE__ << " " << QString::number(__LINE__) << q.lastError().text();
        }
    QStringList listetablespresentescompta;
    QString listStr;
    while(q.next()){
       QString s = q.value(0).toString();
       listetablespresentescompta << s+"\n";
    }
    int resultNbre = listetablespresentescompta.size();
    if(resultNbre == nbrOfTables){
        qWarning() << __FILE__ << QString::number(__LINE__) << "base complétée, nombre de tables = " 
        << QString::number(resultNbre);
        }
    else{
        qWarning() << __FILE__ << QString::number(__LINE__) << "base non complétée ou version 16, nombre de tables = " 
        << QString::number(resultNbre); 
        }
}

int alerteBoutonCompleteBase(QPushButton *Button){

    QSqlDatabase database = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    qDebug() << "in alerteBoutonCompleteBase " << __LINE__;
    Button->setEnabled(false);
    Button->setIcon(QIcon(pixmapPath()+"/lock.xpm"));
    QString str = QObject::trUtf8("Confirmez vous la mise à niveau de la comptabilité (Ok = Oui) ? ");
        int  retour = 2;

    int nbreOfTables = Common::ICore::instance()->settings()->value(S_COMPTA_NBR_OF_TABLES).toInt();
        QSqlQuery q(database);
    QString base = QString(Constants::DB_ACCOUNTANCY);
    QString req;
    if(database.driverName() == "QMYSQL"){
        req = QString("show tables from %1;").arg(base);
    }
    if(database.driverName() == "QSQLITE"){
        req = QString("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << req;
    if(!q.exec(req)){
        qWarning() << __FILE__ << " " << QString::number(__LINE__) << q.lastError().text();
        }
    QStringList listetablespresentescompta;
    QString listStr;
    while(q.next()){
       QString s = q.value(0).toString();
       listetablespresentescompta << s+"\n";
    }


	qDebug() << __FILE__ << QString::number(__LINE__)
	         << " listetablespresentescompta.size() ="+QString::number(listetablespresentescompta.size());
	if(Common::ICore::instance()->databaseManager()->isAnIndependantProgram()){
	    nbreOfTables++;
	}
	if(listetablespresentescompta.size() < nbreOfTables)
	    {
            QMessageBox mess;
    	    if(mess.isActiveWindow() == false){
	          mess   . setText(str+"\nLine = "+QString::number(__LINE__)+"\nFile = "+QString(__FILE__));
              mess   . setStandardButtons(QMessageBox::Cancel|QMessageBox::Ok);
              mess   . setDefaultButton(QMessageBox::Ok);
	         retour = mess.exec();
	         if(retour == QMessageBox::Cancel){
	              return 0;
	          }
           }
	    qDebug() << "in tables < " << QString::number(nbreOfTables)+" "<< __FILE__ << QString::number(__LINE__);
	    Button->setEnabled(true);
	    Button-> setIcon(QIcon(pixmapPath()+"/vert.png"));
	    qDebug() << "in alerteBoutonCompleteBase " << __LINE__;
	    qDebug () << "pixmap vert ="+pixmapPath()+"/vert.png";


	 }
	QStringList nombredechamps;
	    QSqlQuery querynombredechamps(database);
	    querynombredechamps.exec("SELECT * FROM actes_disponibles WHERE id_acte_dispo = 1");
	    QSqlRecord record = querynombredechamps.record();
	    int count = record.count();

	    if(count < 7){
	        Button->setEnabled(true);
	        Button-> setIcon(QIcon(pixmapPath()+"/vert.png"));
        	}
	QStringList nbreChampsSeances;
        QSqlQuery queryChampsSeances(database);
                  queryChampsSeances.exec("SELECT * FROM seances WHERE id_paiement = 1");
            QSqlRecord recordSeances = queryChampsSeances.record();
            int countSeances = recordSeances.count();
            if(countSeances < 13){
                Button->setEnabled(true);
	        Button-> setIcon(QIcon(pixmapPath()+"/vert.png"));
	        }

        QStringList NombreChampsHonoraires;
	    QSqlQuery   queryNombreChampsHonoraires(database);
	    queryNombreChampsHonoraires.exec("SELECT * FROM honoraires ");
	    QSqlRecord RecordHono = queryNombreChampsHonoraires.record();
	    int CountHono         = RecordHono.count();
	    if(CountHono < 21){
	        Button->setEnabled(true);
	        Button-> setIcon(QIcon(pixmapPath()+"/vert.png"));
	        }

        QStringList NombreListeMouvementsDisponibles;
        QSqlQuery  queryMvtsDisp(database);
                   queryMvtsDisp.exec("SELECT id_mvt_dispo FROM mouvements_disponibles");
                   while(queryMvtsDisp.next()){
                       QString str                        = queryMvtsDisp.value(0).toString();
                       NombreListeMouvementsDisponibles  <<  str;
                   }
            if(NombreListeMouvementsDisponibles.size() < 40){
                   Button->setEnabled(true);
                   Button-> setIcon(QIcon(pixmapPath()+"/vert.png"));
	           }

   return retour;
}



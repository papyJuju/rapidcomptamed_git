/***************************************************************************
 *   Copyright (C) 2009 by Eric Maeker, MD.                                *
 *   eric.maeker@free.fr                                                   *
 *   and completed by Pierre-Marie Desombre, GP,                           *
 *   pm.desombre@medsyn.fr                                                 *
 *   This code is free and open source .                                   *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
 /***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Docteur Pierre-Marie Desombre,pm.desombre@medsyn.fr               *
 ***************************************************************************/
#include "databasemanager.h"
#include "settings.h"
#include "constants.h"
#include "icore.h"
#include "commandlineparser.h"
#include "test.h"
#include "./configure/mdp.h"
#include <common/configure/comptatorapidcompta.h>

#include <QProgressDialog>
#include <QMessageBox>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QtSql>
#include <QDebug>
#include <QCursor>
#include <QPushButton>
#include <QDesktopServices>


enum { WarnDebugMessage = true }; //TODO nettoyer qdebugs

using namespace Common;
using namespace Constants;

static inline  Settings *settings() {return Common::ICore::instance()->settings(); }

DatabaseManager::DatabaseManager(QObject *parent) :
        QObject(parent),
        m_IsAccountancyConfigured(false),
        m_createtable("create table if not exists")
{
    
    qWarning() << __FILE__ << QString::number(__LINE__) << "firstTime = " << settings()->value(Constants::S_FIRSTTIME).toString();
    if (settings()->isFirstTimeRunning())
    {
        QString firstText = QString("<html>La nouvelle base de donn&eacute;es de la comptabilit&eacute; s'appelle rapidecomptabilite.<br/>"
        "1) Si vous avez comme ancienne base de comptabilit&eacute; celle de MedinTux, NE CHOISISSEZ PAS Sqlite mais MYSQL!<br/>"
        "2) Si vous avec comme base de donn&eacute;e comptabilite de rapidcomptamed, choisissez le m&ecirc;me driver que celle de rapidcomptamed.<br/>"
        "Sinon vous auriez du mal &agrave; r&eacute;cup&eacute;rer votre ancienne base de donn&eacute;es dans la nouvelle.<br/>"
        "3) Si vous avez une ancienne base, veuillez la transf&eacute;rer imm&eacute;diatement pas configuration/copier l'ancienne base <br/>"
        "4) En cas de premi&egrave;re utilisation du programme, veuillez vous rendre dans Configuration/parametrages "
                "pour configurer la comptabilit&eacute; avant la premi&egrave;re utilisation.</html>");
         QMessageBox msgBox;
         msgBox.setText(firstText);
         //msgBox.setInformativeText("Do you want to save your changes?");
         msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Ok);
         int ret = msgBox.exec();
         switch(ret){
             case QMessageBox::Ok :
                 break;
             case QMessageBox::Cancel :
                 delete qApp;
                 return;
                 break;
             default :
                 break;
             }
        qWarning() << __FILE__ << QString::number(__LINE__) << "firsttime = " << choiceOfDriver() ;
        QMessageBox::information(0,tr("Information"),
        QString("<html>Les donn&eacute;es de connexion sont configur&eacute;es.</html>"),QMessageBox::Ok);
    }
    QString codec;
    #ifdef Q_OS_LINUX
    codec =  Common::ICore::instance()->settings()->value(CODEC_LINUX).toString();
    #elif defined Q_OS_WIN32
    codec =  Common::ICore::instance()->settings()->value(CODEC_WINDOWS).toString();
    #else
    QMessageBox::warning(0,tr("Warning"),tr("OS non supporté."),QMessageBox::Ok);
    #endif
    QByteArray codecarray = codec.toLatin1();
    m_codecchar = codecarray.data();    
}

DatabaseManager::~DatabaseManager()
{}

bool DatabaseManager::isAccoutancyConfigNotConfigured()
{
    return settings()->value(Constants::S_FIRSTTIME).toBool();
}


QString DatabaseManager::choiceOfDriver()
{
    QString myChoices;
    QMessageBox choiceMess;
    choiceMess.setWindowTitle(QString("<html>Choisir le type de base de donn&eacute;es</html>"));
    choiceMess.setText(QString("<html>Quelle base de donn&eacute;es choisirez vous ?</html>"));
    choiceMess.setStandardButtons(QMessageBox::Cancel);
    QPushButton * sqliteButton = choiceMess.addButton(tr("Sqlite"),QMessageBox::YesRole);
    QPushButton * mysqlButton = choiceMess.addButton(tr("MySql"),QMessageBox::NoRole);
    choiceMess.setDefaultButton(mysqlButton);
    int ret = choiceMess.exec();
    if (choiceMess.clickedButton() == sqliteButton)
    {
    	  ret = Driver_SQLite;
        }
    if (choiceMess.clickedButton() == mysqlButton)
    {
    	  ret = Driver_MySQL;
        }
    switch(ret){
        case Driver_SQLite :
            manageSettingsForSqlite();
            break;
        case Driver_MySQL :
            manageSettingsForMysql();
            break;
        case QMessageBox::Cancel :
            manageSettingsForOnlyFirstTimeTrueToFalse();
            break;
        default :
            break;    
        }
    myChoices = settings()->value(Constants::S_COMPTA_DRIVER).toString();
    return myChoices;
}

void DatabaseManager::manageSettingsForMysql()
{
     mdp * databaseData = new mdp;
     if(databaseData->exec() == QDialog::Accepted) {
         QString pass = databaseData->mdpPassword();
         settings()->setValue(Constants::S_COMPTA_DB_LOGIN, databaseData->mdpLogin());
         settings()->setValue(Constants::S_COMPTA_DB_HOST,  databaseData->mdpHost());
         settings()->setValue(Constants::S_COMPTA_DB_PORT,  databaseData->mdpPort());
         settings()->setValue(Constants::S_COMPTA_DB_PASSWORD,pass);
         settings()->setValue(Constants::S_COMPTA_DRIVER,"MYSQL");
         manageSettingsForOnlyFirstTimeTrueToFalse();
         }
}

void DatabaseManager::manageSettingsForSqlite()
{
    manageSettingsForOnlyFirstTimeTrueToFalse();
}


void DatabaseManager::manageSettingsForOnlyFirstTimeTrueToFalse()
{
    settings()->setValue(Constants::S_FIRSTTIME,"false");
}

bool DatabaseManager::connectAccountancy()
{
    Q_ASSERT(!settings()->isClean());
    int driverType = 0;
    QString log    = settings()->value(Constants::S_COMPTA_DB_LOGIN).toString();
    QString pass   = settings()->value(Constants::S_COMPTA_DB_PASSWORD).toString();
    QString host   = settings()->value(Constants::S_COMPTA_DB_HOST).toString();
    QString port   = settings()->value(Constants::S_COMPTA_DB_PORT).toString();
    QString driver = settings()->value(Constants::S_COMPTA_DRIVER).toString();
    //--------test driver--------------------------------
    qWarning() << __FILE__ << QString::number(__LINE__)  << "Driver chosen is "+driver;
    if(driver == "MYSQL"){
        driverType = Driver_MySQL;
    }
    else if(driver == "SQLITE"){
        driverType = Driver_SQLite;
    }
    else if(driver == "POSTGRESQL"){
        driverType = Driver_PostGreSQL;
    }
    else{
        QMessageBox::critical(0,tr("Critical"),tr("Le driver de la base de données ne peut pas être défini.\n"
                    "Vous devez écrire dans config.ini après Driver :\n"
                    "pour MySql : MYSQL\npour Sqlite : SQLITE\npour PostGresql : POSTGRESQL."),QMessageBox::Ok);
        return false;
        }
    //--------------------------------------
    QSqlDatabase db;
    bool dbTest = 0;
    switch (driverType) {
        case Driver_MySQL :
        {
            db = QSqlDatabase::addDatabase("QMYSQL", Constants::DB_ACCOUNTANCY);
            db.setHostName(host);
            db.setDatabaseName(Constants::DB_ACCOUNTANCY);
            db.setUserName(log);
            db.setPassword(pass);
              if (WarnDebugMessage){
                qWarning() << "Connecting Accountancy database" << host << Constants::DB_ACCOUNTANCY << log << "" << port;
              }
            dbTest = (!db.isOpen()) && (!db.open());
            break;
        }
        case Driver_SQLite :
        {
            db = QSqlDatabase::addDatabase("QSQLITE", Constants::DB_ACCOUNTANCY);
            db.setDatabaseName(settings()->userResourcesPath() + QDir::separator() + Constants::DB_ACCOUNTANCY + ".db");
              if (WarnDebugMessage){
                qWarning() << "Connecting Accountancy database SQLite " + db.databaseName() +" "+ QString::number(__LINE__);
              }
            if(!db.open()){
                qWarning() << "database accountancy cannot open";
            }
            QStringList tables = db.tables();
            qDebug() << "tables "+QString::number(tables.size());
            dbTest = (tables.size() < 1);
            break;
        }
        case Driver_PostGreSQL :
            break;
    }

    if (dbTest) {
        qWarning() << __FILE__ << QString::number(__LINE__) << " in dbTest"  ;
        if (!createAccountancyDatabaseSchema(driverType)) {
            QMessageBox::warning(qApp->activeWindow(),
                                 tr("Fatal error"),
                                 tr("Unable to connect Accountancy database. Application will not work."),
                                 QMessageBox::Abort);
            qWarning() << "FATAL ERROR : unable to create the accountancy database";
            return false;
            }
        else
        {
            if (WarnDebugMessage)
                qWarning() << "Connected to Accountancy database"
                           << db.databaseName()
                           << __FILE__
                           << QString::number(__LINE__);
            }
        
        }
    if(isAccountancyDatabaseCorrupted(driverType,db)){
        QMessageBox messCompleteBase;
        messCompleteBase.setWindowTitle(tr("Warning"));
        messCompleteBase.setText(tr("La base  est incomplète : ")
                                   +QString(Constants::DB_ACCOUNTANCY)+" \n"+tr("elle va être mise à jour."));
        messCompleteBase.setStandardButtons(QMessageBox::Ok);
        messCompleteBase.setDefaultButton(QMessageBox::Ok);
        int ret = messCompleteBase.exec();
        switch(ret){
            case QMessageBox::Ok :
                if(!manageNewDatabase(driverType))
                {
                    qWarning() << "FATAL ERROR : unable to fix the accountancy database";
                    return false;
                    }
                break;
            case QMessageBox::Cancel :
                return false;
                break;
            default :
                break;    
            }            
        //return false; non sinon ca bloque le test version
        }
    if (!testVersion(db))
    {
    	  //QUrl ameli("http://www.ameli.fr/accueil-de-la-ccam/telechargement/version-actuelle/index.php");
    	  QString ameliAdress;
          QFile file(settings()->applicationResourcesPath()+"/url/lienameli.txt");
          if (!file.open(QIODevice::ReadOnly))
          {
    	       qWarning() << __FILE__ << QString::number(__LINE__) << "cannot open ameli web page." ;
           }
          QTextStream stream(&file);
          while (!stream.atEnd())
          {
    	      QString line = stream.readLine().trimmed();
    	      if (!line.isEmpty())
    	      {
    		  ameliAdress = line;
    	          }
              }
          QUrl ameli(ameliAdress);
    	  const QString explanation = tr("Votre version a changé.\nVerifiez la version de la CCAM sur "
    	                                 "ameli.fr.Voulez vous vérifier si une nouvelle version existe ?\n"
    	                                 "Si oui vous la chargerez par trouver CCAM\net mise à jour dans Mise à jour CCAM de comptatriee.");
    	  QMessageBox mess;
    	  mess.setText(explanation);
          mess.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
          mess.setDefaultButton(QMessageBox::Yes);
          int ret = mess.exec();
          switch(ret){
              case QMessageBox::Yes :
                  QDesktopServices::openUrl(QUrl(ameli));
                  break;
              case QMessageBox::No :
                  break;
              default :
                  break;    
              }
	  
        }
        if (isNotLockField(db))
        {
            QSqlQuery qLock(db);
            QString reqLock;
            if (driverType == Driver_MySQL)
            {
            	  reqLock = QString("ALTER TABLE %1 ADD %2 BLOB NULL AFTER %3")
                     .arg("utilisateurs","serrure","id_drtux_usr");
                }
            if (driverType == Driver_SQLite)
            {
            	  reqLock = QString("ALTER TABLE '%1' ADD %2 BLOB NULL DEFAULT NULL")
                     .arg("utilisateurs","serrure");
                }
            
            if (!qLock.exec(reqLock))
            {
            	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to add field serrure "
            	  << qLock.lastError().text() ;
                }
          

            }
    return true;
}

bool DatabaseManager::isNotLockField(QSqlDatabase & db)
{
    bool success = false;
    QSqlRecord fields = db.record("utilisateurs");
    if (fields.count() < UTIL_MaxParam)
    {
    	  success = true;
        }
   return success;
}

bool DatabaseManager::isAccountancyDatabaseCorrupted(const int driverType,QSqlDatabase &db) 
{//NAME INDEX EST TOUJOURS CREEE
    bool answer = false;
    QString driverName = "";
    QString req;
    QSqlQuery q(db);
    QString base = QString(Constants::DB_ACCOUNTANCY);
    if(driverType == Driver_MySQL){
        driverName = MYSQL;
        req = QString("show tables from %1;").arg(base);
        }
    if(driverType == Driver_SQLite){
        driverName = SQLITE;
        req = QString("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
        }
    if(driverType == Driver_PostGreSQL){
        driverName = POSTGRESQL;
        }
    if(!db.isOpen())
    {
       qWarning() << "db accountancy is not opened " << __FILE__ << __LINE__;
       }
    qWarning() << __FILE__ << " driverName ="+driverName;
    qWarning() << __FILE__ << " database ="+db.connectionName();
    QHash<QString,QString> hashtablesfields = getDatabaseShemaHash(driverType);
    int tablesCount = hashtablesfields.count();
    qWarning() << __FILE__ << QString::number(__LINE__) << " tablesCount =" << QString::number(tablesCount) ;
    
    if(!q.exec(req))
    {
        qWarning() << __FILE__ << " " << QString::number(__LINE__) << q.lastError().text();
        }
    QStringList list;
    QString listStr;
    while(q.next())
    {
        QString s = q.value(0).toString();
        list << s;
        listStr += s+",";
        }
    if (list.size() !=tablesCount)
    {
          qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << QString::number(list.size()) << QString::number(tablesCount);
          answer = true;
        }
    Test t(this);
    if (!t.areEveryFieldsPresent(driverType,db,list,hashtablesfields))
    {
          QString missingfields;
          QHash<QString,QString> hashofmissingfields;
          hashofmissingfields = t.getMissingFieldsByTable();
          QStringList listoftables;
          listoftables = hashofmissingfields.keys();
          listoftables.removeDuplicates();
          missingfields += QString("<html><body>Des champs sont manquants : <br/>");
          foreach(QString table,listoftables){
              QStringList valuesbytable;
              valuesbytable = hashofmissingfields.values(table);
              QString missfieldsofthistable = valuesbytable.join("<br/>");
              missingfields += QString("<br/><b>%1</b>:%2%3").arg(table,"<br/>",missfieldsofthistable);
              }
          missingfields += QString("</body></html>");
          QMessageBox::information(0,tr("Information"),missingfields,QMessageBox::Ok);
          answer = true;
        }
    return answer;
}

bool DatabaseManager::isAnIndependantProgram(){
    QString programName = Common::ICore::instance()->commandLine()->value(Common::CommandLine::CL_ProgramName).toString();
    if(!programName.isEmpty()){
        return false;
    }
    return true;
}

bool DatabaseManager::createFirstSqlConnection() const
{
     qWarning() << "in createFirstSqlConnection" << __FILE__;
     QSqlDatabase db = QSqlDatabase :: addDatabase(Constants::MYSQL,Constants::DB_FIRST_CONNECTION);
     db.setHostName(settings()->value(S_COMPTA_DB_HOST).toString());
     db.setDatabaseName("mysql");
     db.setUserName(settings()->value(S_COMPTA_DB_LOGIN).toString());
     db.setPassword(settings()->value(S_COMPTA_DB_PASSWORD).toString());
     qDebug() << __FILE__ << QString::number(__LINE__) << " pass in create firstConnection =" << settings()->value(S_COMPTA_DB_PASSWORD).toString() ;
     if(!db.open()) {
         qWarning() << "la base firstConnection ne peut être ouverte.";
     return false;
     }
      if(WarnDebugMessage)
      qDebug() << __FILE__ << " " << QString::number(__LINE__);
    return true;
}

bool DatabaseManager::createAccountancyDatabase(const int driver) const
{
    if(WarnDebugMessage)
    qDebug()<< "in createAccountancyDatabase" << __FILE__ << QString::number(__LINE__) ;
    if (driver==Driver_SQLite)
        return true;

    if (!createFirstSqlConnection()) {//MYSQL
        QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("An error occured during accountancy database creation.\nDo you have ")+Constants::MYSQL+""
                             ""+tr(" installed ?"),
                             QMessageBox::Abort);
        return false;
    }
    else{
        QSqlDatabase dbFirst = QSqlDatabase::database(Constants::DB_FIRST_CONNECTION);
        QString reqForA = QString("create database %1").arg(DB_ACCOUNTANCY);
        QSqlQuery q(dbFirst);
        q.exec(reqForA);
        if (q.lastError().isValid() == true) {
        QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("An error occured during accountancy database creation.\n")
                             +__FILE__+" "+QString::number(__LINE__),
                             QMessageBox::Abort);
        if (WarnDebugMessage)
            qWarning() << "SQL Error" << q.lastError().text() ;
        return false;
        }
        return true;
      }
}

bool DatabaseManager::createCcamDatabase(const int driverType){

    qDebug() << __FILE__ << QString::number(__LINE__) << " in createCcamDatabase " ;
    QString driverName = "";
    if(driverType == Driver_MySQL){
        if(!QSqlDatabase::database(Constants::DB_FIRST_CONNECTION).isValid()){
              qDebug() << __FILE__ << QString::number(__LINE__) << "DB_FIRST_CONNECTION is not valid"  ;
              if (!createFirstSqlConnection()) {//MYSQL
                             QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("An error occured during accountancy database creation.\nDo you have ")
                             +Constants::MYSQL+""
                             ""+tr(" installed ?")+"\n"__FILE__"\n"+QString::number(__LINE__),
                             QMessageBox::Abort);

              }
            }
              if(WarnDebugMessage)
              qDebug() << __FILE__ << " " << QString::number(__LINE__);
              QSqlQuery query(QSqlDatabase::database(Constants::DB_FIRST_CONNECTION));
              if(!query.exec("CREATE DATABASE CCAMTest")){
                  QMessageBox::critical(0,tr("Critical"),tr("CCAMTest cannot be created"),QMessageBox::Abort);
                  }
              
              
        if(WarnDebugMessage)
        qDebug() << __FILE__ << " " << QString::number(__LINE__) << "END OF CREATION CCAM DATABASE";

    }
    if(driverType == Driver_SQLite){
        driverName = SQLITE;
        }
    if(driverType == Driver_PostGreSQL){
        driverName = POSTGRESQL;
        }

  QSqlDatabase db = QSqlDatabase::database(Constants::DB_CCAM);
  if(!parseDumpCcam()){
      qWarning() << "CCAM is badly installed " << __FILE__ << " " << QString::number(__LINE__);
  }
  
  else{
      QMessageBox::information(0,tr("Information"),tr("La base CCAM a été installée correctement."),QMessageBox::Ok);
  }

  //----liste des tables-------------------
  QStringList CcamTables;
              CcamTables <<  "ACCES1"
                         <<  "ACCES2"
                         <<  "ACTES"
                         <<  "ACTION1"
                         << "ACTION2"
                         << "ACTIVITE"
                         << "ACTIVITEACTE"
                         << "ADM_REMB"
                         << "AGR_RADIO"
                         << "ARBORESCENCE"
                         << "ASSOCIABILITE"
                         << "ASSOCIATION"
                         << "ASS_NPREV"
                         << "CAT_MED"
                         << "CAT_SPEC"
                         << "CLASSE_DMT"
                         << "CODE_PAIEMENT"
                         << "COD_REGROUP"
                         << "COMPAT_EXO_TM"
                         << "COND_GEN"
                         << "DATES_EFFET"
                         << "DMT"
                         << "DOM"
                         << "EXO_TM"
                         << "EXT_DOC"
                         << "FRAIS_DEP"
                         << "HISTO_PHASE"
                         << "INCOMPATIBILITE"
                         << "LISTE_MOTS"
                         << "MEDECINS"
                         << "MODIFICATEUR"
                         << "MODIFICATEURACTE"
                         << "NAT_ASS"
                         << "NOTES"
                         << "NOTESARBORESCENCE"
                         << "NUM_DENTS"
                         << "PHASE"
                         << "PHASEACTE"
                         << "PROCEDURES"
                         << "REC_MED_ACTE"
                         << "RGL_TARIF"
                         << "SPECIALITES"
                         << "TB03"
                         << "TB04"
                         << "TB05"
                         << "TB06"
                         << "TB07"
                         << "TB08"
                         << "TB09"
                         << "TB10"
                         << "TB11"
                         << "TB12"
                         << "TB13"
                         << "TB14"
                         << "TB15"
                         << "TB17"
                         << "TB18"
                         << "TB19"
                         << "TB20"
                         << "THESAURUS"
                         << "TOPOGRAPHIE1"
                         << "TOPOGRAPHIE2"
                         << "TYPENOTE"
                         << "TYPE_ACTE"
                         << "TYPE_FORFAIT"
                         << "VAL_UNITE_OEUVRE"
                         << "version";

     return true;

}

bool DatabaseManager::createAccountancyDatabaseSchema(const int driver)
{
    bool result = true;
    if ((driver!=Driver_SQLite) && (!createAccountancyDatabase(driver))) {
        QMessageBox::critical(0, tr("Critical"), tr("Unable to create accountancy database."), QMessageBox::Ok);
        result = false;
        }
    QHash<QString,QString> name_sql = getDatabaseShemaHash(driver);

    // Mass execute SQL queries
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if ((!db.isOpen()) && (!db.open())) {
        if (WarnDebugMessage)
            qWarning() << "Unable to create accountancy database, database can not be open.";
        result = false;
    }
    if (WarnDebugMessage)
    qDebug() << "Hash Size "+QString::number(name_sql.size());
    QHashIterator<QString, QString> it(name_sql);
     while (it.hasNext()) {
         it.next();
         QSqlQuery q(db);
         QString req = it.value();
         if (driver==Driver_SQLite) {
             req = req.remove("COLLATE utf8_unicode_ci", Qt::CaseInsensitive).remove("UNSIGNED", Qt::CaseInsensitive);
             }
         q.exec(req);
         // Is there an error with this req ?
         if (q.lastError().isValid()) {
             QMessageBox::warning(qApp->activeWindow(),
                                  tr("Fatal error"),
                                  tr("An error occured during accountancy database creation.\n"
                                     "Reference : %1\n"
                                     "Error : %2\n\n"
                                     "Application will not work.")
                                  .arg(it.key()).arg(q.lastError().text()),
                                  QMessageBox::Abort);
             if (WarnDebugMessage)
                 qWarning() << "SQL Error" << it.key() << q.lastError().text();
             result = false;
         }
     }
     //-------table paiements et remplissage des bases d'initialisation ----------------------------------
   QString tablesinconfigini = Common::ICore::instance()->settings()->value(S_COMPTA_NBR_OF_TABLES).toString();
   int numberOfTables = tablesinconfigini.split(",").count();
   QMessageBox mess;
               mess.setText(tr("La base a été installée "
                               "avec ")+QString::number(numberOfTables)
                               +tr(" tables : ")+QString(DB_ACCOUNTANCY));
               mess.setStandardButtons(QMessageBox::Close);
               mess.setDefaultButton(QMessageBox::Close);
               int r = mess.exec();
               if(r == QMessageBox::Close)
                 {
                   if(feedAccountancyDatabaseWithDefaults() == true)
                   {
                       qDebug() << __FILE__ << QString::number(__LINE__) << " ICI ON PEUT METTRE UN BOUTON OK POUR IMPORTER "   ;
                       QMessageBox messimport;
                       messimport.setText(tr("Voulez vous importer une ancienne base de données ?"));
                       messimport.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
                       messimport.setDefaultButton(QMessageBox::Yes);
                       int ri = messimport.exec();
                       if (ri==QMessageBox::Yes)
                       {
                             importOldBaseToRapidecomptabilite();
                           }
                       result = true;
                       }
                   else{
                        QMessageBox::warning(0,tr("Erreur"),tr("L'initialisation des tables n'a pas pu se faire."),
                          QMessageBox::Ok);
                        result = false;
                       }
                 }
    return result;
}

/*bool DatabaseManager::feedAccountancyDatabaseWithDefaults() const
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QDir dirDump(Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump");
    QStringList filters;
                filters << "*.sql";
    dirDump.setNameFilters(filters);
    QStringList listDump;
                listDump = dirDump.entryList(filters);
     for(int i = 0 ; i < listDump.size() ; i++){
     qDebug() << Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump/"+listDump[i];
     QStringList list;
     QString strList        = "";
     QFile file(Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump/"+listDump[i]);
         if(!file.open(QIODevice::ReadOnly)){
             QMessageBox::warning(0,QObject::tr("Erreur"),listDump[i]+QObject::tr(" non trouvé."),QMessageBox::Ok);
         }
    QTextStream stream(&file);
                stream.setCodec("ISO 8859-1");
       while(!stream.atEnd()){
          QString ligne = stream.readLine();
          strList      += ligne;
           if (ligne.endsWith(";") && (ligne.endsWith("*////;")) == false ){  pour remettre enlever les // dans endsWhith n° 2
 /*               list       << strList;
                strList          = "";
            }
        }
       for(int i = 0; i < list.size() ; i++){
          QSqlQuery query(db);
          query.exec(list[i]);
       }
     }
    return true;
}*/

bool DatabaseManager::feedAccountancyDatabaseWithDefaults() const{//by pmd
    qDebug() << __FILE__ << QString::number(__LINE__) << " FEED ACCOUNTANCY WITH DEFAULTS " ;
    bool b = true;
    QHash<QString,QStringList> hash;
    QStringList listeActes;
                listeActes      << "id_acte_dispo"
                                << "nom_acte"
                                << "desc_acte"
                                << "type"
                                << "montant_total"
                                << "montant_tiers"
                                << "date_effet";
    QStringList listeMouvements;
                listeMouvements <<  "id_mvt_dispo"
                                <<  "id_mvt_parent"
                                <<  "type"
                                <<  "libelle"
                                <<  "remarque";
    QStringList listPourcentages;
                listPourcentages<<  "id_pourcent"
                                <<  "type"
                                <<  "pourcentage";

    QStringList listUtilisateurs;
                listUtilisateurs<<  "id_usr"
                                <<  "nom_usr"
                                <<  "login"
                                <<  "password"
                                <<  "serrure"
                                <<  "id_drtux_usr";//"id_drtux_usr";

    QStringList listComptesBancaires;
                listComptesBancaires << "id_compte"
                                     << "id_usr"
                                     << "libelle"
                                     << "titulaire"
                                     << "nom_banque"
                                     << "rib_code_banque"
                                     << "rib_code_guichet"
                                     << "rib_numcompte"
                                     << "rib_cle"
                                     << "solde_initial"
                                     << "remarque" ;

     QStringList listInformations;
                 listInformations << "version";
     QStringList listThesaurus;
                 listThesaurus << "id_thesaurus"
                               << "sequence"
                               << "type";
     QStringList listForfaits;
                 listForfaits  << "id_forfaits"
                               << "forfaits"
                               << "valeur"
                               << "type";
     QStringList listIndemnites_deplacement;
                 listIndemnites_deplacement << "id_IK"
                                            << "type"
                                            << "value"
                                            << "abattement"
                                            << "indemnites_forfait";

                hash.insert("actes_disponibles",listeActes);
                hash.insert("mouvements_disponibles",listeMouvements);
                hash.insert("pourcentages",listPourcentages);
                hash.insert("utilisateurs",listUtilisateurs);
                hash.insert("comptes_bancaires",listComptesBancaires);
                hash.insert("informations",listInformations);
                hash.insert("thesaurus",listThesaurus);
                hash.insert("forfaits",listForfaits);
                hash.insert("indemnites_deplacement",listIndemnites_deplacement);


    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QDir dirDump(Common::ICore::instance()->settings()->applicationResourcesPath()+"/csv_files");
    qWarning() << "Common::ICore::instance()->settings()->applicationResourcesPath()/csv_files) = "+Common::ICore::instance()->settings()->applicationResourcesPath()+"/csv_files";
    QStringList filters;
                filters << "*.csv";
    dirDump.setNameFilters(filters);
    QStringList listDump;
                listDump = dirDump.entryList(filters);
     for(int i = 0 ; i < listDump.size() ; i++){
         QStringList listNameTable = listDump[i].split(".");
         QString table = listNameTable[0];
         qDebug() << __FILE__ << QString::number(__LINE__) << "table for default = " << table;
     //---------récupérer les champs---------------
         QStringList listFields = hash[table];
         QString csvFile = Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/csv_files/"+listDump[i];
         QFile file(csvFile);
         if(!file.open(QIODevice::ReadOnly)){
             QMessageBox::warning(0,QObject::tr("Erreur"),QObject::tr("Fichier.csv non trouvé."),QMessageBox::Ok);
             return false;
         }
         QTextStream stream(&file);
                    stream.setCodec(m_codecchar);//ISO 8859-1
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
             //qDebug() << "INSERT INTO "+table+" ("+fieldsStr+") VALUES("+valuesStr+")";
             QSqlQuery insertQuery(db);
                       if(!insertQuery.exec(insertStr)){
                           qWarning() << tr("Erreur feedAccountancyDatabaseWithDefault pour table = ")
                                      << table+":"+insertQuery.lastError().text();
                           b = false;
                       }
         }
     }
     return b;
}

bool DatabaseManager::parseDumpCcam(){
  if(WarnDebugMessage)
  qDebug() << "in parseDumpCcam " << QString(__FILE__)+" "+QString::number(__LINE__);
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_CCAM);
  QString driver = settings()->value(Constants::S_COMPTA_DRIVER).toString();
  //QApplication::desktop()->setCursor(Qt::BusyCursor);
  QMessageBox::information(0,tr("Information"),tr("La CCAM va se remplir, notez que ça peut être long."),QMessageBox::Ok);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  if(driver== "MYSQL"){
    qDebug() << "in Mysql " << __FILE__ << QString::number(__LINE__);
    QStringList listeActes;
    QString strList        = "";
    QString pathDumpActesDispo = Common::ICore::instance()->settings()->applicationResourcesPath()
                                                                      +"/sql/CCAMTest_simple.sql";

    QFile fileActesIni(pathDumpActesDispo);
      if(!fileActesIni.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,QObject::tr("Erreur"),
        QObject::tr("CCAMTest_simple.sql non trouvé\npour : ")+pathDumpActesDispo+"\n"
          +QString(__FILE__)+" "+QString::number(__LINE__),QMessageBox::Ok);
        }
    QTextStream streamActes(&fileActesIni);
        streamActes.setCodec(m_codecchar);
    while(!streamActes.atEnd()){
        QString ligne = streamActes.readLine();
        if(!ligne.isEmpty() && !ligne.contains("*!40101")){
          strList           += ligne;
          //qDebug() << strList << " " << __FILE__ << QString::number(__LINE__);
            if (ligne.endsWith(";") && (ligne.endsWith("*//;")) == false ){
                listeActes       << strList;
                strList          = "";
                }
        }
    }
    qDebug() << "avant for"+QString(__FILE__)+" "+QString::number(__LINE__);
    qDebug() << "somme listeActes ="+QString::number(listeActes.size());
    int nbrOfTables = listeActes.size();
    for(int i = 0; i < nbrOfTables ; i++){
        QSqlQuery query(db);

        if(!query.exec(listeActes[i])){
            qDebug() << __FILE__ << " " << QString::number(__LINE__) << query.lastError().text();
            return false;
        }
    }
  }
  if(driver=="SQLITE"){
          qDebug() << "in SQLITE " << __FILE__ << QString::number(__LINE__);
    QStringList listeActes;
    QString strList        = "";
    QString pathDumpActesDispo = Common::ICore::instance()->settings()->applicationResourcesPath()
                                                                      +"/sql/CCAMTest_sqlite2.sql";

    QFile fileActesIni(pathDumpActesDispo);
      if(!fileActesIni.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,QObject::tr("Erreur"),
        QObject::tr("CCAMTest_simple.sql non trouvé\npour : ")+pathDumpActesDispo+"\n"
          +QString(__FILE__)+" "+QString::number(__LINE__),QMessageBox::Ok);
        }
    QTextStream streamActes(&fileActesIni);
        streamActes.setCodec(m_codecchar);
    while(!streamActes.atEnd()){
        QString ligne = streamActes.readLine();
        if(!ligne.isEmpty() && !ligne.contains("*!40101") && !ligne.contains("KEY")){
          strList           += ligne;

            if (ligne.endsWith(";") && (ligne.endsWith("*//;")) == false ){
                if(strList.contains("CREATE") && strList.contains(",")){
                    QStringList l = strList.split(",");
                    l.replaceInStrings(l[0],l[0]+" primary key ");
                    strList = l.join(",");
                    strList.replace(",);",");");
                    }
                listeActes       << strList;
                strList          = "";
                }
        }
    }
    if(WarnDebugMessage)
    qDebug() << "avant for "+QString(__FILE__)+" "+QString::number(__LINE__);
    if(WarnDebugMessage)
    qDebug() << "somme listeActes ="+QString::number(listeActes.size());
    int nbrOfTables = listeActes.size();
    for(int i = 0; i < nbrOfTables ; i++){
        QSqlQuery query(db);

        if(!query.exec(listeActes[i])){
            qDebug() << __FILE__ << " " << QString::number(__LINE__) << query.lastError().text();
            qDebug() << __FILE__ << " " << QString::number(__LINE__) << query.lastQuery();
            //QApplication::desktop()->setCursor(Qt::ArrowCursor);
            QApplication::restoreOverrideCursor();
            return false;
        }
    }
  }
  //QApplication::desktop()->setCursor(Qt::ArrowCursor);
  QApplication::restoreOverrideCursor();
  return true;
}

bool DatabaseManager::testVersion(QSqlDatabase & db){
    Test t(this);
    return t.testVersion(db);
}

bool DatabaseManager::miseaniveauIdmvtdispo(QSqlDatabase & db)
{
    enum MvtDispoEnum{IDMVTDISPO= 0,LIBELLE,IDMVT};
    QHash<int,QString> hashmvtdispo;
    QSqlQuery qMvtdispo(db);
    QString reqMvtdispo = QString("select %1,%2 from %3").arg("id_mvt_dispo","libelle","mouvements_disponibles");
    if (!qMvtdispo.exec(reqMvtdispo))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qMvtdispo.lastError().text() ;
          return false;
        }
    while (qMvtdispo.next())
    {
        int idmvtdispo = qMvtdispo.value(IDMVTDISPO).toInt();
        QString libelle = qMvtdispo.value(LIBELLE).toString();
        hashmvtdispo.insert(idmvtdispo,libelle);
        }
    QSqlQuery qMvt(db);
    QString reqMvt = QString("select %1,%2,%3 from %4").arg("id_mvt_dispo","libelle","id_mouvement","mouvements");
    if (!qMvt.exec(reqMvt))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qMvt.lastError().text();
          return false;
          }
    while (qMvt.next())
    {
        int idmvtdispo = qMvt.value(IDMVTDISPO).toInt();
        QString libelle = qMvt.value(LIBELLE).toString();
        int idmvt = qMvt.value(IDMVT).toInt();
        int realidmvtdispo = hashmvtdispo.key(libelle);
        if (realidmvtdispo != idmvtdispo)
        {
              QSqlQuery qUpdate(db);
              QString reqUpdate = QString("update %1 set %2='%3' where %4='%5'")
                                 .arg("mouvements","id_mvt_dispo",QString::number(realidmvtdispo),
                                 "id_mouvement",QString::number(idmvt));
              if (!qUpdate.exec(reqUpdate))
              {
                    qWarning() << __FILE__ << QString::number(__LINE__) << qUpdate.lastError().text() ;
                    return false;
                  }
            }
        }
    return true;
}

QHash<QString,QString> DatabaseManager::getDatabaseShemaHash(int driver)
{
    QHash<QString,QString> name_sql;//RESPECTER LA SYNTAXE TESTS POSSIBLES
    if (driver==Driver_MySQL) {
    name_sql.insert("Table actes_disponibles",
                    ""+m_createtable+" actes_disponibles ("
                    "id_acte_dispo  int(10)       UNSIGNED NOT NULL auto_increment,"
                    "nom_acte      varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "desc_acte      varchar(200)  COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(100)  COLLATE utf8_unicode_ci NOT NULL,"
                    "montant_total  double        NOT NULL,"
                    "montant_tiers  double        NOT NULL,"
                    "divers_info    blob NULL,"
                    "date_effet     date          NOT NULL,"
                    "PRIMARY KEY(id_acte_dispo));");
    name_sql.insert("Table thesaurus",
                    ""+m_createtable+" thesaurus ("
                    "id_thesaurus  int(10)       UNSIGNED NOT NULL auto_increment,"
                    "sequence      varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(100)  COLLATE utf8_unicode_ci NOT NULL,"
                    "PRIMARY KEY(id_thesaurus));");
    name_sql.insert("Table forfaits",
                    ""+m_createtable+" forfaits ("
                    "id_forfaits  int(10)       UNSIGNED NOT NULL auto_increment,"
                    "forfaits      varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "desc_acte      varchar(200)  COLLATE utf8_unicode_ci NULL,"
                    "valeur      varchar(10)   COLLATE utf8_unicode_ci NOT NULL,"
                    "date date NULL,"
                    "divers_info    blob NULL,"
                    "type           varchar(10)  COLLATE utf8_unicode_ci NOT NULL,"
                    "PRIMARY KEY(id_forfaits));");
    name_sql.insert("Table  comptes_bancaires",
                    ""+m_createtable+" comptes_bancaires ("
                    "id_compte       int(10)  UNSIGNED    NOT NULL auto_increment  ,"
                    "id_usr        int(10)   UNSIGNED    NOT NULL    ,"
                    "libelle        varchar(150)  COLLATE utf8_unicode_ci   NULL     ,"
                    "titulaire        varchar(100)  COLLATE utf8_unicode_ci  NULL     ,"
                    "nom_banque       varchar(50)  COLLATE utf8_unicode_ci  NULL     ,"
                    "rib_code_banque  varchar(5)  COLLATE utf8_unicode_ci  NULL     ,"
                    "rib_code_guichet varchar(5)  COLLATE utf8_unicode_ci   NULL     ,"
                    "rib_numcompte    varchar(11)  COLLATE utf8_unicode_ci  NULL     ,"
                    "rib_cle        varchar(2)  COLLATE utf8_unicode_ci  NULL     ,"
                    "solde_initial    double       NULL    ,"
                    "remarque        blob         NULL     ,"
                    "PRIMARY KEY(id_compte));");
    name_sql.insert("Table depots",
                    ""+m_createtable+" depots   ("
                    "id_depot    int(11)      NOT NULL auto_increment  ,"
                    "id_usr   int(11)      NOT NULL,"
                    "id_compte   int(10)      UNSIGNED NOT NULL,"
                    "type_depot  varchar(10)  COLLATE utf8_unicode_ci NULL,"
                    "date        date         NOT NULL,"
                    "periode_deb date         NULL,"
                    "periode_fin date         NULL,"
                    "blob_depot  blob         NULL,"
                    "remarque   text         COLLATE utf8_unicode_ci NULL,"
                    "valide   tinyint(4)   NULL,"
                    "PRIMARY KEY(id_depot));");
    name_sql.insert("Table honoraires",
                    ""+m_createtable+" honoraires ("
                    "id_hono      int(11)        UNSIGNED NOT NULL  auto_increment  ,"
                    "id_usr     int(11)        NOT NULL,"
                    "id_drtux_usr  int(11)        NULL,"
                    "patient     varchar(75)    COLLATE utf8_unicode_ci NOT NULL,"
                    "id_site       varchar(40)    COLLATE utf8_unicode_ci NULL,"
                    "id_payeurs    varchar(36)    COLLATE utf8_unicode_ci NULL,"
                    "GUID          varchar(36)    COLLATE utf8_unicode_ci NULL,"
                    "praticien     varchar(75)    COLLATE utf8_unicode_ci NOT NULL,"
                    "date          date           NOT NULL,"
                    "acte          blob           NULL,"
                    "acteclair     varchar(12)    COLLATE utf8_unicode_ci NULL,"
                    "remarque      blob           NULL,"
                    "esp           double         NOT NULL,"
                    "chq           double         NOT NULL,"
                    "cb            double         NOT NULL,"
                    "vir           double         NOT NULL,"
                    "daf           double         NOT NULL,"
                    "autre         double         NOT NULL,"
                    "du            double         NOT NULL,"
                    "du_par        varchar(100)   COLLATE utf8_unicode_ci NULL,"
                    "valide        tinyint(1)     NULL,"
                    "tracabilite   blob           NULL,"
                    "divers        blob           NULL,"
                    "PRIMARY KEY(id_hono));");
    name_sql.insert("Table immobilisations",
                    ""+m_createtable+" immobilisations ("
                    "id_immob       int(10)         UNSIGNED NOT NULL auto_increment,"
                    "id_usr      int(10)         UNSIGNED NOT NULL,"
                    "id_compte      int(10)         UNSIGNED NOT NULL,"
                    "libelle      varchar(150)    COLLATE utf8_unicode_ci NULL,"
                    "date_service   date            NOT NULL,"
                    "duree          int(11)         NOT NULL,"
                    "mode           tinyint(1)      NOT NULL,"
                    "valeur         bigint(20)      UNSIGNED NOT NULL,"
                    "montant_tva    double          NULL,"
                    "valeur_residuelle bigint(20)   NOT NULL,"
                    "resultat       blob            NOT NULL,"
                    "mouvements     blob            NULL,"
                    "remarque       text            COLLATE utf8_unicode_ci NULL,"
                    "tracabilite    blob            NULL,"
                    "divers        blob           NULL,"
                    "PRIMARY KEY(id_immob));");
    name_sql.insert("Table mouvements",
                    ""+m_createtable+" mouvements ("
                    "id_mouvement  int(10)  UNSIGNED NOT NULL auto_increment ,"
                    "id_mvt_dispo  int(11)      NOT NULL,"
                    "id_usr        int(10)  UNSIGNED NOT NULL,"
                    " id_compte    int(10)  UNSIGNED NOT NULL,"
                    " type         tinyint(3)  UNSIGNED NOT NULL,"
                    " libelle      varchar(100) COLLATE utf8_unicode_ci NOT NULL,"
                    " date         date         NOT NULL,"
                    " date_valeur  date         NOT NULL,"
                    " montant      double       UNSIGNED NOT NULL,"
                    " montant_total      double       UNSIGNED  NULL,"
                    "remarque      blob         NULL,"
                    " valide       tinyint(4)   NOT NULL,"
                    " tracabilite  blob         NULL,"
                    " validation   tinyint(1)   NULL,"
                    " detail       varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "divers        blob           NULL,"
                    "PRIMARY KEY(id_mouvement));");
    name_sql.insert("Table mouvements_disponibles",
                    ""+m_createtable+" mouvements_disponibles ("
                    "id_mvt_dispo   int(10)      UNSIGNED NOT NULL auto_increment,"
                    "id_mvt_parent  int(11)      NULL,"
                    "type           tinyint(3)   UNSIGNED NOT NULL,"
                    "libelle      varchar(100) COLLATE utf8_unicode_ci NOT NULL,"
                    "remarque      blob         NULL,"
                    "PRIMARY KEY(id_mvt_dispo));");
    name_sql.insert("Table payeurs",
                    ""+m_createtable+" payeurs ("
                    "ID_Primkey   bigint(20)   NOT NULL auto_increment,"
                    "id_payeurs  bigint(20)   NOT NULL,"
                    "nom_payeur  varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "adresse_payeur     varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "ville_payeur  varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "code_postal  varchar(5)   COLLATE utf8_unicode_ci NULL,"
                    "telephone_payeur   varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "divers        blob           NULL,"
                    "PRIMARY KEY(ID_Primkey));");
    name_sql.insert("Table sites",
                    ""+m_createtable+" sites ("
                    "ID_Primkey   bigint(10)   NOT NULL auto_increment,"
                    "id_site      varchar(40)  COLLATE utf8_unicode_ci NOT NULL,"
                    "site    varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "adresse_site varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "cp_site      varchar(5)  COLLATE utf8_unicode_ci NULL,"
                    "ville_site   varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "telsite    varchar(50)  COLLATE utf8_unicode_ci NULL,"
                    "faxsite    varchar(50)  COLLATE utf8_unicode_ci NULL,"
                    "mailsite    varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "divers        blob           NULL,"
                    "PRIMARY KEY(ID_Primkey));");
    name_sql.insert("Table utilisateurs",
                    ""+m_createtable+" utilisateurs ("
                    "id_usr   int(10)     UNSIGNED NOT NULL auto_increment,"
                    "nom_usr  varchar(150)   COLLATE utf8_unicode_ci NULL,"
                    "login  varchar(15)   COLLATE utf8_unicode_ci NOT NULL,"
                    "password  varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "id_drtux_usr  int(11)  NULL,"
                    "serrure BLOB NULL,"
                    "PRIMARY KEY(id_usr));");
    name_sql.insert("Table pourcentages",
                    ""+m_createtable+" pourcentages ("
                    "id_pourcent    int(10)   UNSIGNED NOT NULL auto_increment,"
                    "type    varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "pourcentage  varchar(6)  COLLATE utf8_unicode_ci  NOT NULL,"
                    "PRIMARY KEY(id_pourcent));");
    name_sql.insert("Table seances",
                    ""+m_createtable+" seances  ("
                    "id_paiement    int(11)  unsigned NOT NULL auto_increment ,"
                    "id_hono        int(11)     UNSIGNED NOT NULL,"
                    "date           date                NOT NULL,"
                    "id_payeurs     bigint(20)          NOT NULL,"
                    "id_usr         varchar(10)         NULL,"
                    "id_site        varchar(40)         NULL,"
                    "acte           text                COLLATE utf8_unicode_ci NULL,"
                    "esp            double   NOT NULL,"
                    "chq            double   NOT NULL,"
                    "cb             double   NOT NULL,"
                    "vir            double      NOT NULL,"
                    "daf            double   NOT NULL,"
                    "autre          double   NOT NULL,"
                    "du             double   NOT NULL,"
                    "divers_infos blob NULL,"
                    "valide  tinyint(1)   NULL,"
                    "PRIMARY KEY(id_paiement));");
    name_sql.insert("Table paiements",
                    ""+m_createtable+" paiements ("
                    "id_paiement   int(11)     UNSIGNED  NOT NULL  auto_increment ,"
                    "id_hono      int(11)     UNSIGNED  NOT NULL ,"
                    "date      date          NOT NULL,"
                    "id_payeurs    bigint(20)          NOT NULL,"
                    "acte      blob               NULL  ,"
                    "esp      double          NOT  NULL,"
                    "chq      double          NOT  NULL,"
                    "cb      double          NOT  NULL,"
                    "vir      double          NOT  NULL,"
                    "daf      double          NOT  NULL,"
                    "autre      double          NOT  NULL,"
                    "du     double          NOT  NULL,"
                    "valide      tinyint(1)          NULL ,"
                    "PRIMARY KEY(id_paiement));");
    name_sql.insert("Table facturation",
                    ""+m_createtable+" facturation ("
                    "id_facture int(11) UNSIGNED  NOT NULL  auto_increment ,"
                    "divers blob NULL,"
                    "PRIMARY KEY(id_facture));");
    name_sql.insert("Table dates_comptable",
                    ""+m_createtable+" dates_comptable ("
                    "id_dates_comptable int(11) UNSIGNED  NOT NULL  auto_increment,"
                    "date_operation date NULL,"
                    "date_paiement date NULL,"
                    "date_valeur date NULL,"
                    "date_validation date NULL,"
                    "PRIMARY KEY(id_dates_comptable));");
    name_sql.insert("Table rapprochement_bancaire",
                    ""+m_createtable+" rapprochement_bancaire ("
                    "id_rap_bank   int(11) UNSIGNED  NOT NULL  auto_increment ,"
                    "id_compte     int(11) UNSIGNED  null,"
                    "solde         double NULL,"
                    "divers        blob           NULL,"
                    "PRIMARY KEY(id_rap_bank));");
    name_sql.insert("Table z_version",
                    ""+m_createtable+" z_version  ("
                    "infos_version blob NULL);");
    name_sql.insert("Table informations",
                    ""+m_createtable+" informations ("
                    "id_info int(6) UNSIGNED  NOT NULL  auto_increment ,"
                    "version varchar(6) NULL,"
                    "PRIMARY KEY(id_info));");
    name_sql.insert("Table indemnites_deplacement",
                    ""+m_createtable+" indemnites_deplacement ("
                    "id_IK int(6) UNSIGNED  NOT NULL  auto_increment ,"
                    "type varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "value double NOT  NULL,"
                    "abattement double NOT  NULL,"
                    "indemnites_forfait double NULL,"
                    "PRIMARY KEY(id_IK));");
    name_sql.insert("Table notes",
                    ""+m_createtable+" notes ("
                    "id_notes int(6) UNSIGNED  NOT NULL  auto_increment ,"
                    "notes blob NULL,"
                    "PRIMARY KEY(id_notes));");
    name_sql.insert("Table ccam",
                    ""+m_createtable+" ccam ("
                    "id_ccam  int(20) UNSIGNED  NOT NULL  auto_increment ,"
                    "code varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "abstract varchar(200) COLLATE utf8_unicode_ci NULL,"
                    "type varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "amount double NOT NULL,"
                    "reimbursment double NULL,"
                    "date date NOT NULL,"
                    "others blob NULL,"
                    "PRIMARY KEY(id_ccam));");               
    name_sql.insert("Table nameindex",
                        ""+m_createtable+"  nameindex ("
                        "id_index         int(6)                 unsigned                    NOT NULL auto_increment,"
                        "name             varchar(50)            COLLATE  utf8_unicode_ci    NULL,"
                        "surname          varchar(50)            COLLATE  utf8_unicode_ci    NULL,"
                        "guid             varchar(6)                                         NOT NULL,"
                        "divers        blob           NULL,"
                        "PRIMARY KEY(id_index));");
    name_sql.insert("Table headings",
                    ""+m_createtable+" headings ("
                    "id_headings int(6) unsigned NOT NULL auto_increment,"
                    "headings blob NULL,"
                    "footers blob NULL,"
                    "divers blob null,"
                    "PRIMARY KEY(id_headings));");
    name_sql.insert("Table alertes",
                     ""+m_createtable+" alertes ("//il faut noter la table entre deux espaces
                     "id_alertes int(20) unsigned NOT NULL auto_increment,"
                     "alertes varchar(50) COLLATE utf8_unicode_ci NULL,"
                     "date date NOT NULL,"
                     "id_usr int(10) UNSIGNED NULL,"
                     "valid int(1) NULL,"
                     "isread int(1) NULL,"
                     "PRIMARY KEY(id_alertes));");
    }
    else if(driver==Driver_SQLite) {
        name_sql.insert("Table actes_disponibles",
                    ""+m_createtable+" actes_disponibles ("
                    "id_acte_dispo  INTEGER PRIMARY KEY,"
                    "nom_acte      varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "desc_acte      varchar(200)  COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(100)  COLLATE utf8_unicode_ci NOT NULL,"
                    "montant_total  double        NOT NULL,"
                    "montant_tiers  double        NOT NULL,"
                    "divers_info    blob NULL,"
                    "date_effet     date          NOT NULL);");
        name_sql.insert("Table thesaurus",
                    ""+m_createtable+" thesaurus ("
                    "id_thesaurus  INTEGER PRIMARY KEY,"
                    "sequence      varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(100)  COLLATE utf8_unicode_ci NOT NULL);");
        name_sql.insert("Table forfaits",
                    ""+m_createtable+" forfaits ("
                    "id_forfaits  INTEGER PRIMARY KEY,"
                    "forfaits      varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "desc_acte      varchar(200)  COLLATE utf8_unicode_ci NULL,"
                    "valeur      varchar(10)   COLLATE utf8_unicode_ci NOT NULL,"
                    "date date NULL,"
                    "divers_info    blob NULL,"
                    "type           varchar(10)  COLLATE utf8_unicode_ci NOT NULL);");

    name_sql.insert("Table comptes_bancaires",
                    ""+m_createtable+" comptes_bancaires ("
                    "id_compte       INTEGER PRIMARY KEY ,"
                    "id_usr        int(10)   UNSIGNED    NOT NULL    ,"
                    "libelle        varchar(150)  COLLATE utf8_unicode_ci   NULL     ,"
                    "titulaire        varchar(100)  COLLATE utf8_unicode_ci  NULL     ,"
                    "nom_banque       varchar(50)  COLLATE utf8_unicode_ci  NULL     ,"
                    "rib_code_banque  varchar(5)  COLLATE utf8_unicode_ci  NULL     ,"
                    "rib_code_guichet varchar(5)  COLLATE utf8_unicode_ci   NULL     ,"
                    "rib_numcompte    varchar(11)  COLLATE utf8_unicode_ci  NULL     ,"
                    "rib_cle        varchar(2)  COLLATE utf8_unicode_ci  NULL     ,"
                    "solde_initial    double       NULL    ,"
                    "remarque        blob         NULL           );");
    name_sql.insert("Table depots",
                    ""+m_createtable+" depots    ("
                    "id_depot    INTEGER PRIMARY KEY ,"
                    "id_usr   int(11)      NOT NULL,"
                    "id_compte   int(10)      UNSIGNED NOT NULL,"
                    "type_depot  varchar(10)  COLLATE utf8_unicode_ci NULL,"
                    "date        date         NOT NULL,"
                    "periode_deb date         NULL,"
                    "periode_fin date         NULL,"
                    "blob_depot  blob         NULL,"
                    "remarque   text         COLLATE utf8_unicode_ci NULL,"
                    "valide   tinyint(4)   NULL);");
    name_sql.insert("Table honoraires",
                    ""+m_createtable+" honoraires ("
                    "id_hono      INTEGER PRIMARY KEY ,"
                    "id_usr     int(11)        NOT NULL,"
                    "id_drtux_usr  blob       NULL,"
                    "patient     varchar(75)    COLLATE utf8_unicode_ci NOT NULL,"
                    "id_site       varchar(40)    COLLATE utf8_unicode_ci NULL,"
                    "id_payeurs    varchar(36)    COLLATE utf8_unicode_ci NULL,"
                    "GUID          varchar(36)    COLLATE utf8_unicode_ci NULL,"
                    "praticien     varchar(75)    COLLATE utf8_unicode_ci NOT NULL,"
                    "date          date           NOT NULL,"
                    "acte          blob           NULL,"
                    "acteclair     varchar(12)    COLLATE utf8_unicode_ci NULL,"
                    "remarque      blob           NULL,"
                    "esp           double         NOT NULL,"
                    "chq           double         NOT NULL,"
                    "cb            double         NOT NULL,"
                    "vir           double         NOT NULL,"
                    "daf           double         NOT NULL,"
                    "autre         double         NOT NULL,"
                    "du            double         NOT NULL,"
                    "du_par        varchar(100)   COLLATE utf8_unicode_ci NULL,"
                    "valide        tinyint(1)     NULL,"
                    "tracabilite   blob           NULL,"
                    "divers        blob           NULL");
    name_sql.insert("Table immobilisations",
                    ""+m_createtable+" immobilisations ("
                    "id_immob      INTEGER PRIMARY KEY,"
                    "id_usr      int(10)         UNSIGNED NOT NULL,"
                    "id_compte      int(10)         UNSIGNED NOT NULL,"
                    "libelle      varchar(150)    COLLATE utf8_unicode_ci NULL,"
                    "date_service   date            NOT NULL,"
                    "duree          int(11)         NOT NULL,"
                    "mode           tinyint(1)      NOT NULL,"
                    "valeur         bigint(20)      UNSIGNED NOT NULL,"
                    "montant_tva    double          NULL,"
                    "valeur_residuelle bigint(20)   NOT NULL,"
                    "resultat       blob            NOT NULL,"
                    "mouvements     blob            NULL,"
                    "remarque       text            COLLATE utf8_unicode_ci NULL,"
                    "tracabilite    blob            NULL,"
                    "divers         blob           NULL");
    name_sql.insert("Table mouvements",
                    ""+m_createtable+" mouvements ("
                    "id_mouvement  INTEGER PRIMARY KEY,"
                    "id_mvt_dispo  int(11)      NOT NULL,"
                    "id_usr        int(10)  UNSIGNED NOT NULL,"
                    " id_compte    int(10)  UNSIGNED NOT NULL,"
                    " type         tinyint(3)  UNSIGNED NOT NULL,"
                    " libelle      varchar(100) COLLATE utf8_unicode_ci NOT NULL,"
                    " date         date         NOT NULL,"
                    " date_valeur  date         NOT NULL,"
                    " montant      double       UNSIGNED NOT NULL,"
                    " montant_total      double       UNSIGNED  NULL,"
                    "remarque      blob         NULL,"
                    " valide       tinyint(4)   NOT NULL,"
                    " tracabilite  blob         NULL,"
                    " validation   tinyint(1)   NULL,"
                    " detail       varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "divers        blob           NULL");
    name_sql.insert("Table mouvements_disponibles",
                    ""+m_createtable+" mouvements_disponibles ("
                    "id_mvt_dispo  INTEGER PRIMARY KEY,"
                    "id_mvt_parent  int(11)      NULL,"
                    "type           tinyint(3)   UNSIGNED NOT NULL,"
                    "libelle      varchar(100) COLLATE utf8_unicode_ci NOT NULL,"
                    "remarque      blob         NULL);");
    name_sql.insert("Table payeurs",
                    ""+m_createtable+" payeurs ("
                    "ID_Primkey   INTEGER PRIMARY KEY,"
                    "id_payeurs  bigint(20)   NOT NULL,"
                    "nom_payeur  varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "adresse_payeur     varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "ville_payeur  varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "code_postal  varchar(5)   COLLATE utf8_unicode_ci NULL,"
                    "telephone_payeur   varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "divers        blob           NULL");
    name_sql.insert("Table sites",
                    ""+m_createtable+" sites ("
                    "ID_Primkey   INTEGER PRIMARY KEY,"
                    "id_site      varchar(40)  COLLATE utf8_unicode_ci NOT NULL,"
                    "site    varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "adresse_site varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "cp_site      varchar(5)  COLLATE utf8_unicode_ci NULL,"
                    "ville_site   varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "telsite    varchar(50)  COLLATE utf8_unicode_ci NULL,"
                    "faxsite    varchar(50)  COLLATE utf8_unicode_ci NULL,"
                    "mailsite    varchar(100)  COLLATE utf8_unicode_ci NULL,"
                    "divers        blob           NULL");
    name_sql.insert("Table utilisateurs",
                    ""+m_createtable+" utilisateurs ("
                    "id_usr   INTEGER PRIMARY KEY,"
                    "nom_usr  varchar(150)   COLLATE utf8_unicode_ci NULL,"
                    "login  varchar(15)   COLLATE utf8_unicode_ci NOT NULL,"
                    "password  varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "id_drtux_usr  int(11)  NULL,"
                    "serrure blob NULL");
    name_sql.insert("Table pourcentages",
                    ""+m_createtable+" pourcentages ("
                    "id_pourcent  INTEGER PRIMARY  KEY,"
                    "type    varchar(100)    COLLATE utf8_unicode_ci NULL,"
                    "pourcentage    varchar(6)  COLLATE utf8_unicode_ci  NOT NULL);");
    name_sql.insert("Table seances",
                    ""+m_createtable+" seances  ("
                    "id_paiement   INTEGER PRIMARY KEY,"
                    "id_hono        int(11)     UNSIGNED NOT NULL,"
                    "date           date                NOT NULL,"
                    "id_payeurs     bigint(20)          NOT NULL,"
                    "id_usr         varchar(10)         NULL,"
                    "id_site        varchar(40)         NULL,"
                    "acte           text                COLLATE utf8_unicode_ci NULL,"
                    "esp            double   NOT NULL,"
                    "chq            double   NOT NULL,"
                    "cb             double   NOT NULL,"
                    "vir             double   NOT NULL,"
                    "daf            double   NOT NULL,"
                    "autre          double   NOT NULL,"
                    "du             double   NOT NULL,"
                    "divers_infos blob NULL,"
                    "valide  tinyint(1)   NULL);");
    name_sql.insert("Table paiements",
                    ""+m_createtable+" paiements ("
                    "id_paiement   INTEGER PRIMARY KEY,"
                    "id_hono      int(11)     UNSIGNED  NOT NULL ,"
                    "date      date          NOT NULL,"
                    "id_payeurs    bigint(20)          NOT NULL,"
                    "acte      blob               NULL  ,"
                    "esp      double          NOT  NULL,"
                    "chq      double          NOT  NULL,"
                    "cb      double          NOT  NULL,"
                    "vir      double          NOT  NULL,"
                    "daf      double          NOT  NULL,"
                    "autre      double          NOT  NULL,"
                    "du     double          NOT  NULL,"
                    "valide      tinyint(1)          NULL);");
    name_sql.insert("Table facturation",
                    ""+m_createtable+" facturation ("
                    "id_facture INTEGER PRIMARY KEY,"
                    "divers BLOB NULL);");
    name_sql.insert("Table dates_comptable",
                    ""+m_createtable+" dates_comptable ("
                    "id_dates_comptable INTEGER PRIMARY KEY,"
                    "date_operation date NULL,"
                    "date_paiement date NULL,"
                    "date_valeur date NULL,"
                    "date_validation date NULL);");
    name_sql.insert("Table rapprochement_bancaire",
                    ""+m_createtable+" rapprochement_bancaire ("
                    "id_rap_bank   INTEGER PRIMARY KEY ,"
                    "id_compte     int(11)   null,"
                    "solde         double NULL,"
                    "divers        blob           NULL");
    name_sql.insert("Table z_version",
                    ""+m_createtable+" z_version  ("
                    "infos_version blob NULL);");
    name_sql.insert("Table informations",
                    ""+m_createtable+" informations ("
                    "id_info integer primary key,"
                    "version varchar(6) NULL);");
     name_sql.insert("Table indemnites_deplacement",
                    ""+m_createtable+" indemnites_deplacement ("
                    "id_IK integer primary key,"
                    "type varchar(50)  NULL,"
                    "value double NOT  NULL,"
                    "abattement double NOT  NULL,"
                    "indemnites_forfait double NULL);");
    name_sql.insert("Table notes",
                    ""+m_createtable+" notes ("
                    "id_notes integer primary key,"
                    "notes blob NULL);");
    name_sql.insert("Table ccam",
                    ""+m_createtable+" ccam ("
                    "id_ccam integer primary key,"
                    "code varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "abstract varchar(200) COLLATE utf8_unicode_ci NULL,"
                    "type varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "amount double NOT NULL,"
                    "reimbursment double NULL,"
                    "date date NOT NULL,"
                    "others blob NULL);");
    name_sql.insert("Table nameindex",
                   ""+m_createtable+"  nameindex ("
                   "id_index         integer primary key,"
                   "name             varchar(50)     NULL,"
                   "surname          varchar(50)     NULL,"
                   "guid             varchar(6)      NOT NULL,"
                   "divers        blob           NULL");
    name_sql.insert("Table headings",
                    ""+m_createtable+" headings ("
                    "id_headings integer primary key,"
                    "headings blob NULL,"
                    "footers blob NULL,"
                    "divers blob NULL");
    name_sql.insert("Table alertes",
                     ""+m_createtable+" alertes ("//il faut noter la table entre deux espaces
                     "id_alertes integer primary key, "
                     "alertes varchar(50) NULL,"
                     "date date NOT NULL,"
                     "id_usr int(10) NULL,"
                     "valid int(1) NULL,"
                     "isread int(1) NULL");
    }
    return name_sql;
}

bool DatabaseManager::manageNewDatabase(const int driver)
{
    QHash<QString,QString> name_sql = getDatabaseShemaHash(driver);
    
    // Mass execute SQL queries
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (WarnDebugMessage)
    qDebug() << "Hash Size "+QString::number(name_sql.size());
    QHashIterator<QString, QString> it(name_sql);
     while (it.hasNext()) {
         it.next();
         //table 
         QString table = it.key();
         table.replace("Table","");
         table.replace("\t","");
         table.replace(" ","");
         if (WarnDebugMessage)         
         qDebug() << __FILE__ << QString::number(__LINE__) << " table =" << table ;
         QSqlQuery qy(db);
         QString altertable =QString("alter table %1 modify ").arg(table);
         QString addtotable = QString("alter table %1 add ").arg(table);
         QString req = it.value();

         if (driver==Driver_SQLite) {
             QMessageBox::warning(0,tr("Warning"),
             tr("La mise à jour automatique n'est pas possible sous SQLITE.\n"
             "Veuillez renommer votre base , relancer l'application et la rapatrier sous la base créée par défaut."),
             QMessageBox::Ok);
             return false;
             //req = req.remove("COLLATE utf8_unicode_ci", Qt::CaseInsensitive).remove("UNSIGNED", Qt::CaseInsensitive);
             }
         if(!tableExists(table,driver))
         {
             if(!qy.exec(req))
             {
                 qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text();
                 return false;
                 }
             }
         else
         { 
            QStringList realcolumns;
            realcolumns = getColumnsFromTable(table);
            QStringList wishedcolumns;
            wishedcolumns = getColumnsFromHash(table,driver);
            int columnNumber = 0;
            foreach(QString column,wishedcolumns)
            {
              //qDebug() << __FILE__ << QString::number(__LINE__) << " columnNumber = " << QString::number(columnNumber);
              //qDebug() << __FILE__ << QString::number(__LINE__) << " realcolumns =" << QString::number(realcolumns.size()) ;             
                if (column.contains("PRIMARY"))
                {
                      qDebug() << __FILE__ << QString::number(__LINE__) << " continue "  ;
                      continue;
                    }       
                //qDebug() << __FILE__ << QString::number(__LINE__) << column; 
                //qDebug() << __FILE__ << QString::number(__LINE__) << " table =" << table ;        
                if(realcolumns.contains(column,Qt::CaseInsensitive))
                {
                    qDebug() << __FILE__ << QString::number(__LINE__);
                    req = altertable+column+" "+columnArgs(column,table,driver);
                    qDebug() << __FILE__ << QString::number(__LINE__) << req;
                    }
                else if (table.contains("informations") && realcolumns.contains("INFORMATION",Qt::CaseSensitive))
                {
                      req = altertable+column+" "+columnArgs(column,table,driver);
                      qDebug() << __FILE__ << QString::number(__LINE__) << " req =" << req ;
                    }
                else
                {
                    //qDebug() << __FILE__ << QString::number(__LINE__);
                    req = addtotable+column+" "+columnArgs(column,table,driver)+" after "+wishedcolumns[columnNumber-1];
                    //qDebug() << __FILE__ << QString::number(__LINE__) << " req =" << req ;
                    
                    }
                ++columnNumber;
                if(!qy.exec(req))
                {
                   qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() << "\n"
                   << qy.lastQuery();
                   return false;
                   }
                }//foreach
        }
     }
     return true;
}

bool DatabaseManager::tableExists(const QString table,const int driver)
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QStringList tables;
    if(driver == Driver_SQLite)
    {
        //TODO
        }
    if(driver == Driver_MySQL)
    {
        tables = db.tables();
        }
    if(!tables.contains(table))
    {qDebug() << __FILE__ << QString::number(__LINE__) << " table exists =" << table ;
        return false;
        }
    return true;
}

QStringList DatabaseManager::getColumnsFromTable(const QString table)
{
    QStringList tableslist;
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlRecord record = db.record(table);
    for ( int i = 0; i < record.count(); ++ i)
    {
          tableslist << record.fieldName(i);
        }
    return tableslist;
}

QStringList DatabaseManager::getColumnsFromHash(const QString table,const int driver)
{
    QStringList listofcolumns;
    QHash<QString,QString> name_sql = getDatabaseShemaHash(driver);
    QString valueofhash;
    QStringList listofkeys;
    listofkeys = name_sql.keys();
    foreach(QString key,listofkeys){
        if (key.contains(table))
        {
              valueofhash = name_sql.value(key);
            }
        }    
    QStringList listofvalueofhash;
    listofvalueofhash = valueofhash.split(+" "+table+" ");
    valueofhash = listofvalueofhash[1];
    QStringList listofargs;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " valueofhash =" << valueofhash ;
    if (!valueofhash.contains(","))
    {
          valueofhash.remove("(");
          valueofhash.remove(")");
          valueofhash.remove(";");
          if (WarnDebugMessage)
          qDebug() << __FILE__ << QString::number(__LINE__) << " valueofhash =" << valueofhash ;
          listofargs << valueofhash;
        }
    else
    {
        listofargs = valueofhash.split(",");
        qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);
        }
    for ( int i = 0; i < listofargs.size(); ++ i)
    {
          if (i==(listofargs.size()-1) && listofargs[listofargs.size()-1].contains("primary"))
          {
                break;
              }          
          QString line = listofargs[i].trimmed();qDebug() << __FILE__ << QString::number(__LINE__) << " line =" << line ;
          for ( int i = 0; i < line.count(); ++ i)
          {
                if (QString(line[i])=="(")//FIXME
                {
                      line.replace(line[i],"");
                      break;
                    }
              }
          QStringList list;
          list = line.split(" ");
          QString column = list[0];
          listofcolumns << column;
          }
    qDebug() << __FILE__ << QString::number(__LINE__) << " listofcolumns =" << listofcolumns.join(",") ;
    return listofcolumns;
}

QString DatabaseManager::columnArgs(const QString column,const QString table,const int driver)
{
    QString args;
    QHash<QString,QString> name_sql = getDatabaseShemaHash(driver);
    QString valueofhash;
    QStringList listofkeys;
    listofkeys = name_sql.keys();
    foreach(QString key,listofkeys){
        if (key.contains(table))
        {
              valueofhash = name_sql.value(key);
            }
        }    
    QStringList listofargsprepared;
    QStringList listofvalueofhash;
    listofvalueofhash = valueofhash.split(" "+table+" ");
    valueofhash = listofvalueofhash[1];
    QStringList listofargs;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " valueofhash =" << valueofhash ;
    if (!valueofhash.contains(","))
    {
          valueofhash.remove("(");
          valueofhash.remove(")");
          valueofhash.remove(";");
          listofargs << valueofhash;
        }
    else
    {
        listofargs = valueofhash.split(",");
        qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);
        }
    for ( int i = 0; i < listofargs.size(); ++ i)
    {
          if (i==(listofargs.size()-1) && listofargs[listofargs.size()-1].contains("primary"))
          {
                break;
              }          
          QString line = listofargs[i].trimmed();
          for ( int i = 0; i < line.count(); ++ i)
          {
                if (QString(line[i])=="(" && i<3)//FIXME
                {
                      line.remove(i,1);
                      break;
                    }
              }
         listofargsprepared << line;
        }
    foreach(QString item,listofargsprepared){//FIXME
        qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << item;
        item.trimmed();
        QStringList itemlist;
        itemlist = item.split(" ");
        QString itemcolumn = itemlist[0];
        itemcolumn.replace(" ","");
        if (itemcolumn == column && !item.contains(m_createtable) && !item.contains("PRIMARY",Qt::CaseSensitive))
        {
              itemlist.removeAt(0);
              args = itemlist.join(" ");
              qDebug() << __FILE__ << QString::number(__LINE__) << " args =" << args ;
            }
        }
    return args;
}

void DatabaseManager::importOldBaseToRapidecomptabilite()
{
    QDialog *dialog = new QDialog;
    QPushButton *quitButton = new QPushButton(dialog);
    quitButton->setText("Close");
    ComptaToRapid *w = new ComptaToRapid(dialog);
    QGridLayout *layout = new QGridLayout(dialog);
    layout->addWidget(w,0,0);
    layout->addWidget(quitButton,1,0);
    dialog->setLayout(layout);
    connect(quitButton,SIGNAL(pressed()),dialog,SLOT(reject()));
    
    int ret = dialog->exec();
    if (ret==QDialog::Rejected)
    {
          delete dialog;          
        }   
}

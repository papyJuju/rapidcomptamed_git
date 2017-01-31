#include "etatdesdus.h"
#include "tableau.h"

#include <common/icore.h>
#include <common/settings.h>
#include <common/constants.h>
#include <common/databasemanager.h>
#include <common/commandlineparser.h>
#include <common/print/qprintereasy.h>
#include <common/position.h>
#include <QtGui>
#include <QtCore>
#include <QtSql>
#include <QtDebug>

using namespace Common;
using namespace Constants;

static inline Common::ICore *core() {return Common::ICore::instance();}
static inline Common::Settings *settings() {return Common::ICore::instance()->settings();}
static inline Common::DatabaseManager *databaseManager() {return Common::ICore::instance()->databaseManager();}
static inline QString absolutePath() {return Common::ICore::instance()->settings()->applicationBundlePath();}
static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}//pointer on commandLineParser

etatdesdus::etatdesdus(QString & name, QString & firstname, QString & guid, QString & user)
{
    m_programName    = dataIn()->value(Common::CommandLine::CL_ProgramName).toString();
    /*m_nomRecupere    = dataIn()->value(Common::CommandLine::CL_PatientName).toString();
    m_prenomRecupere = dataIn()->value(Common::CommandLine::CL_PatientFirstName).toString();
    m_guidIn         = dataIn()->value(Common::CommandLine::CL_PatientUUID).toString();
    m_tarif          = dataIn()->value(Common::CommandLine::CL_Price).toString();
    m_codeTarif      = dataIn()->value(Common::CommandLine::CL_PriceCode).toString();
    m_user           = dataIn()->value(Common::CommandLine::CL_CallingProgramUser).toString();*/
    m_nomRecupere = name;
    m_prenomRecupere = firstname;
    m_guidIn = guid;
    m_user = user;
    qDebug() << __FILE__ << " " << QString::number(__LINE__) << " "
             << " m_programName = "+m_programName << " "
             << m_nomRecupere << " "
             << m_prenomRecupere << " "
             << m_guidIn << " "
             << m_tarif << " "
             << m_codeTarif << " "
             << m_user;
  QStringList listRecup;
    listRecup << m_programName << m_nomRecupere << m_prenomRecupere;
  
    for(int i = 0;i<listRecup.size();i++){
        if(listRecup[i].contains("_")){
            listRecup[i].replace("_"," ");
            }
        }
    
    m_programName    = listRecup[0];
    m_nomRecupere    = listRecup[1];
    m_prenomRecupere = listRecup[2];
    
  //databaseManager()->connectDrTux(DatabaseManager::Driver_MySQL);
  //databaseManager()->connectAccountancy();
  //databaseManager()->connectAccountancy(DatabaseManager::Driver_MySQL);
  /*double*/  m_sommedus       = 0; //instanciation de la somme des dus.
  /*QString*/ m_listeActesDus  =""; //instanciation de la liste des actes dus.
  setupUi(this);
  Position *position = new Position;
  position->centralPosition(this,width(),height());
  m_database	= QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  QSqlQuery queryFirstDate(m_database);
  QStringList listFirstDate;
  
  QString reqFirstDate = QString("SELECT %1 FROM %2").arg("date","honoraires");
  if(!queryFirstDate.exec(reqFirstDate)){
      qWarning() << __FILE__ << QString::number(__LINE__) << queryFirstDate.lastError().text();
      }
  while(queryFirstDate.next()){
      listFirstDate << queryFirstDate.value(0).toString();
      }
  listFirstDate.sort();
  
  QDate datedebut;
  if (listFirstDate.size()<1)
  {
      datedebut = QDate::currentDate();
    }
  else{
      datedebut = QDate::fromString(listFirstDate[0],"yyyy-MM-dd");
      }
  
  dateEdit->setDisplayFormat("dd-MM-yyyy");
  dateEdit->setDate(datedebut);// date de debut,mise au jour courant.
  dateEdit_2->setDisplayFormat("dd-MM-yyyy");
  dateEdit_2->setDate(QDate::currentDate());// date de fin,mise au jour courant.

  // Feed combos
  //debiteur
  comboBox->setEditable(true);
  comboBox->setInsertPolicy(QComboBox::NoInsert);
  QStringList listcomboBox;
  listcomboBox << "patient";
  QSqlQuery query(m_database);
  if(!query.exec("SELECT id_payeurs ,nom_payeur ,adresse_payeur ,ville_payeur ,code_postal ,telephone_payeur "
                 "FROM payeurs")){
                 qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text();

                 }
  while(query.next()){
      QString line = query.value(1).toString();
      listcomboBox << line;
  }
  comboBox->addItems(listcomboBox);
  //site
  comboBox_2->setEditable(true);
  comboBox_2->setInsertPolicy(QComboBox::NoInsert);
  QStringList listcomboBox_2;
  QString reqSite = "SELECT id_site ,site ,adresse_site ,cp_site ,ville_site ,telsite ,faxsite ,mailsite FROM sites";
  QSqlQuery querySite(m_database);
  if(!querySite.exec(reqSite)){
      qWarning() << __FILE__ << QString::number(__LINE__) << querySite.lastError().text();
  }
  while(querySite.next()){
      QString line = querySite.value(1).toString();
      listcomboBox_2 << line;
  }

      listcomboBox_2 << trUtf8("Cabinet Médical rue de Nulle Part 98000 Partout");

  comboBox_2->addItems(listcomboBox_2);
  //praticien
  comboBox_3->setEditable(true);
  comboBox_3->setInsertPolicy(QComboBox::NoInsert);
  QStringList listcomboBox_3;

  //recuperation du user preferentiel :
  QString userPref = core()->settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
          userPref.replace(" ","");
  qDebug() << __FILE__ << QString::number(__LINE__) << " user pref = "+userPref;
  listcomboBox_3 << userPref;
  QSqlQuery queryUser(m_database);
  if(!queryUser.exec("SELECT nom_usr ,login ,password ,id_drtux_usr FROM utilisateurs")){
      qWarning() << __FILE__ << QString::number(__LINE__) << queryUser.lastError().text();
  }
  while(queryUser.next()){
      QString line = queryUser.value(1).toString();
      listcomboBox_3 << line;
  }
  listcomboBox_3.removeDuplicates();
  query.finish();
  comboBox_3->addItems(listcomboBox_3);
  
  // Feed table
  QString debut = datedebut.toString("yyyy-MM-dd");
  QString fin   = QDate::currentDate().toString("yyyy-MM-dd");
  QTextDocument *document = new QTextDocument;
  QTextCursor   *cursor   = new QTextCursor(document);
  QTextTableFormat tableformat;
  QString parametrestableformat = "15,10,15,10,10,10,10,10,10";//fixed pour mémoire : "190,100,140,80,80,250,60,60,60"
  QString appelsqlchamps        = "patient,GUID ,praticien,id_site ,id_payeurs, date,acte,du,du_par";
  QStringList liste             = appelsqlchamps.split(",");
  monformat(tableformat,parametrestableformat);
  modeleConstructeur( debut,fin  ,tableformat, cursor,liste );
  textEdit->setDocument(document);
  pushButton_2->setEnabled(false);
  
  // Create connections
  connect(pushButton,  SIGNAL(pressed()),this,SLOT(close()));
  connect(pushButton_2,SIGNAL(pressed()),this,SLOT(editercourrier()));
  connect(pushButton_3,SIGNAL(pressed()),this,SLOT(trier()));
}

etatdesdus::~etatdesdus()
{}

void etatdesdus::trier()
{
  pushButton_2->setEnabled(true);
  if(m_user == ""){
      m_user = comboBox_3->currentText();
      }

      if(comboBox->currentText() == "patient"){
       if(m_nomRecupere.isEmpty()&& m_programName != "Medintux"){
           QSqlTableModel *model = new QSqlTableModel(this,m_database);
           model->setTable("nameindex");
           model->select();
           tabledialog *table = new tabledialog(model);
           if(table->exec() == QDialog::Accepted){
              m_guidIn = table->tabledialog::recupPatient();
              QSqlQuery q(m_database);
              QString req = QString("SELECT %1 FROM %2 WHERE %4 = '%5'").arg("patient","honoraires","GUID",m_guidIn);
              if(!q.exec(req)){
                  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text();
              }
              while(q.next()){
                  QString s = q.value(0).toString();
                  m_nomRecupere = s.split(",")[0];
                  m_prenomRecupere = s.split(",")[1];
                  }
              }
        }
        else if(m_nomRecupere.isEmpty()&& m_programName == "Medintux"){
            qDebug() <<  __FILE__ << QString::number(__LINE__) << "in medintux gestion";
            tableList *table = new tableList(m_listPatientName);
            if(table->exec() == QDialog::Accepted){
                  QString name = table->findPatientName();
                  m_nomRecupere = name.split(",")[0];
                  m_prenomRecupere = name.split(",")[1];
                  m_guidIn = name.split(",")[2];
                  qDebug() <<  __FILE__ << QString::number(__LINE__)
                           << " name = "+m_nomRecupere
                           << " m_prenomRecupere = "+m_prenomRecupere
                           << " m_guidIn = "+m_guidIn;
            }
        }
    }
  qDebug() << __FILE__ << QString::number(__LINE__) << " nom Recupere = "+m_nomRecupere+","+m_prenomRecupere;
  QString mois                  = "";
  QDate debut 	              = dateEdit->date();
  QDate fin 	              = dateEdit_2->date();
  QString debutstr              = debut.toString("yyyy-MM-dd");//date de debut
  QString finstr                = fin  .toString("yyyy-MM-dd");//date de fin
  QTextDocument *documenttrie  = new QTextDocument;
  QTextCursor   *cursortrie    = new QTextCursor(documenttrie);
  QTextTableFormat tableformat;
  QString parametrestableformat = "15,10,15,10,10,10,10,10,10";//fixed pour mémoire : "190,100,140,80,80,250,60,60,60"
  QString appelsqlchamps        = "patient,GUID ,praticien,id_site ,id_payeurs, date,acte,du,du_par";
  QStringList listefonction     = appelsqlchamps.split(",");
  monformat(tableformat,parametrestableformat);
  modele   (mois, debutstr , finstr ,tableformat, cursortrie,listefonction );
  textEdit->setDocument(documenttrie);
}

//----------------------------------fonction de production d une lettre de reclamation------------------------------
void etatdesdus::editercourrier() {
//-----------------------recuperation des comboBox---------------------------------------
  QString site            = comboBox_2->currentText();
  QString   enTete = "";
//-----------------------recuperation des donnees entete et pied de page --------------------
  QString reqSite = QString("SELECT %1 ,%2,%3, %4 FROM %5 WHERE %6 = '%7' ").arg("adresse_site" ,
                                                                                 "cp_site" ,
                                                                                 "ville_site" ,
                                                                                 "telsite" ,
                                                                                 "sites" ,
                                                                                 "site" ,
                                                                                 site);
  QSqlQuery queryAdress(m_database);
  if(!queryAdress.exec(reqSite)){
      qWarning() << __FILE__ << " " << QString::number(__LINE__) << " " << queryAdress.lastError().text();
      }
  while(queryAdress.next()){
      QString adress = queryAdress.value(0).toString();
      QString cp = queryAdress.value(1).toString();
      QString town = queryAdress.value(2).toString();
      QString tel = queryAdress.value(3).toString();
      enTete = site+"<br/>"+adress+"<br/>"+cp+" "+town+"<br/>";
      }
  if(site == "Cabinet Médical rue de Nulle Part 98000 Partout"){
      enTete = "Cabinet Médical";
      }
  //QSqlDatabase db = QSqlDatabase::database(Constants::DB_DRTUX);
  QString   pHeadText = "tete";
  QString   pBodyText = "body";
  QString   pFootText = "foot";
//------------------------------entete du courrier-----------------------------------------------------

  /*QString requeteTete = "SELECT Nom ,Prenom ,Adresse ,CodePostal ,Ville ,Tel_1 ,Convention ,Tel_2 ,Tel_3 ,EMail ,"
                        "NumOrdre "
                        "FROM Personnes "
                        "WHERE Login = '"+m_praticien+"'";
  QSqlQuery queryTete(db);
            queryTete.exec(requeteTete);
            while(queryTete.next()){
                QString nom            = queryTete.value(0).toString();
                QString prenom         = queryTete.value(1).toString();
                QString adresse        = queryTete.value(2).toString();
                QString cp             = queryTete.value(3).toString();
                QString ville          = queryTete.value(4).toString();
                QString tel1           = queryTete.value(5).toString();
                QString tel2           = queryTete.value(7).toString();
                QString tel3           = queryTete.value(8).toString();
                QString email          = queryTete.value(9).toString();
                QString convention     = queryTete.value(6).toString();
                QString numordre       = queryTete.value(10).toString();
                enTete                 = trUtf8("Docteur ")+nom+" "+prenom+"<br/>"
                         ""+adresse+"<br/>"
                         ""+cp+" "+ville+"<br/>"
                         ""+tel1+"<br/>"
                         ""+tel2+"<br/>"
                         ""+tel3+"<br/>"
                         ""+email+"<br/>"
                         ""+convention+"<br/>"
                         ""+trUtf8("numéro Ordre des Médecins = ")+numordre+"<br/>"
                         "<br/>"
                         ""+ville+" le "+QDate::currentDate().toString("dd-MM-yyyy")+"<br/>";
             }*/
  qDebug() << "entete ="+enTete;
  pHeadText = enTete;
  //-------------------------corps du courrier-----------------------------------------------------
  QString fichierCorps = absolutePath()+"/../resources/txt/bodyUtf8.txt";
  qDebug() << fichierCorps;
  QStringList listBody;
  QFile fileBody(fichierCorps);
    if(!fileBody.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,tr("Erreur"),trUtf8("bodyUtf8.txt ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream streamBody(&fileBody);
    while(!streamBody.atEnd()){
        QString ligne = streamBody.readLine();
        listBody      << ligne+"<br/>";
        }
  QString sommeDus            = "[sommedus]";
  QString listeDesActes       = "[listeactes]";
  QString SiteActe            = "[site]";
  QString Praticien           = "[praticien]";
  QString Signature           = "[signature]";

  QString siteDus;
  QString PratNomPrenom       ="";

  QSqlQuery querySite(m_database);
    querySite.exec("SELECT site FROM sites WHERE site           = '"+site+"'");
    while(querySite.next()){
        siteDus = querySite.value(0).toString();
        }
  /*QSqlQuery queryPrat(db);
    queryPrat.exec("SELECT Nom , Prenom FROM Personnes WHERE Login = '"+m_praticien+"'");
    while(queryPrat.next()){
        QString PratNom          = queryPrat.value(0).toString();
        QString PratPrenom       = queryPrat.value(1).toString();
        PratNomPrenom            = PratNom+" "+PratPrenom;
    }*/
  QStringList listeSignature;
  QFile fileSign(absolutePath()+"/../resources/txt/signatureUtf8.txt"); //ajout au corps de la signature
    if(!fileSign.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,tr("Erreur"),trUtf8("signatureUtf8.txt ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream streamSign(&fileSign);
    while(!streamSign.atEnd()){
        QString lignesign        =  streamSign.readLine();
        listeSignature           << lignesign+"<br/>";
    }
  QString signatureList        = listeSignature.join(" ");
    listBody.replaceInStrings    (sommeDus     ,QString::number(m_sommedus));
    listBody.replaceInStrings(listeDesActes,m_listeActesDus);
    listBody.replaceInStrings(SiteActe     ,siteDus);
    listBody.replaceInStrings(Praticien    ,PratNomPrenom);
    listBody.replaceInStrings(Signature    ,signatureList);

  pBodyText = listBody.join("");

  //-------------------------------pied du courrier ( a partir de footUtf8.txt)--------------------------------
  QStringList listePied;
  QFile fileFoot(absolutePath()+"/../resources/txt/footUtf8.txt");
    if(!fileFoot.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,tr("Erreur"),trUtf8("footUtf8.txt ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream streamFoot(&fileFoot);
    while(!streamFoot.atEnd()){
       QString lignepied = streamFoot.readLine();
       listePied        << lignepied;
    }
  QString piedListe        = listePied.join("");
  pFootText                = piedListe;

  //--------print the document-------------------------------------------------------------------
  QPrinterEasy pe;
          pe.askForPrinter();
          pe.setHeader( pHeadText );
          pe.setFooter( pFootText );
          pe.setContent( pBodyText );
          pe.previewDialog();

  // if (printer==0) return;
  // if (printer->exec()== QDialog::Accepted) {};

}
//-------formating function------------------------------------------------------------------
QTextTableFormat etatdesdus::monformat(QTextTableFormat &tableFormat,QString & parametrespourtableformat){
     QString parametrespourfonction = parametrespourtableformat;
     QStringList listeparametres    = parametrespourfonction.split(",");
     tableFormat.setBackground(QColor("#e0e0e0"));
     tableFormat.setAlignment(Qt::AlignCenter);
     tableFormat.setCellPadding(2);
     tableFormat.setCellSpacing(2);
     QVector<QTextLength> contraintes;
     for(int i = 0;i < listeparametres.size() ; i++){
     contraintes << QTextLength(QTextLength::PercentageLength, listeparametres[i].toInt());
     }

     tableFormat.setColumnWidthConstraints(contraintes);
     return tableFormat;
}
//-------------------------------fonction qui remplit le tableau apres connect trier--------------------------
  void etatdesdus::modele(QString & mois,
                          QString & jour1 ,
                          QString & dernierjour ,
                          QTextTableFormat & tableFormattableau,
                          QTextCursor *cursortriefonction,
	                  QStringList & listefonction )

{
  //QSqlDatabase db = QSqlDatabase::database(Constants::DB_DRTUX);

  double  sommedusDouble = 0.00;
  QStringList listeActes;
  //----------------------------recuperation des comboBox-----------------------------------------------------------
  QString          debiteur                   = comboBox  ->currentText();
  QString          site                       = comboBox_2->currentText();
  /*QString*/      m_praticien                = comboBox_3->currentText();
  //---------------------------instanciation des variables Site, Praticien et Débiteur-------------------------------
  QString          idSite   = "";
  QString          idPayeur = "";
  int              idUser   = 0;


  QSqlQuery querySite(m_database);
    querySite.exec("SELECT id_site FROM sites WHERE site           = '"+site+"';");
    while(querySite.next()){
        idSite = querySite.value(0).toString();
        }
  QSqlQuery queryDeb(m_database);
    queryDeb.exec("SELECT id_payeurs FROM payeurs WHERE nom_payeur = '"+debiteur+"';");
    while(queryDeb.next()){
        idPayeur = queryDeb.value(0).toString();
        }
    //---condition si le payeur est le patient----------------------------
        if(comboBox->currentText() == "patient"){
            qDebug() << __FILE__ << " dans patient";

            QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4' AND %5 = '%6'").arg("id_payeurs",
                                                                                          "honoraires",
                                                                                          "patient",
                                                                                          m_nomRecupere+","+m_prenomRecupere,
                                                                                          "GUID",
                                                                                          m_guidIn);

            qDebug() << __FILE__ << QString::number(__LINE__) << " requete = " << req;
            QSqlQuery queryDeb2(m_database);
            if(!queryDeb2.exec(req)){
                qWarning() << __FILE__ << " " << QString::number(__LINE__) << " " << queryDeb2.lastError().text();
            }

        while(queryDeb2.next()){
           idPayeur = queryDeb2.value(0).toString();
           }
        qDebug() << __FILE__ << QString::number(__LINE__) << " dans patient idPayeur = "+idPayeur;
                }//fin condition patient
  QSqlQuery queryUser(m_database);
    queryUser.exec("SELECT id_usr FROM utilisateurs WHERE login  = '"+m_user+"';");
    while(queryUser.next()){
       idUser = queryUser.value(0).toInt();
    }
  if(site == "Cabinet Médical rue de Nulle Part 98000 Partout"){
      idSite = "";
      }

  QString totalName = m_nomRecupere+","+m_prenomRecupere;
  qDebug() << __FILE__ << QString::number(__LINE__);
  qDebug() << "m_nomRecupere,m_prenomRecupere = "+m_nomRecupere+","+m_prenomRecupere;
  qDebug() << "m_guipatient = "+m_guidpatient;
  qDebug() << "m_guidIn = "+m_guidIn;
  qDebug() << "site = "+site;
  qDebug() << "prat = "+m_praticien;
  qDebug() << "deb = "+debiteur;
  qDebug() << "idsite = "+idSite;
  qDebug() << "idpayeur = "+idPayeur;
  qDebug() << "idUser = "+QString::number(idUser);
  m_guidpatient = "";
  //----------------------------recuperation des arguments de fonction----------------------------------------------
  QString          cemois                     = mois;
  QString          un                         = jour1;
  QString          deux                       = dernierjour;
  QTextTableFormat tableFormat                = tableFormattableau;
  QTextCursor      *cursortrieinfonction      = cursortriefonction;
  QStringList      listforquery               = listefonction;
  //---------------------------requete de tri-------------------------------------------------------------------------
  QString          phraseSql                  = "patient,GUID,praticien,id_site ,id_payeurs ,date,acte,du,du_par";
  QString          phraseTable                = "honoraires";
  QString	   requeteTriDesDus	      = "SELECT "+phraseSql+" FROM "+phraseTable+" "
	                                      " WHERE date BETWEEN '"+un+"' AND '"+deux+"' "
	                                      " AND id_site   =  '"+idSite+"' "
	                                      " AND id_payeurs =  '"+idPayeur+"' "
	                                      " AND id_usr    =  '"+QString::number(idUser)+"' "
	                                      " AND patient = '"+totalName+"' "
	                                      " AND du > 0";
 qDebug() << __FILE__ << QString::number(__LINE__) << " requeteTriDesDus = " << requeteTriDesDus;
  m_model = new QSqlQueryModel;
    m_model ->setQuery(requeteTriDesDus,m_database );
         if(m_model->rowCount(QModelIndex())!= 0){
             QTextTable *table = cursortrieinfonction->insertTable(m_model->rowCount(QModelIndex()),
                                                                   listforquery.size(),
                                                                   tableFormat);
         for(int i=0;i< m_model->rowCount(QModelIndex());i++){
             QStringList liste; // liste des données de la ligne
    	 for (int a = 0;a < listforquery.size(); a++){
           QString str = m_model->data(m_model->index(i,a)).toString();
           if(str.contains("/")){//a cause de la methode Maeker d'entree des recettes.
             QStringList listetrois = str.split("//");
             str = listetrois[0];
             }
           liste << ""+str+"";
             }

         for(int j= 0;j<listforquery.size();j++){
           QTextTableCell cell = table->cellAt(i,j);
           QTextCursor cellCursor = cell.firstCursorPosition();
           cellCursor.insertText(liste[j]);
           }
         sommedusDouble += liste[7].toDouble();
         listeActes      << liste[0]+","+liste[5]+","+liste[6]+","+liste[7]+" euro";
    }//fin de la boucle ligne

    }
    /*double*/  m_sommedus        = sommedusDouble;
    /*QString*/ m_listeActesDus   = listeActes.join("<br/>");
    qDebug() << __FILE__ << " somme des dus = "+QString::number(m_sommedus);
    qDebug() << m_listeActesDus;

}

void etatdesdus::modeleConstructeur(QString & jour1 ,
                                    QString & dernierjour ,
                                    QTextTableFormat & tableFormattableau,
                                    QTextCursor *cursortriefonction,
	                            QStringList & listefonction ){
  QString          un                         = jour1;
  QString          deux                       = dernierjour;
  QTextTableFormat tableFormat                = tableFormattableau;
  QTextCursor      *cursortrieinfonction      = cursortriefonction;
  QStringList      listforquery               = listefonction;
  //---------------------------requete de tri-------------------------------------------------------------------------
  QString          phraseSql                  = "patient,GUID,praticien,id_site ,id_payeurs ,date,acte,du,du_par";
  QString          phraseTable                = "honoraires";
  QString		 requeteTriDesDus     = "SELECT "+phraseSql+" FROM "+phraseTable+" "
	                                        " WHERE date BETWEEN '"+un+"'AND '"+deux+"' "
	                                        " AND du > 0";
  m_model = new QSqlQueryModel;
    m_model ->setQuery(requeteTriDesDus,m_database );
         if(m_model->rowCount(QModelIndex())!= 0){
             QTextTable *table = cursortrieinfonction->insertTable(m_model->rowCount(QModelIndex()),
                                                                   listforquery.size(),
                                                                   tableFormat);
             for(int i=0;i< m_model->rowCount(QModelIndex());i++){
                 QStringList liste; // liste des données de la ligne
                 m_listPatientName << m_model->data(m_model->index(i,0)).toString()
                                    +","+m_model->data(m_model->index(i,1)).toString();//pour tableau de tri
                 m_listPatientName.removeDuplicates();
                 m_listPatientName.sort();
    	         for (int a = 0;a < listforquery.size(); a++){
                    QString str = m_model->data(m_model->index(i,a)).toString();
                    if(str.contains("/")){//a cause de la methode Maeker d'entree des recettes.
                        QStringList listetrois = str.split("//");
                        str = listetrois[0];
                    }
                 liste << str;

             }

            for(int j= 0;j<listforquery.size();j++){
               QTextTableCell cell = table->cellAt(i,j);
               QTextCursor cellCursor = cell.firstCursorPosition();
               cellCursor.insertText(liste[j]);
             }
        }
    }
}



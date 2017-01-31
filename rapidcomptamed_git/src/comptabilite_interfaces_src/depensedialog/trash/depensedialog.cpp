#include "depensedialog.h"
#include "treeviewdelegate.h"
#include "analyse.h"
#include "../../common/configure/utilisateur.h"

#include <common/icore.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/constants.h>
#include <common/position.h>
#include <common/configure/util.h>




using namespace Common;
using namespace Constants;
enum { WarnDebugMessage = true };
static inline QString absolutePath() {return Common::ICore::instance()->settings()->applicationBundlePath();}
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}//pointer on commandLineParser
static inline QString pixmapsPath(){return Common::ICore::instance()->settings()->iconPath();}
static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}

depensedialog::depensedialog()
{
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  setAttribute( Qt::WA_AlwaysShowToolTips);
  position()->centralPosition(this,width(),height());
  m_depensetools = new Depensetools(this);
  if (WarnDebugMessage)
     qDebug() << "application name ="+qApp->applicationName();

  m_utildialogdep = new utilisateurpref;
  m_user = dataIn()->value(Common::CommandLine::CL_CallingProgramUser).toString();

  pushButton_4   -> setShortcut(QKeySequence("Ctrl+Q"));
  pushButton     -> setEnabled(true);
  pushButton     -> setIcon(QIcon(pixmapsPath() +"/pixmap/vert.png"));
  pushButton_2 ->setToolTip(trUtf8  ("Bouton d'aide,\ncliquez ensuite sur la zone voulue."));
  pushButton   ->setWhatsThis(trUtf8("Ce bouton entre la dépense affichée et des paramètres\ndans la base."));
  pushButton_3 ->setWhatsThis(trUtf8("Efface la ligne surlignée."));
  pushButton_4 ->setWhatsThis(trUtf8("Bouton de fermeture"));
  pushButton_5 ->setWhatsThis(trUtf8("Ce bouton permet de valider après coup la réception \ndes factures correspondant aux dépenses."));
  checkBox_5   ->setWhatsThis(trUtf8("Cochez cette case si vous avez reçu\nla facture correspondant à la dépense"));
  dateEdit     ->setWhatsThis(trUtf8("Choisir la date de la dépense"));
  label_5      ->setWhatsThis(trUtf8("Choisir la date de la dépense"));
  groupBox     ->setWhatsThis(trUtf8("Choisir le type de règlement"));
  label_2      ->setWhatsThis(trUtf8("Choisir le pourcentage professionnel de la dépense."));
  comboBox_2   ->setWhatsThis(trUtf8("Choisir le pourcentage professionnel de la dépense."));
  label        ->setWhatsThis(trUtf8("Choisir le type de dépense dans le menu selon les items de la 2035."));
  comboBox     ->setWhatsThis(trUtf8("Choisir le type de dépense dans le menu selon les items de la 2035."));
  label_3      ->setWhatsThis(trUtf8("Mettre la valeur de la dépense."));
  lineEdit     ->setWhatsThis(trUtf8("Mettre la valeur de la dépense."));
  label_6      ->setWhatsThis(trUtf8("Exemple :""salaire de Mme Untel."));
  detailEdit   ->setWhatsThis(trUtf8("Exemple :""salaire de Mme Untel."));
  tableView    ->setWhatsThis(trUtf8("Contenu de la base mouvements."));
  label_7      ->setWhatsThis(trUtf8("Choix du compte bancaire.\nLe compte préférentiel est en premier."));
  comboBox_4   ->setWhatsThis(trUtf8("Choix du compte bancaire.\nLe compte préférentiel est en premier."));

  dateEdit      ->setDate(QDate::currentDate());
  bankingtransactiondateEdit ->setDate(QDate::currentDate());
  

//----appel des bases----------------------
    //m_db1 		   = QSqlDatabase::database(Constants::DB_DRTUX);  //DrTuxTEST
    m_accountancyDb 		   = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);//comptabilite

//---remplir depense.ini avec table mouvements_disponibles----------------
  remplirdepensesini();
//--------------------------------------------------------------------
//completion
  if (!fillCompletionList())
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to get COMPLETION LIST" ;
      }

  checkBox ->setCheckable(true);
  checkBox_2->setCheckable(true);
  checkBox_3->setCheckable(true);
  checkBox_4->setCheckable(true);
  checkBox_5->setCheckable(true);

  checkBox->setAutoExclusive(true);
  checkBox_2->setAutoExclusive(true);
  checkBox_3->setAutoExclusive(true);
  checkBox_4->setAutoExclusive(true);
//------remplir depenses---------------
  comboBox->setEditable(1);
  //comboBox->setToolTip("Apports praticiens");
  comboBox->setInsertPolicy(QComboBox::NoInsert);
  QSqlQuery queryDepensesTypes(m_accountancyDb);
  QString   typesDepenses = "SELECT type,libelle FROM mouvements_disponibles";
            queryDepensesTypes.exec(typesDepenses);
            int nKey = 0;
            while(queryDepensesTypes.next()){
                nKey ++;
                QString lineType = queryDepensesTypes.value(0).toString();
                QString lineLibelle = queryDepensesTypes.value(1).toString();
                m_typeMap.insert(nKey,lineType);
                m_spendingsMap.insert(nKey,lineLibelle);
            }
  if (WarnDebugMessage)
     qDebug() << "map size = "+QString::number(m_spendingsMap.size());
  QMapIterator<int,QString> it(m_spendingsMap);
  while(it.hasNext()){
      it.next();
      int key = it.key();
      QString typeSpendings = m_typeMap.value(key);
      QString libelleSpendings = it.value();
      if (WarnDebugMessage)
         qDebug() << typeSpendings+" "+libelleSpendings;
      if(typeSpendings == QString::number(INFLOW)){
          comboBox->addItem(QIcon(pixmapsPath()+"/plusOrange.png"),libelleSpendings,Qt::ToolTipRole);
          }
      else if(typeSpendings == QString::number(REDUCED)) {
          comboBox->addItem(QIcon(pixmapsPath()+"/minus.png"),libelleSpendings,Qt::ToolTipRole);
          }
      else{
          QMessageBox::warning(0,tr("Erreur"),
          trUtf8("Erreur mapping dépenses")+QString::number(__LINE__)+__FILE__,QMessageBox::Ok);
          }

  }


//---------remplissage comboBox pourcentages-------------------------------
  comboBox_2->setEditable(1);
  comboBox_2->setInsertPolicy(QComboBox::NoInsert);

  QString percentsStr = Common::ICore::instance()->settings()->value(S_POURCENTAGES_DEPENSES).toString();
  m_depensespourcentages = percentsStr.split(" ");
  comboBox_2->addItems(m_depensespourcentages);

//--------choix user preferentiel---------------
  QString utilisateurpref = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
                     m_id = Common::ICore::instance()->settings()->value(S_ID_UTILISATEUR_CHOISI).toInt();

  QSqlQuery query3(m_accountancyDb);
            query3.exec("SELECT id_usr,login FROM utilisateurs WHERE id_usr = '"+QString::number(m_id)+"'");
            if(query3.next()){
               m_idusr     = query3.value(0).toInt();
               m_loginpref = query3.value(1).toString();
            }


  if(m_user == "" && m_loginpref == ""){
        m_u             = new utilisateur(this,"admin");

	QString compta;
	int retno = 0;
	 QMessageBox msgBox;
	 QMessageBox msgBoxNo;

 	msgBox.setText(trUtf8("Il n'est pas possible d'obtenir les utilisateurs\nde la table utilisateurs de comptabilite,\n"
 			      "ou l'utilisateur préférentiel est mal renseigné dans config.ini.\n"
			      "Voulez vous paramétrer les utilisateurs de la comptabilité ?"));
 	//msgBox.setInformativeText(trUtf8("Paramétrage de la table comptabilité"));
 	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
 	QPushButton *elseButton = msgBox.addButton(trUtf8("Else"),QMessageBox::AcceptRole);
 	             elseButton-> setToolTip(trUtf8("Paramétrer l'utilisateur préférentiel.\n"
 	                                            "Vous devez d'abord paramétrer les utilisateurs."));
 	msgBox.setDefaultButton(QMessageBox::Yes);
 	int ret = msgBox.exec();
 	switch(ret){
 	    case QMessageBox::Yes:
               m_u -> show();
               connect(m_u,SIGNAL(destroyed()),this,SLOT(remplirComboUtilisateurs()));
            break;
 	    case QMessageBox::No:
 	       msgBox.close();
 	    break;
 	    case QMessageBox::AcceptRole:
 	       msgBoxNo.setText(trUtf8("Voulez-vous paramétrer l'utilisateur préférentiel dans config.ini ?"));
 	       msgBoxNo.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
 	       msgBoxNo.setDefaultButton(QMessageBox::Yes);
 	       retno = msgBoxNo.exec();

 		switch(retno){
 		    case QMessageBox::Yes:
 		        m_utildialogdep->show();
 		    break;
 		    case QMessageBox::No:
 		       msgBoxNo.close();
 		       msgBox.close();
 		    break;
 		    default:
 		    break;
 		}
 	   break;
 	   default:
 	   break;
 	}

  }
  m_listelogin << m_loginpref;
  //choix user
  comboBoxLogins->setEditable(1);
  comboBoxLogins->setInsertPolicy(QComboBox::NoInsert);
  QSqlQuery query4(m_accountancyDb);
  if (!query4.exec("SELECT id_usr,login FROM utilisateurs"))
  {
        qWarning() << __FILE__ << QString::number(__LINE__) << query4.lastError().text() ;
      }
  while(query4.next()){
      QString idusr = query4.value(0).toString();
      m_login = query4.value(1).toString();
      m_listelogin << m_login;
      m_hashIdusrLogin.insert(idusr,m_login);
  }
  m_listelogin.removeDuplicates();
  if(m_listelogin.size() < 1){
      m_listelogin << trUtf8("Docteur Générique");
      label_4   -> setStyleSheet("*{color:red}");
      label_4   -> setText(trUtf8("Utilisateurs \nnon configurés"));
      label_4   -> setWordWrap(true);
      label_4   -> setToolTip(trUtf8("Veuillez les configurer dans les paramétrages"));
  }

  comboBoxLogins->addItems(m_listelogin);
//------------remplissage du comboBox_4 comptes bancaires---------------------------------------
    comboBox_4                   -> setEditable(1);
    comboBox_4                   -> setInsertPolicy(QComboBox::NoInsert);
    QString requeteCompte = "SELECT id_compte,id_usr,libelle,solde_initial FROM comptes_bancaires";
    QStringList listLibelleCompte;
    m_solde_initial = 0.00; //initialisation double solde_initial
    QSqlQuery queryReqComptes(m_accountancyDb);
              if(!queryReqComptes.exec(requeteCompte)){
                  QMessageBox::warning(0,tr("Erreur"),trUtf8("Erreur =")
                  +queryReqComptes.lastError().text(),QMessageBox::Ok);
              }
              while(queryReqComptes.next()){
                 QString reqLibelle = queryReqComptes.value(2).toString();
                 listLibelleCompte << reqLibelle;
                 m_hashCompte.insert(queryReqComptes.value(0).toString(),reqLibelle);
                 m_solde_initial = queryReqComptes.value(3).toDouble();

              }
    if(m_loginpref.isEmpty() == false){
        if(m_hashCompte.count(QString::number(m_id)) > 1){
            QMessageBox::information(0,trUtf8("Information"),trUtf8("Plusieurs comptes bancaires possibles pour "
                                                   "cet utilisateur,\nveuillez en choisir un"),QMessageBox::Ok);
        }
        else{
            QString reqComptePref = m_hashCompte.value(QString::number(m_id));
            if(!reqComptePref.isEmpty()){
                listLibelleCompte.prepend(reqComptePref);
            }

        }

    }
    comboBox_4 -> addItems(listLibelleCompte);
  //recuperation des annees
  //initialisation liste
  QStringList listOfYears;
  QString lastYearAvailable = QString::number(QDate::currentDate().year());
  listOfYears << lastYearAvailable;
  QSqlQuery qYears(m_accountancyDb);
  QString reqYear = QString("SELECT %1 FROM %2").arg("date","mouvements");
  if (!qYears.exec(reqYear))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << qYears.lastError().text();
  	   
      }
  while (qYears.next())
  {
  	QDate date = qYears.value(0).toDate();
  	QString year = QString::number(date.year());
  	listOfYears << year;
      }
  listOfYears.removeDuplicates();
  listOfYears.sort();
  yearComboBox->addItems(listOfYears);
  yearComboBox->setCurrentIndex(listOfYears.size()-1);
  // test verrouillage user
  QString thisYear = QString::number(QDate::currentDate().year());
  QString filter = userAndYearFilter(m_id,thisYear);
  bool testLock = security()->isUserLocked(m_listelogin[0]);
  //hash mouvements_disponibles
  QString reqMovDispo = QString("select %1,%2 from %3").arg("id_mvt_dispo","libelle","mouvements_disponibles");
  QSqlQuery queryForHashMovDisp(m_accountancyDb);
  if (!queryForHashMovDisp.exec(reqMovDispo))
  {
        qWarning() << __FILE__ << QString::number(__LINE__) << queryForHashMovDisp.lastError().text() ;
      }
   while (queryForHashMovDisp.next())
   {
       QString id = queryForHashMovDisp.value(0).toString();
       QString libelle = queryForHashMovDisp.value(1).toString();       
       m_hashForMovDips.insert(id,libelle);
       }
  m_modele = new QSqlTableModel(this,m_accountancyDb);
  m_model  = new QSqlTableModel(this,m_accountancyDb);
  m_model  ->setTable("mouvements");

  m_model->setHeaderData(LIBELLE,Qt::Horizontal,trUtf8("Poste\ncomptable"));
  m_model->setHeaderData(DATE,Qt::Horizontal,trUtf8("date saisie"));
  m_model->setHeaderData(DATE_VALEUR,Qt::Horizontal,trUtf8("Date\nopération"));
  m_model->setHeaderData(REMARQUE,Qt::Horizontal,trUtf8("Mode\nde paiement"));
  m_model->setHeaderData(VALIDE,Qt::Horizontal,trUtf8("facture\nreçue"));
  m_model->setHeaderData(VALIDATION,Qt::Horizontal,trUtf8("pointage\nbancaire"));
  m_model->setHeaderData(DETAIL,Qt::Horizontal,trUtf8("Détail\nécriture"));
  m_model->setFilter(filter);
  if (!testLock)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "not locked" ;
    	  m_model->select();
    	  if (WarnDebugMessage)
          qDebug() << __FILE__ << QString::number(__LINE__) << m_model->query().lastQuery();
    	  tableView->setModel(m_model);
        }
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  tableView->setSortingEnabled(true);
  tableView->setColumnHidden(ID_MOUVEMENT,true);
  tableView->setColumnHidden(ID_MVT_DISPO,true);
  tableView->setColumnHidden(ID_USR,true);
  tableView->setColumnHidden(ID_COMPTE,true);
  tableView->setColumnHidden(TYPE,true);
  tableView->setColumnHidden(TRACABILITE,true);
  tableView->setItemDelegateForColumn(DATE,new DateEditTreeViewFirstDelegate(this));
  tableView->setItemDelegateForColumn(DATE_VALEUR,new DateEditTreeViewFirstDelegate(this));
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableView->horizontalHeader()->setStretchLastSection ( true );
  tableView->horizontalHeader()->setResizeMode(VALIDATION,QHeaderView::Stretch);
  tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);

  lineEdit->setFocus();
  //tableView->setFocus();
//----------------------------------------------------------------------------------------------
//--------------------------connect--------------------------------------------------------------
  connect(pushButton,  SIGNAL(pressed()),this,SLOT(enregistredepense()));
  connect(pushButton_2,SIGNAL(pressed()),this,SLOT(whatsthismode()));
  connect(pushButton_3,SIGNAL(pressed()),this,SLOT(effacer()));
  connect(pushButton_4,SIGNAL(pressed()),this,SLOT(close()));
  connect(pushButton_5,SIGNAL(pressed()),this,SLOT(valide()));
  connect(bankDebitButton,SIGNAL(pressed()),this,SLOT(bankDebit()));
  connect(comboBox,SIGNAL(highlighted(int)),this,SLOT(showToolTip(int)));
  connect(comboBoxLogins,SIGNAL(currentIndexChanged (int)),this,SLOT(loginIsModified(int)));
  connect(this,SIGNAL(destroyed(QObject*)),this,SLOT(fillFileCompletionList(QObject*)));
  connect(detailEdit,SIGNAL(returnPressed()),this,SLOT(setCompletionList()));
  connect(yearComboBox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(refreshWithYear(const QString&)));
  connect(analyseButton,SIGNAL(pressed()),this,SLOT(analyseYearly()));
  connect(errorButton,SIGNAL(pressed()),this,SLOT(showErrors()));    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------fin du constructeur------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////

depensedialog::~depensedialog(){
    delete   m_utildialogdep;
   // delete   m_u;
    delete   m_model;
    delete   m_modele;
    m_id     = 0;
    m_idusr  = 0;
    m_id_usr = 0;
    m_compte = 0;
    m_loginpref  = "";
    m_login        = "";
    m_listelogin          .clear();
    m_depensespourcentages.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////--------------enregistrer la depense-----------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
void depensedialog::enregistredepense()
{
  if(m_spendingsMap.size() < 1){
      QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("depense.ini est vide.\nProblème de récupération de la "
                                                   "table mouvements_disponibles."),QMessageBox::Ok);
  }
//-----------variables----------------------------------------
  QString depense;
  QString pourcentage;
  QString utilisateur;
  int valide = 0;
  if(checkBox_5->isChecked()){
      valide = 1;
  }
  int pourcent;
  double valeur;
  double valeurdepense;
  QString detail;
  int idmvt = 0;
  int idmvtdispo = 0;
  QString libelle;
  int type = int(INFLOW);//valeur en positif (apport) ou négatif (depense)

//------------fin variables-------------------------------------
  depense   = comboBox->currentText();
  libelle   = depense.toLatin1();
  idmvt = comboBox->currentIndex();
  idmvtdispo = idmvt+1;
  if (WarnDebugMessage)  
  qDebug() << __FILE__ << QString::number(__LINE__) << " idmvt =" << QString::number(idmvt) ;
//---quelles valeurs sont positives(les dépenses) et négatives (les apports), il s'agit de débit...
  QSqlQuery queryplusoumoins(m_accountancyDb);
            queryplusoumoins.exec("SELECT type FROM mouvements_disponibles WHERE id_mvt_dispo = '"+QString::number(idmvtdispo)+"'");
  if(queryplusoumoins.next()){
      type  = queryplusoumoins.value(0).toInt();
  }
//fin type
//---------recuperation donnees inscrites par l'utilisateur----------------------------------
  QDate currentdate    = QDate::currentDate();
  //QString datesaisiestring = currentdate.toString("yyyy-MM-dd");
  
  QDate daterecup      = dateEdit->date();
  QString datesaisiestring     = daterecup.toString("yyyy-MM-dd");
  QDate datetransaction = bankingtransactiondateEdit -> date();
  QString datetransactionstring = datetransaction.toString("yyyy-MM-dd");

  pourcentage          = comboBox_2->currentText();
  pourcent             = pourcentage.toInt();

  utilisateur          = comboBoxLogins->currentText();
  valeur               = lineEdit->text().toDouble();
  valeurdepense        = valeur*pourcent/100;
  detail               = detailEdit->text().toLatin1();
  //--particularites semantiques
     if(detail.contains("'")){
         detail.replace("'","''");
     }
//fin recuperation donnees
  QSqlQuery query1(m_accountancyDb);
            query1.exec("SELECT id_usr,id_drtux_usr FROM utilisateurs WHERE login = '"+utilisateur+"'");
  if(query1.next()){
      //iduser = query1.value(1).toInt();
      m_id_usr = query1.value(0).toInt();
      }

  QSqlQuery query2(m_accountancyDb);
            query2.exec("SELECT id_compte FROM comptes_bancaires WHERE id_usr = "+QString::number(m_id_usr)+"");
  if(query2.next()){
      m_compte = query2.value(0).toInt();
      }

  QMessageBox *message2;

  QSqlQuery query(m_accountancyDb);

  if(checkBox->isChecked()){
      checkBox_2->setChecked(false);
      checkBox_3->setChecked(false);
      checkBox_4->setChecked(false);
      query.exec("INSERT INTO mouvements (id_mvt_dispo,id_usr,id_compte,type,libelle,date,date_valeur,montant,remarque,valide,tracabilite,validation,detail)"
             "VALUES ("+QString::number(idmvtdispo)+","+QString::number(m_id_usr)+","+QString::number(m_compte)+","+QString::number(type)+","
             "'"+libelle+"','"+datesaisiestring+"','"+datetransactionstring+"',"+QString::number(valeurdepense)+",'esp',"+QString::number(valide)+",0,0,'"+detail+"')");
      emit rafraichir();
      emit messagebox();
      }
  else if(checkBox_2->isChecked()){
    checkBox->setChecked(false);
    checkBox_3->setChecked(false);
    checkBox_4->setChecked(false);
    query.exec("INSERT INTO mouvements" "(id_mvt_dispo,id_usr,id_compte,type,libelle,date,date_valeur,montant,remarque,valide,tracabilite,validation,detail)"
               "VALUES ("+QString::number(idmvtdispo)+","+QString::number(m_id_usr)+","+QString::number(m_compte)+","+QString::number(type)+","
	       "'"+libelle+"','"+datesaisiestring+"','"+datetransactionstring+"',"+QString::number(valeurdepense)+",'chq',"+QString::number(valide)+",0,0,'"+detail+"')");
    emit rafraichir();
    emit messagebox();
    }
  else if(checkBox_3->isChecked()){
    checkBox->setChecked(false);
    checkBox_2->setChecked(false);
    checkBox_4->setChecked(false);
    query.exec("INSERT INTO mouvements (id_mvt_dispo,id_usr,id_compte,type,libelle,date,date_valeur,montant,remarque,valide,tracabilite,validation,detail)"
             "VALUES ("+QString::number(idmvtdispo)+","+QString::number(m_id_usr)+","+QString::number(m_compte)+","+QString::number(type)+","
             "'"+libelle+"','"+datesaisiestring+"','"+datetransactionstring+"',"+QString::number(valeurdepense)+",'cb_vir',"+QString::number(valide)+",0,0,'"+detail+"')");
    emit rafraichir();
    emit messagebox();
  }
  else if(checkBox_4->isChecked()){
    checkBox->setChecked(false);
    checkBox_2->setChecked(false);
    checkBox_3->setChecked(false);
    query.exec("INSERT INTO mouvements (id_mvt_dispo,id_usr,id_compte,type,libelle,date,date_valeur,montant,remarque,valide,tracabilite,validation,detail)"
               "VALUES ("+QString::number(idmvtdispo)+","+QString::number(m_id_usr)+","+QString::number(m_compte)+","+QString::number(type)+","
               "'"+libelle+"','"+datesaisiestring+"','"+datetransactionstring+"',"+QString::number(valeurdepense)+",'prelevt',"+QString::number(valide)+",0,0,'"+detail+"')");
    emit rafraichir();
    emit messagebox();
  }
  else
  {
      message2->warning(0,trUtf8("Erreur"),trUtf8("Vous avez oublié de choisir le type de paiement !"),QMessageBox::Ok);
      return;
      }
   

  //--------------insertion dans table rapprochement_bancaire du solde actuel--------------------------
  QString nouveauSolde = "";
  QString reqSolde = "SELECT solde FROM rapprochement_bancaire";
  int idCompte = m_hashCompte.key(comboBox_4->currentText()).toInt();
  QSqlQuery querySolde(m_accountancyDb);
  QSqlQuery queryInsertBank(m_accountancyDb);
  QStringList listSolde;
      querySolde.exec(reqSolde);
      while(querySolde.next()){
          QString solde = querySolde.value(0).toString();
          listSolde << solde;
      }
      double resultSolde = 0.00;
      if((listSolde.size() == 0 ) == false){
          resultSolde = listSolde.last().toDouble();
      }
      else{
          resultSolde = m_solde_initial;
      }
   switch(type){
       case 0 :
           nouveauSolde = QString::number(resultSolde + valeurdepense);
           break;
       case 1 :
           nouveauSolde = QString::number(resultSolde-valeurdepense);
           break;
       default :
           break;    
       }
      
   QString requeteInsertSolde ="INSERT INTO rapprochement_bancaire (id_compte,solde) "
                               "VALUES("+QString::number(idCompte)+",'"+nouveauSolde+"')";
   if(!queryInsertBank.exec(requeteInsertSolde)){
       if (WarnDebugMessage)
     qDebug() << "Erreur ="+queryInsertBank.lastError().text()+"\n"+requeteInsertSolde;
   }
}

/*void depensedialog::voirdepenses()
{
  emit rafraichir();
}*/

void depensedialog::effacer()
{
  QModelIndex index                   = tableView->QAbstractItemView::currentIndex();
  if(!index.isValid()){
      QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("vous avez oublié de sélectionner une ligne à effacer"),QMessageBox::Ok);
  }
  int i                               = index.row();
  if(tableView->model()==m_model){
      if(m_model -> removeRows(i,1,QModelIndex()) == false){
          qWarning() << __FILE__ << QString::number(__LINE__) << "lastError = " << m_model->lastError().text();
          }
      m_model                          -> submitAll();
      rafraichir();
      QMessageBox::information(0,trUtf8("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
  }
  else if(tableView->model()==m_modele){
      if(m_modele -> removeRows(i,1,QModelIndex()) == false){
          qWarning() << __FILE__ << QString::number(__LINE__) << "lastError = " << m_modele->lastError().text();
          }
      m_modele                       -> submitAll();
      rafraichir();
      QMessageBox::information(0,trUtf8("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
  }

  else{
      QMessageBox::critical(0,trUtf8("Erreur"),trUtf8("Impossible d'effacer la ligne!"),QMessageBox::Ok);
  }

}

void depensedialog::rafraichir()
{
  QString login = comboBoxLogins->currentText();
  if (WarnDebugMessage)
     qDebug() << __FILE__ << QString::number(__LINE__) << " login =" << login ;
  int id = 1;
  QSqlQuery qId(m_accountancyDb);
  QString reqId = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                  .arg("id_usr","utilisateurs","login",login);
  if (!qId.exec(reqId))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << qId.lastError().text() ;
      }
  while (qId.next())
  {
  	id = qId.value(0).toInt();
      }
  if (WarnDebugMessage)
     qDebug() << __FILE__ << QString::number(__LINE__) << " id =" << QString::number(id) ;
  QString year = yearComboBox->currentText().trimmed();
  m_model  -> clear();
  QString filter = userAndYearFilter(id,year);
  if (WarnDebugMessage)
     qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
  m_modele ->setTable("mouvements");
  m_modele->setHeaderData(LIBELLE,Qt::Horizontal,trUtf8("Poste\ncomptable"));
  m_modele->setHeaderData(DATE,Qt::Horizontal,trUtf8("date saisie"));
  m_modele->setHeaderData(DATE_VALEUR,Qt::Horizontal,trUtf8("Date\nopération"));
  m_modele->setHeaderData(REMARQUE,Qt::Horizontal,trUtf8("Mode\nde paiement"));
  m_modele->setHeaderData(VALIDE,Qt::Horizontal,trUtf8("facture\nreçue"));
  m_modele->setHeaderData(VALIDATION,Qt::Horizontal,trUtf8("pointage\nbancaire"));
  m_modele->setHeaderData(DETAIL,Qt::Horizontal,trUtf8("Détail\nécriture"));
  m_modele->setFilter(filter);
  // test verrouillage user
  bool testLock = security()->isUserLocked(login);
  if (!testLock)
  {
  	  m_modele ->select();
  	  if (WarnDebugMessage)
          qDebug() << __FILE__ << QString::number(__LINE__) << " sql =" << m_modele->query().lastQuery() ;
  	  tableView->setModel(m_modele);
      }
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
  tableView->setSortingEnabled(true);
  tableView->setColumnHidden(ID_MOUVEMENT,true);
  tableView->setColumnHidden(ID_MVT_DISPO,true);
  tableView->setColumnHidden(ID_USR,true);
  tableView->setColumnHidden(ID_COMPTE,true);
  tableView->setColumnHidden(TYPE,true);
  tableView->setColumnHidden(TRACABILITE,true);
  tableView->setItemDelegateForColumn(DATE,new DateEditTreeViewFirstDelegate(this));
  tableView->setItemDelegateForColumn(DATE_VALEUR,new DateEditTreeViewFirstDelegate(this));
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableView->horizontalHeader()->setStretchLastSection ( true );
  tableView->horizontalHeader()->setResizeMode(VALIDATION,QHeaderView::Stretch);
  tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
  qDebug() << __FILE__ << QString::number(__LINE__) ;
  checkBox  ->setChecked(false);
  checkBox_2->setChecked(false);
  checkBox_3->setChecked(false);
  checkBox_4->setChecked(false);
  checkBox_5->setChecked(false);
}

void depensedialog::messagebox(){
  QMessageBox::information(0,trUtf8("Info"),trUtf8("La dépense a été ajoutée"),QMessageBox::Ok);
}
//------------------------remplir le fichier depenses.ini à partir de la base "mouvements_dispo"
void depensedialog::remplirdepensesini(){
  QFile depensef(absolutePath()+"/depense.ini");
  if(!depensef.open(QIODevice::ReadWrite|QIODevice::Truncate)){
      QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("depense.ini ne peut pas être ouvert,erreur 422."),QMessageBox::Ok);
      return;
      }

  QTextStream fleuvef(&depensef);

  QSqlQuery query(m_accountancyDb);
            query.exec("SELECT id_mvt_dispo, libelle FROM mouvements_disponibles");
  QStringList list;
  while(query.next()){
      QString chiffre   = query.value(0).toString();
      QString mouvement = query.value(1).toString();
              list     << chiffre+"="+mouvement+"\n";
  }
  foreach(QString str,list){
      fleuvef          << str;
  }
}

void depensedialog::valide(){
//-----------recuperation index de la ligne surlignee-----------------
  QModelIndex index                       = tableView->QAbstractItemView::currentIndex();
  if(!index.isValid()){
      QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("vous avez oublié de sélectionner une ligne à valider"),QMessageBox::Ok);
  }
  int i = index.row();

  if(tableView->model()==m_model){
      QModelIndex indexvalide = m_model->index(i,10,QModelIndex());
      m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
      m_model->setData(indexvalide,1,Qt::EditRole);
      m_model->submitAll();
      rafraichir();
            }
  else if(tableView->model()==m_modele){
      QModelIndex indexvalide2 = m_modele->index(i,10,QModelIndex());
      m_modele->setEditStrategy(QSqlTableModel::OnFieldChange);
      m_modele->setData(indexvalide2,1,Qt::EditRole);
      m_modele->submitAll();
      rafraichir();
            }
  else{
      QMessageBox::critical(0,trUtf8("Erreur"),trUtf8("Impossible d'insérer la validation de facture!"),QMessageBox::Ok);
  }
}

void depensedialog::bankDebit()
{
//-----------recuperation index de la ligne surlignee-----------------
  QModelIndex index                       = tableView->QAbstractItemView::currentIndex();
  if(!index.isValid()){
      QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("vous avez oublié de sélectionner une ligne à valider"),QMessageBox::Ok);
  }
  int i = index.row();

  if(tableView->model()==m_model){
      QModelIndex indexvalide = m_model->index(i,VALIDATION,QModelIndex());
      m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
      m_model->setData(indexvalide,1,Qt::EditRole);
      m_model->submitAll();
      rafraichir();
      tableView->selectRow(i+1);
            }
  else if(tableView->model()==m_modele){
      QModelIndex indexvalide2 = m_modele->index(i,VALIDATION,QModelIndex());
      m_modele->setEditStrategy(QSqlTableModel::OnFieldChange);
      m_modele->setData(indexvalide2,1,Qt::EditRole);
      m_modele->submitAll();
      rafraichir();
      tableView->selectRow(i+1);
            }
  else{
      QMessageBox::critical(0,trUtf8("Erreur"),trUtf8("Impossible d'insérer la validation du débit en banque du mouvement!"),QMessageBox::Ok);
  }    
}

/*void depensedialog::altertable(){

  QSqlQuery query1(m_accountancyDb);
  query1.exec("ALTER TABLE mouvements ADD validation  tinyint(1)                             NULL AFTER tracabilite;");
  QSqlQuery query2(m_accountancyDb);
  query2.exec("ALTER TABLE mouvements ADD detail      varchar(100)  COLLATE utf8_unicode_ci  NULL AFTER validation;");

}*/

void depensedialog::remplirComboUtilisateurs(){
  parametrages p(this);
  QMessageBox mess;
              mess   . setText(trUtf8("Voulez vous paramétrer la comptabilité ?"));
              mess   . setStandardButtons(QMessageBox::Ok|QMessageBox::No);
              mess   . setDefaultButton(QMessageBox::Ok);
  int         retour = mess . exec();
  switch(retour){
      case QMessageBox::Ok :
              p . show();
        break;
      case QMessageBox::No :
        break;
      default:
        QMessageBox::warning(0,tr("Erreur"),trUtf8("Erreur switch ")+QString(__FILE__)+" "+QString(__LINE__),QMessageBox::Ok);
        break;

  }
  m_listelogin.clear();
  comboBoxLogins -> clear();

  QString utilisateurpref = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
  m_id = Common::ICore::instance()->settings()->value(S_ID_UTILISATEUR_CHOISI).toInt();

  QSqlQuery query3(m_accountancyDb);
            query3.exec("SELECT id_usr,login FROM utilisateurs WHERE id_drtux_usr = "+QString::number(m_id)+"");
  if(query3.next()){
      m_idusr     = query3.value(0).toInt();
      m_loginpref = query3.value(1).toString();
  }
  m_listelogin << m_loginpref;
  QSqlQuery query4(m_accountancyDb);
            query4.exec("SELECT login FROM utilisateurs");
  while(query4.next()){
      m_login = query4.value(0).toString();
      m_listelogin << m_login;
  }
  if(m_listelogin.size() < 1){
      m_listelogin . prepend( trUtf8("Docteur générique"));
      label_4   -> setStyleSheet("*{color:red}");
      label_4   -> setText(trUtf8("Utilisateurs \nnon configurés"));
      label_4   -> setWordWrap(true);
      label_4   -> setToolTip(trUtf8("Veuillez les configurer dans les paramétrages."));
  }
  comboBoxLogins->addItems(m_listelogin);

}

void depensedialog::whatsthismode()
{
  QWhatsThis::enterWhatsThisMode();
}

void depensedialog::showToolTip(int row){
    QString text = comboBox->itemText(row);
    QAbstractItemModel *m = new QStandardItemModel;
    m = comboBox->model();
    QStandardItemModel *model = qobject_cast<QStandardItemModel*> (m);
    QStandardItem *i = model->item(row);
    i->setToolTip(text);
}

QString depensedialog::userAndYearFilter(int id,QString year)
{
    QString filter;
    filter = "id_usr = "+QString::number(id);
    filter += QString(" AND date between '%1' and '%2'").arg(year+"-01-01",year+"-12-31");
    return filter;
}

void depensedialog::loginIsModified(int r)
{
    Q_UNUSED(r);
    rafraichir();
}

////completion
void depensedialog::on_detailEdit_textChanged(const QString & text){
    Q_UNUSED(text);
    QCompleter *c = new QCompleter(m_completionList,this);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    detailEdit->setCompleter(c);
}

void depensedialog::fillFileCompletionList(QObject* ob)
{
    Q_UNUSED(ob);
    QFile completionFile("completion.txt");
    if (!completionFile.open(QIODevice::ReadWrite|QIODevice::Text))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open completionFile" ;
        }
    QTextStream streamCompletionFile(&completionFile);
    foreach(QString word,m_completionList){
        streamCompletionFile << word+"\n";
        }
}

void depensedialog::setCompletionList(){
    QString text = detailEdit->text();
    m_completionList << text;
    m_completionList.removeDuplicates();
}

bool depensedialog::fillCompletionList()
{
    if (m_completionList.size()>0)
    {
    	  m_completionList.clear();
        }
    QFile completionFile("completion.txt");
    if (!completionFile.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open completionFile" ;
    	  return false;
        }
    QTextStream streamCompletionFile(&completionFile);
    while (!streamCompletionFile.atEnd())
    {
    	QString line = streamCompletionFile.readLine().trimmed();
    	m_completionList << line;
        }
    return true;
}

void depensedialog::refreshWithYear(const QString& year)
{
    rafraichir();
}

QString depensedialog::sumsOfMovements()
{
    QStringList listId;
    QHash<QString,double> hashDepenses;
    QHash<QString,double> hashApports;
    double totalDep = 0.00;
    double totalApports = 0.00;
    listId = m_hashForMovDips.keys();
    foreach(QString id,listId){
        QString value = QString("montant");
        QString table = QString("mouvements");
        QString idmov = QString("id_mvt_dispo");
        QString idusr = QString("id_usr");
        QString login = comboBoxLogins->currentText();
        QString idofuser = m_hashIdusrLogin.key(login);
        QString date = QString("date");
        QString begin = yearComboBox->currentText()+"-01-01";
        QString end = yearComboBox->currentText()+"-12-31";
        QString req = QString("select %1,%2 from %3 where %4 = '%5' and %6 = '%7'")
                     .arg(value,"type",table,idmov,id,idusr,idofuser);
        req = req+ QString(" and %1 between '%2' and '%3'").arg(date,begin,end);

       QSqlQuery q(m_accountancyDb);
        if (!q.exec(req))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
            }

        double val = 0.00;
        int type = 3;
        while (q.next())
        {
            QString foundValue = q.value(0).toString();
            type = q.value(1).toInt();
            if (foundValue.contains(","))
            {
                  foundValue.replace(",",".");
                }
            val += foundValue.toDouble();
            qDebug() << __FILE__ << QString::number(__LINE__) << " val =" <<  QString::number(val);
            }
        switch(type){
            case REDUCED :
                hashDepenses.insert(id,val);
                break;
            case INFLOW :
                hashApports.insert(id,val);
                break;
            default :
                break;    
            }
        }//foreach
    QString htmlDocument = "<html><body><font size=3>";
    htmlDocument += "<TABLE BORDER=1>";
    htmlDocument += "<CAPTION><font color = red size = 6>"+tr("Totaux par type de mouvement")+"</font></CAPTION>";
    htmlDocument += "<TR>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Mouvement")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Somme")+"</bold></font></TH>";
    htmlDocument += "</TR>";
    for (int i = 0; i < hashDepenses.count(); ++i)
    {
          QString id = hashDepenses.keys()[i];
          QString libelle = m_hashForMovDips.value(id);
          double value = hashDepenses.value(id);
          totalDep += value;
          htmlDocument += "<TR>";
          htmlDocument += "<TD>"+libelle+"</TD>"+"<TD>"+QString::number(value)+"</TD>";
          htmlDocument += "</TR>";
        }
    for (int i = 0; i < hashApports.count(); ++i)
    {
          QString id = hashApports.keys()[i];
          QString libelle = m_hashForMovDips.value(id);
          double value = hashApports.value(id);
          totalApports += value;
          htmlDocument += "<TR>";
          htmlDocument += "<TD>"+libelle+"</TD>"+"<TD>"+QString::number(value)+"</TD>";
          htmlDocument += "</TR>";
        }
    htmlDocument += "<TR>";
    htmlDocument += "<TD>TOTAUX DEPENSES</TD><TD>"+QString::number(totalDep)+"</TD>";
    htmlDocument += "</TR>";
    htmlDocument += "<TR>";
    htmlDocument += "<TD>TOTAUX APPORTS</TD><TD>"+QString::number(totalApports)+"</TD>";
    htmlDocument += "</TR>";
    htmlDocument += "</TABLE></font></body></html>";
    return htmlDocument;
}

void depensedialog::analyseYearly()
{
    QString textOfSums = sumsOfMovements();
    AnalyseMovements an(this);
    an.getSums(textOfSums);
    an.exec();
}

QHash<int,QString> depensedialog::hashHeaders()
{
    QHash<int,QString> hash;
    enum Header
    {
        ID=0,
        LIBELLE
        };
    QSqlQuery q(m_accountancyDb);
    QString req = QString("select %1,%2 from %3").arg("id_mvt_dispo","libelle");
    if (!q.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    while (q.next())
    {
        hash.insert(q.value(ID).toInt(),q.value(LIBELLE).toString());
        }
    return hash;
}

void depensedialog::showErrors(){
    QString errorAllString;
    QFile f(settings()->applicationBundlePath()+"/errorLog.txt");
    //qDebug() << __FILE__ << QString::number(__LINE__) << " " << settings()->applicationBundlePath()+"/errorLog";
    if(!f.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("Error log file cannot be opened."),QMessageBox::Ok);
        }
    QTextStream s(&f);
    s.setCodec("UTF8");
    while(!s.atEnd()){
        errorAllString += s.readLine()+"\n";
        }
    QDialog *d = new QDialog(this,Qt::Dialog|Qt::WindowCloseButtonHint);
    d->setMinimumSize(600,650);
    QDialogButtonBox *b = new QDialogButtonBox(QDialogButtonBox::Close,Qt::Horizontal,d);
    QTextEdit *e = new QTextEdit(d);
    e->setMinimumSize(500,600);
    e->setPlainText(errorAllString);
    QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(e);
     mainLayout->addWidget(b);
     d->setLayout(mainLayout);
     connect(b,SIGNAL(rejected()),d,SLOT(reject()));
     if(!d->exec()){delete d;}
}

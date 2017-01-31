#include "recettedialog.h"
#include "pondere.h"
#include "recettesconstants.h"
#include "rafraichirdialog.h"

#include <common/constants.h>
#include <common/icore.h>
#include <common/databasemanager.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/position.h>
#include <ccam/findreceiptsvalues.h>
#include <common/configure/sitesdialog.h>
#include <common/configure/payeursdialog.h>
#include <common/configure/actesdispo.h>
#include <common/configure/pourcentages.h>
#include <common/configure/parametredialog.h>
#include <common/configure/automatisme.h>
#include <common/configure/rapport.h>
#include <common/configure/utilisateur.h>
#include <common/configure/util.h>
#include <common/configure/iotools.h>
#include <common/configure/message_alerte.h>

#include <QDebug>
#include "string.h"
#ifdef Q_WS_WIN
#include "windows.h"
#endif

/*Table Actes_Disponibles = ngap
Table thesaurus = actes preferes du praticien
Table forfaits = majorations et forfaits*/

using namespace Common;
using namespace Constants;
using namespace RecettesConstants;
using namespace MessageSpace;

static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}//pointer on commandLineParser
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}

recettedialog::recettedialog(QWidget * parent)
{
    m_parent = new QMainWindow;
    m_parent = qobject_cast<QMainWindow*>(parent);
    m_pathabsolu = settings()->applicationBundlePath();
    m_test = settings()->value(TEST).toBool();
    if (m_test) qDebug() << "not connected yet" << __FILE__ << QString::number(__LINE__);
    if (Common::ICore::instance()->settings()->isFirstTimeRunning())
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "Accountancy is  configured" ;
      }
    if (!Common::ICore::instance()->databaseManager()->connectAccountancy())
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect Accountancy !" ;
      }
    /*if (!Common::ICore::instance()->databaseManager()->connectCcam())
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect CCAM !" ;
      }    */

    m_programName    = dataIn()->value(Common::CommandLine::CL_ProgramName).toString();
    m_nomRecupere    = dataIn()->value(Common::CommandLine::CL_PatientName).toString();
    m_prenomRecupere = dataIn()->value(Common::CommandLine::CL_PatientFirstName).toString();
    m_guidEntre      = dataIn()->value(Common::CommandLine::CL_PatientUUID).toString();
    m_dateOfBirth    = dataIn()->value(Common::CommandLine::CL_DateOfBirth).toString();
    m_tarif          = dataIn()->value(Common::CommandLine::CL_Price).toString();
    m_codeTarif      = dataIn()->value(Common::CommandLine::CL_PriceCode).toString();
    m_user           = dataIn()->value(Common::CommandLine::CL_CallingProgramUser).toString();
    QStringList listRecup;
    listRecup << m_programName << m_nomRecupere << m_prenomRecupere;

    for(int i = 0;i<listRecup.size();i++){
        if(listRecup[i].contains("_")){
            listRecup[i].replace("_"," ");
            }
        }
    m_programName    = listRecup[PROGRAM_NAME];
    m_nomRecupere    = listRecup[NOM_RECUPERE];
    m_prenomRecupere = listRecup[PRENOM_RECUPERE];
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__)<< "list of recup = " << " program = "+m_programName+"\n"
                                     +" name = "+m_nomRecupere+"\n"
                                     +" firstname = "+m_prenomRecupere+"\n"
                                     +" GUID_Patient = "+m_guidEntre+"\n"
                                     +" price = "+m_tarif+"\n"
                                     +" code of price = "+m_codeTarif;
    if(m_user ==  ""){
        m_user = "admin";
        }

//init free entry
  m_dataLibreOne = tr("somme libre");
//----regles ccam-------------------------
  if (m_test) qDebug() << "settings()->value(Constants::S_REGLE_HABITUELLE).toString() ="+Common::ICore::instance()->settings()->value(Constants::S_REGLE_HABITUELLE).toString();
  m_mapRegleAssociation.insert("habituelle",settings()->value(Constants::S_REGLE_HABITUELLE).toString());
  m_mapRegleAssociation.insert("radiologie",settings()->value(Constants::S_REGLE_RADIO).toString());
  m_mapRegleAssociation.insert("traumatologie",settings()->value(Constants::S_REGLE_TRAUMATO).toString());

//----appel des bases----------------------
    m_accountancyDb  = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (m_test) qDebug() << "driverName in recettedialog = "+m_accountancyDb.driverName();
//----test de la base nameindex si pas de programme appelant------------------
    if(m_programName == ""){
        QStringList listOfTables = m_accountancyDb.tables();
        QString str;
        QString strResult;
        foreach(str,listOfTables){
            strResult += str+"\n";
        }
        //if (m_test) qDebug() << "listOfTables ="+strResult;
    }
//----initialisation no message thesaurus
    m_noMessageThesaurusHash.insert("noMoreMessageThesaurus",false);
//--------test table id_payeur et honoraires ---------------------
    /*QSqlQuery queryTestIdPayeur(m_accountancyDb);
    bool resultIdPayeur = false;
    QStringList listTestIdPayeurs;
    QString testIdPayeur;
    if (m_accountancyDb.driverName().contains("MYSQL"))
    {
    	  testIdPayeur = "SHOW COLUMNS FROM honoraires";
        }
    if (m_accountancyDb.driverName().contains("SQLITE"))
    {
    	  //testIdPayeur = "SELECT sql FROM sqlite_master WHERE tbl_name = 'honoraires' AND type = 'table'";
    	  testIdPayeur = "PRAGMA table_info(honoraires);";
        } 
    if(!queryTestIdPayeur.exec(testIdPayeur)){
        qWarning() << __FILE__ << QString::number(__LINE__) << queryTestIdPayeur.lastError().text();
        }
    while(queryTestIdPayeur.next()){
        QString test = queryTestIdPayeur.value(0).toString();
        listTestIdPayeurs << test;
        if(test == "id_payeur"){
            resultIdPayeur = true;
            }
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << " listTestIdPayeurs =" << listTestIdPayeurs.join(",") ;
    if(resultIdPayeur == true){
        QString reqAlterIdPayeur = "ALTER TABLE `honoraires` CHANGE `id_payeur` `id_payeurs` VARCHAR( 36 ) "
                                    "CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL";
        QSqlQuery queryCorrIdPayeur(m_accountancyDb);
        if(!queryCorrIdPayeur.exec(reqAlterIdPayeur)){
            qWarning() << __FILE__ << QString::number(__LINE__) << queryCorrIdPayeur.lastError().text();
            }
        else{
            QMessageBox::warning(0,tr("Attention"),tr("La colonne id_payeur a été corrigée"),QMessageBox::Ok);
        }
        }
     if (!listTestIdPayeurs.contains("vir"))
     {
     	  QString reqAddColumnVirement;
     	  if (m_accountancyDb.driverName().contains("MYSQL"))
     	  {
     	  	  reqAddColumnVirement = QString("ALTER TABLE %1 ADD %2 %3 %4 AFTER %5;")
     	                                .arg("honoraires","vir","double","NULL","cb");
     	      }
     	  if (m_accountancyDb.driverName().contains("SQLITE"))
     	  {
     	  	  reqAddColumnVirement = QString("ALTER TABLE %1 ADD %2 %3 %4;")
     	                                .arg("honoraires","vir","double","NULL");
     	      }
     	  
     	   
     	  QSqlQuery qAddColumnVirement(m_accountancyDb);
     	  if (!qAddColumnVirement.exec(reqAddColumnVirement))
     	  {
     	  	  qWarning() << __FILE__ << QString::number(__LINE__) << qAddColumnVirement.lastError().text() ;
     	      }
     	  else{
     	      QMessageBox::information(0,tr("Information")
     	      ,tr("vir est ajouté à table honoraires"),QMessageBox::Ok);
     	  }
     	      
         }
//ajout vir a table seances
    QStringList listVirSeances;
    QString reqVirSeances = QString("SHOW COLUMNS FROM seances ;");
    QSqlQuery qVirSeances(m_accountancyDb);
    if (!qVirSeances.exec(reqVirSeances))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qVirSeances.lastError().text() ;
        }
    while (qVirSeances.next())
    {
    	QString fieldSeance = qVirSeances.value(0).toString();
    	listVirSeances << fieldSeance;
        }
    if (!listVirSeances.contains("vir"))
    {
    	  QSqlQuery qAddVirToSeances(m_accountancyDb);
    	  QString reqAddVirToSeances;
    	  if(m_accountancyDb.driverName().contains("MYSQL")){
    	       reqAddVirToSeances= QString("ALTER TABLE %1 ADD %2 %3 %4 AFTER %5;").
    	                               arg("seances","vir","double","NULL","cb");
    	       }
    	  if (m_accountancyDb.driverName().contains("SQLITE"))
    	  {
    	  	  reqAddVirToSeances= QString("ALTER TABLE %1 ADD %2 %3 %4;").
    	                               arg("seances","vir","double","NULL");
    	      }
    	  if (!qAddVirToSeances.exec(reqAddVirToSeances))
    	  {
    	  	  qWarning() << __FILE__ << QString::number(__LINE__) << qAddVirToSeances.lastError().text() ;
    	      }
    	  else{
     	      QMessageBox::information(0,tr("Information")
     	      ,tr("vir est ajouté à table seances"),QMessageBox::Ok);
     	      }
        }*/
    //----initialisation de variables de classe-------------
    m_id 		   = 0;
    m_praticien 	   = "";
    m_iplusmoins 	   = 0;
    m_facteurpourcent      = 100.00;
//---action clic droit----------------------------------------------------------
    m_remontrerActeCourant = new QAction(tr("Effacer\net remettre l'acte préféré."),this);
//---setup et attributs de la fenetre---------------------------------------------
    setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    QString size = settings()->value(Constants::PREF_SIZE_RECETTE).toString();
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " size = " << size;
    int sizeWidth = size.split(" ")[0].toInt();
    int sizeHeight = size.split(" ")[1].toInt();
    resize(sizeWidth,sizeHeight);
    QString minSize = settings()->value(Constants::MIN_SIZE_RECETTE).toString();
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " size = " << minSize;
    int minsizeWidth = minSize.split(" ")[0].toInt();
    int minsizeHeight = minSize.split(" ")[1].toInt();
    setMinimumSize(minsizeWidth,minsizeHeight);
    position()->centralPosition(this,width(),height());
//hide unused widgets
    seanceButton->hide();
//---checkbox pour garder ancienne valeur en cas d'acte partiel
    keepValueCheckBox->setChecked(true);
//----shortcut Quitter-----------------------
    pushButton_5          -> setShortcut(QKeySequence("Ctrl+Q"));



//-----le bouton compléter la base est-il accessible ou non-----
   /* if(m_accountancyDb.isOpen()){
        QStringList nbOfTables;
                    nbOfTables = m_accountancyDb.tables();
        if(nbOfTables.size() < 14){
           if(alerteBoutonCompleteBase(pushButton) == QMessageBox::Ok){
                 m_parametres             =  new parametrages;
                 m_parametres            -> show();
           };//dans completeBase.h , choix 1 pour recettes
        }
    }*/


//---------image sur des bouton ccam---------------------------
    ccamButton -> setIcon(QIcon(settings()->iconPath() +"/pixmap/cactot_cpam.png"));
    const QString valeurtext = tr("Valeurs");
    ccamButton -> setText(valeurtext);
    ccamButton->setShortcut(QKeySequence("ctrl+v"));
    ccamButton->setToolTip("ctrl+v");
    labelPercentages->setText("100%");//pourcentage dus
//---------bouton ccam enabled or not----------------
    if(settings()->value(Constants::S_MEDINTUX_CCAM)==false){
        ccamButton->setEnabled(false);
    }
//---------mise à niveau de widgets----------------------

    dateEdit->setDisplayFormat("dd/MM/yyyy");//date de paiement
    dateEdit->setDate(QDate::currentDate());
    dateEdit_2->setDisplayFormat("dd/MM/yyyy");//date des actes
    dateEdit_2->setDate(QDate::currentDate());


    Espece        ->setCheckable(true);
    Cheque        ->setCheckable(true);
    cb            ->setCheckable(true);
    checkBox_4    ->setCheckable(true);
    checkBox      ->setCheckable(true);
    letOpenedCheckBox    ->setCheckable(true);
//permet de donner aux checkBox un comportement de radioButtons----------
    Espece        ->setAutoExclusive(true);
    Cheque        ->setAutoExclusive(true);
    cb            ->setAutoExclusive(true);
    virementBox   ->setAutoExclusive(true);
    checkBox_4    ->setAutoExclusive(true);
    checkBox      ->setAutoExclusive(true);
    letOpenedCheckBox    ->setAutoExclusive(false);
//------------------configuration de l aide---------------------------------------------------------------------------
    pushButton_16 -> setToolTip  (tr("Pour l'aide cliquez sur ce bouton,\ndéplacez la souris sur la zone voulue,\ncliquez "
                                                      "pour afficher l'aide."));
    lineEdit      -> setWhatsThis(tr("Cette entrée permet de saisir une recette indépendamment "
                                         "du patient.\nPar exemple paiement d'une garde par une association."));
    freeReceipValueEdit    -> setWhatsThis(tr("Permet de saisir une somme non paramétrée.\nPar exemple le paiement global d'une garde."));
    lineEdit_4    -> setWhatsThis(tr("Affichage du nom du patient.\n"
                                         "Si le logiciel fonctionne en dehors d'un programme appelant,\n"
                                         "veuillez rentrer le nom et prénom séparés par une virgule ou un ;."));
    tableView     -> setWhatsThis(tr("Cette zone permet la recherche du nom, mais disparaît si le logiciel est lancé par DrTux."));
    cb            -> setWhatsThis(tr("Virement ou carte bancaire."));
    checkBox      -> setWhatsThis(tr("Dispense d'avance des frais."));
    letOpenedCheckBox    -> setWhatsThis(tr("Permet de bloquer en position ouverte la fenêtre \nsi le choix de fermeture "
                                         "automatique est programmé.\nVous pouvez programmer ce choix "
                                         "par le bouton Automatisme de fermeture."));
    pushButton_13 -> setWhatsThis(tr("Vous pouvez programmer l'automatisme \nde fermeture de la fenêtre en fin de saisie."));
    /*pushButton    -> setWhatsThis(tr("Enregistre l'acte courant.\nPour enregistrer les autres actes, "
                                         "cliquez sur le déroulant \"NGAP\" ou sur la CCAM\net le bouton d'enregistrement "
                                         "des autres actes sera découvert."));*/
    recordButton  -> setWhatsThis(tr("Enregistre les autres actes ou le résultat de la CCAM"));
    controlButton  -> setWhatsThis(tr("Vous pouvez voir les recettes avec ce bouton, les effacer,\nles filtrer selon plusieurs critères"));
    seanceButton  -> setWhatsThis(tr("Permet d'enregistrer plusieurs recettes non liées pour ce patient"
                                         ", par exemple une journée en clinique.\n"
                                         "Les recettes enregistrées seront celles que vous venez de saisir pour ce patient."));
    pushButton_7 -> setWhatsThis(tr("Enregistre dans votre thésaurus d'actes préférés."));
    pushButton_8 -> setWhatsThis(tr("Lance la fenêtre de lecture des messages du programme. A coller dans un rapport de bug."));
    ccamButton -> setWhatsThis(tr("Lance le logiciel CcamView."));
   // pushButton_14 -> setWhatsThis(tr("Vous permez de configurer votre utilisateur préférentiel pour la comptabilité"));
   // pushButton_15 -> setWhatsThis(tr("Lance la fenêtre de création des utilisateurs de la comptabilité"));
   // pushButton_17 -> setWhatsThis(tr("En cliquant sur ce bouton vous pouvez envoyer directement un rapport à l'auteur."));
    groupBox_6    -> setWhatsThis(tr("Vous déterminez ici le pourcentage payé ou du par le patient \n"
                                         "ou un autre payeur du déroulant."));
    label         -> setWhatsThis(tr("Par le bouton enregistrer dans le thésaurus on place dans cette tables de données"
                                         "la séquence des actes préférés .\nPar exemple C+MNO, ou C+DEQP001."
                                         "Cliquer dessus permettra d'inscrire cette séquence directement sur "
                                         "le bouton d'enregistrement."));
    thesaurusComboBox      -> setWhatsThis(tr("Par le bouton enregistrer dans le thésaurus on place dans cette tables de données"
                                         "la séquence des actes préférés .\nPar exemple C+MNO, ou C+DEQP001."
                                         "Cliquer dessus permettra d'inscrire cette séquence directement sur "
                                         "le bouton d'enregistrement."));
    label_2       -> setWhatsThis(tr("La date  d'encaissement de l'acte"));
    dateEdit      -> setWhatsThis(tr("La date  d'encaissement de l'acte"));
    label_14      -> setWhatsThis(tr("La date de l'acte"));
    dateEdit_2    -> setWhatsThis(tr("La date de l'acte"));
    label_11      -> setWhatsThis(tr("Le lieu où a été pratiqué l'acte. Par exemple le cabinet médical, ou la clinique."));
    sitesComboBox    -> setWhatsThis(tr("Le lieu où a été pratiqué l'acte. Par exemple le cabinet médical, ou la clinique."));
    label_3       -> setWhatsThis(tr("Le praticien qui a effectué l'acte"));
    userComboBox    -> setWhatsThis(tr("Le praticien qui a effectué l'acte"));
    label_9       -> setWhatsThis(tr("Choix du payeur : le plus souvent le patient ci nommé,\n"
                                         "mais ça peut être une structure ou une institution."));
    payeurComboBox    -> setWhatsThis(tr("Choix du payeur : le plus souvent le patient ci nommé,\n"
                                         "mais ça peut être une structure ou une institution."));
    /*ccamButton -> setWhatsThis(tr("Ce bouton lance le logiciel de gestion des actes CCAM.\nVous choisirez un acte,"
                                         " vous le validerez en double cliquant dessus, vous confirmerez.\nAprès calcul, l'acte "
                                         "ou les actes seront calculés et leur somme corrigée dans la fenêtre à droite, selon "
                                         "le choix ci-dessous (par exemple  règle d'association habituelle : 100, 50, 0."));*/
    ccamAndValuesLineEdit    -> setWhatsThis(tr("Affichage des actes sélectionnés dans la CCAM, à enregistrer avec le gros bouton de droite."));
    label_17      -> setWhatsThis(tr("Choix du compte bancaire de dépôt."));
    bankAccountCcomboBox    -> setWhatsThis(tr("Choix du compte bancaire de dépôt."));
    rulesKmComboBox    -> setWhatsThis(tr("Type d'indemnités de déplacement."));
    distanceSpinBox       -> setWhatsThis(tr("Nombre de kilomètres Aller Retour."));
    label_18      -> setWhatsThis(tr("Type d'indemnités de déplacement."));
    pondereButton -> setToolTip(tr("Permet d'appliquer un facteur de pondération à chaque acte affiché sur le bouton."));
    //seanceButton  -> setWhatsThis(tr("Enregistre le calcul de la séance."));
   // pushButton_4  -> setWhatsThis(tr("Lance la fenêtre de configuration des sites où exerce le praticien."));
   // seanceButton  -> setWhatsThis(tr("Si ce bouton n'est pas grisé, son activation met à jour vos bases pour
                                            //la nouvelle comptabilité"));
   //pushButton_7  -> setWhatsThis(tr("Lance la fenêtre qui vous permet de faire vos listes d'organismes payeurs.\n"
   //                                     "Cette liste de payeurs s'affichera dans le cadre pourcentage : par le :"));
   // pushButton_9  -> setWhatsThis(tr("Ce bouton permet de rajouter un pourçentage pour paramétrer plus finement "
   //                                      "la barre de pourçentages."));
   // pushButton_8  -> setWhatsThis(tr("Ce bouton lance la fenêtre de saisie d'actes qui seront affichés dans
                                            //le déroulant Autre Actes."));

//---------------------------si pas de recuperation du nom et prenom dans programme appelant-------------------------
    if((m_nomRecupere == "") == true){
        m_texte	     =lineEdit_4->text();
        m_tableModel = new QSqlTableModel(this,m_accountancyDb);
	    if(m_texte.isEmpty())
	        textenonchange();
        connect(lineEdit_4,SIGNAL(textChanged(const QString&)),this,SLOT(textechange(const QString&)));
        }
//------------------------récupération du nom et prenom du patient---------------------------------------------------

    else {
        tableView      -> hide();
        label_16       -> hide();
        lineEdit_4     -> setText(m_nomRecupere+";"+m_prenomRecupere);
        /*m_model          = new QSqlQueryModel;
   	m_model         -> setQuery("SELECT FchGnrl_NomDos,FchGnrl_Prenom,FchGnrl_IDDos "
   	                            " FROM IndexNomPrenom WHERE FchGnrl_IDDos = '"+m_guidEntre+"'",m_db1);
	m_model         -> setHeaderData(0, Qt::Horizontal, tr("Nom"));
	m_model         -> setHeaderData(1, Qt::Horizontal, tr("Prenom"));
	m_model         -> setHeaderData(2, Qt::Horizontal, tr("guid"));
    	tableView       -> setModel(m_model);
	tableView       -> setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView       -> setSelectionMode(QAbstractItemView::SingleSelection);
	tableView       -> resizeColumnsToContents();
	tableView       -> resizeRowsToContents();
	tableView       -> horizontalHeader()->setResizeMode(QHeaderView::Interactive);

	tableView       -> verticalHeader()->setResizeMode(QHeaderView::Stretch);
	tableView       -> setMinimumHeight(70);
	tableView       -> setMaximumHeight(70);
	tableView->show();*/
    }
//-----------------------fin de recuperation du nom prenom-----------------------------------------------------------
//-----------------------recuperation de liste d'actes de la base thesaurus------------------------------------------
    thesaurusComboBox            ->setEditable(1);
    thesaurusComboBox            ->setInsertPolicy(QComboBox::NoInsert);
    QSqlQuery qThesaurus(m_accountancyDb);
    if(!qThesaurus.exec(QString("SELECT %1 ,%2,%3 FROM %4").arg("id_thesaurus",
                                                                "sequence",
                                                                "type",
                                                                "thesaurus"))){
        qWarning() << qThesaurus.lastError().text() << __FILE__ << QString::number(__LINE__);
    }
    int key0 = 0;
    while(qThesaurus.next()){
        key0++;
        QString strUn     = qThesaurus.value(0).toString();
        m_mapIdThesaurus.insert(key0,strUn);
        QString strDeux   = qThesaurus.value(1).toString();
        m_mapSequenceThesaurus.insert(key0,strDeux);
        QString strTrois  = qThesaurus.value(2).toString();
        m_mapTypeThesaurus.insert(key0,strTrois);
        thesaurusComboBox->addItem(strDeux,QVariant());
        }
//indemnites kilometriques
    rulesKmComboBox            ->setEditable(1);
    rulesKmComboBox            ->setInsertPolicy(QComboBox::NoInsert);
    QStringList listIK;
    QSqlQuery qIK(m_accountancyDb);
    if(!qIK.exec(QString("SELECT %1,%2,%3,%4,%5 FROM %6").arg("id_IK",
                                                           "type",
                                                           "value",
                                                           "abattement",
                                                           "indemnites_forfait",
                                                           "indemnites_deplacement"))){
        qWarning() << qIK.lastError().text() << __FILE__ << QString::number(__LINE__);
        }
    while(qIK.next()){
        int id = qIK.value(0).toInt();
        QString type = qIK.value(1).toString();
        QString value = qIK.value(2).toString();
        QString abattement = qIK.value(3).toString();
        QString indemnite_deplacement = qIK.value(4).toString();
        m_mapTypeIK.insert(id,type);
        m_mapValueIK.insert(id,value);
        m_mapAbattementIK.insert(id,abattement);
        m_mapIndemnitesIK.insert(id,indemnite_deplacement);

        listIK << type;
        }
    rulesKmComboBox ->addItems(listIK);


//------------------------choix user preferentiel et recuperation du choix de widget ouvert ou non--------------------

        m_ouinon		    = settings()->value(Constants::S_CLOSING_CHOICE).toString();//laisse le widget
                                                                                                //ouvert ou non
        m_tempsFermeture            = settings()->value(Constants::S_CLOSING_TIME).toInt();
        m_loginpref                 = settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
        m_id                        = settings()->value(Constants::S_ID_UTILISATEUR_CHOISI).toInt();

//--------------------hide letOpenedCheckBox "bloquer ouvert" si inutile---------------------------------------------------
      if(m_ouinon == "oui"){
          letOpenedCheckBox ->hide();
      }

//------------------------choix user----------------------------------------------------------------------------------
      userComboBox                     -> setEditable(1);
      userComboBox                     -> setInsertPolicy(QComboBox::NoInsert);
      if(m_loginpref == ""){
                m_loginpref         = "admin";
                label_3            -> setStyleSheet("*{ color:red }");
                label_3            -> setText(tr("Utilisateur non configuré.\nVeuillez paramétrer les utilisateurs "
                                                     "et vérifier config.ini."));
                //QWhatsThis::enterWhatsThisMode();
                QTimer::singleShot(3000,this,SLOT(redHelp()));

                userComboBox         -> setToolTip(tr("Cet utilisateur est générique.\nVeuillez paramétrer "
                                                          "vos utilisateurs dans Paramétrages Utilisateurs."));
            }

      m_listelogin                   << m_loginpref;

      QSqlQuery query4(m_accountancyDb);
      query4                           .exec("SELECT login FROM utilisateurs");
        while(query4.next()){
           m_login       = query4.value(0).toString();
           m_listelogin << m_login;
           }
      m_listelogin.removeDuplicates();
      int resultNewLogin = 0;
      QString strNewLogin;
      foreach(strNewLogin,m_listelogin){
          if((strNewLogin == m_user) == true){
              resultNewLogin++;
          }
      }
      if(resultNewLogin == 0){
          utilisateur *u = new utilisateur(0,m_user);
                       u-> show();
          QMessageBox::information(0,tr("Information"),tr("veuillez créer votre utilisateur "
                                                              "et le choisir comme préférentiel."),QMessageBox::Ok);

          }
      userComboBox                  ->addItems(m_listelogin);
//--------------------écriture du texte et de la valeur de l'acte courant sur son bouton-------------------------------------
    QString acteprefere           = settings()->value(Constants::S_ACTE_COURANT).toString();
    QString valeuracteprefere     = settings()->value(Constants::S_VALEUR_ACTE_COURANT).toString();
    acteprefere = correctNameOfAct(acteprefere);
    QString valeurduc             = acteprefere+" = "+priceValueOfAct(acteprefere);//ce qui est ecrit sur le gros bouton
    if (m_test) qDebug() << "valeur du C ="+valeurduc;
    std::string valeurducstr      = valeurduc.toStdString();
    const char *valeurchar        = valeurducstr.c_str();
    recordButton                   -> setStyleSheet("*{ color:red ; font-size : 20px }");
    recordButton                   -> setText(QApplication::translate("RecetteDialog", valeurchar, 0,
                                              QApplication::UnicodeUTF8));
    recordButton                   -> setEnabled(true);
    
    //----shortcut Enter-----------------------------------------

   recordButton            -> setShortcut(QKeySequence::InsertParagraphSeparator);

//---------------------on cache le bouton enregistrer les actes du thesaurusComboBox---------------------------------------
//Si la thesaurusComboBox est activée, on se rendra dans la fonction writeOnButtonAndInHashOfValues()-------------------------------
    controlButton                 -> setEnabled(true);

//------------------recuperer liste des sites dans la sitesComboBox-----------------------------------------------
    rafraichirsitesComboBox();
    QString textCombo_5 = sitesComboBox->currentText();
    emit combo_5_toolTip(textCombo_5);
//-------------------------recuperer payeurs-----------------------------------------------
    rafraichirpayeurComboBox();
//--------------------------------------------------------------------------------------------------------------
//------------------------------recuperer pourcentage----------------------------------------------------------

    QStringList listepourcentage;
    QSqlQuery querypourcentage(m_accountancyDb);
              querypourcentage.exec("SELECT type,pourcentage FROM pourcentages");
    while(querypourcentage.next()){
        QString type 	 = querypourcentage.value(0).toString();
        QString pourcent = querypourcentage.value(1).toString();
        QString ligne 	 = pourcent+"="+type;
        listepourcentage << ligne ;
        }
//--------------------------thesaurusComboBox compte bancaire----------------------------------------
    bankAccountCcomboBox                   -> setEditable(1);
    bankAccountCcomboBox                   -> setInsertPolicy(QComboBox::NoInsert);
    QString requeteCompte = "SELECT id_compte,id_usr,libelle,solde_initial FROM comptes_bancaires";
    QStringList listLibelleCompte;
    QSqlQuery queryReqComptes(m_accountancyDb);
              if(!queryReqComptes.exec(requeteCompte)){
                  QMessageBox::warning(0,tr("Erreur"),tr("Erreur =")+queryReqComptes.lastError().text(),
                                          QMessageBox::Ok);
              }
              while(queryReqComptes.next()){
                 QString reqLibelle = queryReqComptes.value(2).toString();
                 listLibelleCompte << reqLibelle;
                 m_hashCompte.insert(queryReqComptes.value(0).toString(),reqLibelle);
                 m_solde_initial = queryReqComptes.value(3).toDouble();

              }
    if(m_loginpref.isEmpty() == false){
        if(m_hashCompte.count(QString::number(m_id)) > 1){
            QMessageBox::information(0,tr("Information"),tr("Plusieurs comptes bancaires possibles pour "
                                                                    "cet utilisateur,\nveuillez en choisir un"),
                                                                    QMessageBox::Ok);
        }
        else{
            QString reqComptePref = m_hashCompte.value(QString::number(m_id));
            if(!reqComptePref.isEmpty()){
                listLibelleCompte.prepend(reqComptePref);
            }

        }

    }
    bankAccountCcomboBox -> addItems(listLibelleCompte);
//-------------------------rapprochement bancaire----------------------------------------------
   m_actesSeance      = "";
   m_especeSeance     = 0.00;
   m_chequeSeance     = 0.00;
   m_carteBleueSeance = 0.00;
   m_virementSeance   = 0.00;
   m_dafSeance        = 0.00;
   m_autreSeance      = 0.00;
   m_duSeance         = 0.00;

//------------------------------copier listepourcentage dans fichier fichierpourcentage.ini------------------------
    QFile fichierpourcent(m_pathabsolu+"/fichierpourcentage.ini");
    if (m_test) qDebug() << "m_pathabsolu ="+m_pathabsolu << __FILE__ << " " << QString::number(__LINE__);
	if(!fichierpourcent.open(QIODevice::ReadWrite | QIODevice::Text)){
	QMessageBox::warning(0,tr("Attention"),tr("fichierpourcentage.ini ne peut pas être ouvert."),
	          QMessageBox::Ok);
	}
     QTextStream streamfichierpourcent(&fichierpourcent);
         for(int i = 0 ; i < listepourcentage.size() ; i++){
            streamfichierpourcent << listepourcentage[i]+"\n";
 	    }

//-----------------------------parametrer le widget factorProgressBar-----------------------------------------------------

    factorProgressBar->setRange(0,100);
    factorProgressBar->setValue(100);
    if (security()->isUserLocked(m_loginpref))
    {
    	  recordButton->setEnabled(false);
        }
    //test messages alertes
    MessageAlerte *messagealerte = new MessageAlerte(this);
    messagealerte->messageAreAvailableAndNotRead();
    //----------------------------les connexions au slots---------------------------------------------------------------
    connect(recordButton, SIGNAL(pressed()),       this,SLOT(actsRegistering()));
    connect(controlButton, SIGNAL(pressed()),       this,SLOT(modifierrecettes()));
    connect(pushButton_5, SIGNAL(pressed()),       this,SLOT(fermer()));
    connect(seanceButton, SIGNAL(pressed()),       this,SLOT(enregistreSeance()));
    connect(pushButton_7, SIGNAL(pressed()),       this,SLOT(saveInThesaurus()));
    connect(pushButton_8, SIGNAL(pressed()),       this,SLOT(showErrors()));
    connect(ccamButton,SIGNAL(pressed()),       this,SLOT(ccamAndValuesProcess()));
    connect(pushButton_11,SIGNAL(pressed()),       this,SLOT(fonctionplus()));//+
    connect(pushButton_12,SIGNAL(pressed()),       this,SLOT(fonctionmoins()));//-
    connect(pushButton_13,SIGNAL(pressed()),       this,SLOT(automatismeOuvert()));
    connect(pushButton_16,SIGNAL(pressed()),       this,SLOT(setWhatsThisMode()));
    connect(sitesComboBox   ,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(combo_5_toolTip(const QString&)));
    connect(thesaurusComboBox,     SIGNAL(activated(const QString&)),  this,SLOT(writeOnButtonAndInHashOfValues(const QString &)));
    connect(payeurComboBox,   SIGNAL(highlighted(const QString &)),this,SLOT(changeToolTip(const QString &)));
    connect(lineEdit,     SIGNAL(textChanged(const QString &)),this,SLOT(freeeditofreceipt(const QString &)));//nom libre
    connect(freeReceipValueEdit,   SIGNAL(textChanged(const QString &)),this,SLOT(freeeditofreceipt(const QString &)));//somme libre
    //connect(distanceSpinBox,      SIGNAL(valueChanged(int)),   this,SLOT(chooseIK(int)));
    connect(distanceSpinBox,      SIGNAL(valueChanged(int)),   this,SLOT(addIK(int)));//
    connect(m_remontrerActeCourant,SIGNAL(triggered()),this,SLOT(clearAll()));//clic droit
    connect(pulldownCheckBox, SIGNAL(stateChanged ( int )),  this,SLOT(putDown(int )));
    connect(pondereButton,SIGNAL(pressed()),this,SLOT(modifyListOfValues()));
    connect(percentSpinBox,SIGNAL(valueChanged(int)),this,SLOT(percentageChanged(int)));
    
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------fin du constructeur----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

recettedialog::~recettedialog(){
    m_acteCcam          = "";
    m_forfaitsForfaits  = "";
    m_valueForfaits     = "";
}
////////////////////////////fonctions et slots//////////////////////////////////////////////////////////////////
void recettedialog::writeOnButtonAndInHashOfValues(const QString & text){
    QString data ;
    QString oldDatas = recordButton->text();
    if(QObject::sender() == qobject_cast<QComboBox*>(thesaurusComboBox))
    {
        qDebug() << __FILE__ << QString::number(__LINE__) << " thesaurus text =" << text ;
        QStringList actsList;
        actsList = text.split("+");
        QHash<QString,QString> hashfromthesaurus = getValuesFromThesaurus(actsList);
        if (m_test)
        qDebug() << __FILE__ << QString::number(__LINE__) << " hashfromthesaurus size =" << QString::number(hashfromthesaurus.count()) ;
        QHash<QString,QString>::iterator it;
        for (it = hashfromthesaurus.begin(); it != hashfromthesaurus.end(); ++it)
        {
              m_ccamAndValuesChosen.insertMulti(it.key(),it.value());
            }
        }
    if(QObject::sender() == qobject_cast<QLineEdit*>(lineEdit))
    {
        m_ccamAndValuesChosen.insert(lineEdit->text(),freeReceipValueEdit->text());
        }
    if(QObject::sender() == qobject_cast<QLineEdit*>(freeReceipValueEdit))
    {
        if (lineEdit->text().isEmpty())
        {
              return;
            }
        m_ccamAndValuesChosen.insert(lineEdit->text(),freeReceipValueEdit->text());
        }
    QStringList keyslist;
    keyslist = m_ccamAndValuesChosen.keys();
    QStringList valueslist;
    valueslist = m_ccamAndValuesChosen.values();    
    data += keyslist.join("+");
    data += "\n=";
    data += "\n"+valueslist.join("+");
    recordButton                    -> setStyleSheet("*{ color:blue }");
    recordButton                    -> setText(tr("Enregistrer\n")+data);
    if (m_test) qDebug() << "in writeOnButtonAndInHashOfValues data ="+data;
}

QHash<QString,QString> recettedialog::getValuesFromThesaurus(QStringList & actsList)
{
    QHash<QString,QString> hashOfAllThesaurus = getValuesFromThesaurus();
    QHash<QString,QString> hashAccordingToActsList;
    foreach(QString act,actsList){
        QString value = hashOfAllThesaurus.value(act);
        qDebug() << __FILE__ << QString::number(__LINE__) << " act =" << act << " value = " << value;
        hashAccordingToActsList.insert(act,value);
        }
    return hashAccordingToActsList;
}

QHash<QString,QString> recettedialog::getValuesFromThesaurus()
{
    enum ActValueEnum {ACT = 0,VALUE};
    if (m_hashOfThesaurusActsAndValues.count()>0)
    {
          return m_hashOfThesaurusActsAndValues;
        }
    QStringList actsInThesauruslist;
    QSqlQuery qyThesaurus(m_accountancyDb);
    QString thesaurusReq = QString("select %1 from %2").arg("sequence","thesaurus");
    if (!qyThesaurus.exec(thesaurusReq))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qyThesaurus.lastError().text();
        }
    while (qyThesaurus.next())
    {
        QString sequenceact = qyThesaurus.value(ACT).toString().trimmed();
        QStringList listofline = sequenceact.split("+");
        foreach(QString act,listofline){
            actsInThesauruslist << act;
            } 
        }
    actsInThesauruslist.removeDuplicates();
    foreach(QString actofthesaurus,actsInThesauruslist){
        QSqlQuery qyNgap(m_accountancyDb);
        QString ngapReq = QString("select %1,%2 from %3").arg("nom_acte","montant_total","actes_disponibles");
        if (!qyNgap.exec(ngapReq))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << qyNgap.lastError().text() ;
            }
        while (qyNgap.next())
        {
            QString nameofact = qyNgap.value(ACT).toString();
            QString value = qyNgap.value(VALUE).toString();
            m_hashOfThesaurusActsAndValues.insert(nameofact,value);
            }
        QSqlQuery qyForfaits(m_accountancyDb);
        QString forfaitsReq = QString("select %1,%2 from %3").arg("forfaits","valeur","forfaits");
        if (!qyForfaits.exec(forfaitsReq))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << qyForfaits.lastError().text() ;
            }
        while (qyForfaits.next())
        {
            QString nameofact = qyForfaits.value(ACT).toString();
            QString value = qyForfaits.value(VALUE).toString();
            m_hashOfThesaurusActsAndValues.insert(nameofact,value);
            }
        QSqlQuery qyCcam(m_accountancyDb);
        QString ccamReq = QString("select %1,%2 from %3").arg("code","amount","ccam");
        if (!qyCcam.exec(ccamReq))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << qyCcam.lastError().text() ;
            }
        while (qyCcam.next())
        {
            QString nameofact = qyCcam.value(ACT).toString();
            QString value = qyCcam.value(VALUE).toString();
            m_hashOfThesaurusActsAndValues.insert(nameofact,value);
            }
        }
    return m_hashOfThesaurusActsAndValues;
}
 

void recettedialog::freeeditofreceipt(const QString & texte){//bouton enregistrer autres actes
    if(QObject::sender() == qobject_cast<QLineEdit*>(lineEdit)){//texte libre
        m_dataLibreOne                    = texte;
        if(!texte.isEmpty()){
            m_dataLibreOne += "\n";
            }
        }
    else if(QObject::sender() == qobject_cast<QLineEdit*>(freeReceipValueEdit)){//somme libre
        m_dataLibreTwo                    = texte;
        if (m_dataLibreOne == "")
        {
              m_dataLibreOne = tr("somme libre");
            }
        m_ccamAndValuesChosen.insert(m_dataLibreOne,m_dataLibreTwo);
        }
    else
    {
        m_dataLibreOne = "";
        }
        recordButton                    -> setStyleSheet("*{ color:green }");
        recordButton                    -> setText(tr("Enregistrer\n")+m_dataLibreOne+m_dataLibreTwo);
}

void recettedialog::clearAll(){
    lineEdit->clear();
    freeReceipValueEdit->clear();
    recordButton   -> setStyleSheet("*{ color:red ; font-size : 20px }");
    QString act = settings()->value(Constants::S_ACTE_COURANT).toString();
    QString value = settings()->value(Constants::S_VALEUR_ACTE_COURANT).toString();
    act = correctNameOfAct(act);
    value = priceValueOfAct(act);
    QString acte = act+" = "+value;
    recordButton   -> setText(acte);
    recordButton   -> setEnabled(true);
    m_acteCcam          = "";
    m_forfaitsForfaits  = "";
    m_valueForfaits     = "";
    m_actesToolTip      = "";
    m_ccamAndValuesChosen.clear();
    distanceSpinBox->setValue(0);
    factorProgressBar->setValue(100);
    ccamAndValuesLineEdit -> clear();
    int closing = settings()->value(Constants::S_CLOSING_TIME).toInt();
    QTimer::singleShot(closing,this,SLOT(clearResultLabelText()));
}

void recettedialog::clearResultLabelText(){
    resultValueLabel -> setText("");
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void recettedialog::actsRegistering(){
    if (!settings()->value(Constants::NO_MORE_THESAURUS_MESSAGE).toBool()&& !m_noMessageThesaurusHash.value("noMoreMessageThesaurus").toBool())
    {
    	    int messReturn = messageThesaurus();
    	    switch(messReturn){
            case QMessageBox::Ok :
            return;
            break;
            case QMessageBox::Cancel :
            break;
            default :
            break;    
            }
        }
     QString data; 
     double result = 0.00 ;
    if (m_ccamAndValuesChosen.count()<1)
    {
        //ecrire la valeur dans m_ccamAndValuesChosen pour choix rapide
        QString acteprefere           = settings()->value(Constants::S_ACTE_COURANT).toString();
        QString valeuracteprefere     = settings()->value(Constants::S_VALEUR_ACTE_COURANT).toString();
        m_ccamAndValuesChosen.insert(acteprefere,valeuracteprefere);
        }
    //progressBar des poucentages
    int factor = 0;
    factor = factorProgressBar->value();
    QStringList actsChoosenList;
    actsChoosenList = m_ccamAndValuesChosen.keys();
    foreach(QString act,actsChoosenList){
        if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " actsChoosenList =" << QString::number(actsChoosenList.size()) ;
        QString value = m_ccamAndValuesChosen.value(act);
        result += eachActRegistering(act,value,factor);
        }
    if(result == 0.00){
        QMessageBox::warning(0,tr("Warning"),
                             tr("Un enregistrement est défectueux,\nVérifiez dans Recettes !\n")
                             +__FILE__
                             +QString::number(__LINE__),
                             QMessageBox::Ok);
        }
    else{
        QString executedQueryResult = QString::number(result);
        if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " result = "+executedQueryResult;
        controlOfQueries(executedQueryResult);
        }
    if(m_ouinon == "non")
    {
            if(letOpenedCheckBox->isChecked() == false)
            {
	            QTimer::singleShot(m_tempsFermeture,this,SLOT(fermer()));
	            }
	    }
	if (!keepValueCheckBox->isChecked()&& factor < 100) m_ccamAndValuesChosen.clear();
}
////////////////////////////////////////////////////////////////////////////////////
//fonction d'enregistrement des actes/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
double recettedialog::eachActRegistering(QString act, QString value,int factor)
{
    if (value.contains(","))
    {
          value.replace(",",".");
        }
    double acte = value.toDouble();
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " act =" << act ;
    QString nom 	                         = "";
    QString prenom 	                         = "";
    QString userchoisi                       = userComboBox->currentText();

    //----nombre de lignes au départ-------------------------------------
    QSqlQueryModel *modele2                  = new QSqlQueryModel;
  	            modele2                  ->setQuery("SELECT * FROM honoraires",m_accountancyDb);
    int count                                = modele2->rowCount();
    modele2                                  ->clear();
//----------------------------------------------somme libre-----------------------------------------------------------
    QString freeentry                            = lineEdit->text();
    double freevalue                            = freeReceipValueEdit->text().toDouble();
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__)<<" freevalue = "+ QString::number(freevalue);

  //-------------------------------------------------------------------------
    QDate daterecup = dateEdit->date();
	m_date = daterecup.toString("yyyy-MM-dd");
    QSqlQuery query7(m_accountancyDb);
    
	query7.exec("SELECT nom_usr,id_usr FROM utilisateurs WHERE login = '"+userchoisi+"'");
	//TODO PROBLÈME id_usr qui peut être égal à 0
	QStringList listprat;
	while (query7.next()) {
	    m_praticien = query7.value(0).toString();
	    m_idusr = query7.value(1).toInt();
	    listprat << m_praticien;
		}
    if(listprat.size() < 1)
    {
        m_praticien = "Docteur";
        m_idusr = 1;
        }
    //TEST id_usr == 0 pour deboggage
    if (m_test && m_idusr == 0) QMessageBox::information(0,tr("Test"),tr("m_idusr == 0"),QMessageBox::Ok);
    
//---------------------pas de programme appelant-----------------------------------
  if((m_nomRecupere == "")== true)
  {
    QModelIndex index = tableView->currentIndex();
    if(index.isValid() == false){
       QString nameSurname = lineEdit_4->text();
       if(nameSurname == ""){
           QMessageBox::warning(0,tr("Attention"),tr("Il est nécessaire de rentrer un nom et un prénom,"
                                                             "\nséparés d'un espace."),QMessageBox::Ok);
           return true;
       }
       if(nameSurname.contains(" ")){
           int espaceCount = nameSurname.count(" ");
           if(espaceCount > 1){
               QMessageBox::warning(0,tr("Attention"),tr("Mettez un ; entre le nom et le prénom svp,\n "
                                                     "sans espace autour,pour un bon enregistrement.\n "
                                                     "Si le nom ou le prénom contient un espace, laissez le"),
                                                     QMessageBox::Ok);
               return true;
           }
           else{
               nameSurname.replace(" ",";");
           }
       }
       nameSurname.replace(",",";");
       if(nameSurname.contains("'")){
           nameSurname.replace("'","''");
       }
       QStringList namePlusSurname = nameSurname.split(";");
       nom    = namePlusSurname[0];
       prenom = namePlusSurname[1];
       QStringList listGuid;
       QString guid;
       Iotools iotool;
       guid = iotool.newGuidOfNameIndex();
       if (!testNomPrenom(nom,prenom,guid))
       {
             messageNomPrenom(nom,prenom,guid);
           }
   }
   else{
      int row  = index.row();
      nom      = m_proxymodel->data(m_proxymodel->index(row,1,QModelIndex())).toString();
      prenom   = m_proxymodel->data(m_proxymodel->index(row,2,QModelIndex())).toString();
      m_guid   = m_proxymodel->data(m_proxymodel->index(row,0,QModelIndex())).toString();
      }

   }//fin pas de programme appelant
   
    m_nomprenom 	= nom+","+prenom;

    m_ida = 0;
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " m_prenomRecupere = " << m_prenomRecupere;
//---------------si récupération du nom du patient ----------------------------------
    if((m_nomRecupere == "")==false){
	    
            QString nom    = m_nomRecupere;
	    QString prenom = m_prenomRecupere;
   	    m_guid = m_guidEntre;

	    m_nomprenom    = nom+","+prenom;
        }

    	if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) <<  " apres replace : "+m_nomprenom;
//--------------------------------entree libre et somme libre ---------------------------------------------------------

    if((freeentry == "") == false ){
	    if((freevalue == 0) == false)
		{
		    m_nomprenom = freeentry;
		    message_libre();
			}
	   else	{
	        m_nomprenom = freeentry;
	        message_libre();
		}
		lineEdit->clear();
		}
    if((freevalue == 0) == false ){
        if (m_test) qDebug() << "dans somme entree : "+m_nomprenom;
        freeReceipValueEdit->clear();
        }
    //-------------------------particularites semantiques --------------------------------------------
    if(m_nomprenom.contains("'"))
    {
        m_nomprenom.replace("'","''");
        }

    double espece     = 0;
    double cheque     = 0;
    double cartebleue = 0;
    double virement   = 0;
    double daf        = 0;
    double autre      = 0;
    double du         = 0;
    // --------------seance--------------------------
    m_actesSeance += " "+act+" ";
    //------------------------------------------------
    //ponderation
    acte = acte * factor/100;
    if(Espece->isChecked()==true){
       espece     = acte;
       m_especeSeance += acte;
        	}
  //fin de espece
    else if(Cheque->isChecked()){
       cheque     = acte;
       m_chequeSeance += acte;
                }
  //fin de cheque
    else if(cb->isChecked()){
       cartebleue = acte;
       m_carteBleueSeance += acte;
       }
    //fin de carte bancaire
    else if(virementBox->isChecked()){
	virement = acte;
        m_virementSeance += acte;
		}
    // fin virement
  else if(checkBox->isChecked()){
    daf = acte;
    m_dafSeance += acte;
  }
  //fin de dispense d'avance des frais
  else if(checkBox_4->isChecked()){
    du = acte;
    m_duSeance += acte;
    }
  //fin de dus

 else{
     QMessageBox::information(0,tr("erreur"),tr("vous avez oublié de choisir le type de paiement"),QMessageBox::Ok);
     return -1.00;
     }
  if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " "+QString::number(acte);
  if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " m_nomprenom = " << m_nomprenom;
  QString requeteActe = "INSERT INTO honoraires "
                       " (id_usr,id_drtux_usr,id_site,id_payeurs,patient,GUID,praticien,date,acte,remarque,esp,chq,cb,vir,daf,autre,du,valide)"
	               "VALUES ("
	               ""+QString::number(m_idusr)+","
	               ""+QString::number(m_ida)+","
	               "'"+findIdSite()+"',"
	               "'"+findIdPayeur()+"',"
	               "'"+m_nomprenom+"',"
	               "'"+m_guid+"',"
	               "'"+m_praticien+"',"
	               "'"+m_date+"',"
	               "'"+act+"',"
	               "NULL,"
	               ""+QString::number(espece)+","
	               ""+QString::number(cheque)+","
	               ""+QString::number(cartebleue)+","
	               ""+QString::number(virement)+","
	               ""+QString::number(daf)+","
	               ""+QString::number(autre)+","
	               ""+QString::number(du)+",0)";
  QSqlQuery query6(m_accountancyDb);
    if(!query6.exec(requeteActe)){
      qWarning() << __FILE__ << QString::number(__LINE__) << " " << query6.lastError().text();
      }

  if (m_test) qDebug() << enregistrement(query6);

  //--------------insertion dans table rapprochement_bancaire du solde actuel--------------------------
  /*QString nouveauSolde = "";
  QString reqSolde = "SELECT solde FROM rapprochement_bancaire";
  int idCompte = m_hashCompte.key(bankAccountCcomboBox->currentText()).toInt();
  QSqlQuery querySolde(m_accountancyDb);
  QSqlQuery queryInsertBank(m_accountancyDb);
  if(Espece->isChecked() || Cheque->isChecked() || cb->isChecked()){
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
      nouveauSolde = QString::number(resultSolde+acte);
   QString requeteInsertSolde ="INSERT INTO rapprochement_bancaire (id_compte,solde) "
                               "VALUES("+QString::number(idCompte)+",'"+nouveauSolde+"')";
   if(!queryInsertBank.exec(requeteInsertSolde)){
       if (m_test) qDebug() << "Erreur ="+queryInsertBank.lastError().text()+"\n"+requeteInsertSolde;
   }
  }*/

  //---------------------------------------------------------------------------------------------
  //----------------------séances ---------------------------------------------------------------
  m_idHono = query6.lastInsertId().toString();
  //---------------------------------------------------------------------------------------------
  modele2->setQuery("SELECT * FROM honoraires",m_accountancyDb);

  int newcount = modele2->rowCount();
  modele2->clear();
  //control registering
  QMessageBox *message2 = new QMessageBox;
  if(newcount == count+1)
  {
      return acte;
      }

  else
  {
      message2->warning(0,tr("erreur"),tr("erreur insertion recette"),QMessageBox::Ok);
      return 0.00;
      }
  rafraichir();
  return acte;
}


QString recettedialog::enregistrement(QSqlQuery &namequery){
  QString result;
  QString result1;
  QString result2;
  QString result3;
  QString result4;
  QString result5;
  QString result6;
  int id = namequery.lastInsertId().toInt();

  QSqlQuery query(m_accountancyDb);
  if(!query.exec("SELECT id_hono,id_usr,id_drtux_usr,patient,GUID,praticien,"
                    "date,acte,remarque,esp,chq,cb,daf,autre,du,valide,tracabilite "
           " FROM honoraires WHERE id_hono = "+QString::number(id)+"")){
               qWarning() <<  __FILE__ << QString::number(__LINE__) << query.lastError().text();

           }
    while(query.next()){
        result1 = query.value(9).toString();
        result2 = query.value(10).toString();
        result3 = query.value(11).toString();
        result4 = query.value(12).toString();
        result5 = query.value(13).toString();
        result6 = query.value(14).toString();
        result = "-"+result1 + "-"+result2 + "-"+result3 + "-"+result4+ "-"+result5 + "-"+result6;
  }
  if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) <<" result = "+result;
  result.replace(".0","");
  result.replace("-0","-");
  result.replace("-","");
  m_executedquery = result;

  //result.replace("0","");

  return m_executedquery;
}

void recettedialog::fermer()
{
  QFile f(m_pathabsolu+"/errorLog.txt");
  f.open(QIODevice::ReadWrite|QIODevice::Truncate);
  f.close();
  close();// QApplication::closeAllWindows();
}

void recettedialog::fermer2()
{
  close();//QApplication::closeAllWindows();
}

void recettedialog::modifierrecettes()
{
    QString guid = m_guidEntre;
    QString user = userComboBox->currentText();
    QString site = sitesComboBox->currentText();
    modifierrecette * modifrecette = modifierrecette::instance();
    if(m_parent)
    {
        m_parent->setCentralWidget(modifrecette);
        }
    modifrecette->setDatas(guid,user,site);
    modifrecette->executeAndShow();
}

void recettedialog::on_lineEdit_textChanged(){
  label_5->setText("<html><body><font size = 6 color = ""red"">somme libre ici</font></body></html>");
  //resultNameLabel->setText("<html><body><font size = 3 color = ""red"">enregistrez par 'autres actes'</font></body></html>");
}

void recettedialog::message_libre(){
  QMessageBox::information(0,tr("info"),tr("l'intitulé libre prime sur le nom et le prénom"),QMessageBox::Ok);
}
//------------------texte changed----------------------------------------------------------------------------------
void recettedialog::textechange(const QString &m_texte){
  QString text = m_texte;
  if(text.contains("'")){
      text.replace("'","''");
  }
  m_tableModel = new QSqlTableModel(this,m_accountancyDb);
  m_tableModel->setTable("nameindex");
  m_tableModel->setFilter("name LIKE '"+text+"%'");
  m_tableModel->setSort(1,Qt::AscendingOrder);//on classe par ordre alphabétique
  m_tableModel->select();
  m_proxymodel = new QSortFilterProxyModel;
  m_proxymodel->setSourceModel(m_tableModel);
  tableView->setModel(m_proxymodel);
  //tableView->setColumnHidden(0,true);//
  //tableView->setColumnHidden(3,true);//
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->resizeColumnsToContents();
  tableView->resizeRowsToContents();
}
//---------------------texte non change-------------------------------------------------
void recettedialog::textenonchange(){
  m_tableModel = new QSqlTableModel(this,m_accountancyDb);
  m_tableModel->setTable("nameindex");
  m_tableModel->setSort(1,Qt::AscendingOrder);//on classe par ordre alphabétique
  m_tableModel->select();
  m_proxymodel = new QSortFilterProxyModel;
  m_proxymodel->setSourceModel(m_tableModel);
  tableView->setModel(m_proxymodel);
 // tableView->setColumnHidden(0,true);//
 // tableView->setColumnHidden(3,true);//
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->resizeColumnsToContents();
  tableView->resizeRowsToContents();
  tableView->selectRow(0);//pour selectionner la premiere ligne si l'utilisateur oublie
}


//------------------------------------fonction bouton plus------------------------------------------------------------------
void recettedialog::fonctionplus(){
  int plus = factorProgressBar->value();
  double plusdouble;

  QStringList liste;
  	QFile fichier(m_pathabsolu+"/fichierpourcentage.ini");
	if(!fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
	    QMessageBox::warning(0,tr("Attention"),tr("fichierpourcentage.ini ne peut pas être ouvert."),
	    QMessageBox::Ok);
	}
   	QTextStream streamfichier(&fichier);
   	while(!streamfichier.atEnd()){
 	    QString line = streamfichier.readLine();
 	    if(line.contains("=")){
 	        liste << line ;
 	    }
 	    liste.sort();
 	}
  if(plus == 100)
 		return;
  if(plus == 0){
  		QString choisi0 	= liste[0];
 	 	QStringList liste0 	= choisi0.split("=");
 	 	plusdouble 		= liste0[0].toDouble();
 	 	QString text0 		= liste0[1];
 	 	labelPercentages		->setText(text0);
 	 	m_iplusmoins 		= 0;
 	 	m_facteurpourcent	= liste0[0].toDouble();
  	}
  else{
  		int itemliste		= m_iplusmoins+1;
  		if( itemliste		== liste.size()){
  		itemliste 		= liste.size()-1;
  			}
 	 	QString choisi 		= liste[itemliste];
 	 	QStringList liste3 	= choisi.split("=");
 	 	plusdouble 		= liste3[0].toDouble();
 	 	QString text 		= liste3[1];
 	 	m_facteurpourcent	= liste3[0].toDouble();
 	 	labelPercentages		->setText(text);
 	 	if(m_iplusmoins<liste.size()-1){
 	 	m_iplusmoins++;
 	 		}
 	 	else{
 	 	plusdouble 		= 100.00;
 	 	labelPercentages		->setText("100%");
 	 	m_facteurpourcent	= 100.00;
 	 		}

 	}
 	int plusint = static_cast<int>(plusdouble);
 	factorProgressBar->setValue(plusint);
}
//------------------------------------------fonction bouton moins-----------------------------------------------------
void recettedialog::fonctionmoins(){
	//En C++ nous utilisons le mot clef "static_cast<type>(variable).
	int moins = factorProgressBar->value();
	double moinsdouble;


  QStringList liste;
  QFile fichier(m_pathabsolu+"/fichierpourcentage.ini");
	if(!fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
	    QMessageBox::warning(0,tr("Attention"),tr("fichierpourcentage.ini ne peut pas être ouvert."),
	                                           QMessageBox::Ok);
		}
   	QTextStream streamfichier(&fichier);
   	while(!streamfichier.atEnd()){
 	QString line = streamfichier.readLine();
 	if(line.contains("=")){
 	liste << line ;
 	}
 	liste.sort();
 		}
 	if(moins == 0)
		return;
  	else if(moins == 100){
  	m_iplusmoins 			= liste.size();
  	QString choisicent 		= liste[m_iplusmoins-1];
  	QStringList listechoisie 	= choisicent.split("=");
 	moinsdouble 			= listechoisie[0].toDouble();
 	QString textcent 		= listechoisie[1];
 	m_facteurpourcent		= moinsdouble;
 	labelPercentages			->setText(textcent);

 	m_iplusmoins--;
 		}
	else if(m_iplusmoins == 0){
	moinsdouble 			= 0.00;
	labelPercentages			->setText("0%");
	 m_facteurpourcent		= moinsdouble;
		}
	else{
	QString choisicent 		= liste[m_iplusmoins-1];
 	QStringList listechoisie 	= choisicent.split("=");
 	moinsdouble 			= listechoisie[0].toDouble();
 	QString textchoisi 		= listechoisie[1];
 	labelPercentages			->setText(textchoisi);
 	m_facteurpourcent		= moinsdouble;
 		if(m_iplusmoins>0){
 		m_iplusmoins--;
 		}

 		}
 	int moinsencore 		= static_cast<int>(moinsdouble);
 	factorProgressBar				->setValue(moinsencore);
}
//---------------------fonction pour recuperer ccamview-------------------------------------------------------------
void recettedialog::ccamAndValuesProcess(){
    QHash<QString,QString> hashOfValuesChoosened;
    FindReceiptsValues * f = new FindReceiptsValues(this,m_accountancyDb);
    if (f->exec()==QDialog::Accepted)
    {    hashOfValuesChoosened = f->getchosenValues();
         //inclure dans m_ccamAndValuesChosen
         QHash<QString,QString>::iterator it;
         for (it = hashOfValuesChoosened.begin(); it != hashOfValuesChoosened.end(); ++it)
         {
               m_ccamAndValuesChosen.insertMulti(it.key(),it.value());
             }
        }
    double totalSum = 0.00;
    for (int i = 0; i < hashOfValuesChoosened.size(); ++i)
    {
          QString value = hashOfValuesChoosened.values()[i];
          totalSum += value.toDouble();
        }
    QString ccamChosenValuesSum = QString::number(totalSum);
    ccamAndValuesLineEdit->setText(ccamChosenValuesSum);
    QStringList keyslist;
    keyslist = m_ccamAndValuesChosen.keys();
    QStringList valueslist;
    valueslist = m_ccamAndValuesChosen.values();    
    QString data;
    data += keyslist.join("+");
    data += "\n=";
    data += "\n"+valueslist.join("+");
    recordButton                    -> setStyleSheet("*{ color:blue }");
    recordButton                    -> setText(tr("Enregistrer\n")+data);
}

QString recettedialog::priceValueOfAct(QString & act)
{
    QString value;
    QSqlQuery query(m_accountancyDb);
    QString req = QString("select %1,%5 from %2 where %3 = '%4'");
    QString reqcomplete;
    for (int table = 0; table < ActsTablesMaxParam; ++table)
    {
          switch(table){
              case NGAP_TABLE :
                  reqcomplete = req.arg("montant_total","actes_disponibles","nom_acte",act,"date_effet");
                  break;
              case FORFAITS_TABLE :
                  reqcomplete = req.arg("valeur","forfaits","forfaits",act,"type");
                  break;
              case CCAM_TABLE :
                  reqcomplete = req.arg("amount","ccam","code",act,"date");
                  break;
              default :
                  break;    
              }
          if (!query.exec(reqcomplete))
          {
                qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
              }
          QHash<QString,QString> hashValuesDate;
          while (query.next())
          {
              QString value = query.value(0).toString();
              if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
              QString dateOrType = query.value(1).toString();
              if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " dateOrType =" << dateOrType ;
              hashValuesDate.insertMulti(value,dateOrType);              
              }
          if (hashValuesDate.keys().size()>1)
          {
                QStringList dates;
                dates = hashValuesDate.values();
                QString date;
                QDate dateLast(1900,01,01);
                foreach(QString dateStud,dates){
                    if (QVariant(dateStud).canConvert<QDate>())
                    {
                          QDate dateStudied = QDate::fromString(dateStud,"yyyy-MM-dd");
                          if (dateStudied >= dateLast)
                          {
                                dateLast = dateStudied;
                              }
                        }
                    }
                  date = dateLast.toString("yyyy-MM-dd");
                  value = hashValuesDate.key(date);
              }
           else
           {
               if (hashValuesDate.keys().size()>0)
               {
                     value = hashValuesDate.keys()[0];
                     if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " value next =" << value ;
                   }            
               }
        }
    return value;
}

void recettedialog::automatismeOuvert(){
    automatismeDialog *at = new automatismeDialog;
    at -> show();
}

void recettedialog::changeToolTip(const QString & texte){
  if (m_test) qDebug() << texte <<  __FILE__ << QString::number(__LINE__);
  QString texteCombo          = texte;
  QString adresse             = "pas d'adresse connue.";
  for(int i = 0 ; i < m_listAdress.size() ; i++){
      if (m_test) qDebug() << m_listAdress[i];
      QStringList listeCombo  = m_listAdress[i].split("=");
      if(listeCombo[0] == texteCombo){
          adresse             = listeCombo[1];
      }

  }
  payeurComboBox                -> setToolTip(adresse);
}

void recettedialog::rafraichirpayeurComboBox(){
    QStringList listepayeurs;
    payeurComboBox               ->setEditable(true);
    payeurComboBox               ->setInsertPolicy(QComboBox::NoInsert);
    QSqlQuery querypayeurs(m_accountancyDb);
              querypayeurs    .exec("SELECT nom_payeur,adresse_payeur,ville_payeur,code_postal,telephone_payeur FROM payeurs");
        while(querypayeurs.next()){
            QString strpayeur = querypayeurs.value(0).toString();
            QString adresse   = querypayeurs.value(0).toString()+"="+querypayeurs.value(1).toString()+"\n"+ querypayeurs.value(3).toString() +" "+ querypayeurs.value(2).toString()+"\n"+ querypayeurs.value(4).toString();;
            listepayeurs     << strpayeur;
            m_listAdress     << adresse;
        }
    listepayeurs            . sort();
    if(listepayeurs.size() == 0){
        listepayeurs         << "";
    }
    listepayeurs            . prepend ("patient");
    payeurComboBox             -> addItems(listepayeurs);
}

void recettedialog::rafraichirsitesComboBox(){
    sitesComboBox                    ->setEditable(true);
    sitesComboBox                    ->setInsertPolicy(QComboBox::NoInsert);
    QStringList listesites;
    QSqlQuery querysite(m_accountancyDb);
              querysite.exec("SELECT site FROM sites ");
    while( querysite.next())
    {
	    QString strsite= querysite.value(0).toString();
	    listesites    << strsite ;
	      }
	QString idsiteprefentiel = settings()->value(S_ID_SITE_PREFERENTIEL).toString();
	QSqlQuery qySitepref(m_accountancyDb);
	QString reqsitepref = QString("select %1 from %2 where %3 = '%4' ").arg("site","sites","id_site",idsiteprefentiel);
	if (!qySitepref.exec(reqsitepref))
	{
	      qWarning() << __FILE__ << QString::number(__LINE__) << qySitepref.lastError().text() ;
	    }
	while (qySitepref.next())
	{
	    listesites.prepend(qySitepref.value(0).toString());
	    }
    if(listesites.size() < 1){
        listesites        << tr("Veuillez paramétrer un site");
    }
    listesites.removeDuplicates();
    sitesComboBox->addItems(listesites);
}

void recettedialog::setWhatsThisMode(){
  QWhatsThis::enterWhatsThisMode();
}



void recettedialog::mousePressEvent(QMouseEvent * event){
  if(event->button() == Qt::RightButton){
    if (m_test) qDebug() << "dans clic droit" << __FILE__ << __LINE__;
    m_menu         = new QMenu;
    m_menu        -> addAction(m_remontrerActeCourant);
    m_menu        -> exec(QCursor::pos());
  }

}

void recettedialog::redHelp(){
  pushButton_16      -> setStyleSheet("* {background : red }");
  pushButton_16      -> setText(tr("Aide"));
}

void recettedialog::enregistreSeance(){
  QString idPayeur;
  if(payeurComboBox->currentText() == "patient")
  {
      if(!m_guidEntre.isEmpty())
      {
         idPayeur = m_guidEntre;
         }
      else
      {
          idPayeur = "1";
          }
      }
  else
  {
     QString payeur = payeurComboBox->currentText();
     QSqlQuery queryPayeur(m_accountancyDb);
     QString reqPayeur = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("id_payeurs","payeurs","nom_payeur",payeur);
     if (!queryPayeur.exec(reqPayeur))
     {
           qWarning() << __FILE__ << QString::number(__LINE__) << queryPayeur.lastError().text() ;
         }
     while(queryPayeur.next())
     {
        idPayeur =  queryPayeur.value(0).toString();
        }
      }
  QString idUsr;
  QSqlQuery queryUsr(m_accountancyDb);
            queryUsr.exec("SELECT id_usr FROM utilisateurs WHERE login = '"+userComboBox->currentText()+"';");
     while(queryUsr.next()){
         idUsr = queryUsr.value(0).toString();
     }
  QString idSite;
  QSqlQuery querySite(m_accountancyDb);
            querySite.exec("SELECT id_site FROM sites WHERE site = '"+sitesComboBox->currentText()+"';");
     while(querySite.next()){
         idSite = querySite.value(0).toString();
     }

  QString requetePaiement = "INSERT INTO seances "
                               "(id_hono, date, id_payeurs,id_usr,id_site, acte, esp, chq, cb, vir, daf, autre, du) "
                               "VALUES ("
                               "'"+m_idHono+"',"
                               "'"+m_date+"',"
                               "'"+idPayeur+"',"
                               "'"+idUsr+"',"
                               "'"+idSite+"',"
                               "'"+m_actesSeance+"',"
                               "'"+QString::number(m_especeSeance)+"',"
                               "'"+QString::number(m_chequeSeance)+"',"
                               "'"+QString::number(m_carteBleueSeance)+"',"
                               "'"+QString::number(m_virementSeance)+"',"
                               "'"+QString::number(m_dafSeance)+"',"
                               "'"+QString::number(m_autreSeance)+"',"
                               "'"+QString::number(m_duSeance)+"')";
 QSqlQuery queryPaiement(m_accountancyDb);
      if(!queryPaiement.exec(requetePaiement)){
          qWarning() << __FILE__ << QString::number(__LINE__) << queryPaiement.lastError().text()+"\n"+requetePaiement;
          }
  if(m_ouinon == "non"){
      if(letOpenedCheckBox->isChecked() == false){
                    m_actesSeance      = "";
                    m_especeSeance     = 0.00;
                    m_chequeSeance     = 0.00;
                    m_carteBleueSeance = 0.00;
                    m_virementSeance   = 0.00;
                    m_dafSeance        = 0.00;
                    m_autreSeance      = 0.00;
                    m_duSeance         = 0.00;
		QTimer::singleShot(m_tempsFermeture,this,SLOT(fermer()));
			}
		}
  if(m_ouinon == "oui"){
                m_actesSeance      = "";
                m_especeSeance     = 0.00;
                m_chequeSeance     = 0.00;
                m_carteBleueSeance = 0.00;
                m_virementSeance   = 0.00;
                m_dafSeance        = 0.00;
                m_autreSeance      = 0.00;
                m_duSeance         = 0.00;
		QTimer::singleShot(m_tempsFermeture,this,SLOT(fermer()));
  }
}

void recettedialog::saveInThesaurus(){
    QString textOfActs;
    if(recordButton->isEnabled()){
        textOfActs = recordButton -> text();
        if(textOfActs.contains("Enregistrer\n")){
            textOfActs.remove("Enregistrer\n");
            }
        }
    QStringList listOfTexts = textOfActs.split("\n");
    QString type = "C";
    if(listOfTexts[0].contains("V")){
        type = "V";
        }
    QSqlQuery query(m_accountancyDb);
    QString queryString = QString("INSERT INTO %1 (%2,%3) values('%4','%5')").arg("thesaurus",
                                                                                  "sequence",
                                                                                  "type",
                                                                                  listOfTexts[0],
                                                                                  type);
    if(!query.exec(queryString)){
        qWarning()<< query.lastError().text() << __FILE__ << QString::number(__LINE__);
        }
    else{
        QString item = listOfTexts[0];
        QMessageBox::information(0,tr("Information"),item+tr(" a été enregistré dans le thésaurus."),
                                 QMessageBox::Ok);
        thesaurusComboBox->addItem(item);
        }
}

void recettedialog::addIK(int km)
{
    int id = m_mapTypeIK.key(rulesKmComboBox->currentText());
                   if (m_test) qDebug() << __FILE__ << QString::number(__LINE__)
                            << QString::number(m_mapValueIK.value(id).toDouble())
                            << QString::number(distanceSpinBox->value())
                            << QString::number(m_mapAbattementIK.value(id).toDouble());
    QString valueStr = QString::number((m_mapValueIK.value(id).toDouble() * km)
                          - ((m_mapValueIK.value(id).toDouble() * m_mapAbattementIK.value(id).toDouble()) * 2)) ;
    if(valueStr.toDouble() < 0.00)
    {
        valueStr = 0.00 ;
        }
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " valueStr =" << valueStr ;
    m_ccamAndValuesChosen.insert(tr("IK"),valueStr);
    QStringList keyslist;
    keyslist = m_ccamAndValuesChosen.keys();
    QStringList valueslist;
    valueslist = m_ccamAndValuesChosen.values();
    QString data;
    data += keyslist.join("+");
    data += "\n=";
    data += "\n"+valueslist.join("+");
    recordButton                    -> setStyleSheet("*{ color:blue }");
    recordButton                    -> setText(tr("Enregistrer\n")+data);
}


QList<double> recettedialog::variousSums(QString & strActs){
   if(strActs.contains("Enregistrer\n")){
       strActs.remove("Enregistrer\n");
   }
   strActs = strActs.split("\n")[0];
   double totalSum = 0.00;
   QList<double> resultOfValues;
   QStringList listOfActs;
   if(strActs.contains("+")){
        listOfActs = strActs.split("+");
   }
   else{
       listOfActs << strActs;
       }
       QString str;
       foreach(str,listOfActs){
           if (m_test) qDebug() << "str ="+str;
           QString valueStr;
           if(str == "IK"){
               int id = m_mapTypeIK.key(rulesKmComboBox->currentText());
               if (m_test) qDebug() << __FILE__ << QString::number(__LINE__)
                                    << QString::number(m_mapValueIK.value(id).toDouble())
                                    << QString::number(distanceSpinBox->value())
                                    << QString::number(m_mapAbattementIK.value(id).toDouble());
               valueStr = QString::number((m_mapValueIK.value(id).toDouble() * distanceSpinBox->value())
                          - ((m_mapValueIK.value(id).toDouble() * m_mapAbattementIK.value(id).toDouble()) * 2)) ;
               if(valueStr.toDouble() < 0.00){
                   valueStr = 0.00 ;
               }
           }
           else if(!m_actesToolTip.isEmpty() && m_actesToolTip.contains(str)){
               valueStr = m_ccamAndValuesChosen.value(str);
           }
           else{
              QList<int> keyList ;
              keyList = m_mapNameActesNGAP.keys(str);
              if(!keyList.size() == 0){
                  valueStr = m_mapPriceActesNGAP.value(keyList[0]);
              }
              else{
                  keyList = m_mapForfaitsForfaits.keys(str);
                  valueStr = m_mapValueForfaits.value(keyList[0]);
              }
           }
        resultOfValues << valueStr.toDouble();
        if (m_test) qDebug() << "valueStr = "+valueStr;
        totalSum += valueStr.toDouble();

       }

   if(resultOfValues.size() != 0){
       resultOfValues.prepend(totalSum);
   }
   return resultOfValues;//totalSum << eachSum;
}

void recettedialog::controlOfQueries(QString & executedQueryResult){
      QString labeltexte = tr("la somme : ")+executedQueryResult+tr(" a été enregistrée.");
      resultValueLabel           -> setStyleSheet("*{ color:red }");
      if(executedQueryResult.toDouble() > 0.00){
          resultValueLabel  -> setText(labeltexte);
          }
      //remise à zero des checkBox
      //letOpenedCheckBox->setCheckState(Qt::Unchecked);
      if(Espece->isChecked())
         {Espece->setCheckState(Qt::Unchecked);}
      else if(Cheque->isChecked())
         {Cheque->setCheckState(Qt::Unchecked);}
      else if(cb->isChecked())
         {cb->setCheckState(Qt::Unchecked);}
      else if(virementBox->isChecked())
         {virementBox->setCheckState(Qt::Unchecked);}
      else if(checkBox_4->isChecked())
         {checkBox_4->setCheckState(Qt::Unchecked);}
      else if(checkBox->isChecked())
         {checkBox->setCheckState(Qt::Unchecked);}
      else {
          QMessageBox::warning(0,tr("erreur"),tr("aucune case n'est cochée..."),QMessageBox::Ok);
       }
       int progressBarTwoValue = factorProgressBar->value();
       if(progressBarTwoValue == 100){
           ccamAndValuesLineEdit        -> clear();
           QTimer::singleShot(200,this,SLOT(clearAll()));
       }

}

void recettedialog::combo_5_toolTip(const QString& textCombo_5){
  QString text = textCombo_5;
  sitesComboBox->setToolTip(text);
}

void recettedialog::showErrors(){
    QString errorAllString;
    QFile f(settings()->applicationBundlePath()+"/errorLog.txt");
    if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " " << settings()->applicationBundlePath()+"/errorLog";
    if(!f.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,tr("Error"),tr("Error log file cannot be opened."),QMessageBox::Ok);
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

QString recettedialog::correctNameOfAct(QString & str){
    QString strAct = str;
    //Qt::CaseInsensitive
    QSqlQuery query(m_accountancyDb);
    QString req = QString("select %1 from %2");
    QString reqcomplete;    
    for (int table = 0; table < ActsTablesMaxParam; ++table)
    {
          QStringList listOfActs;
          switch(table){
              case NGAP_TABLE :
                  reqcomplete = req.arg("nom_acte","actes_disponibles");
                  break;
              case FORFAITS_TABLE :
                  reqcomplete = req.arg("forfaits","forfaits");
                  break;
              case CCAM_TABLE :
                  reqcomplete = req.arg("code","ccam");
                  break;
              default :
                  break;    
              }
          if (!query.exec(reqcomplete))
          {
                qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
              }
          while (query.next())
          {
              QString a = query.value(0).toString();
              listOfActs << a;
              }
          if (listOfActs.contains(strAct,Qt::CaseInsensitive))
          {
                strAct=listOfActs[listOfActs.indexOf(QRegExp(strAct,Qt::CaseInsensitive))];
                if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " strAct correct =" << strAct ;
                break;
              }
          }
    return strAct;
}



QString recettedialog::findIdPayeur(){
    QString comboText = payeurComboBox->currentText();
    if(comboText.contains("'")){
        comboText.replace("'","''");
        }
    QString ret;
    QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4';").arg("id_payeurs","payeurs","nom_payeur",comboText);
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlQuery q(db);
    if(!q.exec(req)){
        qWarning()  << __FILE__ << " " << QString::number(__LINE__) << " "<< q.lastError().text();
        }
    while(q.next()){
        QString r = q.value(0).toString();
        ret = r;
        }
        if(comboText=="patient"){
            ret = "p";
        }
    return ret;
    }

QString recettedialog::findIdSite(){
    QString comboText = sitesComboBox->currentText();
    if(comboText.contains("'")){
        comboText.replace("'","''");
        }
    QString ret;
    QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4';").arg("id_site","sites","site",comboText);
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlQuery q(db);
    if(!q.exec(req)){
        qWarning()  << __FILE__ << " " << QString::number(__LINE__) << " "<< q.lastError().text();
        }
    while(q.next()){
        QString r = q.value(0).toString();
        ret = r;
        }
        if(comboText=="patient"){
            ret = "p";
        }
    return ret;
    }

void recettedialog::putDown(int state){
   if(state == Qt::Checked){
       userComboBox->hide();
       sitesComboBox->hide();
       bankAccountCcomboBox->hide();
       label_11->hide();
       label_17->hide();
       label_3->hide();
       pulldownCheckBox->setText(tr("Augmenter"));
       }
   else{
       userComboBox->show();
       sitesComboBox->show();
       bankAccountCcomboBox->show();
       label_11->show();
       label_17->show();
       label_3->show();
       pulldownCheckBox->setText(tr("Diminuer"));
       }

}

QStringList recettedialog::findPatientNameGuidForIndependantProgram(){
    QStringList returnList;
    QString nom;
    QString prenom;
    QModelIndex index = tableView->currentIndex();
    if(index.isValid() == false){
       QString nameSurname = lineEdit_4->text();
       if(nameSurname == ""){
           QMessageBox::warning(0,tr("Attention"),tr("Il est nécessaire de rentrer un nom et un prénom,"
                                                             "\nséparés d'un espace."),QMessageBox::Ok);
           return returnList;
       }
       if(nameSurname.contains(" ")){
           int espaceCount = nameSurname.count(" ");
           if(espaceCount > 1){
               QMessageBox::warning(0,tr("Attention"),tr("Mettez un ; entre le nom et le prénom svp,\n "
                                                     "sans espace autour,pour un bon enregistrement.\n "
                                                     "Si le nom ou le prénom contient un espace, laissez le"),
                                                     QMessageBox::Ok);
               return returnList;
           }
           else{
               nameSurname.replace(" ",";");
           }
       }
       nameSurname.replace(",",";");
       if(nameSurname.contains("'")){
           nameSurname.replace("'","''");
       }
       QStringList namePlusSurname = nameSurname.split(";");
       nom    = namePlusSurname[0];
       prenom = namePlusSurname[1];
       QStringList listGuid;
       QString guid;
       QSqlQuery queryGuid(m_accountancyDb);
                 queryGuid.exec("SELECT guid FROM nameindex");
                 while(queryGuid.next()){
                     QString s = queryGuid.value(0).toString();
                     listGuid << s;
                 }
       if(listGuid.size() == 0){
           guid = "1";
           }
       else{
           listGuid.sort();
           guid = QString::number(listGuid.last().toInt()+1);
       }
       m_guid = guid;
       /*QSqlQuery queryTest(m_accountancyDb);
       QSqlQuery queryMessOk(m_accountancyDb);
       QStringList listTest;
       QMessageBox mess;
                   mess.setText(tr("Voulez vous enregistrer ce nom et ce prénom ?"));
                   mess.setStandardButtons(QMessageBox::Ok|QMessageBox::No);
                   mess.setDefaultButton(QMessageBox::Ok);
                   int retMess = mess.exec();
                   switch(retMess){
                       case QMessageBox::Ok :
                         queryTest.exec("SELECT * FROM nameindex WHERE name = '"+nom+"' AND surname = '"+prenom+"'");
                         while(queryTest.next()){
                             QString s = queryTest.value(0).toString();
                             listTest << s;
                         }
                         if(listTest.size()> 0){
                             QMessageBox::StandardButton ret;
                             ret = QMessageBox::warning(0,tr("warning"),tr("Le  nom et le prénom "
                             "existent déjà dans la base nameindex.\nVoulez vous quand même le réenregistrer ?"),
                                 QMessageBox::Yes|QMessageBox::No);
                             if (m_test) qDebug() << "ret ="+QString::number(ret);
                             if(ret == QMessageBox::Yes){
                                 queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                                  "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')");
                                 QMessageBox::information(0,tr("Information"),tr("Le  nom et le prénom "
                                                     "ont été enregistrés de nouveau dans la base nameindex."),
                                                     QMessageBox::Ok);
                             }
                             else{
                                 if (m_test) qDebug() << "in else " << __FILE__ << QString::number(__LINE__);
                                 break;
                             }
                         }
                         else{
                             if(!queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                              "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')")){
                                 QMessageBox::warning(0,tr("Warning"),tr("Le  nom et le prénom "
                                                     "n'ont pas été enregistrés dans la base nameindex.\n")
                                                     +queryMessOk.lastError().text(),QMessageBox::Ok);
                             }
                             else{
                                 QMessageBox::information(0,tr("Information"),tr("Le  nom et le prénom "
                                                     "ont été enregistrés dans la base nameindex."),QMessageBox::Ok);
                             }
                           }
                         break;
                       case QMessageBox::No :
                         break;
                       default :
                         break;
                   }*/
    }
      else{
   int row = index.row();
      nom          = m_proxymodel->data(m_proxymodel->index(row,1,QModelIndex())).toString();
      prenom       = m_proxymodel->data(m_proxymodel->index(row,2,QModelIndex())).toString();
      m_guid       = m_proxymodel->data(m_proxymodel->index(row,0,QModelIndex())).toString();
      }
   if (m_test) qDebug() << __FILE__ << QString::number(__LINE__)
            << " nom = "+nom
            << " prenom = "+prenom
            << " m_guid = "+m_guid;
   returnList << nom << prenom << m_guid;
   return returnList;
}

void recettedialog::modifyListOfValues()
{
    PondereActes p(this,m_ccamAndValuesChosen);
    if (p.exec()==QDialog::Accepted)
    {
    	  m_ccamAndValuesChosen = p.getModifiedValues();
        }
    QStringList keyslist;
    keyslist = m_ccamAndValuesChosen.keys();
    QStringList valueslist;
    valueslist = m_ccamAndValuesChosen.values();
    QString data;
    data += keyslist.join("+");
    data += "\n=";
    data += "\n"+valueslist.join("+");
    recordButton                    -> setStyleSheet("*{ color:blue }");
    recordButton                    -> setText(tr("Enregistrer\n")+data);
}

int recettedialog::messageThesaurus()
{
    QCheckBox *checkBox = new QCheckBox(tr("Ne plus présenter ce message."));
    QMessageBox mess;
    mess.setWindowTitle("Thesaurus");
    mess.setText(tr("Voulez vous enregistrer ce shéma dans le thésaurus personnel ?"));
    mess.setInformativeText(tr("Vous pouvez aussi le faire par le bouton plus thesaurus avant un enregistrement."));
    mess.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    mess.addButton(checkBox,QMessageBox::ApplyRole);
    int ret = mess.exec();
    if (checkBox->isChecked())
    {
            	 if (m_test) qDebug() << __FILE__ << QString::number(__LINE__) << " in qmessagebox apply " ;
                 m_noMessageThesaurusHash.insert("noMoreMessageThesaurus",true);
                 settings()->noMoreMessageThesaurus();
                 return ret;
                }
    return ret;
}

void recettedialog::rafraichir()
{
  QSqlTableModel *tableModel = new QSqlTableModel(this,m_accountancyDb);
  tableModel->setTable("nameindex");
  tableModel->setSort(1,Qt::AscendingOrder);//on classe par ordre alphabétique
  tableModel->select();
  tableView->setModel(tableModel);
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->resizeColumnsToContents();
  tableView->resizeRowsToContents();
  tableView->selectRow(0);
}

void recettedialog::messageNomPrenom(QString nom, QString prenom, QString guid)
{
    QSqlQuery queryTest(m_accountancyDb);
       QSqlQuery queryMessOk(m_accountancyDb);
       QStringList listTest;
       QMessageBox mess;
                   mess.setText(tr("Voulez vous enregistrer ce nom et ce prénom ?"));
                   mess.setStandardButtons(QMessageBox::Ok|QMessageBox::No);
                   mess.setDefaultButton(QMessageBox::Ok);
                   int retMess = mess.exec();
                   switch(retMess){
                       case QMessageBox::Ok :
                         queryTest.exec("SELECT * FROM nameindex WHERE name = '"+nom+"' AND surname = '"+prenom+"'");
                         while(queryTest.next()){
                             QString s = queryTest.value(0).toString();
                             listTest << s;
                             }
                         if(listTest.size()> 0){
                             QMessageBox::StandardButton ret;
                             ret = QMessageBox::warning(0,tr("warning"),tr("Le  nom et le prénom "
                             "existent déjà dans la base nameindex.\nVoulez vous quand même le réenregistrer ?"),
                                 QMessageBox::Yes|QMessageBox::No);
                             if(ret == QMessageBox::Yes){
                                 queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                                  "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')");
                                 QMessageBox::information(0,tr("Information"),tr("Le  nom et le prénom "
                                                     "ont été enregistrés de nouveau dans la base nameindex."),
                                                     QMessageBox::Ok);
                                 break;
                                 }
                             else{
                                 if (m_test) qDebug() << "in else " << __FILE__ << __LINE__;
                                 break;
                                 }
                             }
                         else{
                             if(!queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                              "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')")){
                                 QMessageBox::warning(0,tr("Warning"),tr("Le  nom et le prénom "
                                                     "n'ont pas été enregistrés dans la base nameindex.\n")
                                                     +queryMessOk.lastError().text(),QMessageBox::Ok);
                                 break;
                                 }
                             else{
                                 QMessageBox::information(0,tr("Information"),tr("Le  nom et le prénom "
                                                     "ont été enregistrés dans la base nameindex."),QMessageBox::Ok);
                                break;
                                }
                           }
                         break;
                       case QMessageBox::No :
                         break;
                       default :
                         break;
                   }
}

bool recettedialog::testNomPrenom(QString nom,QString prenom,QString guid)
{
    QSqlQuery q(m_accountancyDb);
    QString req = QString("select * from %1 where %2 = '%3' and %4 = '%5'")
    .arg("nameindex","name",nom,"surname",prenom,"guid",guid);
    if (!q.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    QStringList testlist;
    while (q.next())
    {
        testlist << q.value(0).toString();
        }
    if (testlist.size()<1)
    {
          return false;
        }
    return true;
}

void recettedialog::percentageChanged(int percent)
{
    factorProgressBar->setValue(percent);
    QString percentage = QString::number(percent)+" %";
    labelPercentages->setText(percentage);
}



#include "recettedialog.h"
#include "pondere.h"
#include "recettesconstants.h"

#include <common/constants.h>
#include <common/icore.h>
#include <common/databasemanager.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/position.h>
#include <ccam/findreceiptsvalues.h>

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QDebug>
#include "string.h"
#ifdef Q_WS_WIN
#include "windows.h"
#endif

#include "rafraichirdialog.h"
#include "../../common/creationtable.h"
#include "../../common/configure/sitesdialog.h"
#include "../../common/configure/payeursdialog.h"
#include "../../common/configure/actesdispo.h"
#include "../../common/configure/pourcentages.h"
#include "../../common/configure/parametredialog.h"
#include "../../common/configure/automatisme.h"
#include "../../common/configure/rapport.h"
#include "../../common/completeBase.h"
#include "../../common/configure/utilisateur.h"
#include <common/configure/util.h>

/*Table Actes_Disponibles = ngap
Table thesaurus = actes preferes du praticien
Table forfaits = majorations et forfaits*/

using namespace Common;
using namespace Constants;
using namespace RecettesConstants;
enum { WarnDebugMessage = true };

static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}//pointer on commandLineParser
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}
QString g_pathabsolu;

recettedialog::recettedialog(QWidget * parent)
{
    m_parent = new QMainWindow;
    m_parent = qobject_cast<QMainWindow*>(parent);
    g_pathabsolu = settings()->applicationBundlePath();
    if (WarnDebugMessage) qDebug() << "not connected yet" << __FILE__ << QString::number(__LINE__);
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
    m_programName    = listRecup[0];
    m_nomRecupere    = listRecup[1];
    m_prenomRecupere = listRecup[2];
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__)<< "list of recup = " << " program = "+m_programName+"\n"
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
  if (WarnDebugMessage) qDebug() << "settings()->value(Constants::S_REGLE_HABITUELLE).toString() ="+Common::ICore::instance()->settings()->value(Constants::S_REGLE_HABITUELLE).toString();
  m_mapRegleAssociation.insert("habituelle",settings()->value(Constants::S_REGLE_HABITUELLE).toString());
  m_mapRegleAssociation.insert("radiologie",settings()->value(Constants::S_REGLE_RADIO).toString());
  m_mapRegleAssociation.insert("traumatologie",settings()->value(Constants::S_REGLE_TRAUMATO).toString());

//----appel des bases----------------------
    m_accountancyDb  = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);//comptabilite
    if (WarnDebugMessage) qDebug() << "driverName in recettedialog ="+m_accountancyDb.driverName();
//----test de la base nameindex si pas de programme appelant------------------
    if(m_programName == ""){
        QStringList listOfTables = m_accountancyDb.tables();
        QString str;
        QString strResult;
        foreach(str,listOfTables){
            strResult += str+"\n";
        }
        //if (WarnDebugMessage) qDebug() << "listOfTables ="+strResult;
        if((listOfTables.contains("nameindex")) == false){
            if(createTable_IndexName()== false){
              QMessageBox::critical(0,trUtf8("Fatal"),trUtf8("La table nameindex n'a pas été créée.\nLe programme "
                                                             "ne peut pas fonctionner.\nError = ")
                                                             +m_accountancyDb.lastError().text(),QMessageBox::Ok);
            }
        }
    }
//----initialisation no message thesaurus
    m_noMessageThesaurusHash.insert("noMoreMessageThesaurus",false);
//--------test table id_payeur et honoraires ---------------------
    QSqlQuery queryTestIdPayeur(m_accountancyDb);
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
    if(resultIdPayeur == true){
        QString reqAlterIdPayeur = "ALTER TABLE `honoraires` CHANGE `id_payeur` `id_payeurs` VARCHAR( 36 ) "
                                    "CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL";
        QSqlQuery queryCorrIdPayeur(m_accountancyDb);
        if(!queryCorrIdPayeur.exec(reqAlterIdPayeur)){
            qWarning() << __FILE__ << QString::number(__LINE__) << queryCorrIdPayeur.lastError().text();
            }
        else{
            QMessageBox::warning(0,trUtf8("Attention"),trUtf8("La colonne id_payeur a été corrigée"),QMessageBox::Ok);
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
     	      QMessageBox::information(0,trUtf8("Information")
     	      ,trUtf8("vir est ajouté à table honoraires"),QMessageBox::Ok);
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
     	      QMessageBox::information(0,trUtf8("Information")
     	      ,trUtf8("vir est ajouté à table seances"),QMessageBox::Ok);
     	      }
        }
//----initialisation de variables de classe-------------
    m_id 		   = 0;
    m_praticien 	   = "";
    m_iplusmoins 	   = 0;
    m_facteurpourcent      = 100.00;
    m_sommeentree          = 0;
//---action clic droit----------------------------------------------------------
    m_remontrerActeCourant = new QAction(trUtf8("Effacer\net remettre l'acte préféré."),this);
//---setup et attributs de la fenetre---------------------------------------------
    setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    QString size = settings()->value(Constants::PREF_SIZE_RECETTE).toString();
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " size = " << size;
    int sizeWidth = size.split(" ")[0].toInt();
    int sizeHeight = size.split(" ")[1].toInt();
    resize(sizeWidth,sizeHeight);
    QString minSize = settings()->value(Constants::MIN_SIZE_RECETTE).toString();
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " size = " << minSize;
    int minsizeWidth = minSize.split(" ")[0].toInt();
    int minsizeHeight = minSize.split(" ")[1].toInt();
    setMinimumSize(minsizeWidth,minsizeHeight);
    position()->centralPosition(this,width(),height());
//hide unused widgets
    seanceButton->hide();

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
    pushButton                  -> setIcon(QIcon());// unset the icon
    ccamButton               -> setIcon(QIcon(settings()->iconPath() +"/pixmap/ccam_view_icn.png"));

    label_12->setText("100%");//pourcentage dus
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
    checkBox_2    ->setCheckable(true);
//permet de donner aux checkBox un comportement de radioButtons----------
    Espece        ->setAutoExclusive(true);
    Cheque        ->setAutoExclusive(true);
    cb            ->setAutoExclusive(true);
    virementBox   ->setAutoExclusive(true);
    checkBox_4    ->setAutoExclusive(true);
    checkBox      ->setAutoExclusive(true);
    checkBox_2    ->setAutoExclusive(false);
//------------------configuration de l aide---------------------------------------------------------------------------
    pushButton_16 -> setToolTip  (trUtf8("Pour l'aide cliquez sur ce bouton,\ndéplacez la souris sur la zone voulue,\ncliquez "
                                                      "pour afficher l'aide."));
    lineEdit      -> setWhatsThis(trUtf8("Cette entrée permet de saisir une recette indépendamment "
                                         "du patient.\nPar exemple paiement d'une garde par une association."));
    freeReceipSumEdit    -> setWhatsThis(trUtf8("Permet de saisir une somme non paramétrée.\nPar exemple le paiement global d'une garde."));
    lineEdit_4    -> setWhatsThis(trUtf8("Affichage du nom du patient.\n"
                                         "Si le logiciel fonctionne en dehors d'un programme appelant,\n"
                                         "veuillez rentrer le nom et prénom séparés par une virgule ou un ;."));
    tableView     -> setWhatsThis(trUtf8("Cette zone permet la recherche du nom, mais disparaît si le logiciel est lancé par DrTux."));
    cb            -> setWhatsThis(trUtf8("Virement ou carte bancaire."));
    checkBox      -> setWhatsThis(trUtf8("Dispense d'avance des frais."));
    checkBox_2    -> setWhatsThis(trUtf8("Permet de bloquer en position ouverte la fenêtre \nsi le choix de fermeture "
                                         "automatique est programmé.\nVous pouvez programmer ce choix "
                                         "par le bouton Automatisme de fermeture."));
    pushButton_13 -> setWhatsThis(trUtf8("Vous pouvez programmer l'automatisme \nde fermeture de la fenêtre en fin de saisie."));
    pushButton    -> setWhatsThis(trUtf8("Enregistre l'acte courant.\nPour enregistrer les autres actes, "
                                         "cliquez sur le déroulant \"NGAP\" ou sur la CCAM\net le bouton d'enregistrement "
                                         "des autres actes sera découvert."));
    recordButton  -> setWhatsThis(trUtf8("Enregistre les autres actes ou le résultat de la CCAM"));
    controlButton  -> setWhatsThis(trUtf8("Vous pouvez voir les recettes avec ce bouton, les effacer,\nles filtrer selon plusieurs critères"));
    seanceButton  -> setWhatsThis(trUtf8("Permet d'enregistrer plusieurs recettes non liées pour ce patient"
                                         ", par exemple une journée en clinique.\n"
                                         "Les recettes enregistrées seront celles que vous venez de saisir pour ce patient."));
    pushButton_7 -> setWhatsThis(trUtf8("Enregistre dans votre thésaurus d'actes préférés."));
    pushButton_8 -> setWhatsThis(trUtf8("Lance la fenêtre de lecture des messages du programme. A coller dans un rapport de bug."));
    ccamButton -> setWhatsThis(trUtf8("Lance le logiciel CcamView."));
   // pushButton_14 -> setWhatsThis(trUtf8("Vous permez de configurer votre utilisateur préférentiel pour la comptabilité"));
   // pushButton_15 -> setWhatsThis(trUtf8("Lance la fenêtre de création des utilisateurs de la comptabilité"));
   // pushButton_17 -> setWhatsThis(trUtf8("En cliquant sur ce bouton vous pouvez envoyer directement un rapport à l'auteur."));
    groupBox_6    -> setWhatsThis(trUtf8("Vous déterminez ici le pourcentage payé ou du par le patient \n"
                                         "ou un autre payeur du déroulant."));
    label         -> setWhatsThis(trUtf8("Par le bouton enregistrer dans le thésaurus on place dans cette tables de données"
                                         "la séquence des actes préférés .\nPar exemple C+MNO, ou C+DEQP001."
                                         "Cliquer dessus permettra d'inscrire cette séquence directement sur "
                                         "le bouton d'enregistrement."));
    comboBox      -> setWhatsThis(trUtf8("Par le bouton enregistrer dans le thésaurus on place dans cette tables de données"
                                         "la séquence des actes préférés .\nPar exemple C+MNO, ou C+DEQP001."
                                         "Cliquer dessus permettra d'inscrire cette séquence directement sur "
                                         "le bouton d'enregistrement."));
    label_2       -> setWhatsThis(trUtf8("La date  d'encaissement de l'acte"));
    dateEdit      -> setWhatsThis(trUtf8("La date  d'encaissement de l'acte"));
    label_14      -> setWhatsThis(trUtf8("La date de l'acte"));
    dateEdit_2    -> setWhatsThis(trUtf8("La date de l'acte"));
    label_11      -> setWhatsThis(trUtf8("Le lieu où a été pratiqué l'acte. Par exemple le cabinet médical, ou la clinique."));
    comboBox_5    -> setWhatsThis(trUtf8("Le lieu où a été pratiqué l'acte. Par exemple le cabinet médical, ou la clinique."));
    label_3       -> setWhatsThis(trUtf8("Le praticien qui a effectué l'acte"));
    comboBox_2    -> setWhatsThis(trUtf8("Le praticien qui a effectué l'acte"));
    label_9       -> setWhatsThis(trUtf8("Choix du payeur : le plus souvent le patient ci nommé,\n"
                                         "mais ça peut être une structure ou une institution."));
    comboBox_3    -> setWhatsThis(trUtf8("Choix du payeur : le plus souvent le patient ci nommé,\n"
                                         "mais ça peut être une structure ou une institution."));
    ccamButton -> setWhatsThis(trUtf8("Ce bouton lance le logiciel de gestion des actes CCAM.\nVous choisirez un acte,"
                                         " vous le validerez en double cliquant dessus, vous confirmerez.\nAprès calcul, l'acte "
                                         "ou les actes seront calculés et leur somme corrigée dans la fenêtre à droite, selon "
                                         "le choix ci-dessous (par exemple  règle d'association habituelle : 100, 50, 0."));
    CcamLineEdit    -> setWhatsThis(trUtf8("Affichage des actes sélectionnés dans la CCAM, à enregistrer avec le gros bouton de droite."));
    label_17      -> setWhatsThis(trUtf8("Choix du compte bancaire de dépôt."));
    comboBox_6    -> setWhatsThis(trUtf8("Choix du compte bancaire de dépôt."));
    rulesKmComboBox    -> setWhatsThis(trUtf8("Type d'indemnités de déplacement."));
    spinBox       -> setWhatsThis(trUtf8("Nombre de kilomètres Aller Retour."));
    label_18      -> setWhatsThis(trUtf8("Type d'indemnités de déplacement."));
    pondereButton -> setToolTip(trUtf8("Permet d'appliquer un facteur de pondération à chaque acte affiché sur le bouton."));
    //seanceButton  -> setWhatsThis(trUtf8("Enregistre le calcul de la séance."));
   // pushButton_4  -> setWhatsThis(trUtf8("Lance la fenêtre de configuration des sites où exerce le praticien."));
   // seanceButton  -> setWhatsThis(trUtf8("Si ce bouton n'est pas grisé, son activation met à jour vos bases pour
                                            //la nouvelle comptabilité"));
   //pushButton_7  -> setWhatsThis(trUtf8("Lance la fenêtre qui vous permet de faire vos listes d'organismes payeurs.\n"
   //                                     "Cette liste de payeurs s'affichera dans le cadre pourcentage : par le :"));
   // pushButton_9  -> setWhatsThis(trUtf8("Ce bouton permet de rajouter un pourçentage pour paramétrer plus finement "
   //                                      "la barre de pourçentages."));
   // pushButton_8  -> setWhatsThis(trUtf8("Ce bouton lance la fenêtre de saisie d'actes qui seront affichés dans
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
    comboBox            ->setEditable(1);
    comboBox            ->setInsertPolicy(QComboBox::NoInsert);
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
        comboBox->addItem(strDeux,QVariant());
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

//--------------------hide checkBox_2 "bloquer ouvert" si inutile---------------------------------------------------
      if(m_ouinon == "oui"){
          checkBox_2 ->hide();
      }

//------------------------choix user----------------------------------------------------------------------------------
      comboBox_2                     -> setEditable(1);
      comboBox_2                     -> setInsertPolicy(QComboBox::NoInsert);
      if(m_loginpref == ""){
                m_loginpref         = "admin";
                label_3            -> setStyleSheet("*{ color:red }");
                label_3            -> setText(trUtf8("Utilisateur non configuré.\nVeuillez paramétrer les utilisateurs "
                                                     "et vérifier config.ini."));
                //QWhatsThis::enterWhatsThisMode();
                QTimer::singleShot(3000,this,SLOT(redHelp()));

                comboBox_2         -> setToolTip(trUtf8("Cet utilisateur est générique.\nVeuillez paramétrer "
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
          QMessageBox::information(0,tr("Information"),trUtf8("veuillez créer votre utilisateur "
                                                              "et le choisir comme préférentiel."),QMessageBox::Ok);

          }
      comboBox_2                  ->addItems(m_listelogin);
//--------------------écriture du texte et de la valeur de l'acte courant sur son bouton-------------------------------------
    QString acteprefere           = settings()->value(Constants::S_ACTE_COURANT).toString();
    QString valeuracteprefere     = settings()->value(Constants::S_VALEUR_ACTE_COURANT).toString();
    acteprefere = correctNameOfAct(acteprefere);
    QString valeurduc             = acteprefere+" = "+priceValueOfAct(acteprefere);//ce qui est ecrit sur le gros bouton
    if (WarnDebugMessage) qDebug() << "valeur du C ="+valeurduc;
    std::string valeurducstr      = valeurduc.toStdString();
    const char *valeurchar        = valeurducstr.c_str();
    pushButton                   -> setStyleSheet("*{ color:red ; font-size : 20px }");
    pushButton                   -> setText(QApplication::translate("RecetteDialog", valeurchar, 0,
                                              QApplication::UnicodeUTF8));
    pushButton                   -> setEnabled(true);
    //----shortcut Enter-----------------------------------------

    recordButton          -> setShortcut(QKeySequence::InsertLineSeparator);

    pushButton            -> setShortcut(QKeySequence::InsertParagraphSeparator);
    //recordButton          -> setShortcut(QKeySequence::InsertParagraphSeparator));
//---------------------on cache le bouton enregistrer les actes du comboBox---------------------------------------
//Si la comboBox est activée, on se rendra dans la fonction changebouton()-------------------------------
    recordButton                 -> hide();
    recordButton                 -> setEnabled(false);
    controlButton                 -> setEnabled(true);

//------------------recuperer liste des sites dans la comboBox_5-----------------------------------------------
    rafraichirComboBox_5();
    QString textCombo_5 = comboBox_5->currentText();
    emit combo_5_toolTip(textCombo_5);
//-------------------------recuperer payeurs-----------------------------------------------
    rafraichirComboBox_3();
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
//--------------------------comboBox compte bancaire----------------------------------------
    comboBox_6                   -> setEditable(1);
    comboBox_6                   -> setInsertPolicy(QComboBox::NoInsert);
    QString requeteCompte = "SELECT id_compte,id_usr,libelle,solde_initial FROM comptes_bancaires";
    QStringList listLibelleCompte;
    QSqlQuery queryReqComptes(m_accountancyDb);
              if(!queryReqComptes.exec(requeteCompte)){
                  QMessageBox::warning(0,tr("Erreur"),trUtf8("Erreur =")+queryReqComptes.lastError().text(),
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
            QMessageBox::information(0,trUtf8("Information"),trUtf8("Plusieurs comptes bancaires possibles pour "
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
    comboBox_6 -> addItems(listLibelleCompte);
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
    QFile fichierpourcent(g_pathabsolu+"/fichierpourcentage.ini");
    if (WarnDebugMessage) qDebug() << "g_pathabsolu ="+g_pathabsolu << __FILE__ << " " << QString::number(__LINE__);
	if(!fichierpourcent.open(QIODevice::ReadWrite | QIODevice::Text)){
	QMessageBox::warning(0,tr("Attention"),trUtf8("fichierpourcentage.ini ne peut pas être ouvert."),
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
    	  pushButton->setEnabled(false);
    	  recordButton->setEnabled(false);
        }
//----------------------------les connexions au slots---------------------------------------------------------------
    connect(pushButton,   SIGNAL(pressed()),       this,SLOT(prefActRegistering()));
    connect(recordButton, SIGNAL(pressed()),       this,SLOT(actsRegistering()));
    connect(controlButton, SIGNAL(pressed()),       this,SLOT(modifierrecettes()));
    connect(pushButton_5, SIGNAL(pressed()),       this,SLOT(fermer()));
    connect(seanceButton, SIGNAL(pressed()),       this,SLOT(enregistreSeance()));
    connect(pushButton_7, SIGNAL(pressed()),       this,SLOT(saveInThesaurus()));
    connect(pushButton_8, SIGNAL(pressed()),       this,SLOT(showErrors()));
    connect(ccamButton,SIGNAL(pressed()),       this,SLOT(ccam_process()));
    connect(pushButton_11,SIGNAL(pressed()),       this,SLOT(fonctionplus()));//+
    connect(pushButton_12,SIGNAL(pressed()),       this,SLOT(fonctionmoins()));//-
    connect(pushButton_13,SIGNAL(pressed()),       this,SLOT(automatismeOuvert()));
    connect(pushButton_16,SIGNAL(pressed()),       this,SLOT(setWhatsThisMode()));
    connect(comboBox_5   ,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(combo_5_toolTip(const QString&)));
    connect(comboBox,     SIGNAL(activated(int)),  this,SLOT(changebouton(int)));
    connect(comboBox,     SIGNAL(highlighted(int)),this,SLOT(changeButtonHighlighted(int)));
    connect(comboBox_3,   SIGNAL(highlighted(const QString &)),this,SLOT(changeToolTip(const QString &)));
    connect(lineEdit,     SIGNAL(textChanged(const QString &)),this,SLOT(changebouton2(const QString &)));//nom libre
    connect(freeReceipSumEdit,   SIGNAL(textChanged(const QString &)),this,SLOT(changebouton2(const QString &)));//somme libre
    connect(CcamLineEdit,   SIGNAL(textChanged(const QString &)),this,SLOT(changebouton2(const QString &)));
    connect(CcamLineEdit,   SIGNAL(textChanged(const QString &)),this,SLOT(chooseCCAM(const QString &)));
    connect(spinBox,      SIGNAL(valueChanged(int)),   this,SLOT(chooseIK(int)));
    connect(spinBox,      SIGNAL(valueChanged(int)),   this,SLOT(addIK(int)));//
    connect(m_remontrerActeCourant,SIGNAL(triggered()),this,SLOT(clearAll()));//clic droit
    connect(checkBox_3, SIGNAL(stateChanged ( int )),  this,SLOT(putDown(int )));
    connect(pondereButton,SIGNAL(pressed()),this,SLOT(modifyListOfValues()));
    
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------fin du constructeur----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

recettedialog::~recettedialog(){
    m_sommeentree       = 0 ;
    m_acteCcam          = "";
    m_forfaitsForfaits  = "";
    m_valueForfaits     = "";
}
////////////////////////////fonctions et slots//////////////////////////////////////////////////////////////////
void recettedialog::changebouton(int index){//bouton enregistrer acte
    QString data = comboBox->itemText(index);

        pushButton                       ->setEnabled(false);
        pushButton                       ->hide();
        recordButton                     ->show();
        recordButton                     ->setEnabled(true);

    recordButton                    -> setStyleSheet("*{ color:blue }");
    recordButton                    -> setText(trUtf8("Enregistrer\n")+data);
    if (WarnDebugMessage) qDebug() << "in changebouton_7 data ="+data;
}
 void recettedialog::changeButtonHighlighted(int index){
  m_ccamAndValuesChosen.clear();
  QString data = comboBox->itemText(index);
  recordButton                    -> setText(trUtf8("Enregistrer\n")+data);
  if (data.contains("+"))
  {
        QStringList listOfActs;
        listOfActs = data.split("+");
        foreach(QString act,listOfActs){
            if (act.contains("*"))
            {
                  QStringList l ;
                  l = act.split("*");
                  if (l.size()>0)
                  {
                        act = l[0];
                      }
                }
            QString value = priceValueOfAct(act);
            m_ccamAndValuesChosen.insertMulti(act,value);
            }
      }
    else
    {
        QString value = priceValueOfAct(data);
        m_ccamAndValuesChosen.insertMulti(data,value);
        }
 }

void recettedialog::changebouton2(const QString & texte){//bouton enregistrer autres actes
    if(QObject::sender() == qobject_cast<QLineEdit*>(lineEdit)){//texte libre
        m_dataLibreOne                    = texte;
        if(!texte.isEmpty()){
            m_dataLibreOne += "\n";
            }
        }
    else if(QObject::sender() == qobject_cast<QLineEdit*>(freeReceipSumEdit)){//somme libre
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
    if(recordButton -> isEnabled() == false){
        pushButton                       -> setEnabled(false);
        pushButton                       -> hide();
        recordButton                     -> show();
        recordButton                     -> setEnabled(true);
        }
        recordButton                    -> setStyleSheet("*{ color:green }");
        recordButton                    -> setText(trUtf8("Enregistrer\n")+m_dataLibreOne+m_dataLibreTwo);


}

void recettedialog::clearAll(){
    lineEdit->clear();
    freeReceipSumEdit->clear();
    recordButton -> hide();
    recordButton -> setEnabled(false);
    pushButton   -> show();
    pushButton   -> setStyleSheet("*{ color:red ; font-size : 20px }");
    QString act = settings()->value(Constants::S_ACTE_COURANT).toString();
    QString value = settings()->value(Constants::S_VALEUR_ACTE_COURANT).toString();
    act = correctNameOfAct(act);
    value = priceValueOfAct(act);
    QString acte = act+" = "+value;
    pushButton   -> setText(acte);
    pushButton   -> setEnabled(true);
    m_sommeentree       = 0 ;
    m_acteCcam          = "";
    m_forfaitsForfaits  = "";
    m_valueForfaits     = "";
    m_actesToolTip      = "";
    m_ccamAndValuesChosen.clear();
    spinBox->setValue(0);
    factorProgressBar->setValue(100);
    CcamLineEdit -> clear();
    int closing = settings()->value(Constants::S_CLOSING_TIME).toInt();
    QTimer::singleShot(closing,this,SLOT(clearLabel_7Text()));
}

void recettedialog::clearLabel_7Text(){
    resultValueLabel -> setText("");
}
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//fonction d'enregistrement du C = acte courant//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void recettedialog::prefActRegistering()
{
    QString textPushButton = pushButton->text();
   /* if (settings()->value(NO_MORE_THESAURUS_MESSAGE)==false)
    {
    	int messReturn = messageThesaurus();
        switch(messReturn){
        case QMessageBox::Ok :
            return;
            break;
        case QMessageBox::ApplyRole :
            settings()->noMoreMessageThesaurus();
            break;
        case QMessageBox::Cancel :
            break;
        default :
            break;    
        }
        }*/
    
        
    if (WarnDebugMessage) qDebug() << __FILE__ << " " << QString::number(__LINE__) << " in prefActRegistering()";
    /*if(textPushButton != QString(settings()
                          ->value(Constants::S_ACTE_COURANT).toString()
                          +" = "+
                          settings()
                          ->value(Constants::S_VALEUR_ACTE_COURANT).toString()).replace(" ","")){
        if (WarnDebugMessage) qDebug() << __FILE__ << " " << QString::number(__LINE__) << " no CS = "+settings()
                          ->value(Constants::S_ACTE_COURANT).toString()
                          +"="+
                          settings()
                          ->value(Constants::S_VALEUR_ACTE_COURANT).toString();
        emit actsRegistering();

    }*/
    //else{
    QString nom        = "";
    QString prenom     = "";
    QString userchoisi = comboBox_2->currentText();

    QSqlQueryModel *modele = new QSqlQueryModel;
	modele->setQuery("SELECT * FROM honoraires",m_accountancyDb);
	int count = modele->rowCount();
	modele->clear();

//----------- pas de programme appelant-----------------------------
   if((m_nomRecupere == "")== true){
       QModelIndex index = tableView->currentIndex();
   if(index.isValid() == false){
       QString nameSurname = lineEdit_4->text();
       if(nameSurname == ""){
           QMessageBox::warning(0,trUtf8("Attention"),trUtf8("Il est nécessaire de rentrer un nom et un prénom,"
                                                             "\nséparés d'un espace."),QMessageBox::Ok);
           return;
       }
       if(nameSurname.contains(" ")){
           int espaceCount = nameSurname.count(" ");
           if(espaceCount > 1){
               QMessageBox::warning(0,trUtf8("Attention"),trUtf8("Mettez un ; entre le nom et le prénom svp,\n "
                                                     "sans espace autour,pour un bon enregistrement.\n "
                                                     "Si le nom ou le prénom contient un espace, laissez le"),
                                                     QMessageBox::Ok);
               return;
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
       if (!testNomPrenom(nom,prenom,guid))
       {
             messageNomPrenom(nom,prenom,guid);
           }
   }
   else{
   int row = index.row();
      nom          = m_proxymodel->data(m_proxymodel->index(row,1,QModelIndex())).toString();
      prenom       = m_proxymodel->data(m_proxymodel->index(row,2,QModelIndex())).toString();
      m_guid       = m_proxymodel->data(m_proxymodel->index(row,0,QModelIndex())).toString();
      }

   }
//nom et valeur de l'acte courant
    QString C           = settings()->value(Constants::S_ACTE_COURANT).toString();
    double acte 	= settings()->value(Constants::S_VALEUR_ACTE_COURANT).toDouble();	//valeur de l'acte---
	   acte	        = acte*m_facteurpourcent/100;	//acte rapporte au pourcentage
//------------------------------------------------------------------------------------------------------------------
    QDate daterecup                = dateEdit->date();
	  m_date                       = daterecup.toString("yyyy-MM-dd");


    if (WarnDebugMessage) qDebug() << "userchoisi ="+userchoisi;
    QSqlQuery query5(m_accountancyDb);
	query5.exec("SELECT nom_usr,id_usr FROM utilisateurs WHERE login = '"+userchoisi+"'");
	QStringList listprat;
	while(query5.next()) {
	    m_praticien = query5.value(0).toString();
	    m_idusr = query5.value(0).toInt();
	    listprat << m_praticien;
		}
        if(listprat.size() < 1){
            m_praticien = "Docteur";
            m_idusr = 1;
        }
    m_nomprenom = nom+","+prenom;
	    m_ida  = 0;
//----------------si récupération du nom dans programme appelant--------------------------
    if((m_nomRecupere == "")==false){
	m_guid = m_guidEntre;
	QString nom = m_nomRecupere;
	QString prenom = m_prenomRecupere;
	m_nomprenom = nom+","+prenom;
	}
	if(m_nomprenom.contains("'")){
	    m_nomprenom.replace("'","''");
    		}
    		if (WarnDebugMessage) qDebug() << m_nomprenom;
    QMessageBox *message;


//----------------------------------------------------------------------------------------------------------------
    double espece     = 0;
    double cheque     = 0;
    double cartebleue = 0;
    double virement   = 0;
    double daf        = 0;
    double autre      = 0;
    double du         = 0;
    m_actesSeance += " "+C+" ";
    QSqlQuery query2(m_accountancyDb);

    if(Espece->isChecked()==true){
       espece     = acte;
       m_especeSeance += acte;
		}
    else if(Cheque->isChecked()){
        cheque     = acte;
        m_chequeSeance += acte;
		}
    else if(cb->isChecked()){
	cartebleue = acte;
        m_carteBleueSeance += acte;
		}
    else if(virementBox->isChecked()){
	virement = acte;
        m_virementSeance += acte;
		}
    else if(checkBox->isChecked()){
	daf        = acte;
        m_dafSeance += acte;
		}
    else if(checkBox_4->isChecked()){
	du      = acte;
	m_duSeance += acte;

		}
    else{
	    message->information(0,trUtf8("Erreur"),trUtf8("Vous avez oublié de choisir le type de paiement"),
	    QMessageBox::Ok);
	    return;
	 }
 //------------requete d'insertion----------------------------------------------------------
   QString requeteC = "INSERT INTO honoraires "
                      " (id_hono,id_usr,id_drtux_usr,patient,id_site,id_payeurs,GUID,praticien,date,acte,remarque,esp,chq,cb,vir,daf,autre,du,valide)"
	              "VALUES (NULL,"+QString::number(m_idusr)+","+QString::number(m_ida)+",'"+m_nomprenom+"',"
	              "'"+findIdSite()+"','"+findIdPayeur()+"','"+m_guid+"',"
	              "'"+m_praticien+"','"+m_date+"','"+C+"',NULL,"
	              ""+QString::number(espece)+","+QString::number(cheque)+","+QString::number(cartebleue)+","
	              ""+QString::number(virement)+","+QString::number(daf)+","+QString::number(autre)+","
	              ""+QString::number(du)+",0)";

  /*if (WarnDebugMessage) qDebug() << QString::number(m_idusr)+","+QString::number(m_ida)+",'"+m_nomprenom+"','"+m_guid+"',"
	              "'"+m_praticien+"','"+m_date+"','"+C+"',NULL,"
	              ""+QString::number(espece)+","+QString::number(cheque)+","
	              ""+QString::number(cartebleue)+","+QString::number(daf)+","+QString::number(autre)+","
	              +QString::number(du);*/
  if(!query2.exec(requeteC)){
      qWarning() << __FILE__ << QString::number(__LINE__) << " " << query2.lastError().text();
      }
  if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " "<<	"enregistrement() ="+enregistrement(query2);

  //--------------insertion dans table rapprochement_bancaire du solde actuel--------------------------
  QString nouveauSolde = "";
  QString reqSolde = "SELECT solde FROM rapprochement_bancaire";
  int idCompte = m_hashCompte.key(comboBox_6->currentText()).toInt();
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
       if (WarnDebugMessage) qDebug() << "Erreur ="+queryInsertBank.lastError().text()+"\n"+requeteInsertSolde;
   }
  }

  //---------------------------------------------------------------------------------------------

  //----------------------somme séance--------------------------------------------------
  m_idHono = query2.lastInsertId().toString();

//---------------------------------------------------------------------------------------------
    modele->setQuery("SELECT * FROM honoraires",m_accountancyDb);

    int newcount = modele->rowCount();
    modele->clear();

    if(newcount == count+1)
		{
	QString labeltexte = tr("la somme : ")+m_executedquery+trUtf8(" a été enregistrée.");
	resultValueLabel           -> setStyleSheet("*{ color:red }");
	resultValueLabel           -> setText(""+labeltexte+"");

	//--------------------------------remise à zero des checkBox------------------------------------------------
        //checkBox_2->setCheckState(Qt::Unchecked);
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
    	
	else {//QMessageBox::warning(0,tr("erreur"),trUtf8("aucune case n'est cochée..."),QMessageBox::Ok);
	     return;
	     }
		}

    else
	{QMessageBox::warning(0,tr("erreur"),tr("erreur insertion recette"),QMessageBox::Ok);}

//--------------------------------condition sur la checkBox "plus acte"-----------------------------------------------
	//garde ouvert le widget si cette checkBox est cochée puis décoche toutes les checkBox pour le tour suivant
	//permet de ne pas décocher à la main

    if(m_ouinon == "non"){
    if(checkBox_2->isChecked() == false){
    	pushButton->setEnabled(false);
	QTimer::singleShot(m_tempsFermeture,this,SLOT(fermer()));
			}
		}
    if(m_ouinon == "oui"){
    pushButton->setEnabled(false);
    pushButton->setEnabled(true);
    		}

  //}//end of else
}//end of prefActRegistering
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
    if(QObject::sender() == qobject_cast<QPushButton*>(recordButton)){
        data      = recordButton->text();
        if (WarnDebugMessage) qDebug() << QString::number(__LINE__) <<"qobject = recordButton "+data;
        }
    else if(QObject::sender() == qobject_cast<QPushButton*>(pushButton)){
        data      = pushButton->text().split("=")[0].trimmed();
        if (WarnDebugMessage) qDebug() << "qobject = pushButton "+data << __FILE__ << QString::number(__LINE__);
        }
    //progressBar
    int factor = 0;
    factor = factorProgressBar->value();
    QStringList actsChoosenList;
    actsChoosenList = m_ccamAndValuesChosen.keys();
    foreach(QString act,actsChoosenList){
        if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " actsChoosenList =" << QString::number(actsChoosenList.size()) ;
        QString value = m_ccamAndValuesChosen.value(act);
        result += eachActRegistering(act,value,factor);
        }
    if(result == 0.00){
        QMessageBox::warning(0,trUtf8("Warning"),
                             trUtf8("Un enregistrement est défectueux,\nVérifiez dans Recettes !\n")
                             +__FILE__
                             +QString::number(__LINE__),
                             QMessageBox::Ok);
        }
    else{
        QString executedQueryResult = QString::number(result);
        if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " result = "+executedQueryResult;
        controlOfQueries(executedQueryResult);
        }
    if(m_ouinon == "non"){
            if(checkBox_2->isChecked() == false){
	            pushButton->setEnabled(false);
		        QTimer::singleShot(m_tempsFermeture,this,SLOT(fermer()));
	         }
	}
	if(m_ouinon == "oui"){
	    pushButton->setEnabled(false);
        recordButton->setEnabled(true);
        }
}
////////////////////////////////////////////////////////////////////////////////////
//fonction d'enregistrement des autres actes/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
double recettedialog::eachActRegistering(QString act, QString value,int factor)
{
    m_acteautre = act;
    double acte = value.toDouble();
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " act =" << act ;
    QString nom 	                         = "";
    QString prenom 	                         = "";
    QString userchoisi                       = comboBox_2->currentText();

    //----nombre de lignes au départ-------------------------------------
    QSqlQueryModel *modele2                  = new QSqlQueryModel;
  	            modele2                  ->setQuery("SELECT * FROM honoraires",m_accountancyDb);
    int count                                = modele2->rowCount();
    modele2                                  ->clear();
//----------------------------------------------somme libre-----------------------------------------------------------
    m_entreelibre                            = lineEdit->text();
    m_sommeentree                            = freeReceipSumEdit->text().toDouble();
    m_sommeentree                            = m_sommeentree*m_facteurpourcent/100;
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__)<<" m_sommeentree = "+ QString::number(m_sommeentree);
//-----quel type d'acte -------------------------------------------------------------------

    
          /* else if(m_acteautre == "IK"){
               int id = m_mapTypeIK.key(rulesKmComboBox->currentText());
               if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__)
                                    << QString::number(m_mapValueIK.value(id).toDouble())
                                    << QString::number(spinBox->value())
                                    << QString::number(m_mapAbattementIK.value(id).toDouble());
               acteEnString = QString::number((m_mapValueIK.value(id).toDouble() * spinBox->value())
                          - ((m_mapValueIK.value(id).toDouble() * m_mapAbattementIK.value(id).toDouble()) * 2)) ;
               if(acteEnString.toDouble() < 0.00){
                   acteEnString = 0.00 ;
               }
           }*/

  //-------------------------------------------------------------------------

    QString actelibre = "libre";

    QDate daterecup = dateEdit->date();
	m_date = daterecup.toString("yyyy-MM-dd");
    QSqlQuery query7(m_accountancyDb);
    
	query7.exec("SELECT nom_usr,id_usr FROM utilisateurs WHERE login = '"+userchoisi+"'");
	QStringList listprat;
	while (query7.next()) {
	    m_praticien = query7.value(0).toString();
	    m_idusr = query7.value(1).toInt();
	    listprat << m_praticien;
		}
         if(listprat.size() < 1){
            m_praticien = "Docteur";
            m_idusr = 1;
        }
//---------------------pas de programme appelant-----------------------------------
  if((m_nomRecupere == "")== true){
    QModelIndex index = tableView->currentIndex();
    if(index.isValid() == false){
       QString nameSurname = lineEdit_4->text();
       if(nameSurname == ""){
           QMessageBox::warning(0,trUtf8("Attention"),trUtf8("Il est nécessaire de rentrer un nom et un prénom,"
                                                             "\nséparés d'un espace."),QMessageBox::Ok);
           return true;
       }
       if(nameSurname.contains(" ")){
           int espaceCount = nameSurname.count(" ");
           if(espaceCount > 1){
               QMessageBox::warning(0,trUtf8("Attention"),trUtf8("Mettez un ; entre le nom et le prénom svp,\n "
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
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " m_prenomRecupere = " << m_prenomRecupere;
//---------------si récupération du nom du patient ----------------------------------
    if((m_nomRecupere == "")==false){
	    m_guid = m_guidEntre;
        QString nom    = m_nomRecupere;
	    QString prenom = m_prenomRecupere;
	    m_nomprenom    = nom+","+prenom;
        }

    	if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) <<  " apres replace : "+m_nomprenom;
//--------------------------------entree libre et somme libre ---------------------------------------------------------

    if((m_entreelibre == "") == false ){

	   if((m_sommeentree == 0) == false)
			{
		m_nomprenom = m_entreelibre;
		m_acteautre = actelibre;
		acte        = m_sommeentree;
		message_libre();
			}
	   else	{
	        m_nomprenom = m_entreelibre;
	        m_acteautre = actelibre;
	        message_libre();
		}
		lineEdit->clear();
		}
    if((m_sommeentree == 0) == false ){
               m_acteautre = actelibre;
               acte        = m_sommeentree;
               if (WarnDebugMessage) qDebug() << "dans somme entree : "+m_nomprenom;
               freeReceipSumEdit->clear();
    }
    //-------------------------particularites semantiques --------------------------------------------
    if(m_nomprenom.contains("'")){
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
    m_actesSeance += " "+m_acteautre+" ";
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
     QMessageBox::information(0,tr("erreur"),trUtf8("vous avez oublié de choisir le type de paiement"),QMessageBox::Ok);
     return -1.00;
     }
  if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " "+QString::number(acte);
  if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " m_nomprenom = " << m_nomprenom;
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
	               "'"+m_acteautre+"',"
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

  if (WarnDebugMessage) qDebug() << enregistrement(query6);

  //--------------insertion dans table rapprochement_bancaire du solde actuel--------------------------
  /*QString nouveauSolde = "";
  QString reqSolde = "SELECT solde FROM rapprochement_bancaire";
  int idCompte = m_hashCompte.key(comboBox_6->currentText()).toInt();
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
       if (WarnDebugMessage) qDebug() << "Erreur ="+queryInsertBank.lastError().text()+"\n"+requeteInsertSolde;
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

  else{
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
  if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) <<" result = "+result;
  result.replace(".0","");
  result.replace("-0","-");
  result.replace("-","");
  m_executedquery = result;

  //result.replace("0","");

  return m_executedquery;
}

void recettedialog::fermer()
{
  QFile f(g_pathabsolu+"/errorLog.txt");
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
    QString user = comboBox_2->currentText();
    QString site = comboBox_5->currentText();
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
  QMessageBox::information(0,tr("info"),trUtf8("l'intitulé libre prime sur le nom et le prénom"),QMessageBox::Ok);
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
  	QFile fichier(g_pathabsolu+"/fichierpourcentage.ini");
	if(!fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
	    QMessageBox::warning(0,tr("Attention"),trUtf8("fichierpourcentage.ini ne peut pas être ouvert."),
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
 	 	label_12		->setText(text0);
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
 	 	label_12		->setText(text);
 	 	if(m_iplusmoins<liste.size()-1){
 	 	m_iplusmoins++;
 	 		}
 	 	else{
 	 	plusdouble 		= 100.00;
 	 	label_12		->setText("100%");
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
  QFile fichier(g_pathabsolu+"/fichierpourcentage.ini");
	if(!fichier.open(QIODevice::ReadWrite | QIODevice::Text)){
	    QMessageBox::warning(0,tr("Attention"),trUtf8("fichierpourcentage.ini ne peut pas être ouvert."),
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
 	label_12			->setText(textcent);

 	m_iplusmoins--;
 		}
	else if(m_iplusmoins == 0){
	moinsdouble 			= 0.00;
	label_12			->setText("0%");
	 m_facteurpourcent		= moinsdouble;
		}
	else{
	QString choisicent 		= liste[m_iplusmoins-1];
 	QStringList listechoisie 	= choisicent.split("=");
 	moinsdouble 			= listechoisie[0].toDouble();
 	QString textchoisi 		= listechoisie[1];
 	label_12			->setText(textchoisi);
 	m_facteurpourcent		= moinsdouble;
 		if(m_iplusmoins>0){
 		m_iplusmoins--;
 		}

 		}
 	int moinsencore 		= static_cast<int>(moinsdouble);
 	factorProgressBar				->setValue(moinsencore);
}
//---------------------fonction pour recuperer ccamview-------------------------------------------------------------
void recettedialog::ccam_process(){
    FindReceiptsValues * f = new FindReceiptsValues(this);
    if (f->exec()==QDialog::Accepted)
    {
         m_ccamAndValuesChosen = f->getchosenValues();
        }
    double totalSum = 0.00;
    for (int i = 0; i < m_ccamAndValuesChosen.size(); ++i)
    {
          QString value = m_ccamAndValuesChosen.values()[i];
          totalSum += value.toDouble();
        }
    QString ccamChosenValuesSum = QString::number(totalSum);
    CcamLineEdit->setText(ccamChosenValuesSum);
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
              if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
              QString dateOrType = query.value(1).toString();
              if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " dateOrType =" << dateOrType ;
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
                     if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " value next =" << value ;
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
  if (WarnDebugMessage) qDebug() << texte <<  __FILE__ << QString::number(__LINE__);
  QString texteCombo          = texte;
  QString adresse             = "pas d'adresse connue.";
  for(int i = 0 ; i < m_listAdress.size() ; i++){
      if (WarnDebugMessage) qDebug() << m_listAdress[i];
      QStringList listeCombo  = m_listAdress[i].split("=");
      if(listeCombo[0] == texteCombo){
          adresse             = listeCombo[1];
      }

  }
  comboBox_3                -> setToolTip(adresse);
}

void recettedialog::rafraichirComboBox_3(){
    QStringList listepayeurs;
    comboBox_3               ->setEditable(true);
    comboBox_3               ->setInsertPolicy(QComboBox::NoInsert);
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
    comboBox_3             -> addItems(listepayeurs);
}

void recettedialog::rafraichirComboBox_5(){
    comboBox_5                    ->setEditable(true);
    comboBox_5                    ->setInsertPolicy(QComboBox::NoInsert);
    QStringList listesites;
    QSqlQuery querysite(m_accountancyDb);
              querysite.exec("SELECT site FROM sites ");
              while( querysite.next()){
	         QString strsite= querysite.value(0).toString();
	         listesites    << strsite ;
	      }
    if(listesites.size() < 1){
       /* QStringList listeAdresseDefaut;
        QSqlQuery queryAdresseDefaut(m_db1);
        QString   requeteAdresseDefaut     = "SELECT Adresse, CodePostal , Ville FROM Personnes";
                  queryAdresseDefaut       . exec(requeteAdresseDefaut);
        while(queryAdresseDefaut.next()){
            QString lineAdrDef             = queryAdresseDefaut.value(0).toString();
            QString CP                     = queryAdresseDefaut.value(1).toString();
            QString Ville                  = queryAdresseDefaut.value(2).toString();
                    listeAdresseDefaut   << lineAdrDef + "=" + CP + "=" + Ville;
        }
        for(int i = 0 ; i < listeAdresseDefaut.size() ; i++){
            QString Adr                    = listeAdresseDefaut[i];
            if (WarnDebugMessage) qDebug() << "Adr ="+Adr;
            QStringList AdrList            = Adr.split("=");
                        listesites        << trUtf8("Cabinet Médical")+" "+AdrList[0]+" "+AdrList[1]+" "+AdrList[2] ;

        }*/
        listesites        << trUtf8("Cabinet Médical rue de Nulle Part 98000 Partout");
    }
    comboBox_5->addItems(listesites);
}

void recettedialog::setWhatsThisMode(){
  QWhatsThis::enterWhatsThisMode();
}



void recettedialog::mousePressEvent(QMouseEvent * event){
  if(event->button() == Qt::RightButton){
    if (WarnDebugMessage) qDebug() << "dans clic droit" << __FILE__ << __LINE__;
    m_menu         = new QMenu;
    m_menu        -> addAction(m_remontrerActeCourant);
    m_menu        -> exec(QCursor::pos());
  }

}

void recettedialog::BoutonActeCourant(){
    recordButton                       ->setEnabled(false);
    recordButton                       ->hide();
    pushButton                         ->show();
    pushButton                         ->setEnabled(true);

}

void recettedialog::redHelp(){
  pushButton_16      -> setStyleSheet("* {background : red }");
  pushButton_16      -> setText(trUtf8("Aide"));
}

void recettedialog::enregistreSeance(){
  QString idPayeur;
  if(comboBox_3->currentText() == "patient"){
      if(!m_guidEntre.isEmpty()){
         idPayeur = m_guidEntre;
         }
      else{
         idPayeur = "1";
      }
  }
  else{
     QSqlQuery queryPayeur(m_accountancyDb);
               queryPayeur.exec("SELECT id_payeurs FROM payeurs WHERE nom_payeur = '"+comboBox_3->currentText()+"';");
     while(queryPayeur.next()){
       idPayeur =  queryPayeur.value(0).toString();
     }
  }
  QString idUsr;
  QSqlQuery queryUsr(m_accountancyDb);
            queryUsr.exec("SELECT id_usr FROM utilisateurs WHERE login = '"+comboBox_2->currentText()+"';");
     while(queryUsr.next()){
         idUsr = queryUsr.value(0).toString();
     }
  QString idSite;
  QSqlQuery querySite(m_accountancyDb);
            querySite.exec("SELECT id_site FROM sites WHERE site = '"+comboBox_5->currentText()+"';");
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
          if (WarnDebugMessage) qDebug() << "Erreur ="+queryPaiement.lastError().text()+"\n"+requetePaiement;
      }
  if(m_ouinon == "non"){
      if(checkBox_2->isChecked() == false){
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
    if(pushButton->isEnabled()){
        textOfActs = pushButton -> text();

        }
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
        QMessageBox::information(0,trUtf8("Information"),item+trUtf8(" a été enregistré dans le thésaurus."),
                                 QMessageBox::Ok);
        comboBox->addItem(item);
        }
}

void recettedialog::chooseIK(int i){
  if(i != 0){
      QString IK = "+IK";
      newTextOnButtons(IK);
  }
}

void recettedialog::addIK(int km)
{
    int id = m_mapTypeIK.key(rulesKmComboBox->currentText());
                   if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__)
                            << QString::number(m_mapValueIK.value(id).toDouble())
                            << QString::number(spinBox->value())
                            << QString::number(m_mapAbattementIK.value(id).toDouble());
    QString valueStr = QString::number((m_mapValueIK.value(id).toDouble() * km)
                          - ((m_mapValueIK.value(id).toDouble() * m_mapAbattementIK.value(id).toDouble()) * 2)) ;
    if(valueStr.toDouble() < 0.00)
    {
        valueStr = 0.00 ;
        }
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " valueStr =" << valueStr ;
    m_ccamAndValuesChosen.insert(trUtf8("kilomètres"),valueStr);
}


void recettedialog::chooseCCAM(const QString & ccamResult){
    double val = ccamResult.toDouble();    
    QStringList lccam = m_ccamAndValuesChosen.keys();
    m_actesToolTip = lccam.join(" ");
    QString ccam = "+"+lccam.join("+");
    newTextOnButtons(ccam);
}

void recettedialog::newTextOnButtons(QString & newEntry){
  if(pushButton->isEnabled()){
      QString preferedAct = settings()->value(Constants::S_ACTE_COURANT).toString();
      QString newTextPB = correctNameOfAct(preferedAct)+newEntry;
      if(newTextPB.contains("+")){
          QStringList newTextListPB = newTextPB.split("+");
                      newTextListPB.removeDuplicates();
          newTextPB    = newTextListPB.join("+");
          if (WarnDebugMessage) qDebug()  << __FILE__ << QString::number(__LINE__) << "newTextPB = "+newTextPB;
      }
      pushButton->setText(newTextPB+"\n"+QString::number(variousSums(newTextPB)[0]));

  }
  else if (recordButton->isEnabled()){
      QString textPushButton_2 ;
      textPushButton_2 = recordButton->text();
      QStringList list = textPushButton_2.split("\n");
      QString newText;
      if (list.size()>1)
      {
      	  newText = list[1]+newEntry;
          }
      if(newText.contains("ccam+")){
              if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << newText;
              newText.remove("ccam+");
              }
      if(newText.contains("+")){

          QStringList newTextList = newText.split("+");
                      newTextList.removeDuplicates();
          if(newTextList[0].isEmpty()){
              newTextList.removeFirst();
              }
          newText    = newTextList.join("+");
          if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << "newText = "+newText;
      }
      recordButton->setText("Enregistrer\n"+newText+"\n"+QString::number(variousSums(newText)[0])+" "
                             +settings()->value(Constants::S_TYPE_OF_MONEY).toString());
  }
  else{
      qWarning() << "pushButton and recordButton are not enabled.";
  }
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
           if (WarnDebugMessage) qDebug() << "str ="+str;
           QString valueStr;
           if(str == "IK"){
               int id = m_mapTypeIK.key(rulesKmComboBox->currentText());
               if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__)
                                    << QString::number(m_mapValueIK.value(id).toDouble())
                                    << QString::number(spinBox->value())
                                    << QString::number(m_mapAbattementIK.value(id).toDouble());
               valueStr = QString::number((m_mapValueIK.value(id).toDouble() * spinBox->value())
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
        if (WarnDebugMessage) qDebug() << "valueStr = "+valueStr;
        totalSum += valueStr.toDouble();

       }

   if(resultOfValues.size() != 0){
       resultOfValues.prepend(totalSum);
   }
   return resultOfValues;//totalSum << eachSum;
}

void recettedialog::controlOfQueries(QString & executedQueryResult){
      QString labeltexte = tr("la somme : ")+executedQueryResult+trUtf8(" a été enregistrée.");
      resultValueLabel           -> setStyleSheet("*{ color:red }");
      if(executedQueryResult.toDouble() > 0.00){
          resultValueLabel  -> setText(labeltexte);
          }
      //remise à zero des checkBox
      //checkBox_2->setCheckState(Qt::Unchecked);
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
          QMessageBox::warning(0,tr("erreur"),trUtf8("aucune case n'est cochée..."),QMessageBox::Ok);
       }
       int progressBarTwoValue = factorProgressBar->value();
       if(progressBarTwoValue == 100){
           CcamLineEdit        -> clear();
           QTimer::singleShot(200,this,SLOT(clearAll()));
       }

}

void recettedialog::combo_5_toolTip(const QString& textCombo_5){
  QString text = textCombo_5;
  comboBox_5->setToolTip(text);
}

void recettedialog::showErrors(){
    QString errorAllString;
    QFile f(settings()->applicationBundlePath()+"/errorLog.txt");
    if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " " << settings()->applicationBundlePath()+"/errorLog";
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
                if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " strAct correct =" << strAct ;
                break;
              }
          }
    return strAct;
}



QString recettedialog::findIdPayeur(){
    QString comboText = comboBox_3->currentText();
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
    QString comboText = comboBox_5->currentText();
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
       comboBox_2->hide();
       comboBox_5->hide();
       comboBox_6->hide();
       label_11->hide();
       label_17->hide();
       label_3->hide();
       checkBox_3->setText(trUtf8("Augmenter"));
       }
   else{
       comboBox_2->show();
       comboBox_5->show();
       comboBox_6->show();
       label_11->show();
       label_17->show();
       label_3->show();
       checkBox_3->setText(trUtf8("Diminuer"));
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
           QMessageBox::warning(0,trUtf8("Attention"),trUtf8("Il est nécessaire de rentrer un nom et un prénom,"
                                                             "\nséparés d'un espace."),QMessageBox::Ok);
           return returnList;
       }
       if(nameSurname.contains(" ")){
           int espaceCount = nameSurname.count(" ");
           if(espaceCount > 1){
               QMessageBox::warning(0,trUtf8("Attention"),trUtf8("Mettez un ; entre le nom et le prénom svp,\n "
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
                   mess.setText(trUtf8("Voulez vous enregistrer ce nom et ce prénom ?"));
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
                             ret = QMessageBox::warning(0,trUtf8("warning"),trUtf8("Le  nom et le prénom "
                             "existent déjà dans la base nameindex.\nVoulez vous quand même le réenregistrer ?"),
                                 QMessageBox::Yes|QMessageBox::No);
                             if (WarnDebugMessage) qDebug() << "ret ="+QString::number(ret);
                             if(ret == QMessageBox::Yes){
                                 queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                                  "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')");
                                 QMessageBox::information(0,trUtf8("Information"),trUtf8("Le  nom et le prénom "
                                                     "ont été enregistrés de nouveau dans la base nameindex."),
                                                     QMessageBox::Ok);
                             }
                             else{
                                 if (WarnDebugMessage) qDebug() << "in else " << __FILE__ << QString::number(__LINE__);
                                 break;
                             }
                         }
                         else{
                             if(!queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                              "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')")){
                                 QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Le  nom et le prénom "
                                                     "n'ont pas été enregistrés dans la base nameindex.\n")
                                                     +queryMessOk.lastError().text(),QMessageBox::Ok);
                             }
                             else{
                                 QMessageBox::information(0,trUtf8("Information"),trUtf8("Le  nom et le prénom "
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
   if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__)
            << " nom = "+nom
            << " prenom = "+prenom
            << " m_guid = "+m_guid;
   returnList << nom << prenom << m_guid;
   return returnList;
}

void recettedialog::modifyListOfValues()
{
    QString datas;
    if(recordButton->isEnabled()){
        datas      = recordButton->text();
        if (WarnDebugMessage) qDebug() << QString::number(__LINE__) <<"datas button_2 = "+datas;
        
    PondereActes p(this,datas);
    if (p.exec()==QDialog::Accepted)
    {
    	  datas = p.getModifiedValues();
        }
    recordButton->setText(datas);
    }
    if(pushButton->isEnabled()){
        datas      = pushButton->text();
        if (WarnDebugMessage) qDebug() << QString::number(__LINE__) <<"datas button = "+datas;
        
    PondereActes p(this,datas);
    if (p.exec()==QDialog::Accepted)
    {
    	  datas = p.getModifiedValues();
        }
    pushButton->setText(datas);
    }
    
}

int recettedialog::messageThesaurus()
{
    QCheckBox *checkBox = new QCheckBox(trUtf8("Ne plus présenter ce message."));
    QMessageBox mess;
    mess.setWindowTitle("Thesaurus");
    mess.setText(trUtf8("Voulez vous enregistrer ce shéma dans le thésaurus personnel ?"));
    mess.setInformativeText(trUtf8("Vous pouvez aussi le faire par le bouton plus thesaurus avant un enregistrement."));
    mess.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    mess.addButton(checkBox,QMessageBox::ApplyRole);
    int ret = mess.exec();
    if (checkBox->isChecked())
    {
            	 if (WarnDebugMessage) qDebug() << __FILE__ << QString::number(__LINE__) << " in qmessagebox apply " ;
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
                   mess.setText(trUtf8("Voulez vous enregistrer ce nom et ce prénom ?"));
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
                             ret = QMessageBox::warning(0,trUtf8("warning"),trUtf8("Le  nom et le prénom "
                             "existent déjà dans la base nameindex.\nVoulez vous quand même le réenregistrer ?"),
                                 QMessageBox::Yes|QMessageBox::No);
                             if(ret == QMessageBox::Yes){
                                 queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                                  "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')");
                                 QMessageBox::information(0,trUtf8("Information"),trUtf8("Le  nom et le prénom "
                                                     "ont été enregistrés de nouveau dans la base nameindex."),
                                                     QMessageBox::Ok);
                                 break;
                                 }
                             else{
                                 if (WarnDebugMessage) qDebug() << "in else " << __FILE__ << __LINE__;
                                 break;
                                 }
                             }
                         else{
                             if(!queryMessOk.exec("INSERT INTO nameindex (id_index,name,surname,guid) "
                                              "VALUES(NULL,'"+nom+"','"+prenom+"','"+guid+"')")){
                                 QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Le  nom et le prénom "
                                                     "n'ont pas été enregistrés dans la base nameindex.\n")
                                                     +queryMessOk.lastError().text(),QMessageBox::Ok);
                                 break;
                                 }
                             else{
                                 QMessageBox::information(0,trUtf8("Information"),trUtf8("Le  nom et le prénom "
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



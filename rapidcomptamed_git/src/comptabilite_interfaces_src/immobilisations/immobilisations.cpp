#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "immobilisations.h"
#include "NouvelleImmobilisation.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <common/position.h>
#include <common/configure/util.h>

using namespace Common;
using namespace Constants;
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}
immobilisations::immobilisations(){
  setupUi(this);
  resize(500,300);
  position()->centralPosition(this,width(),height());
  m_db                          = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  QPalette p                    = palette();
           p                    . setColor(QPalette::Active, QPalette::Base, QColor ("#DDDDDD"));
  textEdit                     -> setPalette(p);
  pushButton_4                 -> setToolTip  (trUtf8("Pour l'aide cliquez sur ce bouton,\ndéplacez la souris sur la zone voulue,\ncliquez "
                                                      "pour afficher l'aide."));
  pushButton_2                 -> setWhatsThis(trUtf8("Ce bouton lance la fenêtre de création d'immobilisation."));
  tableView                    -> setWhatsThis(trUtf8("Les immobilisations enregistrées dans la base\nsont notées ici."));
  pushButton_3                 -> setWhatsThis(trUtf8("Ce bouton permet d'effacer la ligne sélectionnée."));
  pushButton                   -> setWhatsThis(trUtf8("Ce bouton quitte sans enregistrer."));
  textEdit                     -> setWhatsThis(trUtf8("La valeur annuelle à déclarer aux impôts en déduction est calculée ici."));
  
  QString loginPref = settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
  QString idPref = settings()->value(Constants::S_ID_UTILISATEUR_CHOISI).toString();
  labelLogins->setText(trUtf8("Utilisateurs"));
  labelLogins->setWordWrap(true);
  
  if (loginPref.isEmpty())
  {
  	  loginPref = "admin";
      }
  QStringList listOfLogins;
  listOfLogins << loginPref;
  QSqlQuery qLogins(m_db);
  QString reqLogins = QString("SELECT %1 FROM %2").arg("login","utilisateurs");
  if (!qLogins.exec(reqLogins))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to get logins :"
  	  << qLogins.lastError().text();
  	  
      }
  while (qLogins.next())
  {
  	listOfLogins << qLogins.value(0).toString();
      }
  listOfLogins.removeDuplicates();
  comboBoxLogins->addItems(listOfLogins);
  
  //----------------------------tableView-------------------------------------------------------------------------
  m_model                       = new QSqlTableModel(this,m_db);
  m_modele                      = new QSqlTableModel(this,m_db);
  m_model                      -> setTable("immobilisations");
  QString loginUser = listOfLogins[0];
  if (!security()->isUserLocked(loginUser))
  {
  	  m_model -> select();
  	  tableView -> setModel(m_model);
      }
  tableView                    -> setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView                    -> setSelectionMode(QAbstractItemView::SingleSelection);
  tableView                    -> setEditTriggers(QAbstractItemView::SelectedClicked);
  tableView                    -> setSortingEnabled(true);
  tableView                    -> setColumnHidden(0,true);
  tableView                    -> setColumnHidden(1,true);
  tableView                    -> setColumnHidden(8,true);
  tableView                    -> setColumnHidden(10,true);
  tableView                    -> setColumnHidden(11,true);
  tableView                    -> setColumnHidden(13,true);
  tableView                    -> horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableView                    -> horizontalHeader()->setStretchLastSection ( true );
  tableView                    -> verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
  tableView                    -> show();
  //---------------------------textEdit--------------------------------------------------------------------
  //QString espace(QChar(QChar::Nbsp));
  QString valeurCalcul         = "";
          valeurCalcul         = calcul();
  QString valAnnTexte          = trUtf8("La valeur annuelle à déclarer est de = ");
  QString Euros                = trUtf8(" Euros.");
  /*QString valeurResid          = trUtf8("La valeur résiduelle")+espace+espace+espace+espace+espace+espace+espace+espace+
                                 espace+espace+espace+espace+espace+" = "+valeursCalcul[1];*/
  
  QString resultatTexte        = "<html><body><font face = Comics sans MS size = 4 color = ""blue"">"
                                 ""+valAnnTexte+"</font><font face = Comics sans MS size = 5 color = ""red"">"
                                 "<bold>"+valeurCalcul+"</bold></font>"
                                 /*""+valeurResid+"<br/>"*/
                                 "<font face = Comics sans MS size = 4 color = ""blue"">"+Euros+"</font>"
                                 "</body></html>";
                                 

  textEdit                    -> setHtml(resultatTexte);
  
  
  connect(pushButton,   SIGNAL(pressed()),this,SLOT(close()));
  connect(pushButton_2, SIGNAL(pressed()),this,SLOT(nouveau()));
  connect(pushButton_3, SIGNAL(pressed()),this,SLOT(effacer()));
  connect(pushButton_4, SIGNAL(pressed()),this,SLOT(setWhatsThis()));

}

immobilisations::~immobilisations(){}


void immobilisations::nouveau(){
  nouvelleImmobilisation *immobilisation = new nouvelleImmobilisation;
  QString login = comboBoxLogins->currentText();
  if (!security()->isUserLocked(login))
  {
  	  immobilisation -> show();
      }
  else
  {
  	QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Cet utilisateur est verrouillé !"),QMessageBox::Ok);
  	return;
      }
  
  connect(immobilisation,SIGNAL(destroyed()),this,SLOT(rafraichir()));
}

void immobilisations::effacer(){
  QModelIndex index                   = tableView->QAbstractItemView::currentIndex();
  if(!index.isValid()){
      QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("Vous avez oublié de sélectionner une ligne à effacer"),QMessageBox::Ok);
      return;
  }
  int i                               = index.row();
  if(tableView->model()==m_model){
      m_model                          -> removeRows(i,1,QModelIndex());
      m_model                          -> submitAll();
      rafraichir();
      QMessageBox::information(0,trUtf8("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
  }
  else if(tableView->model()==m_modele){
      m_modele                       -> removeRows(i,1,QModelIndex());
      m_modele                       -> submitAll();
      rafraichir();
      QMessageBox::information(0,trUtf8("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
  }

  else{
      QMessageBox::critical(0,trUtf8("Erreur"),trUtf8("Impossible d'effacer la ligne!"),QMessageBox::Ok);
  }
}


void immobilisations::rafraichir()
{
  m_model  -> clear();
  
  m_modele  ->setTable("immobilisations");
  QString login = comboBoxLogins->currentText();
  if (!security()->isUserLocked(login))
  {
  	  m_modele    ->select();
  	  tableView->setModel(m_modele);
      }
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
  tableView->setSortingEnabled(true);
  tableView->setColumnHidden(0,true);
  tableView->setColumnHidden(1,true);
  tableView->setColumnHidden(8,true);
  tableView->setColumnHidden(10,true);
  tableView->setColumnHidden(11,true);
  tableView->setColumnHidden(13,true);
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableView->horizontalHeader()->setStretchLastSection ( true );
  tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
  tableView->show();
  
    QString valeurCalcul         = "";
          valeurCalcul         = calcul();
  QString valAnnTexte          = trUtf8("La valeur annuelle à déclarer est de = ");
  QString Euros                = trUtf8(" Euros.");
  /*QString valeurResid          = trUtf8("La valeur résiduelle")+espace+espace+espace+espace+espace+espace+espace+espace+
                                 espace+espace+espace+espace+espace+" = "+valeursCalcul[1];*/
  
  QString resultatTexte        = "<html><body><font face = Comics sans MS size = 4 color = ""blue"">"
                                 ""+valAnnTexte+"</font><font face = Comics sans MS size = 5 color = ""red"">"
                                 "<bold>"+valeurCalcul+"</bold></font>"
                                 /*""+valeurResid+"<br/>"*/
                                 "<font face = Comics sans MS size = 4 color = ""blue"">"+Euros+"</font>"
                                 "</body></html>";
                                 

  textEdit                    -> setHtml(resultatTexte);
}

void immobilisations::setWhatsThis(){
  QWhatsThis::enterWhatsThisMode();
}

QString immobilisations::calcul(){
  QString       calcul                     = "";
  double        resultats                  = 0;
  QStringList   dateList;
  QList<int>    dureeList;
  QList<int>    modeList;
  QList<double> valeurList;
  QSqlQuery   query(m_db);
  QString     requete                      = "SELECT date_service ,duree ,mode ,valeur FROM immobilisations";
              query                        . exec( requete);
              while(query.next()){
                  QString date             = query.value(0).toString();
                  int     duree            = query.value(1).toInt();
                  int     mode             = query.value(2).toInt();
                  double  valeur           = query.value(3).toDouble();
                          dateList        << date  ;
                          dureeList       << duree ;
                          modeList        << mode  ;
                          valeurList      << valeur;
              }
  for(int i = 0 ; i < valeurList.size() ; i ++ ){
      double           result             = 0;
      if   (modeList[i] == 2){//mode 2 degressif
                           result         = calculDegressif(valeurList[i] , dureeList[i] , dateList[i]);
                           qDebug() << "in degressif result = "+QString::number(result);
      }
      else {//mode 1 lineaire
                           result         = calculLineaire(valeurList[i] , dureeList[i] , dateList[i]);
      }
      resultats                          += result;
  }
  calcul                                  = QString::number(resultats);
  return calcul; // calculList[0] = valeur annuelle a declarer
}

double immobilisations::calculLineaire(double & valeur , int & duree , QString & date){
  double  valeurInvestie        = valeur;
  qDebug() << "valeurInvestie ="+QString::number(valeurInvestie);
  int     dureeAmortissementInt = duree;
  qDebug() << "dureeAmortissementInt ="+QString::number(dureeAmortissementInt);
  QString dateDureeStr          = date;
  qDebug() << "dateDureeStr ="+dateDureeStr;
  QDate   currentDate           = QDate::currentDate();
  int     currDateYear          = currentDate.year();
  qDebug() << "currDateYear ="+QString::number(currDateYear);
  QDate   dateDuree             = QDate::fromString( dateDureeStr,"yyyy-MM-dd");
  int     dureeInt              = dateDuree.year();
  qDebug() << " dureeInt ="+QString::number( dureeInt);
  
  double  dureeAmortissement    = double(dureeAmortissementInt);
  double  valeurAnnuelle        = valeurInvestie/dureeAmortissement;
  //QString valeurAnnuelleStr     = QString::number(valeurAnnuelle);
  int     differenceDates       = currDateYear - dureeInt ;
  qDebug() << "differenceDates ="+QString::number(differenceDates);
  if(differenceDates < 0 || differenceDates > dureeAmortissementInt){
      valeurAnnuelle            = 0 ;
  }
  return  valeurAnnuelle;
}

double immobilisations::calculDegressif(double & val , int & duree , QString & date){
    double      valeur          = val;
    double      ans             = double(duree);
    QString     dateDureeStr    = date;
    QDate       dateDebut       = QDate::fromString(dateDureeStr,"yyyy-MM-dd");
    QDate       dateCourante    = QDate::currentDate();
    double      valeurAnn       = 0 ;
    
    //-----------------premiere annee----------------------------------
    double      taux            = 1.25;
    if(ans > 4 && ans < 7){
        taux                    = 1.75;
    }
    if(ans > 6){
        taux                    = 2.25;
    }
    int       mois              = dateDebut.month();
    int       dateDebutAn       = dateDebut.year();
    int       dateCourAn        = dateCourante.year();
    qDebug() << "mois calculDegressif ="+QString::number(mois);
    qDebug() << "dateDebutAn ="+QString::number(dateDebutAn);
    qDebug() << "dateCourAn ="+QString::number(dateCourAn);
    //----------------premiere annee---------------------------------------
    
    double    nbreMois          = double(12 - mois);
    double    tauxDegrCent      = 100/ans * taux;
    qDebug() << "tauxDegrCent ="+QString::number(tauxDegrCent);
    double    tauxDegr          = (tauxDegrCent)/100;
    qDebug() << "tauxDegr ="+QString::number(tauxDegr);
    double   valeurAnn1         = (valeur * tauxDegr) * (nbreMois/12) ;
              qDebug() << "valeurAnn1 ="+QString::number(valeurAnn1);
    double    valResid          = valeur - valeurAnn1;
              
    //------------------autres annees--------------------------------------
    for(int i = 1 ; i < int(ans) ; i++){
        ans --;
        if(tauxDegrCent < 100/ans){
            tauxDegr             = 1/ans;
        }
        double valeurAnnFor      = valResid * tauxDegr;
        qDebug() << "valeurAnn ="+QString::number(valeurAnnFor);
        valResid                -= valeurAnnFor;
        qDebug() << "valeurResid ="+QString::number(valResid);
        if(dateCourAn - dateDebutAn == i){
            valeurAnn            = valeurAnnFor ;
        }
        
    }
    if(dateCourAn - dateDebutAn < 1){
        valeurAnn                = valeurAnn1 ;
    }

    return valeurAnn;
}


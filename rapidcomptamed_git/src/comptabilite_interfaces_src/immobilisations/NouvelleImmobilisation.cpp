#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "NouvelleImmobilisation.h"
#include "../../common/constants.h"
#include "../../common/icore.h"
#include "../../common/settings.h"
#include <common/databasemanager.h>
#include <common/position.h>


using namespace Common;
using namespace Constants;
static inline Common::Position *position() {return Common::ICore::instance()->position();}

nouvelleImmobilisation::nouvelleImmobilisation(){
  setupUi(this);
  setAttribute                 ( Qt::WA_DeleteOnClose);
  position()->centralPosition(this,width(),height());
  m_db                         = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  pushButton                  -> setToolTip  (trUtf8("Pour l'aide cliquez sur ce bouton,\ndéplacez la souris sur la zone voulue,\ncliquez "
                                                      "pour afficher l'aide."));
  comboBox_2                  -> setWhatsThis(trUtf8("Choisir l'utilisateur propriétaire du bien immobilisé."));
  comboBox_3                  -> setWhatsThis(trUtf8("Choisir le compte courant de l'utilisateur."));
  lineEdit_3                  -> setWhatsThis(trUtf8("Description brève du bien immobilisé. Par exemple électrocardiographe."));
  lineEdit_4                  -> setWhatsThis(trUtf8("Valeur du bien professionnel immobilisé."));
  dateEdit                    -> setWhatsThis(trUtf8("Date de départ de l'immobilisation."));
  lineEdit_5                  -> setWhatsThis(trUtf8("Durée choisie de l'immobilisation, en années."));
  comboBox                    -> setWhatsThis(trUtf8("Choisir le mode de calcul de l'immobilisation :\n- linéaire : "
                                                     "la valeur déductible à déclarer est égale à la valeur immobilisée "
                                                     "divisée par le nombre d'années d'immobilisation.\n- dégressive : "
                                                     "la valeur annuelle à déclarer est plus forte les premières années que "
                                                     "les dernières.\nLe calcul est automatique."));
  textEdit                    -> setWhatsThis(trUtf8("Mettre ici le détail de l'immobilisation."));
  pushButton_2                -> setWhatsThis(trUtf8("Ce bouton enregistre, un message s'affiche, et quitte après OK."));
  pushButton_3                -> setWhatsThis(trUtf8("Ce bouton quitte sans enregistrer.")); 
  m_model                      = new QSqlQueryModel;
  m_model                     -> setQuery("SELECT * FROM immobilisations",m_db);
  m_count                      = m_model->rowCount();
  dateEdit                    -> setDisplayFormat("dd-MM-yyyy");
  dateEdit                    -> setDate(QDate::currentDate());
  label                       -> setWhatsThis(trUtf8("Exemple :""salaire de Mme Untel."));
  
  //---------------------------remplir combo utilisateurs----------------------------------------------------
  //------------------------choix user preferentiel -----------------------------------------------------------
    QStringList listelogin;
    QString loginpref = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
    QString idprefere = Common::ICore::instance()->settings()->value(S_ID_UTILISATEUR_CHOISI).toString();
            idprefere  .replace(" ","");
    qDebug() << __FILE__ << QString::number(__LINE__) << "login = " << loginpref << " idprefere = " << idprefere;
        int id        = idprefere.toInt();
        int idUsr     = 0;
        QSqlQuery query3(m_db);
            if(!query3.exec("SELECT id_usr,login , nom_usr FROM utilisateurs WHERE id_usr = "
                            ""+QString::number(id)+"")){
                qWarning() << __FILE__ << QString::number(__LINE__) << " " << query3.lastError().text();               
            }
            if(query3.next()){
                idUsr               = query3.value(0).toInt();
                loginpref           = query3.value(2).toString()+"="+query3.value(1).toString();//"user name = login" sera affiche
                }
        listelogin  << loginpref;
  //------------------------choix user--------------------------------------------------------------------------------------
    comboBox_2                      -> setEditable(1);
    comboBox_2                      -> setInsertPolicy(QComboBox::NoInsert);
    
    QSqlQuery query4(m_db);
              if(!query4.exec("SELECT login , nom_usr FROM utilisateurs")){
                  qWarning() << __FILE__ << QString::number(__LINE__) << " " << query4.lastError().text();
              }
        while(query4.next()){
           QString       login       = query4.value(1).toString()+"="+query4.value(0).toString();
           qDebug() << __FILE__ << "login ="+login;
           listelogin              <<  login;
           }
    listelogin.removeDuplicates();
    comboBox_2                      ->addItems(listelogin);
   //------------------------comptes bancaires------------------------------------------------------
  comboBox_3                      -> setEditable(1);
  comboBox_3                      -> setInsertPolicy(QComboBox::NoInsert);          
  QString requeteComptesBanques    = "SELECT libelle FROM comptes_bancaires WHERE id_usr = '"+QString::number(idUsr)+"'";
  QSqlQuery queryCB(m_db);
            if(!queryCB.exec(requeteComptesBanques)){
                qWarning() << __FILE__ << QString::number(__LINE__) << " " << queryCB.lastError().text();
            }
  QStringList rcbUsrList;
  while(queryCB.next()){
      QString rcb                  = queryCB.value(0).toString();
      qDebug() << __FILE__ << QString::number(__LINE__) << rcb;
              rcbUsrList          << rcb;
  }
  comboBox_3                      -> addItems(rcbUsrList);
  //-------------------combo mode--------------------------------------------------------  
    comboBox                        ->setEditable(1);
    comboBox                        ->setInsertPolicy(QComboBox::NoInsert);
    QStringList listeMode;
                listeMode           << trUtf8("linéaire") << trUtf8("dégressif") ;
    comboBox                        ->addItems(listeMode);
  
  connect(pushButton,   SIGNAL(pressed()),                         this , SLOT(setWhatsThisMode()));
  connect(pushButton_2, SIGNAL(pressed()),                         this , SLOT(enregistrer()));
  connect(pushButton_3, SIGNAL(pressed()),                         this , SLOT(close()));
  connect( comboBox_2 , SIGNAL(editTextChanged( const QString &)), this , SLOT(changeCombo_3(const QString &)));
}

nouvelleImmobilisation::~nouvelleImmobilisation(){
  
}

void nouvelleImmobilisation::enregistrer(){
  QString resultat             = "";//valeur annuelle a declarer
  double  resultatDouble       = 0;
  double  valeurResiduelle     = 0;
  QString utilisateur          = comboBox_2->currentText();
  int     idUsr                = 0;
  QStringList listUtil         = utilisateur.split("=");
  QString login                = listUtil[1];
  qDebug() << "login fonction="+login;
  QString requeteUtilisateur   = "SELECT id_usr FROM utilisateurs WHERE login = '"+login+"'";
  QSqlQuery queryUtil(m_db);
            queryUtil          . exec(requeteUtilisateur);
    while(queryUtil.next()){
      idUsr                    = queryUtil.value(0).toInt();
      qDebug() << "idUsr ="+QString::number(idUsr);
    }
  QString compteStr            = comboBox_3->currentText();
  int     compte               = compteStr.toInt();
  QString libelle              = lineEdit_3->text();
          libelle              . replace("'","''");
  QString valeurStr            = lineEdit_4->text();
  double  valeur               = valeurStr.toDouble();
  QDate   dateDebut            = dateEdit  ->date();
  QString dateDebutStr         = dateDebut.toString("yyyy-MM-dd");
  QString dureeStr             = lineEdit_5->text();
  int     duree                = dureeStr.toInt();
  QString modeStr              = comboBox  ->currentText();
  int     mode                 = 1;

  double  tempsMois            = 12;
  QString detail               = textEdit  -> toPlainText();
  qDebug() << "detail "+detail;
          detail               . replace("'","''");
  if(modeStr == trUtf8("linéaire")){
        qDebug() << "dans lineaire";
        resultat             = calculLineaire();//valeur annuelle a declarer
        resultatDouble       = resultat.toDouble();
        valeurResiduelle     = valeur - (resultatDouble * tempsMois/12);
  }

  if(modeStr == trUtf8("dégressif")){
      qDebug() << "dans degressif";
      mode                     = 2;
      resultat                 = calculDegressif();
      valeurResiduelle         = m_valResid;
  }
  QString requete              = "INSERT INTO immobilisations "
                                 "(id_usr,"
                                 "id_compte,"
                                 "libelle,"
                                 "date_service,"
                                 "duree,"
                                 "mode,"
                                 "valeur,"
                                 "valeur_residuelle,"
                                 "resultat,"
                                 "remarque) "
                                 "VALUES ("
                                 "'"+QString::number(idUsr)+"',"
                                 "'"+QString::number(compte)+"',"
                                 "'"+libelle+"',"
                                 "'"+dateDebutStr+"',"
                                 "'"+QString::number(duree)+"',"
                                 "'"+QString::number(mode)+"',"
                                 "'"+QString::number(valeur)+"',"
                                 "'"+QString::number(valeurResiduelle)+"',"
                                 "'"+resultat+"',"
                                 "'"+detail+"')";
                                 
   qDebug() << QString::number(idUsr)+"="+QString::number(compte)+"="+libelle+"="+dateDebutStr+"="+QString::number(duree)+"="+QString::number(mode)+
              "="+QString::number(valeur)+"="+QString::number(valeurResiduelle)+"="+resultat+"="+detail;
                                 
  QSqlQuery query(m_db);
            if(!query.exec(requete)){
                qWarning() << __FILE__ << QString::number(__LINE__) << " " << query.lastError().text();
            }
            
  m_modele                     = new QSqlQueryModel(this);
  m_modele                    -> setQuery("SELECT * FROM immobilisations",m_db);
  m_newcount                   = m_modele->rowCount();
  qDebug() << "m_newcount ="+QString::number(m_newcount)+" m_count ="+QString::number(m_count);
  if(m_newcount == m_count + 1){
      QString titre            = trUtf8("Information");
      QString corps            = trUtf8("Une nouvelle immobilisation a été enregistrée,<br/>la fenêtre va fermer.");
      m_message                = new messageBox(titre,corps);
      m_message               -> show();
      m_count++;
      connect(m_message,SIGNAL(destroyed()),this,SLOT(close()));
     // QTimer::singleShot(2000,this,SLOT(close()));
      
  }
  else{
      QString titre            = trUtf8("Erreur");
      QString corps            = trUtf8("L'immobilisation n'a pas pu être enregistrée");
      m_message                = new messageBox(titre,corps);
      m_message               -> show();
      //emit                     close();
  }
            
  

}

QString nouvelleImmobilisation::calculLineaire(){
  QString valeurInvestieStr     = lineEdit_4->text();
  QString dureeAmortissementStr = lineEdit_5->text();
  double  valeurInvestie        = valeurInvestieStr.toDouble();
  double  dureeAmortissement    = dureeAmortissementStr.toDouble();
  double  valeurAnnuelle        = valeurInvestie/dureeAmortissement;
  QString valeurAnnuelleStr     = QString::number(valeurAnnuelle);
  return  valeurAnnuelleStr;
}

QString nouvelleImmobilisation::calculDegressif(){
    double      valeur     = lineEdit_4->text().toDouble();
    double      ans        = lineEdit_5->text().toDouble();
    QDate       date       = dateEdit->date();
    
    QStringList resultList;
    //-----------------premiere annee----------------------------------
    double      taux       = 1.25;
    if(ans > 4 && ans < 7){
        taux               = 1.75;
    }
    if(ans > 6){
        taux               = 2.25;
    }
    int       mois         = date.month();
    qDebug() << "mois ="+QString::number(mois);
    double    nbreMois     = double(12 - mois);
    double    tauxDegrCent = 100/ans * taux;
    qDebug() << "tauxDegrCent ="+QString::number(tauxDegrCent);
    double    tauxDegr     = (tauxDegrCent)/100;
    qDebug() << "tauxDegr ="+QString::number(tauxDegr);
    double    valeurAn1    = (valeur * tauxDegr) * (nbreMois/12) ;
              resultList <<  QString::number(valeurAn1);
              qDebug() << "valeurAn1 ="+QString::number(valeurAn1);
    double    valResid     = valeur - valeurAn1;
    /*double*/m_valResid   = valResid;
    //------------------autres annees--------------------------------------
    for(int i = 0 ; i < int(ans) ; i++){
        ans --;
        if(tauxDegrCent < 100/ans){
            tauxDegr    = 1/ans;
        }
        double valeurAnn   = valResid * tauxDegr;
        resultList        << QString::number(valeurAnn);
        qDebug() << "valeurAnn ="+QString::number(valeurAnn);
        valResid          -= valeurAnn;
        qDebug() << "valeurResid ="+QString::number(valResid);
        
    }
    
              
              
    QString     result     = resultList.join(",");
    return result;
}

void nouvelleImmobilisation::changeCombo_3(const QString & text){
  QString     userStr           = text.split("=")[0];
  QStringList cbList;
  QSqlQuery query(m_db);
            if(!query.exec("SELECT libelle FROM comptes_bancaires WHERE titulaire = '"+userStr+"'" )){
                qWarning() << __FILE__ << QString::number(__LINE__) << " " << query.lastError().text();
            }
  while(query.next()){
      QString cb               = query.value(0).toString();
              cbList          << cb;
  }
  comboBox_3                  -> addItems(cbList);
  
}

void nouvelleImmobilisation::setWhatsThisMode(){
  QWhatsThis::enterWhatsThisMode();
}

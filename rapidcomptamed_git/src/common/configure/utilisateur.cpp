
#include "utilisateur.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <common/position.h>
#include <common/configure/passdialog.h>
#include <common/configure/util.h>


static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline  Common::Settings *settings() {return Common::ICore::instance()->settings(); }
static inline Common::Security * security() {return Common::ICore::instance()->security();}
using namespace Common;
using namespace Constants;

enum Warn {WarnDebugMessage = true };

utilisateur::utilisateur(QWidget * parent , QString loginUser):QWidget(parent)
{
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " m_loginUser =" << m_loginUser ;
  setupUi(this);
           //   m_db1               = QSqlDatabase::database(Constants::DB_DRTUX);
  m_accountancyDb = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  //----------attributs du widget----------------
  setAutoFillBackground(true);
  setWindowFlags(Qt::Window);
  //setAttribute     (Qt::WA_DeleteOnClose);
  //setWindowFlags   (Qt::WindowStaysOnTopHint);
  //setWindowModality(Qt::ApplicationModal);
  position()->centralPosition(this,width(),height());
  m_modificationUtilisateurs = new modifutil(this);
  m_loginUser = loginUser;
  //settings
  resetWidget();
  
 
  connect(recordButton  ,SIGNAL(pressed())       ,this,SLOT(enregistrer()));
  connect(quitButton,SIGNAL(pressed())       ,this,SLOT(close()));
  connect(modifierButton,SIGNAL(pressed())       ,this,SLOT(modifier()));
  connect(choosePrefButton,SIGNAL(pressed())       ,this,SLOT(choisir()));
  connect(newButton,SIGNAL(pressed()),this,SLOT(prepareNewUser()));
  connect(removeButton,SIGNAL(pressed()),this,SLOT(deleteUser()));  
  connect(comboUserName    ,SIGNAL(highlighted(int)),this,SLOT(remplirLogin(int)));
  connect(comboLibelle,SIGNAL(highlighted(const QString &)),this,SLOT(fillEdits(const QString &)));
  connect(verrouCheckBox,SIGNAL(toggled(bool)),this,SLOT(verrouIsChecked(bool)));
}

utilisateur::~utilisateur(){
  m_hashUtil                     .  clear();
}

void utilisateur::resetWidget()
{
  if (comboUserName->model()->rowCount()>0)
  {
  	comboUserName->clear()  ;
      }
  if (comboLibelle->model()->rowCount()>0)
  {
  	  comboLibelle->clear();
      }
  if (comboBankName->model()->rowCount()>0)
  {
  	  comboBankName->clear();
      }
  m_loginUser = settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " m_loginUser =" << m_loginUser ;
  QString reqBefore = "SELECT id_usr , nom_usr , login ,password FROM utilisateurs WHERE login = '"+m_loginUser+"'";
  QSqlQuery queryFirst(m_accountancyDb);
  if (!queryFirst.exec(reqBefore))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << queryFirst.lastError().text() ;
      }
  QStringList listBefore;
  while(queryFirst.next()){
      QString id = queryFirst.value(0).toString();
      QString name = queryFirst.value(1).toString();
      QString pass = queryFirst.value(3).toString();
      listBefore << id+"="+name+"="+pass;
  }
  if(listBefore.size() > 1){
      QMessageBox::warning(0,tr("Information"),trUtf8("Deux noms ont le même login, erreur à corriger."),QMessageBox::Ok);
       }
       
  if(listBefore.size() == 0){
      listBefore << "1=Docteur=";
              m_hashUtil   .insert(1,"Docteur");
              m_listLogin        .insert("1","admin");
              m_listPassword     .insert("1","");
      }
  QStringList prefList = listBefore[0].split("=");
  QString namePref = prefList[IDUSER]+" "+prefList[NAME];
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " prefList0 =" << prefList[0] ;
  //int idUser = prefList[IDUSER].toInt();
  QString namePrefered =  prefList[NAME]; 
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " idUser =" << prefList[IDUSER] ;
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " namePrefered =" << namePrefered ;
  QString     requete             = "SELECT id_usr , nom_usr , login ,password FROM utilisateurs";
  QStringList listUtilisateurs;
  QSqlQuery query(m_accountancyDb) ;
  if (!query.exec(requete))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
      }

      while(query.next()){
          QString id_usr     = query.value(0).toString();
          QString nom        = query.value(1).toString();         
          QString login      = query.value(2).toString();
          QString password   = query.value(3).toString();
              listUtilisateurs   << nom;
              m_hashUtil    . insert(id_usr.toInt(),nom)  ;
              m_listLogin    .insert(id_usr,login);
              m_listPassword .insert(id_usr,password);
              
              
          }
      
  //-----------------comboUserName nom user--------------------------------------------
  int j=1;
  for (int i = 0; i < m_listLogin.size(); i += 1)
  {
  	if (m_hashUtil.keys()[i]==prefList[IDUSER].toInt())
  	{
  		  m_hashListUsers.insert(0,m_hashUtil.value(m_hashUtil.keys()[i]));
  		  m_hashIdUsers.insert(0,m_hashUtil.keys()[i]);
  		  
  	    }
  	else
  	{
  		m_hashListUsers.insert(j,m_hashUtil.value(m_hashUtil.keys()[i]));
  		m_hashIdUsers.insert(j,m_hashUtil.keys()[i]);
  		j++;
  	    }
      }
  comboUserName                       -> setEditable(true);
  comboUserName                       -> addItems( m_hashListUsers.values());
  loginEdit                     -> setText(m_loginUser);
  //passwdEdit                     -> setText(prefList[PASSWORD]);//password
  passwdEdit -> setText("xxxxxx");
  //--------------banque-----------------------------------
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__)  ;
  if (!fillHashOfBankDatas())
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << trUtf8("Error = fillHashOfBankDatas")
  	                                                        +QString(__FILE__)+QString::number(__LINE__)  ;
  	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Error = fillHashOfBankDatas")
  	                                          +QString(__FILE__)+QString::number(__LINE__),QMessageBox::Ok);
      }
      if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__)  ;
  //const QString utilizateurZero = listUtilisateurs[0];
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__)  ;
  fillEdits(namePrefered);
  comboLibelle     -> setEditable(true);
  comboBankName    -> setEditable(true);
  remplirLogin(0);
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__)   ;

}

void utilisateur::enregistrer(){
  int id_compte = 0;
  QString nomUsr                = comboUserName->currentText();
  if (WarnDebugMessage)
  qDebug() << "nomUsr ="+nomUsr;
  QString login                    = loginEdit->text();
  QString password;
  QString passEdit = passwdEdit->text();
  if (passEdit != "xxxxxx")
  {
  	  password = passEdit;
      }
  else
  {
  	QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Inscrivez votre mot de passe !"),QMessageBox::Ok);
  	return;
      }                
  QString libelle                  = comboLibelle->currentText();
  cleanMysqlDatas(libelle);
  QString nomBanque                = comboBankName->currentText();
  cleanMysqlDatas(nomBanque);
  QString ribCodeBanque            = codeBankEdit->text();
  QString ribCodeGuichet           = guichetEdit->text();
  QString ribNumCompte             = ribCompteEdit->text();
  QString ribCle                   = cleEdit->text();
  double  soldeInitial             = soldeEdit->text().toDouble();
  
  const QString reqExistingLogins = QString("SELECT %1 FROM %2").arg("login","utilisateurs");
  QStringList listOfLogins;
  QSqlQuery qExistingLogins(m_accountancyDb);
  if (!qExistingLogins.exec(reqExistingLogins))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << __FILE__ << QString::number(__LINE__) 
  	  << qExistingLogins.lastError().text();
      }
  while (qExistingLogins.next())
  {
  	QString l = qExistingLogins.value(0).toString();
  	listOfLogins << l;
      }
  QString requeteUtilisateurs;
  QSqlQuery queryUtilisateurs(m_accountancyDb);
  int lastInsertId = 0;
  QMessageBox msgBox;
  msgBox.setText(trUtf8("Voulez vous créer un nouvel utilisateur ? Bouton : Nouvel utilisateur"));
  msgBox.setInformativeText(trUtf8("Ou bien mettre à jour cet utilisateur (comptes) ? Bouton : Mettre à jour"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.button(QMessageBox::Yes)->setText(tr("Nouvel utilisateur"));
  msgBox.button(QMessageBox::No)->setText(trUtf8("Mettre à jour"));
  int ret = msgBox.exec();
  QString requeteComptesBancaires;
  switch(ret){
      case QMessageBox::Yes :
          if (listOfLogins.contains(login))
          {
          	  QMessageBox::warning(0,trUtf8("Warning"),
          	  trUtf8("Vous ne pouvez pas choisir deux fois le même login !"),QMessageBox::Ok);
          	  return;
              }
          else{
          requeteUtilisateurs      = "INSERT INTO utilisateurs (nom_usr, login, password) "
                                     "VALUES ('"+nomUsr+"','"+login+"','"+password+"')";
          queryUtilisateurs      . exec(requeteUtilisateurs);
          lastInsertId           = queryUtilisateurs.lastInsertId().toInt();
          requeteComptesBancaires  = "INSERT INTO comptes_bancaires "
                                     "(id_usr ,libelle ,titulaire ,nom_banque ,rib_code_banque ,"
                                     "rib_code_guichet ,rib_numcompte ,rib_cle ,solde_initial,remarque) "
                                     "VALUES ("+QString::number(lastInsertId)+" ,"
                                              "'"+libelle+       "',"
                                              "'"+nomUsr+        "',"
                                              "'"+nomBanque+     "',"
                                              "'"+ribCodeBanque+ "',"
                                              "'"+ribCodeGuichet+"',"
                                              "'"+ribNumCompte+  "',"
                                              "'"+ribCle+        "',"
                                              ""+QString::number(soldeInitial)+","
                                              " NULL)";
                }
           break;
      case QMessageBox::No :
            libelle                  = comboLibelle->currentText();
            cleanMysqlDatas(libelle);
            nomBanque                = comboBankName->currentText();
            cleanMysqlDatas(nomBanque);
            login                    = loginEdit->text();
            ribCodeBanque            = codeBankEdit->text();
            ribCodeGuichet           = guichetEdit->text();
            ribNumCompte             = ribCompteEdit->text();
            ribCle                   = cleEdit->text();
            soldeInitial             = soldeEdit->text().toDouble();
          requeteComptesBancaires = "UPDATE comptes_bancaires SET  libelle = '"+libelle+"',"
                                                                  "nom_banque = '"+nomBanque+"',"
                                                                  "rib_code_banque = '"+ribCodeBanque+ "',"
                                                                  "rib_code_guichet = '"+ribCodeGuichet+"',"
                                                                  "rib_numcompte = '"+ribNumCompte+"',"
                                                                  "rib_cle = '"+ribCle+"',"
                                                                  "solde_initial = '"+QString::number(soldeInitial)+"'"
                                                                  " WHERE id_usr = '"+getIdUserFromLogin(login)+"'";
           break;
      case QMessageBox::Cancel :
          return;
          break;
      }

  QSqlQuery queryComptesBancaires(m_accountancyDb);
            if(!queryComptesBancaires  . exec(requeteComptesBancaires)){
                qWarning() << "Erreur ="+queryComptesBancaires.lastError().text()+"\n"+requeteComptesBancaires;
                QMessageBox::warning(this,tr("Warning"),trUtf8("Erreur : Insertion dans comptes"
                 "bancaires non faite : \n")
                 +queryComptesBancaires.lastError().text()+"\n"+QString(__FILE__)+QString::number(__LINE__),QMessageBox::Ok);           
                  }
            else
            {
            	QMessageBox::information(0,trUtf8("Information"),trUtf8("Update succeed")+"\n"+queryComptesBancaires.lastQuery(),QMessageBox::Ok);
                }
  if (ret==QMessageBox::Yes)
  {
  	  id_compte = queryComptesBancaires.lastInsertId().toInt();
      }
  if(ret==QMessageBox::No)
  {
  	id_compte = m_hashLibelle.key(libelle).toInt();
      }
  else{
        id_compte = -1;
  }
  QString reqRapprochement_bank = "INSERT INTO rapprochement_bancaire (id_compte,solde) "
                                  "VALUES("+QString::number(id_compte)+","
                                  ""+QString::number(soldeInitial)+");";
  QSqlQuery queryRapBank(m_accountancyDb);
            if(!queryRapBank.exec(reqRapprochement_bank)){
                if (WarnDebugMessage)
  qDebug() << "Erreur ="+queryRapBank.lastError().text()+"\n"+reqRapprochement_bank;
            }
  if((queryUtilisateurs.numRowsAffected() == 1) && 
     (queryComptesBancaires .numRowsAffected() == 1) && 
     (queryRapBank.numRowsAffected() == 1)){
      int ret = QMessageBox::information(0,tr("Information"),trUtf8("L'enregistrement a été effectué."),QMessageBox::Ok);
      if (ret == QMessageBox::Ok)
      {
      	  resetWidget();
          }
  }
}

void utilisateur::remplirLogin(int indexcomboUserName){
  QString     lignecomboUserName    = m_hashListUsers.value(indexcomboUserName);
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " lignecomboUserName =" << lignecomboUserName ;
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " indexcomboUserName =" << QString::number(indexcomboUserName) ;
  QString     idUsr                 = QString::number(m_hashIdUsers.value(indexcomboUserName));
  QString     loginAssocie          = m_listLogin.value(idUsr);
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " idUsr =" << idUsr ;
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " loginAssocie =" << loginAssocie ;
              loginEdit            -> setText(loginAssocie);
  QString     passwordAssocie       = m_listPassword.value(idUsr);
              passwdEdit           -> setText("xxxxxx");
  //test verrou
  if (security()->isUserLocked(loginAssocie))
  {
  	  verrouCheckBox->setChecked(true);
      }
  else
  {
  	verrouCheckBox->setChecked(false);
      }
  QStringList listCompte;
  QStringList listBanque;
  QStringList listOfDatasFromUser;
  listOfDatasFromUser = queryBank(idUsr.toInt());
  for (int i = 0; i < listOfDatasFromUser.size(); i += 1)
  {
  	QStringList list;
  	list = listOfDatasFromUser[i].split("=");
  	if (list.size()> LIBELLE)
  	{
  		  listCompte << list[LIBELLE];
  	    }
  	if (list.size()> NOM_BANQUE )
  	{
  		  listBanque << list[NOM_BANQUE];
  	    }
      }
  comboLibelle ->clear();
  comboBankName->clear();
  comboLibelle      -> addItems(listCompte);
  comboBankName     -> addItems(listBanque);
  fillEdits(listCompte[0]);
 
}

void utilisateur::modifier(){
  
  m_modificationUtilisateurs           -> show();
}

void utilisateur::choisir(){
  QString utilisateurchoisi = loginEdit->text();
  QString req = "SELECT id_usr FROM utilisateurs WHERE login = '"+utilisateurchoisi+"'";
  QSqlQuery query(m_accountancyDb);
  if (! query.exec(req))
  {
  	  QMessageBox::warning(0,trUtf8("Error"),trUtf8("Unable to choose user")+"\n__FILE__"+QString::number(__LINE__),QMessageBox::Ok);
      }
  QStringList list;
  while(query.next()){
      QString str = query.value(0).toString();
      list << str;
      }
  if(list.size()>1){
      QMessageBox::warning(0,tr("Information"),trUtf8("Deux fois le même login, erreur à corriger."),QMessageBox::Ok);
      }
  
  Common::ICore::instance()->settings()->setValue(S_UTILISATEUR_CHOISI,utilisateurchoisi);
  Common::ICore::instance()->settings()->setValue(S_ID_UTILISATEUR_CHOISI,list[0]);
  Common::ICore::instance()->settings()->setValue(S_COMPTE_PREFERENTIEL,comboLibelle->currentText());
  QMessageBox::information(0,trUtf8("Information"),utilisateurchoisi+trUtf8(" a été choisi. Erreur = ")+
  QString::number(Common::ICore::instance()->settings()->status()),QMessageBox::Ok);  
}

QStringList utilisateur::queryBank(int idUsr){
       QStringList listBank;
       QStringList comptes;
       if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " idUsr =" << QString::number(idUsr) ;
       comptes = m_hashId_compte.values(QString::number(idUsr));
       if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " comptes size =" << QString::number(comptes.size()) ;
       for (int i = 0; i < comptes.size(); i += 1)
       {
       	    QString id_compte         = comptes[i];
            QString id_Usr            = QString::number(idUsr);
            QString libelle           = m_hashLibelle.value(id_compte);
            QString bankName          = m_hashBankName.value(id_compte);
            QString rib_code_banque   = m_hashRib_code_banque.value(id_compte);
            QString rib_code_guichet  = m_hashRib_code_guichet.value(id_compte);
            QString rib_numcompte     = m_hashRib_numcompte.value(id_compte);
            QString rib_cle           = m_hashRib_cle.value(id_compte);
            QString solde_initial     = m_hashSolde_initial.value(id_compte);
            listBank << id_compte +"="+ id_Usr +"="+ libelle +"="+bankName +"="+rib_code_banque +"="
                       +rib_code_guichet+"="+rib_numcompte+"="+rib_cle +"="+solde_initial ;
           }
  
    /*if(listBank.size() >1){
        QMessageBox::warning(0,tr("Warning"),
        trUtf8("Deux fois le même id, erreur à corriger.")+QString::number(__LINE__),QMessageBox::Ok);
    }*/
    if(listBank.size() < 1){
        listBank << "1=compte=ma banque=0000=0000=00000000=00=1";
        if (WarnDebugMessage)
  qDebug() << "listBank.size() = 0";
    }
    if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << "list Result : "+listBank[0];
    //QStringList listResult = listBank[0].split("=");
    return listBank;
}

bool utilisateur::fillHashOfBankDatas(){
      bool b = true;
      QString req ="SELECT  id_compte ,"
                           "id_usr ,"
                           "libelle ,"
                           "nom_banque ,"
                           "rib_code_banque,"
                           "rib_code_guichet ,"
                           "rib_numcompte,"
                           "rib_cle ,"
                           "solde_initial "
                           "FROM comptes_bancaires; ";
   QSqlQuery queryBank(m_accountancyDb);
   if (!queryBank.exec(req))
   {
   	  qWarning() << __FILE__ << QString::number(__LINE__) << queryBank.lastError().text() ;
   	  QMessageBox::warning(0,trUtf8("Error"),__FILE__+QString::number(__LINE__)+"\n"+queryBank.lastError().text(),QMessageBox::Ok);
   	  b = false;
       }         
   
   while(queryBank.next()){
       QString id_compte         = queryBank.value(ID_COMPTE).toString();
       QString idUsr             = queryBank.value(ID_USR).toString();
       if (WarnDebugMessage)
       qDebug() << __FILE__ << QString::number(__LINE__) << " id_compte =" << id_compte ;
       if (WarnDebugMessage)
       qDebug() << __FILE__ << QString::number(__LINE__) << " idUsr =" << idUsr ;
       QString libelle           = queryBank.value(LIBELLE).toString();
       QString bankName          = queryBank.value(NOM_BANQUE).toString();
       QString rib_code_banque   = queryBank.value(RIB_CODE_BANQUE).toString();
       QString rib_code_guichet  = queryBank.value(RIB_CODE_GUICHET).toString();
       QString rib_numcompte     = queryBank.value(RIB_NUMCOMPTE).toString();
       QString rib_cle           = queryBank.value(RIB_CLE).toString();
       QString solde_initial     = queryBank.value(SOLDE_INITIAL).toString();
       m_hashId_compte       .insertMulti(idUsr,id_compte);
       m_hashIdUsr           .insertMulti(id_compte,idUsr);
       m_hashLibelle         .insertMulti(id_compte,libelle );
       m_hashBankName        .insertMulti(id_compte,bankName);
       m_hashRib_code_banque .insertMulti(id_compte,rib_code_banque);
       m_hashRib_code_guichet.insertMulti(id_compte,rib_code_guichet);
       m_hashRib_numcompte   .insertMulti(id_compte,rib_numcompte);
       m_hashRib_cle         .insertMulti(id_compte,rib_cle);
       m_hashSolde_initial   .insertMulti(id_compte,solde_initial);
       }
    return b;
}

void utilisateur::fillEdits(const QString& str){
  QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4' ").arg("rib_code_banque,rib_code_guichet,rib_numcompte,rib_cle,solde_initial",
                                                                  "comptes_bancaires",
                                                                  "libelle",
                                                                  str);
  QSqlQuery q(m_accountancyDb);
  if (!q.exec(req))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
  	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Error\n")+q.lastError().text(),QMessageBox::Ok);
  	  
      }
  while (q.next())
  {
  	soldeEdit         -> setText(q.value(SOLDE).toString()); //initialisation du code
        codeBankEdit      -> setText(q.value(CODE_BANQUE).toString());
        guichetEdit       -> setText(q.value(CODE_GUICHET).toString());
        ribCompteEdit     -> setText(q.value(NUMCOMPTE).toString());
        cleEdit           -> setText(q.value(CLE).toString());
      }
  
}

QString utilisateur::cleanMysqlDatas(QString & data){
    if(data.contains("'")){
           data.replace("'","''");
       }
    return data;
}

QString utilisateur::getIdUserFromLogin(const QString & login){
    QSqlQuery q(m_accountancyDb);
    QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("id_usr","utilisateurs","login",login);
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    QString str;
    QStringList list;
    while (q.next())
    {
    	str = q.value(0).toString();
    	list << str;
        }
    if (list.size()>1)
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Two same logins = ")+str,QMessageBox::Ok);
        }    
    return str;
}

void utilisateur::prepareNewUser(){
        comboUserName->insertItem(-1,"");
        comboUserName->setCurrentIndex(-1);
        loginEdit->setText("");
        passwdEdit->setText("");
        comboLibelle->insertItem(-1,"");
        comboLibelle->setCurrentIndex(-1);
        comboBankName->insertItem(-1,"");
        comboBankName->setCurrentIndex(-1);

      	soldeEdit         -> setText(""); //initialisation du code
        codeBankEdit      -> setText("");
        guichetEdit       -> setText("");
        ribCompteEdit     -> setText("");
        cleEdit           -> setText("");
}

void utilisateur::deleteUser()
{
    QMessageBox mess;
    mess.setWindowTitle("Effacer l'utilisateur.");
    mess.setText("Voulez vous vraiment effacer cet utilisateur ?");
    mess.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
    mess.setDefaultButton(QMessageBox::No);
    int answer = mess.exec();
    switch(answer){
        case QMessageBox::Ok :
            deleteThisUser();
            break;
        case QMessageBox::No :
            break;
        default :
            break;    
        }
}

bool utilisateur::deleteThisUser()
{
    bool success = true;
    QString userName = comboUserName->currentText();
    QString login = loginEdit->text();
    QString req = QString("DELETE FROM %1 WHERE %2 LIKE '%3' AND %4 LIKE '%5'")
                 .arg("utilisateurs","nom_usr",userName,"login",login);
    QSqlQuery qDelete(m_accountancyDb);
    if (!qDelete.exec(req))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	                       trUtf8("Ne peut pas effacer cet utilisateur !" ) +__FILE__+QString::number(__LINE__)
    	                       ,QMessageBox::Ok);
    	  success = false;
        }
    else
    {
    	remplirLogin(0);
        }
    return success;
}

void utilisateur::verrouIsChecked(bool checked)
{
    QString login = loginEdit->text();
    QString pass;
    QSqlQuery qSearchPass(m_accountancyDb);
    QString reqPass = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                     .arg("password","utilisateurs","login",login);
    QString getPass;
    qDebug() << __FILE__ << QString::number(__LINE__);
    PassDialog dPass(this,login);
    if (dPass.exec()==QDialog::Accepted)
    {
    	  getPass = dPass.getPassword();
        }qDebug() << __FILE__ << QString::number(__LINE__);
    if (pass != getPass)
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Ce n'est pas le bon mot de passe."),QMessageBox::Ok);
    	  return;
        }qDebug() << __FILE__ << QString::number(__LINE__);
    if (checked)
    {
    	  QSqlQuery qVer(m_accountancyDb);
    	  QString reqVerr = QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'")
    	                   .arg("utilisateurs","serrure","close","login",login);
    	  if (!qVer.exec(reqVerr))
    	  {
    	  	  QMessageBox::warning(0,trUtf8("Warning"),
    	  	  trUtf8("Impossible de mettre le verrou."),QMessageBox::Ok);
    	    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set secure" 
    	  	             << qVer.lastError().text();
    	      }
    	  
        }
     if (!checked)
     {
    	  QSqlQuery qUnVer(m_accountancyDb);
    	  QString reqUnVerr = QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'")
    	                   .arg("utilisateurs","serrure","open","login",login);
    	  if (!qUnVer.exec(reqUnVerr))
    	  {
    	  	  QMessageBox::warning(0,trUtf8("Warning"),
    	  	  trUtf8("Impossible de mettre le verrou."),QMessageBox::Ok);
    	    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set secure" 
    	  	             << qUnVer.lastError().text();
    	      }     	  
         }
}

#ifndef UTILISATEUR_H
#define UTILISATEUR_H
#include "modifutil.h"
#include "ui_utilisateurdialog.h"
#include <QtGui>
#include <QtCore>
#include <QtSql>

class utilisateur : public QWidget,public Ui::UtilisateursDialog
{
    Q_OBJECT
    enum Comptes_bancaires_fields{
      ID_COMPTE = 0,
      ID_USR ,
      LIBELLE ,
      NOM_BANQUE ,
      RIB_CODE_BANQUE,
      RIB_CODE_GUICHET ,
      RIB_NUMCOMPTE,
      RIB_CLE ,
      SOLDE_INITIAL ,
      MaxParam_CB
      };
    enum ribs{
       CODE_BANQUE = 0,
       CODE_GUICHET,
       NUMCOMPTE,
       CLE,
       SOLDE,
       MaxParamRIB
      };
    enum logins{
       IDUSER = 0,
       NAME,
       PASSWORD
    };
public:
    utilisateur(QWidget * parent = 0,QString loginUser = "");
    ~utilisateur();
    //QSqlDatabase m_db1;
    QSqlDatabase m_accountancyDb;
    QStringList queryBank(int /*idUsr*/);
protected slots:
    void enregistrer();
    void deleteUser();
    void remplirLogin(int);
    void fillEdits(const QString &);
    void modifier();
    void choisir();
    void prepareNewUser();
    void verrouIsChecked(bool);
private :
    bool fillHashOfBankDatas();
    QString getIdUserFromLogin(const QString & login);
    QString cleanMysqlDatas(QString & data);
    bool deleteThisUser();
    void resetWidget();
    //QStringList m_listUtil ;
    QHash<int,QString> m_hashUtil;
    QHash<QString,QString>  m_listLogin ;
    QHash<QString,QString> m_listPassword;
    QHash<QString,QString> m_hashId_compte;
    QHash<QString,QString> m_hashIdUsr;
    QHash<QString,QString> m_hashLibelle;
    QHash<QString,QString> m_hashBankName;
    QHash<QString,QString> m_hashRib_code_banque;
    QHash<QString,QString> m_hashRib_code_guichet;
    QHash<QString,QString> m_hashRib_numcompte;
    QHash<QString,QString> m_hashRib_cle ;
    QHash<QString,QString> m_hashSolde_initial;
    QHash<int,QString> m_hashListUsers;
    QHash<int,int> m_hashIdUsers;
    modifutil *m_modificationUtilisateurs;
    QString m_loginUser;
    
};

#endif

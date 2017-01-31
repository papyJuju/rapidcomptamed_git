#include "iotools.h"
#include "tools.h"

#include <common/constants.h>
#include <common/configure/medintux/connexion_medintux.h>
#include <common/configure/configmedintuxini.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QHash>
#include <QStringList>
#include <QDate>
#include <QDebug>
#include <QMessageBox>

using namespace Common;
using namespace Constants;
using namespace ToolsSpace;
using namespace MedintuxConnexionSpace;

Iotools::Iotools(QObject * parent)
{
    Q_UNUSED(parent);
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    m_iotoolsreturn = true;
}

Iotools::~Iotools(){}

double Iotools::getValueFromAct(const QString & act) 
{
    enum{VALUE_INSIDE=0,DATE_INSIDE};
    QString actinside = act;
    double value = 0.00;
    QHash<QString,double> valueshash;
    QString base;
    QString amountcolumn;
    QString datecolumn;
    QString codecolumn;
    base = getbasefromact(actinside);
    if (base == "actes_disponibles")
    {
          amountcolumn = "montant_total";
          datecolumn = "date_effet";
          codecolumn = "nom_acte";
        }
    if (base == "forfaits")
    {
          amountcolumn = "valeur";
          datecolumn = "date";
          codecolumn = "forfaits";
        }
    if (base == "ccam")
    {
          amountcolumn = "amount";
          datecolumn = "date";
          codecolumn = "code";
        }
    QString req = QString("select %1,%2 from %3 where %4 = '%5'")
                  .arg(amountcolumn,datecolumn,base,codecolumn,actinside);
    QSqlQuery qy(m_db);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        double v = qy.value(VALUE_INSIDE).toDouble();
        QString d = qy.value(DATE_INSIDE).toString();
        valueshash . insert(d,v);
        }
    
    QString datestr;
    if (valueshash.keys().size()>0)
    {
          datestr = valueshash.keys()[0];
        }     
    
    QDate datefirst = QDate::fromString(datestr,"yyyy-MM-dd");
    for ( int i = 0 ; i < valueshash.size() ; ++i)
    {
          QDate datenext = QDate::fromString(valueshash.keys()[i],"yyyy-MM-dd");
          if (datenext >= datefirst)
          {
                datefirst = datenext;
              }
        }   
    value = valueshash.value(datefirst.toString("yyyy-MM-dd"));
    return value;
}

QString Iotools::getbasefromact(const QString & act)
{
    QString reqbase = ("select * from %2 where %3 = '%4'");
    QString base;
    QString amountcolumn;
    QString datecolumn;
    QString reqbaseactesdispo = reqbase.arg("actes_disponibles","nom_acte",act);
    QString reqbaseforfaits = reqbase.arg("forfaits","forfaits",act);
    QString reqbaseccam = reqbase.arg("ccam","code",act);
    QHash<QString,QString> hash;
    hash.insert(reqbaseactesdispo,"actes_disponibles");
    hash.insert(reqbaseforfaits,"forfaits");
    hash.insert(reqbaseccam,"ccam");
    QStringList reqlist = hash.keys();
    foreach(QString req,reqlist){
        QSqlQuery qy(m_db);
        if (!qy.exec(req))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
            }
        while (qy.next())
        {
            base = hash.value(req);
            }
        }
    return base;
}

QString Iotools::newGuidOfNameIndex()
{
    QString guid;
    QStringList listGuid;
    QSqlQuery queryGuid(m_db);
    QString reqguid = QString("select %1 from %2").arg("guid","nameindex");
    if (!queryGuid.exec(reqguid))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << queryGuid.lastError().text() ;
        }
    while(queryGuid.next()){
        QString s = queryGuid.value(0).toString();
        listGuid << s;
        }
    if(listGuid.size() == 0){
        guid = "1";
        }
    else
    {
        listGuid.sort();
        guid = QString::number(listGuid.last().toInt()+1);
       }
    return guid;
}

QMap<QString,QString> Iotools::getSitesFromDatabase()
{
    enum e {ID_FOR_MAP=0,SITE_FOR_MAP};
    QMap<QString,QString> map;
    QSqlQuery qy(m_db);
    QString req = QString("select %1,%2 from %3").arg("id_site","site","sites");
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        QString id = qy.value(ID_FOR_MAP).toString();
        QString site = qy.value(SITE_FOR_MAP).toString();
        map.insert(id,site);
        }
    return map;
}

QMap<QString,QString> Iotools::getPayeursFromDatabase()
{
    enum e {ID_FOR_MAP=0,LOGIN_FOR_MAP};
    QMap<QString,QString> map;
    map.insert(0,tr("Patient"));
    QSqlQuery qy(m_db);
    QString req = QString("select %1,%2 from %3").arg("id_payeurs","nom_payeur",TABLE_PAYEURS);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        QString id = qy.value(ID_FOR_MAP).toString();
        QString login = qy.value(LOGIN_FOR_MAP).toString();
        map.insert(id,login);
        }
    return map;
}

QString Iotools::getIdFromUserLogin(const QString login)
{
    QString id;
    QSqlQuery qy(m_db);
    QString req = QString("select %1 from %2 where %3 = '%4'").arg("id_usr",TABLE_UTILISATEURS,"login",login);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        id = qy.value(0).toString();
        }
    return id;
}

QString Iotools::getIdFromSiteLogin(const QString login)
{
    QString id;
    QSqlQuery qy(m_db);
    QString req = QString("select %1 from %2 where %3 = '%4'").arg("id_site",TABLE_SITES,"site",login);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        id = qy.value(0).toString();
        }
    return id;
}

QString Iotools::getIdFromPayeurLogin(const QString login)
{
    QString id;
    QSqlQuery qy(m_db);
    QString req = QString("select %1 from %2 where %3 = '%4'").arg("id_payeurs",TABLE_PAYEURS,"nom_payeur",login);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        id = qy.value(0).toString();
        }
    return id;
}

QString Iotools::getGuidFromName(const QString totalname,const QString programName)
{
    enum 
    {
        NAME=0,
        FIRSTNAME
        };
    Tools t(this);
    QString name = totalname.split(",")[NAME];
    QString firstname = totalname.split(",")[FIRSTNAME];
    int enumprogram = t.hashEnumPrograms().key(programName);
    QString indexnomprenom = "IndexNomPrenom";
    QString guid;
    QStringList guidlist;
    QSqlDatabase db = m_db;
    ConnexionMedintux cnx(this);
    QString req;
    switch(enumprogram){
        case MEDINTUX_TYPE :
            if (!cnx.connectToDrTux())
            {
                  m_iotoolsreturn = false;
                  QMessageBox mess;
                  mess.setText(tr("Voulez vous configurer la connection vers DrTux ?"));
                  mess.setStandardButtons(QMessageBox::Close|QMessageBox::Ok);
                  mess.setDefaultButton(QMessageBox::Ok);
                  int ret = mess.exec();
                  switch(ret){
                      case QMessageBox::Ok :
                          configureConnexionMedintux();
                          break;
                      case QMessageBox::Close :
                          
                          break;
                      default :
                          break;    
                      }
                  
                      
                }
            db = QSqlDatabase::database(cnx.medintuxbasename());
            req = QString("select %1 from %2 where %3 = '%4' and %5 = '%6'").arg("FchGnrl_IDDos",indexnomprenom,"FchGnrl_NomDos",name,"FchGnrl_Prenom",firstname);
            break;
        case FREEMEDFORMS_TYPE :
            //TODO
            break;
        default :
            req = QString("select %1 from %2 where %3 = '%4' and %5 = '%6'").arg("guid","nameindex","name",name,"surname",firstname);
            break;
        }
    QSqlQuery qy(db);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        guid = qy.value(0).toString();
        guidlist << guid;
        }
    if (guidlist.size()>1)
    {
          QMessageBox::warning(0,tr("Warning"),tr("Veuillez choisir un des noms."),QMessageBox::Ok);
          //TODO
        }
    //qDebug() << __FILE__ << QString::number(__LINE__) << " guid  =" << guid ;
    return guid;
}

bool Iotools::isReturnedTrue()
{
    return m_iotoolsreturn;
}

void Iotools::configureConnexionMedintux()
{
    QDialog *dialog = new QDialog;
    QPushButton *quitButton = new QPushButton(dialog);
    quitButton->setText("Close");
    ConfigMedintuxIni *cmi = new ConfigMedintuxIni(dialog);
    QGridLayout *layout = new QGridLayout(dialog);
    layout->addWidget(cmi,0,0);
    layout->addWidget(quitButton,1,0);
    dialog->setLayout(layout);
    connect(quitButton,SIGNAL(pressed()),dialog,SLOT(reject()));    
    int ret = dialog->exec();
    if (ret==QDialog::Rejected)
    {
          delete dialog;          
        }      
}

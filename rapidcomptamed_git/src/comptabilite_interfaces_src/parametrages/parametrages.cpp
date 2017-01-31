#include "parametrages.h"

#include "../../common/configure/sitesdialog.h"
#include "../../common/configure/parametredialog.h"
#include "../../common/configure/payeursdialog.h"
#include "../../common/configure/pourcentages.h"
#include "../../common/configure/actesdispo.h"
#include "../../common/configure/automatisme.h"
#include "../../common/configure/rapport.h"
#include "../../common/configure/modifutil.h"
#include "../../common/configure/utilisateur.h"
#include "../../common/configure/thesaurus.h"
#include <common/configure/configmedintuxini.h>
#include <common/configure/configdatas.h>

//#include "../../common/completeBase.h"
#include "../../common/print/headingUserParam/headingParam.h"
#include <common/icore.h>
#include <common/settings.h>
#include <common/position.h>
#include <common/constants.h>

using namespace Common;
using namespace Constants;
using namespace ParametragesSpace;
using namespace ConfigSpace;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
parametrages::parametrages(QWidget *parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    Position *position = new Position;
    position->centralPosition(this,width(),height());
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    quitButton   -> setShortcut(QKeySequence("Ctrl+Q"));
    quitButton->hide();
    //-----le bouton compléter la base est-il accessible ou non ? -----
    //alerteBoutonCompleteBase(pushButton_8);
    pushButton_8->hide();
    
    //----------------------------------------------------------------
    connect(pushButton,    SIGNAL(pressed()), this , SLOT(utilisateurPreferentiel()));
    connect(pushButton_2,  SIGNAL(pressed()), this , SLOT(utilisateursComptabilite()));
    connect(pushButton_3,  SIGNAL(pressed()), this , SLOT(parametreSite()));
    connect(pushButton_4,  SIGNAL(pressed()), this , SLOT(entrePayeur()));
    connect(pushButton_5,  SIGNAL(pressed()), this , SLOT(entrePourcentage()));
    connect(pushButton_6,  SIGNAL(pressed()), this , SLOT(entreActe()));
    connect(pushButton_7,  SIGNAL(pressed()), this , SLOT(autoFermeture()));
    //connect(pushButton_8,  SIGNAL(pressed()), this , SLOT(completeBase()));
    connect(pushButton_9,  SIGNAL(pressed()), this , SLOT(rapportBug()));
    connect(quitButton, SIGNAL(pressed()), this , SLOT(close()));
    connect(pushButton_11, SIGNAL(pressed()), this , SLOT(headingsParams()));
    connect(thesaurusButton,SIGNAL(pressed()), this , SLOT(thesaurusModif()));
    connect(reinitConfigButton,SIGNAL(pressed()), this , SLOT(regenereConfigIni()));
    connect(configmedintuxdatasButton,SIGNAL(pressed()),this,SLOT(configmedintuxini()));    
    connect(configiniButton,SIGNAL(pressed()),this,SLOT(configdatasofrapidcomptamed()));
}

parametrages::~parametrages()
{}

void parametrages::utilisateurPreferentiel()
{
    parametredialog *m =  new parametredialog ;
    m -> show();
}
void parametrages::utilisateursComptabilite()
{
    QString str = "";
    utilisateur *u =  new utilisateur(0,str);
    u -> show();
}
void parametrages::parametreSite(){
    sites       *s =  new sites(this);
    s -> show();
}
void parametrages::entrePayeur(){
    payeurs     *p =  new payeurs(this);
    p -> show();
}
void parametrages::entrePourcentage(){
    pourcentage *pc =  new pourcentage;
    pc -> show();
}
void parametrages::entreActe(){
    actesdispo  *act =  new actesdispo(this);
    act -> show();
}
void parametrages::autoFermeture(){
    automatismeDialog *au =  new automatismeDialog;
    au -> show();
}
/*void parametrages::completeBase(){
    completeBaseCompta(pushButton_8);
}*/
void parametrages::rapportBug(){
    QString errorAllString;
    QFile f(settings()->applicationBundlePath()+"/errorLog.txt");
    qDebug() << __FILE__ << QString::number(__LINE__) << " " << settings()->applicationBundlePath()+"/errorLog";
    if(!f.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("Error log file cannot be opened."),QMessageBox::Ok);
        }
    QTextStream s(&f);
    s.setCodec("UTF8");
    while(!s.atEnd()){
        errorAllString += s.readLine()+"\n";
        }
  QString rapport = "rapport de bug";
  mail   *retour  =  new mail;
  if(retour       -> exec() == QDialog::Accepted){
      rapport     = retour->mail::textEdit->toPlainText();

  QString mailing = "mailto:pm.desombre@medsyn.fr?subject=Bug&body="+rapport+"\n"+errorAllString;
  QDesktopServices::openUrl(QUrl(mailing));
  }
  else{
    return;
  }
}

void parametrages::headingsParams(){
   headingParam *h = new headingParam(this);
   h->show();
}

void parametrages::thesaurusModif(){
   thesaurusModify * th = new thesaurusModify;
   th->show();
}

void parametrages::regenereConfigIni()
{
    
    QString hiddenconfigini = settings()->pathToConfigIni();
    qDebug() << __FILE__ << QString::number(__LINE__) << " config.ini =" << hiddenconfigini ;
    QString hiddenmedintux_configini = settings()->pathToHiddenRepertory()+"/config_medintux.ini";
    QFile hiddenconfigfile(hiddenconfigini);
    QFile hiddenmedintux_configfile(hiddenmedintux_configini);
    hiddenconfigfile.copy(settings()->pathToConfigIni()+"."+QDateTime::currentDateTime().toString(Qt::ISODate));
    hiddenmedintux_configfile.copy(settings()->pathToHiddenRepertory()+"/config_medintux.ini."+QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!hiddenconfigfile.remove())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to remove config.ini" ;
        }
    if (!hiddenmedintux_configfile.remove())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to remove config_medintux.ini" ;
        }
    QString resourcesconfigini = settings()->applicationResourcesPath()+"/config/config.ini";
    QString resourcesmedintuxconfigini = settings()->applicationResourcesPath()+"/config/config_medintux.ini";
    QFile resourcesconfigfile(resourcesconfigini);
    QFile resourcesmedintuxconfigfile(resourcesmedintuxconfigini);
    resourcesconfigfile.copy(settings()->pathToConfigIni());
    resourcesmedintuxconfigfile.copy(settings()->pathToHiddenRepertory()+"/config_medintux.ini");
    QMessageBox::information(0,tr("Info"),tr("Veuillez relancer le programme \npour prendre en compte les nouvelles données."),QMessageBox::Ok);
}

void parametrages::configmedintuxini()
{
    ConfigMedintuxIni * cmi = new ConfigMedintuxIni(this);
    cmi->show();
}

void parametrages::configdatasofrapidcomptamed()
{
    qDebug() << __FILE__ << QString::number(__LINE__);
    ConfigDatas * cfg = new ConfigDatas(this);qDebug() << __FILE__ << QString::number(__LINE__);
    cfg->show();qDebug() << __FILE__ << QString::number(__LINE__);
}


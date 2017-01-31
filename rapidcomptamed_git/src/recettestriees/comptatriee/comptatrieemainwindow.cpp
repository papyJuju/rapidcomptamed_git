#include "comptatrieemainwindow.h"
#include "../grand_livre/grandlivre.h"
#include "../frenchTax/frenchTax.h"
#include "../recettestriees/recettestriees.h"
#include "../depensestriees/depensestriees.h"
#include "../etatdesdus/etatdesdus.h"
#include "../export_fiscal/exportfiscal.h"
#include "../../setup/wiz1.h"
#include "../../notes/notes.h"
#include "../../comptabilite_interfaces_src/immobilisations/immobilisations.h"
#include "../../comptabilite_interfaces_src/immobilisations/NouvelleImmobilisation.h"
#include "../../comptabilite_interfaces_src/parametrages/parametrages.h"
#include "../../comptabilite_interfaces_src/depensedialog/depensedialog.h"
#include "../../comptabilite_interfaces_src/recettedialog/recettedialog.h"
#include "../../comptabilite_interfaces_src/recettedialog/modifierrecette.h"
#include "../../comptabilite_interfaces_src/recettedialog/rafraichirdialog.h"
#include "../../comptabilite_interfaces_src/bank/bankMainWindow.h"
#include "../../comptabilite_interfaces_src/bankcontrols/inputswidget.h"
#include "../../comptabilite_interfaces_src/facturation/facturation.h"

#include <parse_CCAM/parseCCAM.h>

#include <sauvegarde/sauvedialog.h>
#include <alertes/alertes.h>

#include <common/icore.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/constants.h>
#include <common/databasemanager.h>
#include <common/position.h>
#include <common/configure/help.h>
#include <common/configure/utilisateur.h>
#include <common/configure/comptatorapidcompta.h>
#include <common/configure/message_alerte.h>

#include <QProcess>
#include <QString>
#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

using namespace Common;
using namespace Constants;
using namespace ParametragesSpace;
using namespace MessageSpace;


static inline Common::Settings *settings() {return Common::ICore::instance()->settings();}
static inline Common::CommandLine *commandLine() {return Common::ICore::instance()->commandLine();}
static inline Common::DatabaseManager *databaseManager() {return Common::ICore::instance()->databaseManager();}
static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline QString patientUUID() {return commandLine()->value(Common::CommandLine::CL_PatientUUID).toString();}

static inline QString getProcessFullPath(const QString &process)
{
#ifdef Q_OS_MAC
    return settings()->applicationBundlePath() + QDir::separator() + process + ".app/Contents/MacOs/" + process;
#else
    return settings()->applicationBundlePath() + QDir::separator() + process;
#endif
}

comptatriee::comptatriee() :
        mFile(0),
        mAnalyse(0),
        mInterfaces(0),
        mSettings(0),
        mUpdateCCAM(0),
        mSauvegarde(0),
        mHelp(0),
        m_fermer(0),
        m_tridesrecettes(0),
        m_etatsdesdus(0),
        m_recettes(0),
        m_facturation(0),
        m_depenses(0),
        m_grandLivre(0),
        m_immobilisations(0),
        m_setup(0),
        m_parametres(0),
        m_bankAndUsers(0),
        m_bankDeposits(0),
        m_notes(0),
        m_openUrlAmeli(0),
        m_parseCCAM(0),
        m_error(0),
        m_regenereconfig(0),
        m_recupoldbaseaction(0),
        m_alertesaction(0)
{
    setupUi(this);
    QString size = settings()->value(Constants::PREF_SIZE_RECETTE).toString();qDebug() << __FILE__ << QString::number(__LINE__);
    int sizeWidth = 1000;
    int sizeHeight = 600;
    int minsizeWidth = 500;
    int minsizeHeight = 500;
    QStringList sizelist = size.split(" ");
    if (sizelist.count()>1)
    {
        sizeWidth = sizelist[0].toInt();
        sizeHeight = sizelist[1].toInt();
        }    
    resize(sizeWidth,sizeHeight);
    QString minSize = settings()->value(Constants::MIN_SIZE_RECETTE).toString();
    QStringList minsizelist = minSize.split(" ");
    if (minsizelist.count()>1)
    {
        minsizeWidth = minsizelist[0].toInt();qDebug() << __FILE__ << QString::number(__LINE__);
        minsizeHeight = minsizelist[1].toInt();qDebug() << __FILE__ << QString::number(__LINE__); 
        }
    setMinimumSize(minsizeWidth,minsizeHeight);
    position()->centralPosition(this,width(),height());
    QBrush brush(QImage(settings()->iconPath()+"/la_comptabilite.png"));
    mdiArea->setBackground(brush);
    createMenus();
    createActions();
    connectActions();
    databaseManager()->connectAccountancy();
    //test message alertes charges a payer
    MessageAlerte *messagealerte = new MessageAlerte(this);
    messagealerte->messageAreAvailable();
}

comptatriee::~comptatriee()
{
}

void comptatriee::lancerrecettestriees()
{
    recettestriees * recettesTriees = new recettestriees;
    //recettesTriees->execRec();
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    QWidget * recwidget = recettesTriees->widgetrectrie();
    setCentralWidget(recwidget);
}

void comptatriee::lancerdepensestriees(){
    depensestriees *d = new depensestriees;
    //d->execRec();
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    QWidget * depwidget = d->widgetdeptrie();
    setCentralWidget(depwidget);    
}

void comptatriee::lanceretatdesdus()
{ 
    QString name = "";
    QString firstname = "";
    QString guid = "";
    QString user = "";
    etatdesdus *etat  = new etatdesdus(name,firstname,guid,user);
    //etat -> show();
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(etat);
}

void comptatriee::lancerecettes()
{
    recettedialog *r = new recettedialog(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(r);
}

void comptatriee::showBillings()
{
    Facture *f = new Facture(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(f);
}
void comptatriee::lancedepenses()
{
    depensedialog *widget = new depensedialog();
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(widget);
}

void comptatriee::lanceImmobilisations()
{
    immobilisations *widget = new immobilisations;
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(widget);
}


void comptatriee::lanceGrandLivre(){
    grandlivre *livre = new grandlivre;
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(livre);
}

void comptatriee::lanceFrenchTax(){
    frenchTax *frt = new frenchTax;
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(frt);
}

void comptatriee::lanceSetup()
{
    wiz1 *wiz = new wiz1;
    if (wiz->returnfalse())//FIXME
    {
          qDebug() << __FILE__ << QString::number(__LINE__) << "wiz->returnfalse()";
          wiz->close();
        }
    else{
        QWidget *w = centralWidget();
        if (w)
        {
            delete w;
            }
        setCentralWidget(wiz);
    }
}

void comptatriee::lanceParametrages()
{
    parametrages *param = new parametrages(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
  setCentralWidget(param);
}

void comptatriee::lanceHelp(){
    helpWidget *aide = new helpWidget(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(aide);
    }
    
void comptatriee::showBankAndUsers(){
    utilisateur *u = new utilisateur(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(u);
}

void comptatriee::showBankDeposits(){
    MainWindowBank * mw = new MainWindowBank(0);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    mw->show();
}

void comptatriee::showInputsBank()
{
    Inputs * input = new Inputs(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(input);
}


void comptatriee::showNotes(){
    Notes *n = new Notes(0);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(n);
}

void comptatriee::lanceAlertes()
{
    Alertes * a = new Alertes(0);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(a);
}

void comptatriee::showErrors(){
    QString errorAllString;
    QFile f(settings()->applicationBundlePath()+"/errorLog.txt");
    //qDebug() << __FILE__ << QString::number(__LINE__) << " " << settings()->applicationBundlePath()+"/errorLog";
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

void comptatriee::createMenus()
{
    mFile = menuBar()->addMenu(tr("&Fichier"));
    mAnalyse = menuBar()->addMenu(tr("&Analyse"));
    mInterfaces = menuBar()->addMenu("&Programmes");
    mSettings = menuBar()->addMenu(tr("&Configuration"));
    mUpdateCCAM = menuBar()->addMenu(tr("&Mise à jour CCAM"));
    mSauvegarde = menuBar()->addMenu(tr("&Sauvegarde"));
    mHelp = menuBar()->addMenu(tr("&Help"));
}

void comptatriee::createActions()
{
    /** \todo Translate tr text to english */
    m_fermer = mFile->addAction(QIcon(settings()->iconPath()+"/stop.png"),tr("&Quitter"));
    m_fermer->setShortcut(QKeySequence::Close);

    m_tridesrecettes = mAnalyse->addAction(QIcon(settings()->iconPath()+"/gear.png"),tr("Trier les rece&ttes"));
    m_tridesdepenses = mAnalyse->addAction(QIcon(settings()->iconPath()+"/gear.png"),tr("Trier les depen&ses"));
    m_etatsdesdus = mAnalyse->addAction(QIcon(settings()->iconPath()+"/gear.png"),tr("Etat des d&us"));
    m_grandLivre = mAnalyse->addAction(QIcon(settings()->iconPath()+"/edit.png"),tr("Grand &Livre"));
    m_frenchTax = mAnalyse->addAction(QIcon(settings()->iconPath()+"/edit.png"),tr("C&onstruire 2035"));
    m_exporterfichierpourimpots = mAnalyse->addAction(QIcon(settings()->iconPath()+"/edit.png"),tr("E&xportation fiscale"));
    m_frenchTax->setStatusTip("Construit et imprime la 2035");
    m_recettes = mInterfaces->addAction(QIcon(settings()->iconPath()+"/recette.png"),tr("&Recettes"));
    m_alertesaction = mInterfaces->addAction(QIcon(settings()->iconPath()+"/alerte.png"),tr("&Alertes"));
    m_facturation = mInterfaces->addAction(QIcon(settings()->iconPath()+"/facture.png"),tr("&Facturation"));
    qDebug() << __FILE__ << QString::number(__LINE__) << " icones =" << settings()->iconPath()+"/facture.png" ;
    m_depenses = mInterfaces->addAction(QIcon(settings()->iconPath()+"/depense.png"),tr("&Dépenses"));
    m_immobilisations = mInterfaces->addAction(QIcon(settings()->iconPath()+"/immobilisations.png"),tr("&Immobilisations"));
    m_bankAndUsers = mInterfaces->addAction(QIcon(settings()->iconPath()+"/immobilisations.png"),tr("U&tilisateurs"));
    m_bankDeposits = mInterfaces->addAction(QIcon(settings()->iconPath()+"/immobilisations.png"),tr("Rapproc&hements bancaires"));
    m_bankInputs = mInterfaces->addAction(QIcon(settings()->iconPath()+"/immobilisations.png"),tr("Apports recettes bancaires"));
    m_notes = mInterfaces->addAction(QIcon(settings()->iconPath()+"/edit.png"),tr("&Notes personnelles."));
    m_setup = mSettings->addAction(QIcon(settings()->iconPath()+"/setup.png"),tr("Set&up for MedinTux"));
    m_regenereconfig = mSettings->addAction(QIcon(settings()->iconPath()+"/setup.png"),tr("Régénère $HOME/.config.ini"));
    m_setup->setStatusTip("Lance le setup d'installation des boutons de menu dans MedinTux");
    m_setup->setIconText("Lance");
    m_parametres = mSettings->addAction(QIcon(settings()->iconPath()+"/param.tiff"),tr("Pa&ramétrages"));
    m_parametres->setStatusTip("Lance le logiciel de paramétrage de la Comptabilité.");
    m_help = mHelp->addAction(QIcon(settings()->iconPath()+"/param.tiff"),tr("Help"));
    m_help->setStatusTip("Lance la fenêtre d'aide de la comptabilité.");
    m_error = mHelp->addAction(QIcon(settings()->iconPath()+"/param.tiff"),tr("Show debug"));
    m_error->setStatusTip("Lance la fenêtre de visualisation des message de debug.");
    m_openUrlAmeli = mUpdateCCAM->addAction(QIcon(settings()->iconPath()+"/web.png"),tr("Trouver CCAM"));
    m_openUrlAmeli->setStatusTip(tr("Page web de la CCAM fichier CACTOT d'ameli"));
    m_parseCCAM = mUpdateCCAM->addAction(QIcon(),tr("Intégrer la mise à jour"));
    m_recupoldbaseaction = mSettings->addAction(QIcon(),tr("Copie l'ancienne base dans la nouvelle."));
    m_sauvegarder = mSauvegarde->addAction(QIcon(),tr("Sauvegarde"));

}

void comptatriee::connectActions()
{
    connect(m_fermer        ,  SIGNAL(triggered()), this, SLOT(close()));
    connect(m_tridesrecettes,  SIGNAL(triggered()), this, SLOT(lancerrecettestriees()));
    connect(m_tridesdepenses,  SIGNAL(triggered()), this, SLOT(lancerdepensestriees()));
    connect(m_etatsdesdus,     SIGNAL(triggered()), this, SLOT(lanceretatdesdus()));
    connect(m_recettes,        SIGNAL(triggered()), this, SLOT(lancerecettes()));
    connect(m_facturation,     SIGNAL(triggered()), this, SLOT(showBillings()));    
    connect(m_depenses,        SIGNAL(triggered()), this, SLOT(lancedepenses()));
    connect(m_grandLivre,      SIGNAL(triggered()), this, SLOT(lanceGrandLivre()));
    connect(m_immobilisations, SIGNAL(triggered()), this, SLOT(lanceImmobilisations()));
    connect(m_bankAndUsers,SIGNAL(triggered()),this,SLOT(showBankAndUsers()));
    connect(m_setup          , SIGNAL(triggered()), this, SLOT(lanceSetup()));
    connect(m_parametres     , SIGNAL(triggered()), this, SLOT(lanceParametrages()));
    connect(m_frenchTax     ,  SIGNAL(triggered()), this, SLOT(lanceFrenchTax()));
    connect(m_help          ,  SIGNAL(triggered()), this, SLOT(lanceHelp()));
    connect(m_error         ,  SIGNAL(triggered()), this, SLOT(showErrors()));
    connect(m_notes,SIGNAL(triggered()),this,SLOT(showNotes()));
    connect(m_bankDeposits,SIGNAL(triggered()),this,SLOT(showBankDeposits()));
    connect(m_bankInputs,SIGNAL(triggered()),this,SLOT(showInputsBank()));
    connect(m_openUrlAmeli,SIGNAL(triggered()),this,SLOT(showAmeliWebPage()));   
    connect(m_parseCCAM,SIGNAL(triggered()),this,SLOT(runParseCcam()));
    connect(m_regenereconfig,SIGNAL(triggered()),this,SLOT(regenereConfigIni()));
    connect(m_recupoldbaseaction,SIGNAL(triggered()),this,SLOT(recupoldbase()));
    connect(m_sauvegarder,SIGNAL(triggered()),this,SLOT(sauvegarderbases()));
    connect(m_exporterfichierpourimpots,SIGNAL(triggered()),this,SLOT(exportfiscal()));
    connect(m_alertesaction,SIGNAL(triggered()),this,SLOT(lanceAlertes()));    
}

void comptatriee::errorMessage(QString & error){
    QMessageBox::warning(0,tr("Error"),tr("Error while start process :")+error,QMessageBox::Ok);
}

void comptatriee::showAmeliWebPage()
{
    QString ameliAdress;
    QFile file(settings()->applicationResourcesPath()+"/url/lienameli.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "cannot open ameli web page." ;
        }
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
    	QString line = stream.readLine().trimmed();
    	if (!line.isEmpty())
    	{
    		  ameliAdress = line;
    	    }
        }
    QUrl ameli(ameliAdress);
    if (QDesktopServices::openUrl(QUrl(ameli)))
    {
        QTimer::singleShot(4000,this,SLOT(messageameli()));
        }
}

void comptatriee::messageameli()
{
          QString explicativetext = tr("Ouvrez par libreoffice calc le fichier excel d'ameli.\n"
          "Faites File->Save as-> Text CSV.\n"
          "Save dans rapidcomptamed/resources/ameli en \n"
          "utf8, \nfield delimiteur = | \ntext delimiter = \"  \n"
          "Puis vous lancerez la mise à jour de la ccam."
          "Vous enregistrerez aussi le fichied ccam.xls dans rapidcomptamed/resources/ameli.");
          
          QMessageBox::information(0,tr("Info"),explicativetext,QMessageBox::Ok);
}

void comptatriee::runParseCcam()
{
    ParseCcam *parse = new ParseCcam(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(parse);
}

void comptatriee::regenereConfigIni()//FIXME
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
    QMessageBox::information(0,tr("Info"),tr("Le programme va fermer, veuillez le relancer"),QMessageBox::Ok);
    close();
}

void comptatriee::recupoldbase()
{
    ComptaToRapid * ctr = new ComptaToRapid(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(ctr);
}

void comptatriee::sauvegarderbases()
{
    sauvedialogue * save = new sauvedialogue(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(save);  
}

void comptatriee::exportfiscal()
{
    ExportFiscal * exp = new ExportFiscal(this);
    QWidget *w = centralWidget();
    if (w)
    {
          delete w;
        }
    setCentralWidget(exp); 
}




// UNNEEDED IF YOU WORK WITH QDIR INSTEAD OF QSTRING...
//QString comptatriee::adapteCheminWindows(QString & chemin){
//    QString str           = chemin;
//    QString nouveauChemin = "";
//    QStringList listStr   = str.split("/");
//    for(int i = 0;i < listStr.size();i++){
//        QString partie    = listStr[i];
//        if(listStr[i].contains(" ")){
//            partie = "\""+listStr[i]+"\"";
//        }
//        QString apres = "\\";
//        if(i == listStr.size()-1){
//            apres     = ".exe";
//        }
//        nouveauChemin += partie+apres;
//    }
//
//    return nouveauChemin;
//}



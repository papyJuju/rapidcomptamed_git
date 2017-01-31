#include "grandlivre.h"

#include <common/icore.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/constants.h>
#include <common/databasemanager.h>
#include <common/position.h>
#include <QtGui>
#include <QtCore>
#include <QtSql>


using namespace Common;
using namespace Constants;

QDate          g_date;
QString g_utilisateur;
static inline Common::Settings *settings() {return Common::ICore::instance()->settings();}
static inline Common::CommandLine *commandLine() {return Common::ICore::instance()->commandLine();}
static inline Common::DatabaseManager *databaseManager() {return Common::ICore::instance()->databaseManager();}
static inline QString patientUUID() {return commandLine()->value(Common::CommandLine::CL_PatientUUID).toString();}
grandlivre::grandlivre()
{
    setupUi(this);
    Position *position = new Position;
    position->centralPosition(this,width(),height());
//    databaseManager()->connectDrTux(DatabaseManager::Driver_MySQL);
    databaseManager()->connectAccountancy();

    QSqlDatabase db    = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    label_2           -> hide();
    label_3           -> hide();
    dateEdit_2        -> hide();
    dateEdit_3        -> hide();

    QDate dateCourante = QDate::currentDate();
    dateEdit           ->setDate(dateCourante);
    dateEdit           ->setDisplayFormat("yyyy");
    g_date             = dateEdit->date();
    QStringList listNamePredUsr;
    QString prefUsrName;
    QSqlQuery qPrefUsr(db);
    if(!qPrefUsr.exec(QString("SELECT %1 FROM %2 WHERE login = '%3'")
        .arg("nom_usr",
             "utilisateurs",
              settings()->value(Constants::S_UTILISATEUR_CHOISI).toString()))){
        qWarning() << __FILE__ << QString::number(__LINE__) << qPrefUsr.lastError().text();          
    }
    while(qPrefUsr.next()){
        prefUsrName = qPrefUsr.value(0).toString();
        listNamePredUsr << prefUsrName;
    }
    if(listNamePredUsr.size()> 1){
       QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Deux utilisateurs ont le même login !"),QMessageBox::Ok);
    }
    QString prefUsr = prefUsrName+","+settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
    comboBox           ->setEditable(false);
    QStringList listUtil;
                listUtil << prefUsr;
    QString requeteUtil =  "SELECT nom_usr, id_usr ,login FROM utilisateurs";
    QSqlQuery queryUtil(db);
          queryUtil.exec(requeteUtil);
          while(queryUtil.next()){
              QString u = queryUtil.value(0).toString();
              QString n = queryUtil.value(1).toString();
              QString l = queryUtil.value(2).toString();
              QString s = u+","+l;
              listUtil << s;
          }
    listUtil.removeDuplicates();
    comboBox           ->addItems(listUtil);
    
  m_doctrie           = new QTextDocument;
  m_doctrieJanvRec    = new QTextDocument;
  m_doctrieJanvDep    = new QTextDocument;
  m_doctrieFevRec     = new QTextDocument;
  m_doctrieFevDep     = new QTextDocument;
  m_doctrieMarRec     = new QTextDocument;
  m_doctrieMarDep     = new QTextDocument;
  m_doctrieAvrRec     = new QTextDocument;
  m_doctrieAvrDep     = new QTextDocument;
  m_doctrieMaiRec     = new QTextDocument;
  m_doctrieMaiDep     = new QTextDocument;
  m_doctrieJuinRec    = new QTextDocument;
  m_doctrieJuinDep    = new QTextDocument;
  m_doctrieJuilRec    = new QTextDocument;
  m_doctrieJuilDep    = new QTextDocument;
  m_doctrieAouRec     = new QTextDocument;
  m_doctrieAouDep     = new QTextDocument;
  m_doctrieSeptRec    = new QTextDocument;
  m_doctrieSeptDep    = new QTextDocument;
  m_doctrieOctRec     = new QTextDocument;
  m_doctrieOctDep     = new QTextDocument;
  m_doctrieNovRec     = new QTextDocument;
  m_doctrieNovDep     = new QTextDocument;
  m_doctrieDecRec     = new QTextDocument;
  m_doctrieDecDep     = new QTextDocument;
  label_5            ->setText("Ready");

 QPalette p=palette();
    p               .setColor(QPalette::Active, QPalette::Base, QColor ("#DDDDDD"));
    textEdit       ->setPalette(p);
  m_monThread         = new produceDoc(this);
    connect(pushButton  ,SIGNAL(pressed()),                 this,       SLOT(imprimer()));
    connect(pushButton_2,SIGNAL(pressed()),                 this,       SLOT(close()));
    connect(pushButton_4,SIGNAL(pressed()),                 this,       SLOT(fonctionDeTri()));
    connect(m_monThread ,SIGNAL(outThread(const QString &)),this,       SLOT(inThis(const QString &)));
    connect(m_monThread ,SIGNAL(started()),                 this,       SLOT(inThread()));
    connect(m_monThread ,SIGNAL(finished()),                this,       SLOT(sortiedoc()));
    connect(this        ,SIGNAL(deleteThread()),            this,       SLOT(deleteSlotThread()));
  //connect(this   ,SIGNAL(signDoc(const QTextDocument *)),monThread,SLOT(slotDoc(const QTextDocument *)));
}


grandlivre::~grandlivre(){
    delete m_doctrie ;
    delete m_doctrieJanvRec;
    delete m_doctrieJanvDep ;
    delete m_doctrieFevRec ;
    delete m_doctrieFevDep ;
    delete m_doctrieMarRec ;
    delete m_doctrieMarDep ;
    delete m_doctrieAvrRec ;
    delete m_doctrieAvrDep ;
    delete m_doctrieMaiRec ;
    delete m_doctrieMaiDep ;
    delete m_doctrieJuinRec;
    delete m_doctrieJuinDep;
    delete m_doctrieJuilRec;
    delete m_doctrieJuilDep;
    delete m_doctrieAouRec ;
    delete m_doctrieAouDep ;
    delete m_doctrieSeptRec;
    delete m_doctrieSeptDep;
    delete m_doctrieOctRec ;
    delete m_doctrieOctDep ;
    delete m_doctrieNovRec ;
    delete m_doctrieNovDep ;
    delete m_doctrieDecRec ;
    delete m_doctrieDecDep ;
    
    }

void grandlivre::fonctionDeTri(){
    g_date          = dateEdit->date();
    g_utilisateur   = comboBox->currentText();
    textEdit -> clear();
    m_monThread     ->start();
     
    }

void grandlivre::inThis(const QString & text){
    label_5 -> setText(text);
}
void grandlivre::imprimer(){
    QPrinter printer;
    QPrintDialog *printdialog = new QPrintDialog(&printer,this);
    printdialog               ->setWindowTitle(tr("Imprimer"));
    printdialog               ->exec();
    textEdit->document()      ->print(&printer);
}

void grandlivre::sortiedoc(){
    /*m_doctrie = m_monThread->recupTextDoc()->clone();
    emit      deleteThread();
    textEdit  ->setDocument(m_doctrie);*/
    
    QTextDocument *doc = new QTextDocument;
    doc = m_monThread->recupTextDoc()->clone();
    emit deleteThread();
    textEdit -> setDocument(doc);
    
}

void grandlivre::inThread(){
  m_monThread->dateChosen(m_date);
}

void grandlivre::deleteSlotThread(){
  delete m_monThread;
  m_monThread = new produceDoc(this); 
  label_5 ->setText("Ready");
  connect(m_monThread ,SIGNAL(outThread(const QString &)),this,       SLOT(inThis(const QString &)));
  connect(m_monThread ,SIGNAL(started()),                 this,       SLOT(inThread()));
  connect(m_monThread ,SIGNAL(finished()),                this,       SLOT(sortiedoc()));
}


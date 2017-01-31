#include "sauvedialog.h"
#include "findFileSauve.h"
#include "recherche_mysqldump.h"
#include "recup.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>


using namespace Common;
using namespace Constants;
using namespace MedintuxConnexionSpace;
using namespace ConstantsMedintuxSpace;
using namespace SauvegardeSpace;
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

sauvedialogue::sauvedialogue(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    /*QString warningApp = tr("Attention, cette partie du logiciel est beta, "
                           "il vous faut verifier la bonne sauvegarde des bases, en particulier la taille des fichiers.");
    warningLabel-> setStyleSheet("*{color : red;font-size : 14px}");
    warningLabel->setText(warningApp);*/
    warningLabel->setText("");
    QStringList args;
    args = qApp->arguments();
    enum SauveArgs{SAVEPROGRAM = 0,TEST};
    if (args.size()>1 && args[TEST].contains("standalone"))
    {
        QRect place(QApplication::desktop()->screenGeometry());
        move(place.width()/2 -width()/2 , place.height()/2 - height()/2);
        resize(800,500);
        }
    m_connexionmedintux = new ConnexionMedintux(this);
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);
    QString usrbinmysqldump;
    usrbinmysqldump = settings()->value(S_PATH_TO_MYSQLDUMP,QVariant("no mysqldump")).toString(); 
    if (usrbinmysqldump.contains("no mysqldump"))
    {
          QMessageBox::warning(0,tr("Erreur"),tr("Vérifiez et régénérez le config.ini de .rapidecomptamednew,\n"          
                                                 "dans les menus de comptatriee."),QMessageBox::Ok);
        }
    partitionPathEdit->setText(usrbinmysqldump);    
    m_processsauvecompta = new QProcess;
    m_processsauvedrtux = new QProcess;
    m_processsauveAlarmes = new QProcess;
    comboBox->setEditable(true);
    comboBox->setInsertPolicy(QComboBox::NoInsert);
    //chemin de sauvegarde
    QFile file(settings()->applicationResourcesPath()+"/sauvegarde/cheminsauvegarde.ini");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return;
    QStringList listechemins;
    QTextStream textstream(&file);
    while(!textstream.atEnd()){
        QString chemin = textstream.readLine();
        listechemins << chemin;
	    }
    comboBox->addItems(listechemins);
   //liste de base a sauvegarder
   basesComboBox->setEditable(1);
   basesComboBox->setInsertPolicy(QComboBox::NoInsert);
   QStringList bases;
   m_hashsavingchoices.insert(DRTUXTEST_COMPTA_ALARMES_CHOICE,"DrTuxTest+comptabilite+alarmes");
   m_hashsavingchoices.insert(COMPTA_CHOICE,DB_ACCOUNTANCY);
   m_hashsavingchoices.insert(ALARME_CHOICE,DB_ALARMES);
   m_hashsavingchoices.insert(DRTUX_CHOICE,DB_DRTUX);
   m_hashsavingchoices.insert(ALL_CHOICES,"Toutes bases.");
   bases = m_hashsavingchoices.values();
   basesComboBox->addItems(bases);

  //rythme de sauvegarde
  QHash<int,QString> rythmHash;
  rythmHash.insert(NOAUTO,tr("Manuel"));
  rythmHash.insert(VINGTQUATREHEURES,tr("toutes les 24 heures"));
  rythmHash.insert(DOUZEHEURES,tr("toutes les 12 heures"));
  rythmHash.insert(SIXHEURES,tr("toutes les six heures"));
  rythmHash.insert(HEURE,"toutes les heures");
  rythmHash.insert(DIXMINUTES,"toutes les dix minutes");
  rythmComboBox->setEditable(true);
  rythmComboBox->setInsertPolicy(QComboBox::NoInsert);
  for ( int i = 0; i < rythmHash.count(); ++ i)
  {
       rythmComboBox->addItem(rythmHash.value(i)) ;
      }

  //regler la date
  QHash<int,int> timerHash;
  timerHash.insert(NOAUTO,0);
  timerHash.insert(VINGTQUATREHEURES,86400000);
  timerHash.insert(DOUZEHEURES,43200000);
  timerHash.insert(SIXHEURES,21600000);
  timerHash.insert(HEURE,3600000);
  timerHash.insert(DIXMINUTES,600000);
  m_minuteur = new QTimer(this);
  if (rythmComboBox->currentIndex()>0)
  {
      m_minuteur->start(timerHash.value(rythmComboBox->currentIndex()));
      connect(m_minuteur,SIGNAL(timeout()),this,SLOT(fonctionrouge()));
      }  
  connect(saveButton,SIGNAL(pressed()),this,SLOT(fonctionrouge()));
  connect(savingPathButton,SIGNAL(clicked()),this,SLOT(trouverchemin()));
  connect(quitButton,SIGNAL(pressed()),this,SLOT(fermer()));
  connect(restoreButton,SIGNAL(pressed()),this,SLOT(recuperer()));
    connect(mysqldumpButton,SIGNAL(pressed()),this,SLOT(searchMysqldump()));
    
}

sauvedialogue::~sauvedialogue()
{}

void sauvedialogue::fonctionrouge(){
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    resultLabel->setText("<html><body><font size = 6 color = ""red"">en action</font></body></html>");
    resultLabel->show();
    QTimer::singleShot(3000,this,SLOT(mafonctionsauvegarde()));
}

//fonction de sauvegarde des bases mysql choisies

void sauvedialogue::mafonctionsauvegarde()
{
    bool result = true;
    QString medintux_base = m_connexionmedintux->medintuxbasename();
    QString hostmedintux = m_connexionmedintux->medintuxhost();
    QString utilmedintux = m_connexionmedintux->medintuxlogin();
    QString passwordmedintux = m_connexionmedintux->medintuxpassword();
    QString portmedintux ; //FIXME and TODO
    QString comptabase = QString(DB_ACCOUNTANCY); //FIXME
    QString hostcompta = settings()->value(S_COMPTA_DB_HOST).toString();
    QString utilcompta = settings()->value(S_COMPTA_DB_LOGIN).toString();
    QString passwordcompta = settings()->value(S_COMPTA_DB_PASSWORD).toString();
    QString portcompta;//FIXME and TODO
    m_cheminaffiche = comboBox->currentText();
    QDir medintuxsauve("medintuxsauve");
    QDir medintuxsauveplus("medintuxsauvetout");
    QDir repertoire(m_cheminaffiche);
    repertoire.cd(m_cheminaffiche);
    if(!medintuxsauve.exists())
    {
      repertoire.mkdir("medintuxsauve");
      }
    m_medintuxsauveDirPath = m_cheminaffiche+"/medintuxsauve";
    if(!medintuxsauveplus.exists()){
        repertoire.mkdir("medintuxsauvetout");
        }
    m_cheminafficheplus = m_cheminaffiche+"/medintuxsauvetout";
    m_datesauve = QDateTime::currentDateTime().toString();
    m_datesauve.replace(" ","_");
    m_datesauve.replace(".","");
    m_datesauve.replace(":","-");
    m_basesauve = basesComboBox->currentIndex();
    QString chemindump = partitionPathEdit->text();
    #if defined Q_OS_WIN
    QStringList listchemindump = chemindump.split("/");
    for(int i = 0;i < listchemindump.size() ; i++)
    {
        if(listchemindump[i].contains(" ")){
            listchemindump.replaceInStrings(listchemindump[i],"\""+listchemindump[i]+"\"");
            }
        }
    chemindump = listchemindump.join("\\");
    chemindump.replace("\"","\\\"");
    chemindump = chemindump+".exe";
    #endif
    QString mysqlmedintux = chemindump+QString(" --quick --compatible mysql40 -u %1 -h %2").arg(utilmedintux,hostmedintux);
    QString mysqlcompta = chemindump+QString(" --quick --compatible mysql40 -u %1 -h %2").arg(utilcompta,hostcompta);
    QString mysqlalarmes = chemindump+QString(" --quick --compatible mysql40 -u %1 -h %2").arg(utilmedintux,hostmedintux);
    QString outputfiledrtux = QString("%1/sauve%2%3.sql").arg(m_medintuxsauveDirPath,"drtux",m_datesauve);
    QString outputfilecompta = QString("%1/sauve%2%3.sql").arg(m_medintuxsauveDirPath,"rapidecompta",m_datesauve);
    QString outputfilealarmes = QString("%1/sauve%2%3.sql").arg(m_medintuxsauveDirPath,"alarmes",m_datesauve);
    QString mysqldumpdrtux = QString("%1 --password=%2 %3").arg(mysqlmedintux,passwordmedintux,medintux_base);
    QString mysqldumpcompta = QString("%1 --password=%2 %3").arg(mysqlcompta,passwordcompta,comptabase);
    qDebug() << __FILE__ << QString::number(__LINE__) << mysqldumpcompta ;
    QString mysqldumpalarmes = QString("%1 --password=%2 %3").arg(mysqlalarmes,passwordmedintux,DB_ALARMES);
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);
    //sauvegarde DrTux seul.
    if(m_basesauve == DRTUX_CHOICE){
        QDir dir(m_medintuxsauveDirPath);
        if(!dir.exists()){
             QMessageBox::warning(0,tr("erreur"),trUtf8("votre périphérique n'est-il pas débranché ?"),QMessageBox::Ok);
             }
        removeExceeds();      
        qDebug() << __FILE__ << QString::number(__LINE__) << " mysqldumpdrtux =" << mysqldumpdrtux ;           
        m_processsauvedrtux->setStandardOutputFile(outputfiledrtux,QIODevice::Truncate);
        m_processsauvedrtux->start(mysqldumpdrtux);
        connect(m_processsauvedrtux,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(fonctioncouleur()));
        }
    //sauvegarde comptabilité seule
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);
    if(m_basesauve == COMPTA_CHOICE){
        removeExceeds();
        qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);         
        m_processsauvecompta->setStandardOutputFile(outputfilecompta,QIODevice::Truncate);
        m_processsauvecompta->start(mysqldumpcompta);
      if(!m_processsauvecompta->waitForStarted()){
          readerror(m_processsauvecompta->error(),QString::number(__LINE__));
          }
      if(!m_processsauvecompta->waitForFinished(-1)){
          readerror(m_processsauvecompta->error(),QString::number(__LINE__));
          result = false;
          }
        connect(m_processsauvecompta,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(fonctioncouleur()));
        }
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__);
    //sauvegarde DrTux+comptabilite + alarmes
    if(m_basesauve == DRTUXTEST_COMPTA_ALARMES_CHOICE ){
        QDir dir(m_medintuxsauveDirPath);
        if(!dir.exists()){
            QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("Votre périphérique n'est-il pas débranché? "
                       "A moins que le chemin du périphérique de sauvegarde ne soit pas le bon."),QMessageBox::Ok);
            resultLabel -> setText("");
            return;
            }
        QStringList listefichiers;
        listefichiers = dir.entryList(QDir::Files,QDir::Time);
        for(int i = 0 ; i < listefichiers.size() ; i ++){
            qDebug () << listefichiers[i];
            }
        removeExceeds();
      /*QFile datadrtux(outputdrtux);
      QDataStream datastreamdrtux(&datadrtux);
      QFile datacompta(outputcompta);
      QDataStream datastreamcompta(&datacompta);
      QFile dataAlarmes(outputfilealarmes);
      QDataStream datastreamAlarmes(&dataAlarmes);*/
      qDebug() << __FILE__ << QString::number(__LINE__) << " mysqldumpdrtux =" << mysqldumpdrtux << outputfiledrtux;
      qDebug() << __FILE__ << QString::number(__LINE__) << " mysqldumpcompta =" << mysqldumpcompta ;
      qDebug() << __FILE__ << QString::number(__LINE__) << " mysqldumpalarmes =" << mysqldumpalarmes ;
      m_processsauvedrtux->setStandardOutputFile(outputfiledrtux,QIODevice::Truncate);
      //m_processsauvedrtux->setProcessChannelMode(QProcess::ForwardedChannels);
      m_processsauvedrtux->start(mysqldumpdrtux);
      if(!m_processsauvedrtux->waitForStarted()){
         readerror(m_processsauvedrtux->error(),QString::number(__LINE__));
         }
      if(!m_processsauvedrtux->waitForFinished(-1)){
         readerror(m_processsauvedrtux->error(),QString::number(__LINE__));
         result = false;
         }
      m_processsauvecompta->setStandardOutputFile(outputfilecompta,QIODevice::Truncate);
      m_processsauvecompta->start(mysqldumpcompta);
      if(!m_processsauvecompta->waitForStarted()){
          readerror(m_processsauvecompta->error(),QString::number(__LINE__));
          }
      if(!m_processsauvecompta->waitForFinished(-1)){
          readerror(m_processsauvecompta->error(),QString::number(__LINE__));
          result = false;
          }
      m_processsauveAlarmes->setStandardOutputFile(outputfilealarmes,QIODevice::Truncate);
      m_processsauveAlarmes->start(mysqldumpalarmes);
      if(!m_processsauveAlarmes->waitForStarted()){
          readerror(m_processsauveAlarmes->error(),QString::number(__LINE__));
          }
      if(!m_processsauveAlarmes->waitForFinished(-1)){
          readerror(m_processsauveAlarmes->error(),QString::number(__LINE__));
          result = false;
          }

      if(result){
          QTimer::singleShot(6000,this,SLOT(fonctioncouleur()));
          }

      }
//////////////////////////////////////////////////////////////
//sauvegarde de tout medintux
////////////////////////////////////////////////////////////////
    if(m_basesauve == ALL_CHOICES){
        bool result = true;
        QDir dir(m_cheminafficheplus);
        if(!dir.exists()){
            QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("Votre périphérique n'est-il pas débranché ?"),QMessageBox::Ok);
            QApplication::restoreOverrideCursor();
            }
        QFile basesfile(settings()->applicationResourcesPath()+"/sauvegarde/bases.ini");
        if (!basesfile.open(QIODevice::ReadOnly))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file bases.ini" ;
            }
        QTextStream basesfilestream(&basesfile);
        QStringList baseslist;
        while (!basesfilestream.atEnd())
        {
            baseslist << basesfilestream.readLine();
            }
        QProcess * processbase = new QProcess(this);
        foreach(QString base,baseslist){
            QString outputfilebase = QString("%1/sauve%2%3.sql").arg(m_medintuxsauveDirPath,base,m_datesauve);
            QString mysqldumpbase = QString("%1 --password=%2 %3").arg(chemindump,m_pass,base);
            QString fichier = m_cheminafficheplus+"/"+base+m_datesauve+".sql";
            QFile data(fichier);
            QDataStream datastream(&data);
            processbase->setStandardOutputFile(outputfilebase,QIODevice::Truncate);
            processbase->start(mysqldumpbase);
            if(!processbase->waitForStarted()){
                readerror(processbase->error(),QString::number(__LINE__));
                result = false;
                }
            if(!processbase->waitForFinished(-1)){
                readerror(processbase->error(),QString::number(__LINE__));
                result = false;
                }
                }
         //vérifier que les fichiers sql sontbien remplis et lancer la fonctioncouleur()
         if(result){
             QTimer::singleShot(12000,this,SLOT(fonctioncouleur()));
             }
         }
}

int sauvedialogue::fonctioncouleur() //TODO calculer taille des fichiers sauvegardes
{
  if (!checkfilessizes())
  {
        QMessageBox::warning(0,tr(""),tr("La taille des fichiers sauvegardés est nulle\nveuillez vérifier "
                "la taille de votre support de sauvegarde."),QMessageBox::Ok);      }
  QApplication::restoreOverrideCursor();
  QString texteResult  = "";
  QString repertoire_2 = m_cheminaffiche+"/medintuxsauve";
  if(basesComboBox->currentText() == "tout"){
      repertoire_2 = m_cheminaffiche+"/medintuxsauvetout";
      }
  QDir dir(repertoire_2);
  QStringList filters;
              filters << "*.sql";
  QStringList list;
              list = dir.entryList(filters);
  QStringList listSort;

 for(int i= 0; i < list.size() ; i++){
     if(((list[i] == "." )== false)|| ((list[i] == "..")== false)){
         listSort << list[i];
         }
     }
     listSort.sort();
    for(int i = 0 ; i < listSort.size() ; i ++){
       QFileInfo info(repertoire_2+"/"+listSort[i]);
       qint64 sizeBites = info.size();
       texteResult += listSort[i]+" "+QString::number(sizeBites)+" octets\n";
      }
    resultLabel -> setStyleSheet("*{color : blue;font-size : 11px}");
    resultLabel -> setText(texteResult);
    resultLabel -> setWordWrap(true);
    //resultLabel -> show();
    return true;
}

void sauvedialogue::fermer(){
    emit close();
}

void sauvedialogue::trouverchemin()
{
    findFileSauve *findfile = new findFileSauve(this,QString());
    if (findfile->exec()==QDialog::Accepted)
    {
          m_savingpath = findfile->findFileSauvePath();
          qDebug() << __FILE__ << QString::number(__LINE__) << " m_savingpath =" << m_savingpath ;
        }
    comboBox->setItemText(0,m_savingpath);
    QFile cheminsfile(settings()->applicationResourcesPath()+"/sauvegarde/cheminsauvegarde.ini");
    if (!cheminsfile.open(QIODevice::ReadOnly))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file cheminsauvegarde.ini" ;
        }
    QTextStream stream(&cheminsfile);
    QStringList listoflines;
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        listoflines << line;
        }
    cheminsfile.close();
    if (!cheminsfile.open(QIODevice::WriteOnly))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file cheminsauvegarde.ini for writing" ;
        }
    listoflines.prepend(m_savingpath);
    listoflines.removeDuplicates();
    QTextStream streamwrite(&cheminsfile);
    foreach(QString line,listoflines){
        streamwrite << line+"\n";
        }
}

void sauvedialogue::recuperer(){
    recup *r  = new recup;
    r -> show();
}

bool sauvedialogue::removeExceeds()
{
    bool success = true;
    QSettings set(settings()->pathToConfigIni(),QSettings::IniFormat);
    int maxnumber = set.value(MAXSAVINGNUMBER).toInt();
    if (maxnumber < 1)
    {
          QMessageBox::warning(0,tr("Attention"),
          tr("Le nombre max de fichiers gardés est zero, veuillez contrôler votre fichier config.ini"),QMessageBox::Ok);
          maxnumber = 3;
        }
    QDir dir(m_medintuxsauveDirPath);
    if(!dir.exists()) QMessageBox::warning(0,tr("Erreur"),trUtf8("Votre périphérique n'est-il pas débranché ?"),QMessageBox::Ok);
    QList<QStringList> listoftypesoffiles;
    QStringList listdrtuxfiles;
    QStringList listdrtuxfilter;
    listdrtuxfilter << "*drtux*";
    listdrtuxfiles = dir.entryList(listdrtuxfilter,QDir::Files,QDir::Time);
    
    QStringList listcomptafiles;
    QStringList listcomptafilter;
    listcomptafilter << "*compta*";
    listcomptafiles = dir.entryList(listcomptafilter,QDir::Files,QDir::Time);
    
    QStringList listalarmesfiles;
    QStringList listalarmesfilter;
    listalarmesfilter << "*Alarmes*" << "*alarmes*";
    listalarmesfiles = dir.entryList(listalarmesfilter,QDir::Files,QDir::Time);
    
    listoftypesoffiles << listdrtuxfiles << listcomptafiles << listalarmesfiles;
    qDebug() << __FILE__ << QString::number(__LINE__) << " listoftypesoffiles =" << QString::number( listoftypesoffiles.count()) ;
    
    for ( int i = 0; i < listoftypesoffiles.count(); ++ i)
    {
          QStringList thatList;
          thatList = listoftypesoffiles[i];qDebug() << __FILE__ << QString::number(__LINE__) << " thatList size =" << QString::number(thatList.size()) ;
          if ( thatList.size() > maxnumber)
          {
               qDebug() << __FILE__ << QString::number(__LINE__) << " thatList last =" << thatList.last() ;
               success = dir.remove(thatList.last());
              }
        }
    return success;
}

void sauvedialogue::searchMysqldump()
{
    recherchedump *recherche = new recherchedump(this);
    if (recherche->exec() == QDialog::Accepted)
    {
          partitionPathEdit->setText(recherche->getPathOfMysqldump());
        }
}

void sauvedialogue::readout()
{
    qDebug() << __FILE__ << QString::number(__LINE__) << " output =" << static_cast<QProcess*>(sender())->readAllStandardOutput();
}

void sauvedialogue::readerror(QProcess::ProcessError error,QString line)
{ 
    switch(error){
        case QProcess::FailedToStart :
            qWarning() << __FILE__ << line << "process failed to start" ;
            break;
        case QProcess::Crashed :
            qWarning() << __FILE__ << line << "Process crashed" ;
            break;
        case QProcess::Timedout :
            qWarning() << __FILE__ << line << "Process timed out" ;
            break;
        default :
            break;    
        }
    QApplication::restoreOverrideCursor();
}

bool sauvedialogue::checkfilessizes()
{
    QString repmedsauve = m_cheminaffiche+"/medintuxsauve";
    QString repmedsauvetout = m_cheminaffiche+"/medintuxsauvetout";
    QDir dirmedintuxsauve(repmedsauve);
    QDir dirmedintuxsauvetout(repmedsauvetout);
    QStringList listmedsauve;
    listmedsauve = dirmedintuxsauve.entryList(QDir::Files);
    foreach(QString filename,listmedsauve){
        QFile file(repmedsauve+QDir::separator()+filename);
        if (!file.exists())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << file.fileName() << "doesn' exist" ;
            }
        if (filename.size()==0)
        {
              return false;
            }
        }
    QStringList listmedsauvetout;
    listmedsauvetout = dirmedintuxsauvetout.entryList(QDir::Files);
    foreach(QString filename,listmedsauvetout){
        QFile file(repmedsauvetout+QDir::separator()+filename);
        if (!file.exists())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << file.fileName() << "doesn' exist" ;
            }
        if (filename.size()==0)
        {
              return false;
            }
        }  
    
    return true;
}

#include "recup.h"
#include "connect.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <common/configure/medintux/connexion_medintux.h>
#include <common/configure/medintux/constants_medintux.h>

#include <QDir>

using namespace Common;
using namespace Constants;
using namespace ConnexionsDatasForSauvegarde;
using namespace MedintuxConnexionSpace;
using namespace ConstantsMedintuxSpace;
using namespace SauvegardeSpace;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

recup::recup(QObject * parent)
{
    Q_UNUSED(parent);
    setupUi(this);   
    m_dbcompta     = QSqlDatabase::database(DB_ACCOUNTANCY);//comptabilite
    //m_dbalarmes = = QSqlDatabase::database(DB_ALARMES);FIXME verifier alarmes au cabinet
    //----placement------------------------
    QRect place(QApplication::desktop()->screenGeometry());
    move(place.width()/2 -width()/2 , place.height()/2 - height()/2);
    //-------------------------------------
    QString WhatsThis  = "";
    QFile fileWhatsThis(settings()->applicationResourcesPath()+"/sauvegarde/WhatsThis.ini");
    if(!fileWhatsThis.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,trUtf8("Erreur"),tr("WhatsThis.ini ne peut pas être ouvert."),QMessageBox::Ok);
        }
    QTextStream streamWhatsThis(&fileWhatsThis);
    while(!streamWhatsThis.atEnd()){
        QString line       = streamWhatsThis.readLine();
        WhatsThis += line+"\n";
        }
    setWhatsThis(WhatsThis);
    comboBox -> setEditable(false);
    comboBox -> setInsertPolicy(QComboBox::NoInsert);
    QStringList listCombo;
    QFile file(settings()->applicationResourcesPath()+"/sauvegarde/bases.ini");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("bases.ini ne peut pas être ouvert."),QMessageBox::Ok);
        }
    QTextStream stream(&file);
    while(!stream.atEnd()){
        QString line = stream.readLine();
        listCombo << line;
        }
    comboBox  -> addItems(listCombo);
    connect(whatsthisButton,SIGNAL(pressed()),this,SLOT(setWhatsThisMode()));
    connect(restoreButton,SIGNAL(pressed()),this,SLOT(wait()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(comboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(drtuxselected(const QString &)));
    
}

recup::~recup()
{
    delete find;
}

void recup::drtuxselected(const QString & text) 
{
    if (text.contains("drtux",Qt::CaseInsensitive))
    {
        ConnexionMedintux * conn = new ConnexionMedintux(this);
        if (!conn->connectToDrTux())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to connect to DrTuxTest" ;
            }
        m_dbdrtux = QSqlDatabase::database(DRTUXBASE);//drtuxtest        
        }
}

void recup::setWhatsThisMode()
{
    QWhatsThis::enterWhatsThisMode();
}
void recup::wait(){
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    resultLabel ->setStyleSheet("color : red ; font : bold ; font-size : 20px");
    resultLabel-> setText("WAIT");
    emit restaurer();
}
void recup::restaurer()
{
    QString recupChoisie = comboBox->currentText();
    QSqlDatabase database = m_dbdrtux;
    if(recupChoisie .contains(DB_ACCOUNTANCY))
    {
        database     = m_dbcompta;
        }
    QString host = database.hostName();
    QString user = database.userName();
    QString pass = database.password();
    int port = database.port();
    qWarning() << "connexion params = " << host << user << host;    
    if (user.isEmpty() || host.isEmpty()|| QString::number(port).isEmpty())
    {
          QApplication::restoreOverrideCursor();
          ConnexionsDatas conn(this);
          if (conn.exec() == QDialog::Accepted)
          {
                host = conn.host();
                user = conn.user();
                pass = conn.password();
                port = conn.port();
              }           
        }
    //todo : refaire connection à la base à récupérer.
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","mysql");
    db.setDatabaseName("mysql");
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);
    db.setPort(port);
    if(!db.open()){
        QMessageBox::critical(0,trUtf8("Fatal"),trUtf8("Database cannot open"),QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        resultLabel -> setText("");
        return;
        }
    int dossier = 0;
    QString dossierARestaurer = "";
    QMessageBox msgBox;
               msgBox.setText(trUtf8("Recupérons le dossier de backup."));
               msgBox.setInformativeText(trUtf8("Voulez vous continuer ?"));
               msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
               msgBox.setDefaultButton(QMessageBox::Close);
               int ret = msgBox.exec();
    switch(ret) {
       case QMessageBox::Ok:
            find = new findFileSauve(this);
            find->show();
            if(find->exec() == QDialog::Accepted){
                m_backupFile = find->fileRecord();
                qDebug() << "m_backupFile = "+m_backupFile;
                m_backupFilePath = find->findFileSauvePath();
                qDebug() << "m_backupFilePath ="+m_backupFilePath;
                QFile fileBackup(m_backupFilePath);
                      fileBackup.copy(QDir::currentPath()+"/recup/"+m_backupFile);
                QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
                }
            break;
       case QMessageBox::Close:
            QApplication::restoreOverrideCursor();
            emit close();
            return;

       }
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    QDir dir(qApp->applicationDirPath()+"/recup");
    QStringList filters;
                filters << "*.sql";
    QStringList listDossiers;
                listDossiers = dir.entryList(filters);
    qDebug() << "listDossiers ="+QString::number(listDossiers.size());
    if(listDossiers.size() == 0)
    {
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("No file in recup repertory,\nabort."),QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        resultLabel -> setText("");
        return;
        }
    if(listDossiers.size() > 1)
    {
         QMessageBox::warning(0,trUtf8("Error"),trUtf8("Deux dossiers de récupération dans le dossier recup,\nabandon."),QMessageBox::Ok);
         QApplication::restoreOverrideCursor();
         resultLabel -> setText("");
         return;
         }
    for(int i = 0 ; i < listDossiers.size() ; ++i)
    {
        QFile f(qApp->applicationDirPath()+"/recup/"+listDossiers[i]);
        if(!f.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(0,trUtf8("Erreur"),listDossiers[i]+trUtf8(" ne peut pas être ouvert."),QMessageBox::Ok);
            QApplication::restoreOverrideCursor();
            resultLabel -> setText("");
            return;
            }
        QTextStream s(&f);
        int sCompteur = 0;
        while(!s.atEnd())
        {
            sCompteur ++;
            QString line = s.readLine();
            qDebug() << "line ="+line;
            if(sCompteur > 5 )
            {
                QMessageBox::warning(0,trUtf8("Erreur"),listDossiers[i]+trUtf8(" est-il le bon fichier pour "
                                                                                      "la base demandée ?.\n"
                                                                                      "La restauration ne peut pas se faire.\n"
                                                                                      "Abandon."),QMessageBox::Ok);
                QApplication::restoreOverrideCursor();
                resultLabel -> setText("");
                return ;
                }
            if(line.contains(recupChoisie) )
            {
                dossier = i;
                break;
                }
            }//while
        }//for
    dossierARestaurer = listDossiers[dossier];
    //---remplissage par le fichier.sql--------------
    if(dossierARestaurer == "")
    {
        QMessageBox::warning(0,trUtf8("Erreur"),tr("Le dossier de ")+recupChoisie+trUtf8(" n'est pas disponible "
                                                   "dans le répertoire recup.\nAbandon."),QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        resultLabel -> setText("");
        return;
        }
    QSqlQuery queryDrop(db);
    QString Drop = "DROP DATABASE IF EXISTS "+recupChoisie;
    if (!queryDrop . exec(Drop))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << queryDrop.lastQuery() << queryDrop.lastError().text() ;
        }
    QSqlQuery queryCreate(db);
    QString Create = "CREATE DATABASE IF NOT EXISTS "+recupChoisie;
    if (queryCreate . exec(Create))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << queryCreate.lastQuery() << queryCreate.lastError().text() ;
        }
    QStringList listeRecup;
    QString strList = "";
    QFile fileRecupIni(qApp->applicationDirPath()+"/recup/"+dossierARestaurer);
    if(!fileRecupIni.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(0,QObject::tr("Erreur"),dossierARestaurer+trUtf8(" non trouvé."),QMessageBox::Ok);
        }
    QTextStream streamRecup(&fileRecupIni);
    while(!streamRecup.atEnd())
    {
        QString ligneRecup = streamRecup.readLine();

            if ((ligneRecup.startsWith("--") == false )&& (ligneRecup.endsWith("*/;") == false) )
            {
                strList += ligneRecup;
                if(ligneRecup.endsWith(";"))
                {
                    listeRecup  << strList;
                    strList = "";
                    }
                }
        }//while
    qDebug() << "listeRecup.size()="+QString::number(listeRecup.size());
    if(database.isOpen())
    {
        for(int i = 0; i < listeRecup.size() ; ++i)
        {
            if((listeRecup[i] == "") == false)
            {
                QSqlQuery query(database);
                if (!query.exec(listeRecup[i]))
                {
                      qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
                    }
                }
            }//for
        }
    else
    {     
        qDebug() << "in else";
        QSqlDatabase newDatabase = QSqlDatabase::addDatabase("QMYSQL",recupChoisie);
        newDatabase.setDatabaseName(recupChoisie);
        newDatabase.setHostName(host);
        newDatabase.setUserName(user);
        newDatabase.setPassword(pass);
        newDatabase.setPort(port);
        if(!newDatabase.open())
        {
            QMessageBox::critical(0,trUtf8("Error"),trUtf8("Database ")+recupChoisie+ trUtf8(" cannot open"),QMessageBox::Ok);
            QApplication::restoreOverrideCursor();
            resultLabel -> setText("");
            return;
            }
        for(int i = 0; i < listeRecup.size() ; ++i)
        {
            if(!listeRecup[i].isEmpty())
            {
                QSqlQuery query2(newDatabase);
                if (!query2.exec(listeRecup[i]))
                {
                      qWarning() << __FILE__ << QString::number(__LINE__) << query2.lastError().text() ;
                    }                     
                }
            }
        }
    resultLabel -> setStyleSheet("color : blue ; font-size : 14px");
    resultLabel -> setText(recupChoisie+trUtf8(" a été restaurée"));
    QApplication::restoreOverrideCursor();
}

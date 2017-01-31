#include "recherche_mysqldump.h"
#include "findFileSauve.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <QMessageBox>
#include <QDir>
#include <QDebug>


using namespace Common;
using namespace Constants;
using namespace SauvegardeSpace;
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

recherchedump::recherchedump(QWidget *parent):QDialog(parent){
    setupUi(this);
    comboBox->setEditable(true);
    QStringList listdir;
    QDir dir;
    QFileInfoList list;
    list = dir.drives();
    for(int i = 0; i < list.size();i++){
        QString infolist = list[i].filePath();
        listdir << infolist;
        }
    comboBox->addItems(listdir);
    connect(autosearchButton,SIGNAL(pressed()),this,SLOT(fonctioninsertion()));
}

recherchedump::~recherchedump(){}

void recherchedump::fonctioninsertion(){
    QString racinechoisie = comboBox->currentText();// par exemple C:/
    QString programfiles = racinechoisie+"Program Files";
    QString usrbin = "/usr/bin";
    QDir dirprogramfiles(programfiles);
    QDir dirroot(racinechoisie);
    QDir dirusrbin(usrbin);
    QStringList listeracine = dirroot.entryList();
    QStringList medintuxfiltre = listeracine.filter("Medintux");
    QStringList listeprogramfiles = dirprogramfiles.entryList();
    QStringList easyfiltre = listeprogramfiles.filter("easyphp",Qt::CaseInsensitive);
    QString mysqldump;
    if(easyfiltre.size() == 1){
        QString easyphp = easyfiltre[0];
        if(easyfiltre.size() > 1) {
            QMessageBox::warning(0,tr("info"),trUtf8("Il existe deux dossiers easyphp, le programme ne peut pas choisir."),QMessageBox::Ok);
            }
        //a la sortie easy contient le nom de easyphp
        mysqldump = programfiles+"/"+easyphp+"/mysql/bin/mysqldump";
        //proteger easy ?
        pathMysqlDumpEdit->setText(mysqldump);
        settings()->setValue(S_PATH_TO_MYSQLDUMP,mysqldump);
        QMessageBox::information(0,tr("info"),trUtf8("le chemin est donc dans config.ini"),QMessageBox::Ok);
        //emit close();
        }
    else if(medintuxfiltre.size()!=0){
        QString medintux = racinechoisie+"Medintux";
        QDir dirmedintuxwamp(medintux);   //trouver le chemin de dump de Bruno
        QStringList listemedintux = dirmedintuxwamp.entryList();
        listemedintux.filter("mysql");
        if(listemedintux.size()!=0){
	        mysqldump = medintux+"/mysql/bin/mysqldump";
	        pathMysqlDumpEdit->setText(mysqldump);     
	        settings()->setValue(S_PATH_TO_MYSQLDUMP,mysqldump);       
            QMessageBox::information(0,tr("info"),trUtf8("le chemin est bien dans config.ini"),QMessageBox::Ok);
            //emit close();
            }       
        else return ;
        }
    #ifdef Q_OS_LINUX
    else if (dirusrbin.entryList().contains("mysqldump",Qt::CaseInsensitive))
    {
          mysqldump = dirusrbin.absolutePath()+"/mysqldump";
          qDebug() << __FILE__ << QString::number(__LINE__) << " mysqldump =" << mysqldump ;
          pathMysqlDumpEdit->setText(mysqldump);
          settings()->setValue(S_PATH_TO_MYSQLDUMP,mysqldump);
          QMessageBox::information(0,tr("info"),trUtf8("le chemin sous linux est bien dans config.ini"),QMessageBox::Ok);
        }
    #endif    
    else 
    {
        findFileSauve *find = new findFileSauve(this,"mysqldump");
        //find->show();
        if(find->exec() == QDialog::Accepted){
            QString foundFile = find->fileRecord();
            QString foundFilePath = find->findFileSauvePath();
            mysqldump = foundFilePath+foundFile;
            }
        if (!mysqldump.contains("mysqldump"))
        {
              QMessageBox::warning(0,tr("Erreur"),tr("Vous n'avez pas choisi le binaire mysqldump"),QMessageBox::Ok);
              return;
            }
        }
}

QString recherchedump::getPathOfMysqldump()
{
    return pathMysqlDumpEdit->text();
}

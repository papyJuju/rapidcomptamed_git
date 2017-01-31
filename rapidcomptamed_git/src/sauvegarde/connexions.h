#ifndef CONNEXIONS_H
#define CONNEXIONS_H
#include "constants.h"
#include <QtCore>
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>

using namespace Constants;
//lance la connection a DrTuxTest



bool CreateConnexion2()
{
QStringList liste;
QStringList hostuser;
QFile fichier(""+g_pathabsolu+"/utilisateur.ini");
if(fichier.open(QIODevice::ReadOnly | QIODevice::Text) == false){
QMessageBox::warning(0,"erreur",QObject::trUtf8("le fichier utilisateur.ini n'est pas accessible"),QMessageBox::Ok);}
QTextStream loire(&fichier);
while(!loire.atEnd()){
QString ligne = loire.readLine();
QStringList liste = ligne.split("=");
hostuser << ""+liste[1]+"";
}
QString host = hostuser[0];
QString utilisateur = hostuser[1];
QString mapoule = hostuser[2];

if(mapoule == ""){

QSqlDatabase db2 = QSqlDatabase :: addDatabase("QMYSQL",COMPTABILITE_BASE);
db2.setHostName(""+host+"");
db2.setDatabaseName(COMPTABILITE_BASE);
db2.setUserName(""+utilisateur+"");
db2.setPassword("");

if (!db2.open()) {
	QMessageBox::critical(0,"Erreur ouverture de base", db2.lastError().text());
	return false;
}

return true;}
else{


QString motdepasse = mapoule;// hellothatsme;
motdepasse = motdepasse.replace(QRegExp("\\n"),"");
QSqlDatabase db2 = QSqlDatabase :: addDatabase("QMYSQL",COMPTABILITE_BASE);
db2.setHostName(""+host+"");
db2.setDatabaseName(COMPTABILITE_BASE);
db2.setUserName(""+utilisateur+"");
db2.setPassword(""+motdepasse+"");

if (!db2.open()) {
	QMessageBox::critical(0,"Erreur ouverture de base", db2.lastError().text());
	return false;
}

return true;
}
}

#endif

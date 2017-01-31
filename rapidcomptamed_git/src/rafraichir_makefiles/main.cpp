#include <QApplication>
#include <QtGui>
#include <QtCore>
#include <QDebug>

void myMessageOutput(QtMsgType type, const char *msg)
 {
     QFile file(qApp->applicationDirPath ()+"/errorLog.txt");
     file.open(QIODevice::ReadWrite|QIODevice::Append);
     QTextStream s(&file);
     switch (type) {
     case QtDebugMsg:
         s << "Debug: " << QString(msg) << "\n";
         break;
     case QtWarningMsg:
         s << "Warning: " << QString(msg) << "\n";
         break;
     case QtCriticalMsg:
         s << "Critical: " << QString(msg) << "\n";
         break;
     case QtFatalMsg:
         s << "Fatal: " << QString(msg) << "\n";
         abort();
     }
     file.close();
 }

int main (int argc, char *argv[])
{
    #ifdef Q_OS_WIN32
    qInstallMsgHandler(myMessageOutput);
    #endif
    QApplication a(argc,argv);
    QFile f(a.applicationDirPath ()+"/newmakefilesLog.txt");
    f.remove();
	const QString path = qApp->applicationDirPath();
	const QString pathRecetteDialog = path+"/../src/comptabilite_interfaces_src/recettedialog";
	qWarning() << __FILE__ << QString::number(__LINE__) << " pathRecetteDialog =" << pathRecetteDialog ;
	const QString pathDepenseDialog = path+"/../src/comptabilite_interfaces_src/depensedialog";
	qWarning() << __FILE__ << QString::number(__LINE__) << path+"/../src/comptabilite_interfaces_src/depensedialog" ;
	const QString pathComptatriee = path+"/../src/recettestriees/comptatriee";
	qWarning() << __FILE__ << QString::number(__LINE__) << path+"/../src/recettestriees/comptatriee";
	QString qmake ;
	QString makefile;
	#ifdef Q_OS_LINUX
	qmake = "/usr/bin/qmake-qt4 ";
	makefile = "Makefile";
	#endif
	#ifdef Q_OS_WIN32
	const QString settingsfile = path+"/../src/rafraichir_makefiles";
	QSettings set(settingsfile,QSettings::IniFormat);
	qmake = set.value("compilateur_path/qmake").toString();
	makefile = set.value("compilateur_path/makefile").toString();
	#endif
	qDebug() << __FILE__ << QString::number(__LINE__) << " qmake =" << qmake ;
	QProcess procRecetteDialog;
	procRecetteDialog.start(qmake + pathRecetteDialog +QDir::separator()+ "recettedialog.pro -o "+ pathRecetteDialog +QDir::separator()+makefile);
	qDebug() << __FILE__ << QString::number(__LINE__) << qmake + pathRecetteDialog +QDir::separator()+"recettedialog.pro -o "+ pathRecetteDialog +QDir::separator()+makefile; ;
	QProcess procDepenseDialog;
	procDepenseDialog.start(qmake + pathDepenseDialog +QDir::separator()+"depensedialog.pro -o " +pathDepenseDialog +QDir::separator()+makefile);
	QProcess procComptatriee;
	procComptatriee.start(qmake + pathComptatriee +QDir::separator()+"comptatriee.pro -o " + pathComptatriee +QDir::separator()+makefile );
	a.exec();
	qDebug() << __FILE__ << QString::number(__LINE__) << " result =" << procRecetteDialog.errorString() ;
	qDebug() << __FILE__ << QString::number(__LINE__) << " result =" << procDepenseDialog.readAllStandardError() ;
	qDebug() << __FILE__ << QString::number(__LINE__) << " result =" << procComptatriee.readAllStandardError() ;
	qWarning() << __FILE__ << QString::number(__LINE__) << "ALL SEEMS OK" ;
	
	a.exit(0);
	return 0;
}

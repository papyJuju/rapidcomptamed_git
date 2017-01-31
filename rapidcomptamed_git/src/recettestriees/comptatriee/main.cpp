/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.                  *
 *   pm.desombre@medsyn.fr                                                 *
 *                                                                         *
 *   Copyright (C) 2009 by Eric Maeker, MD.                                *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "comptatrieemainwindow.h"

#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <common/xmlparser.h>
#include <common/test.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>


using namespace Common;
using namespace Constants;

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

int main(int argc, char *argv[])
{
    #ifdef Q_WS_WIN
    qInstallMsgHandler(myMessageOutput);
    #endif
    #ifdef Q_OS_LINUX
    qInstallMsgHandler(myMessageOutput);
    #endif
    QApplication app(argc,argv);
    app.setApplicationName(BINARY_NAME);
    QFile f(app.applicationDirPath ()+"/errorLog.txt");
    f.remove();
    #ifdef Q_WS_WIN
    QStringList libPaths;
    QString binPath = app.applicationDirPath();
    libPaths << binPath;
    app.setLibraryPaths ( libPaths  );
    #endif
    QString strLibPath;
    QStringList listLibPath;
    listLibPath = app.libraryPaths();
    QString strOfListLibPath;
    foreach(strLibPath,listLibPath){
        strOfListLibPath += strLibPath+"\n";
        }
    qWarning() << "main of comptatriee, list of libraries path = " << strOfListLibPath;

    // Instanciate Core
    Common::ICore::instance();
    if (!Common::ICore::instance()->initialize()) {
        qWarning() << "Error : Unable to initialize Core";
        return 123;
    }
        //regenerer .config.ini
    #ifdef Q_OS_LINUX
    if (Common::ICore::instance()->settings()->allKeys().contains(CODEC_LINUX))
    {
        QString codec =  Common::ICore::instance()->settings()->value(CODEC_LINUX).toString();    
        QByteArray codecarray = codec.toLatin1();
        const char * codecchar = codecarray.data();
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codecchar));
        }
    else
    {
        QString warning = QString("<html>Une fonction a &eacute;t&eacute; ajout&eacute;e,"
        "<br/>veuillez r&eacute;g&eacute;n&eacute;rer votre $HOME/.config.ini"
        "<br/>dans comptatrie/configuration</html>");
        QMessageBox::warning(0,"Warning",warning,QMessageBox::Ok);
        }
    #elif defined Q_OS_WIN32
    if (Common::ICore::instance()->settings()->allKeys().contains(CODEC_WINDOWS))
    {
        QString codec =  Common::ICore::instance()->settings()->value(CODEC_WINDOWS).toString();    
        QByteArray codecarray = codec.toLatin1();
        const char * codecchar = codecarray.data();
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codecchar));
        }
    else
    {
        QString warning = QString("<html>Une fonction a &eacute;t&eacute; ajout&eacute;e,"
        "<br/>veuillez r&eacute;g&eacute;n&eacute;rer votre $HOME/.config.ini"
        "<br/>dans comptatrie/configuration</html>");
        QMessageBox::warning(0,"Warning",warning,QMessageBox::Ok);
        }
    #else
        QMessageBox::warning(0,"Warning","OS non support&eacute;.",QMessageBox::Ok);
    #endif
    comptatriee *mainwindow = new comptatriee();
                mainwindow->show();

    return app.exec();
}

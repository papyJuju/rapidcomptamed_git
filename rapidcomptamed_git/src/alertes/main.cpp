#include "alertes.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/databasemanager.h>
#include <common/settings.h>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

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
 
 int main (int argc, char * argv[])
 {
    qInstallMsgHandler(myMessageOutput);
    QApplication app(argc,argv);
    QFile f(app.applicationDirPath ()+"/errorLog.txt");
    f.remove();
    if (!ICore::instance()->initialize()) {
        qWarning() << "Error : unable to initialize Core";
        return 123;
        }
    if (Common::ICore::instance()->settings()->allKeys().contains(CODEC_LINUX))
    {
        QString codec =  Common::ICore::instance()->settings()->value(CODEC_LINUX).toString();
        QByteArray codecarray = codec.toLatin1();
        const char * codecchar = codecarray.data();
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codecchar));
        }
    qDebug () << __FILE__ << QString::number(__LINE__);
    Common::ICore::instance()->databaseManager()->connectAccountancy();    
    Alertes * al = new Alertes;
    al->show();
    return app.exec();
 }

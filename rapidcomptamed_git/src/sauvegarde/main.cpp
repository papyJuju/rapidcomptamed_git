#include "sauvedialog.h"
#include <QApplication>
#include <common/icore.h>

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
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    QFile f(a.applicationDirPath ()+"/errorLog.txt");
    f.remove();
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    if (!Common::ICore::instance()->initialize()) {
        qWarning() << "Error : Enable to initialize Core";
        return 123;
       }
    sauvedialogue *dialog = new sauvedialogue;
    dialog->show();
    return a.exec();
}

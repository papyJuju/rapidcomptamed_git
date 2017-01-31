#include "grandlivre.h"
#include <common/constants.h>
#include <common/icore.h>
#include <QApplication>
#include <QtGui>



#define INSTANCE Common::ICore::instance
using namespace Common;

int main(int argc,char *argv[])
{
QApplication a(argc,argv);
    /*QStringList arguments;
    arguments = a.arguments();
    QFileInfo pathinfo(arguments[0]);
    g_pathabsolu = pathinfo.absolutePath();*/
    a.setApplicationName(BINARY_NAME);

    // Instanciate Core
    if (!INSTANCE()->initialize()) {
        qWarning() << "Error : Enable to intialize Core";
        return 123;
    }
    ICore::instance()->databaseManager();
    grandlivre *widget = new grandlivre;
    widget->show();
    return a.exec();
}

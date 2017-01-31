#include "deposit.h"
#include <common/icore.h>
#include <common/settings.h>
#include <common/databasemanager.h>
#include <QApplication>
#include <QDebug>
using namespace Common;
int main (int argc, char * argv[])
{
    QApplication a(argc,argv);
    a.setApplicationName(BINARY_NAME);
    if (!ICore::instance()->initialize()) {
        qWarning() << "Error : Enable to intialize Core";
        return 123;
    }
    Common::ICore::instance()->databaseManager()->connectAccountancy();
    Deposits * d = new Deposits();
    d->show();
    return a.exec();
}

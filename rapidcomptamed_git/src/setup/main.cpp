#include <QApplication>
#include <QtGui>
#include "wiz1.h"

QString g_pathabsolu;

int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  QStringList arguments;
  arguments        = a.arguments();
  QFileInfo pathinfo(arguments[0]);
  g_pathabsolu     = pathinfo.absolutePath();
  wiz1 *wiz        = new wiz1;
        wiz       -> show();
  return a.exec();
}

/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.   *
 *   pm.desombre@medsyn.fr  *
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
#include <QtCore>
#include <QtGui>
#include <QApplication>

#include "etatdesdus.h"
#include <common/constants.h>
#include <common/icore.h>

#define INSTANCE Common::ICore::instance
using namespace Common;

QString g_pathabsolu;
QString Global_PathIni = g_pathabsolu + "etatdesdus.ini";
int main(int argc, char *argv[])
{
  QApplication a(argc,argv);
  QStringList arguments;
              arguments = a.arguments();
    QFileInfo pathinfo(arguments[0]);
    g_pathabsolu = pathinfo.absolutePath();
    a.setApplicationName(BINARY_NAME);
    // Instanciate Core
    if (!INSTANCE()->initialize()) {
        qWarning() << "Error : Enable to intialize Core";
        return 123;
    }
    //ICore::instance()->databaseManager();
  etatdesdus *dialog = new etatdesdus;
              dialog->show();
  return a.exec();
}

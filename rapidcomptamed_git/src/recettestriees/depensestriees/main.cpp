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

#include "../etatstries/etatstries.h"
#include <common/constants.h>
#include <common/icore.h>
#include <QtGui>
#include <QApplication>
#include <QtCore>

using namespace Common;
QString g_pathabsolu;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList arguments;
    arguments = a.arguments();
    QFileInfo pathinfo(arguments[0]);
    g_pathabsolu = pathinfo.absolutePath();
    #ifdef Q_WS_WIN
    a.addLibraryPath("/lib_QT4");
    #endif
    a.setApplicationName(BINARY_NAME);
    if (!ICore::instance()->initialize()) {
        qWarning() << "Error : Enable to intialize Core";
        return 123;
    }
    ICore::instance()->databaseManager();
    //----------------------------parametrage, les items doivent etre colles aux virgules--------------------------------------------------------------------
    QString appelsqlchamps        = "libelle,date,date_valeur,remarque,montant"; 	// champs appeles
    QString parametrestableformat = "250,150,150,80,80"; 				// largeur des champs de la table
    QString tablesql              = "mouvements";					//table appelee
    int nombrechamps              =  4; 					//champs a concatener avant especes pour ecrire les mois.
    QString itemspourcombobox     = "par mois,par type,par praticien" ;			//items pour le comboBox
    //-----------------------------------------------------------------------------------------------------------
    
    etatstries *w = new etatstries( appelsqlchamps ,tablesql,nombrechamps,parametrestableformat,itemspourcombobox);
                w ->show();
    
    return a.exec();
}



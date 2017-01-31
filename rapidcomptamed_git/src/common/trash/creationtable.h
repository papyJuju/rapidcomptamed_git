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
#ifndef CREATIONTABLE_H
#define CREATIONTABLE_H

#include <QtCore>
#include <QtSql>

bool createBaseAccountancy();
bool createConnexion3();
bool createbase();
void createtable_actes_disponibles();
void createtable_ccam();
void createtable_comptes_bancaires();
void createtable_depots();
void createtable_honoraires();
void createtable_immobilisations();
void createtable_mouvements();
void createtable_mouvements_disponibles();
void createtable_paiements();
void createtable_payeurs();
void createtable_sites();
void createtable_utilisateurs();
void createtable_z_version();
void createtable_pourcentage();
void createtable_seances();
void createtable_rapprochement_bancaire();
bool createTable_IndexName();
bool createTable_thesaurus();
bool createTable_forfaits();
bool createTable_informations();
bool createTable_indemnites_deplacement();
bool createTable_notes();
bool createTable_CCAMTest();
bool cvsParser(QStringList & /*fieldsList*/,QString & /*tableName*/, QString & /*cvsName*/);
void lancerutilisateurconfig();
void create_defaults_in_tables(QString & /*table*/, QStringList & /*listFields*/);



#endif

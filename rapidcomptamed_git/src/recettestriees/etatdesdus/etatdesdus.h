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
#ifndef ETATDESDUS_H
#define ETATDESDUS_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_etatdesdusdialog.h"

class etatdesdus:public QWidget,public Ui::EtatdesdusDialog
{
  Q_OBJECT
  public:
  etatdesdus(QString & ,QString &,QString &, QString &);
  ~etatdesdus();
  private :
  QTextTableFormat monformat(QTextTableFormat &tableFormat,QString & parametrespourtableformat);
  void modele(QString & /*mois*/,QString &/* jour1*/ , QString & /*dernierjour*/ ,
			    QTextTableFormat & /*tableFormattableau*/,QTextCursor */*cursortriefonction*/,QStringList & /*listefonction*/ );
  void modeleConstructeur(QString &/* jour1*/ , QString & /*dernierjour*/ ,
			    QTextTableFormat & /*tableFormattableau*/,QTextCursor */*cursortriefonction*/,QStringList & /*listefonction*/ );
    QString m_programName; // nom du programme appelant
    QString m_nomRecupere; // patient name
    QString m_prenomRecupere;//patient surname
    QString m_guidIn;    // uid patient history file
    QString m_tarif;        // price
    QString m_codeTarif;    // price code
    QString m_user;         //user from calling program

  QSqlDatabase m_database;
  QSqlQueryModel *m_model;
  QString m_guidpatient;  //patient trie
  QString m_praticien;    //praticien bénéficière des dus tries
  double  m_sommedus;     //somme des dus tries
  QString m_listeActesDus;//liste des actes dus tries pour ce site, ce praticien, ce debiteur ou ce patient
  QStringList m_listPatientName;//list of patient who have debts.
  private slots:
  void trier();
  void editercourrier();
};

#endif

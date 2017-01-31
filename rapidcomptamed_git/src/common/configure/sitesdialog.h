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
#ifndef SITESDIALOG_H
#define SITESDIALOG_H

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "ui_sitesdialog.h"

//-------------------class destinee a alimenter la table des sites d'activite-----------------------------------
class sites:public QWidget, public Ui::SitesDialog{
  Q_OBJECT
  public:
  sites(QWidget * parent =0);
  ~sites();
  QString preferentialSiteId();
  QSqlDatabase m_databaseUn;
  QSqlDatabase m_database;
  QString m_nomsite;
  QString m_adressesite;
  QString m_codepostal;
  QString m_ville;
  QString m_telsite;
  QString m_faxsite;
  QString m_mailsite;
  QStringList m_listeDesSites;
  QIcon m_smileIcon;

  protected slots:
      void inseredanstablesites();
      void voir();
      void integrer(int);
      void add();
      void del();
      void chooseAsPreferential();
  private:
      void setComboBox();
      QString newsiteid();
      QString m_current_id_site;
      QString m_id;

};

#endif

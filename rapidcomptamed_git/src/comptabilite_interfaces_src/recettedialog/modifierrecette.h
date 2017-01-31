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
#ifndef MODIFIERRECETTE_H
#define MODIFIERRECETTE_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_modifierrecette.h"
#include "recettedialog.h"
#include "customModel.h"
#include "rapidmodel.h"


class modifierrecette : public QWidget, public Ui::ModifierRecette
{
  Q_OBJECT
  private :
    modifierrecette();
    ~modifierrecette();
  public:

  static modifierrecette * instance(){
      if(m_singleton == NULL){
          m_singleton = new modifierrecette;
          qWarning() << " singleton modifierrecettes is created";
          }
      else{
          qWarning() << " singleton modifierrecettes is already created";
          }
      return m_singleton;
      }
      void setDatas(const QString & /*guid*/,const QString &/*user*/,const QString&/*site*/);
      void executeAndShow();
  protected slots:
      void effacer();
      void lancerGestionDus();
      void rechercher();
      void setWhatsThisMode();
      void imprimer();
      void honorairesIsChecked();
      void seancesIsChecked();
      void sortTable(int column,Qt::SortOrder order);
      void changeUser(const QString & text);
  protected:
      QString m_user;
      QString m_site;
      QString m_guid;
      QString m_id_usr;
  private :
      void errorMessage(QString &);
      void  coloringDoubles();
      void paintCororedItems();
      QString headDocumentToPrint();
      QString textOfSums(customModel *);
      QString textOfSums(RapidModelNamespace::RapidModel * model);
      QStringList getUsersDatas();
      QStringList getUsersDatas(const QString & iduserpref);
      QStringList getSitesDatas();
      void getRapidModel(const QString & filter);
      void setTableViewForHonoraires();
      void resetTableViewForHonoraires();
      void setTableViewHeadersForLargeAmounts(RapidModelNamespace::RapidModel * model);
      void prepareTextOfSums();
  //customModel *m_modelHonoraires;
  //customModel *m_modelSeances;
      QSqlDatabase    m_accountancyDb;
      QString m_typeOfMoney;
      customModel *m_modelfiltre;
      QMap<int,QString> m_mapColumns;
      QString  m_documentTotal;
      QStringList m_listechamps;
      QMap<int,QVariant> m_roles;
      static modifierrecette * m_singleton;  
      QVariant m_data;
      int m_row;
      int m_col;
      bool m_test;
      QHash<QString,QString> m_hashsites;
      RapidModelNamespace::RapidModel * m_rapidmodel;
  //QString adapteCheminWindows(QString &);
};

#endif

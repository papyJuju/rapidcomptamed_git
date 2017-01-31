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
#ifndef DEPENSEDIALOG_H
#define DEPENSEDIALOG_H

#include "depensetools.h"
#include "ui_depensedialog.h"
#include <QtGui>
#include <QtCore>
#include <QtSql>

class depensedialog : public QWidget,public Ui::DepenseDialog
{
    Q_OBJECT
    enum TypeSpendigs
    {
        INFLOW=0,
        REDUCED
        };
    enum TypePaiement
    {
        ESPECES=0,
        CHEQUES,
        CARTE,
        PRELEVEMENT,
        VIREMENT
        };
     public:
         depensedialog();
         ~depensedialog();
  
protected :
  void remplirdepensesini();
  QString userAndYearFilter(int id,QString year);
  bool fillCompletionList();
  QString sumsOfMovements();
  
 // void altertable();

signals:


protected slots:
  void enregistredepense();
  //void voirdepenses();
  void effacer();
  void rafraichir();
  void valide();
  void bankDebit();
  void messagebox();
  void whatsthismode();
  void showToolTip(int);
  void loginIsModified(int);
  void fillFileCompletionList(QObject*);
  void on_detailEdit_textChanged(const QString & text);
  void setCompletionList();
  void refreshWithYear(const QString&);
  void analyseYearly();
  void showErrors();
  void showSums(const QModelIndex & topLeft, const QModelIndex & bottomRight );
  void controlIdWithLibelle(const QModelIndex & beginindex, const QModelIndex & endindex);

    private :
        QSqlDatabase   m_accountancyDb;
        QSqlTableModel *m_model;
        QSqlTableModel *m_modele;
        QSqlTableModel *m_mouvdispomodel;
        QStringList m_depensespourcentages;
        int m_id;
        int m_idusr;
        int m_id_usr;
        int m_compte;
        QString m_loginpref;
        QString m_login;
        QStringList m_listelogin;
        QString m_user;
        QHash<QString,QString> m_hashCompte;
        double m_solde_initial;
        QMultiMap<int,QString> m_typeMap , m_spendingsMap;
        QStringList m_completionList;
        QHash<QString,QString> m_hashForMovDips;
        QHash<QString,QString> m_hashIdusrLogin;
        Depensetools * m_depensetools;
        bool m_test;
        QHash<int,QString> hashHeaders();
        QHash<int,QCheckBox*> boxes();
        QString typeofpaiement(int choice);
};

#endif

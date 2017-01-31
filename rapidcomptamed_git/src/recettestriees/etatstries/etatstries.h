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
#ifndef ETATSTRIES_H
#define ETATSTRIES_H

#include <QtGui>
#include <QtSql>
#include "ui_etatstries.h"
#include "etatsconstants.h"


//patient,praticien, date,acte,esp,chq,cb,daf,autre,du
//libelle,date,date_valeur,remarque,montant

class etatstries : public QWidget,public Ui::EtatstriesDialog
{
    Q_OBJECT
  enum recettesParams{
      PATIENT = 0,
      PRATICIEN,
      DATE,
      ACTE,
      ESP,
      CHQ,
      CB,
      VIR,
      DAF,
      AUTRE,
      DU,
      RECETTES_MaxParams
  };
  enum depensesParams{
      LIBELLE = 0,
      DATE_MOUVEMENT,
      DATE_VALEUR,
      REMARQUE,
      MONTANT,
      DEPENSES_MaxParams
  };

  public:
    etatstries(	QString & /*appelsqlchamps*/,
    		QString & /*tablesql*/,
    		int & /*champsaconcatener*/,
    		QString & /*parametrespourtableformat*/,
    		QHash<int,QString> & /*comboitem*/,
    		int /*TABLE*/);
    ~etatstries();
    
    	QSqlDatabase m_db;
	QProgressBar *m_progressbar;
	QSqlQueryModel *m_model;
	QString m_phrasesql;
	QStringList m_listforquery;
	QString m_phrasetable;
	int m_champsamerger;
	QDate m_date;
	QString m_parametrespourtableformat;
   void fermeprogressbar();
   QTextTableFormat monformat(QTextTableFormat& , QString &);
   QString calcul(QString&,QString&,QString&);//fonction de calcul de chauqe types comptable par mois.
   void calculparmois(QStringList & /*liste*/,QTextTable * /*tableau*/, QString & /*datedebut*/, QString & /*datefin*/);//fonction 
   //  deremplissage des types calcules par mois.
   void modele3031(QString & /*mois*/,
                   QString & /*jour1*/ , 
                   QString & /*jour31*/ ,
                   QTextTableFormat & /*tableFormattableau */,
		   QTextCursor * /*cursortriefonction*/,
		   QStringList & /*liste */,
		   int  /*comboIndex*/,
		   int TABLETYPE);

  private slots:
	void imprimer();
	void trier();
	void avancerdate();
	void reculerdate();
  private :
        QString m_idUsr;
        QStringList  m_listOfActs;
        QHash<QString,double>  m_SumUpOfActs;
        QHash<QString,double>  m_SumUpOfType;
        QHash<QString,QString> m_hashLoginUser;
        QHash<QString,QString> m_hashOfType;
        QHash<int,QString> m_hashComboItems;
        int m_TABLETYPE;
        bool showByAct(QString & ,
                       QString & ,
                       QString & ,
                       QTextCursor*);
        bool showByType(QString & ,
                        QString & ,
                        QString & ,
                        QTextCursor*);
        bool showInUsualCase(QString & ,
                             QString & ,
                             QString & ,
                             QString & ,
                             QTextTableFormat & ,
                             QTextCursor*,
                             QStringList &);
        void insertTableOfSumOfActs(QTextCursor*);
        void insertTableOfSumOfTypes(QTextCursor*);
        QTextTableFormat formatFinAnnee();
        void insertEndOfTable(QTextCursor *cursortrieinfonction, const QHash<QString,QString> & hashTypeValues);
        QHash<QString,QString> getHashTypeValues();
        void insertTotalOfTable(QTextCursor *cursortrieinfonction, double value);
        double getTotalValueOfYear();
        
	
};

#endif // ETATSTRIES_H

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

#ifndef RECETTEDIALOG_H
#define RECETTEDIALOG_H

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "ui_recettedialog.h"
#include "modifierrecette.h"
#include "../parametrages/parametrages.h"

class QDialog;
class modifierrecette;


class recettedialog : public QWidget, public Ui::RecetteDialog
{
Q_OBJECT
    enum GetDatasEnum
    {
        PROGRAM_NAME = 0,
        NOM_RECUPERE,
        PRENOM_RECUPERE,        
        };
public:
    recettedialog(QWidget * parent = 0);
    ~recettedialog();

protected :
  double eachActRegistering(QString act, QString value,int factor);
  void message_libre();
  void mousePressEvent(QMouseEvent *);
  QList<double> variousSums(QString & /*strActs*/);
  void controlOfQueries(QString & /*executedQueryResult*/);
  QString priceValueOfAct(QString & ccamAct);
  QString correctNameOfAct(QString & /*Act*/);
  QString findIdPayeur();
  QString findIdSite();
  QStringList findPatientNameGuidForIndependantProgram();

  QMultiMap <int,QString> m_mapIdActesNGAP,
                          m_mapNameActesNGAP,
                          m_mapDescrActesNGAP,
                          m_maptypeActesNGAP,
                          m_mapPriceActesNGAP,
                          m_mapPartPriceActesNGAP;
  QMultiMap <int,QString> m_mapIdThesaurus,
                          m_mapSequenceThesaurus,
                          m_mapTypeThesaurus;
  QMultiMap <int,QString> m_mapIdForfaits,
                          m_mapForfaitsForfaits,
                          m_mapValueForfaits,
                          m_mapTypeForfaits;
  QMultiMap <int,QString> m_mapTypeIK,
                          m_mapValueIK,
                          m_mapAbattementIK,
                          m_mapIndemnitesIK;
  QMap <QString,QString>m_mapRules;

signals:
  //void pressed();
  //void activated();

protected slots:
  void actsRegistering();
  void fermer();
  void fermer2();
  void modifierrecettes();
  void on_lineEdit_textChanged();
  void textenonchange();
  void textechange(const QString&);
  void fonctionplus();
  void fonctionmoins();
  void ccamAndValuesProcess();
  void saveInThesaurus();
  void writeOnButtonAndInHashOfValues(const QString&);
  void freeeditofreceipt(const QString &);
  void clearAll();
  void automatismeOuvert();
  void setWhatsThisMode();
  void changeToolTip(const QString &);
  void rafraichirsitesComboBox();
  void rafraichirpayeurComboBox();
  void redHelp();
  void enregistreSeance();
  void addIK(int);
  void clearResultLabelText();
  void combo_5_toolTip(const QString&);
  void showErrors();
  void putDown(int);
  void modifyListOfValues();
  void percentageChanged(int);

  private :
    int messageThesaurus();
    void rafraichir();
    void messageNomPrenom(QString nom, QString prenom, QString guid);
    bool testNomPrenom(QString nom,QString prenom,QString guid);
    QHash<QString,QString> getValuesFromThesaurus(QStringList & actsList);
    QHash<QString,QString> getValuesFromThesaurus();
    QSqlDatabase m_accountancyDb;
    QSqlQueryModel *m_model;
    QSqlTableModel *m_tableModel;
    QSortFilterProxyModel *m_proxymodel;
    QStringList m_usernames;
    QStringList m_textes;
    QStringList m_listelogin;
    QStringList m_reglesListe;
    QStringList m_pourcentsListe;
    double m_facteurpourcent;//facteur du pourcentage du
    int    m_nbredos;
    int    m_idusr;
    int    m_id;
    int    m_ida;
    int    m_iplusmoins;//fonctionplus et fonctionmoins
    int    m_tempsFermeture;
    QString m_ouinon;//choix de l'ouverture du widget dans comptabilite.ini, par exemple choixouvert=non
    QString m_texte;
    QString m_str;
    QString m_nomprenom;
    QString m_dateOfBirth;
    QString m_guid;
    QString m_date;
    QString m_acteCcam;
    QString m_loginpref;
    QString m_login;
    QString m_praticien;
    QString m_executedquery;
    QString enregistrement(QSqlQuery &);
    QStringList m_listAdress;
    QString m_dataLibreOne;
    QString m_dataLibreTwo;
    QMenu  *m_menu;
    QAction *m_remontrerActeCourant;
    QMainWindow * m_parent;
    QString m_pathabsolu;
    QString m_programName; // nom du programme appelant
    QString m_nomRecupere; // patient name
    QString m_prenomRecupere;//patient surname
    QString m_guidEntre;    // uid patient history file
    QString m_tarif;        // price
    QString m_codeTarif;    // price code
    QString m_user;         //user from calling program
    QHash<QString,QString> m_hashCompte;//hash comptes_bancaires
    QString m_valueForfaits;//valeur forfait choisi
    QString m_forfaitsForfaits;//dénomination forfait choisi
    double m_solde_initial;
    QString m_idHono;
    QString m_actesSeance;
    double m_especeSeance;
    double m_chequeSeance;
    double m_carteBleueSeance;
    double m_virementSeance;
    double m_dafSeance;
    double m_autreSeance;
    double m_duSeance;
    QString m_actesToolTip;
    QMap<QString,QString> m_mapRegleAssociation;
    QHash<QString,QVariant> m_noMessageThesaurusHash;
    QHash<QString,QString> m_ccamAndValuesChosen;//all the values choosened
    QHash<QString,QString> m_hashOfThesaurusActsAndValues;
    bool m_test;
};

#endif

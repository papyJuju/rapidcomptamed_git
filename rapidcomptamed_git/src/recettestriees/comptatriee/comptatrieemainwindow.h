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
#ifndef COMPTATRIEE_H
#define COMPTATRIEE_H

#include <QMainWindow>
#include <QObject>
#include <QMenu>
#include <QAction>
#include <QString>

#include "ui_comptatrieemainwindow.h"

class comptatriee : public QMainWindow, public Ui::ComptatrieeMainWindow
{
    Q_OBJECT
public:
    comptatriee();
    ~comptatriee();

    void createMenus();
    void createActions();
    void connectActions();
    void errorMessage(QString & );
//    QString adapteCheminWindows(QString &);


protected Q_SLOTS:
    void lancerrecettestriees();
    void lancerdepensestriees();
    void lanceretatdesdus();
    void lancerecettes();
    void showBillings();
    void lancedepenses();
    void lanceImmobilisations();
    void lanceGrandLivre();
    void lanceSetup();
    void lanceParametrages();
    void lanceFrenchTax();
    void lanceHelp();
    void showErrors();
    void showBankAndUsers();
    void showBankDeposits();
    void showInputsBank();
    void showNotes();
    void showAmeliWebPage();
    void runParseCcam();
    void messageameli();
    void regenereConfigIni();
    void recupoldbase();
    void sauvegarderbases();
    void exportfiscal();
    void lanceAlertes();


private:
    QMenu *mFile;
    QMenu *mAnalyse;
    QMenu *mInterfaces;
    QMenu *mSettings;
    QMenu *mUpdateCCAM;
    QMenu *mSauvegarde;
    QMenu *mHelp;

    QAction *m_fermer;
    QAction *m_tridesrecettes;
    QAction *m_tridesdepenses;
    QAction *m_etatsdesdus;
    QAction *m_recettes;
    QAction *m_facturation;
    QAction *m_depenses;
    QAction *m_grandLivre;
    QAction *m_immobilisations;
    QAction *m_setup;
    QAction *m_parametres;
    QAction *m_frenchTax;
    QAction *m_bankAndUsers;
    QAction *m_bankDeposits;
    QAction *m_bankInputs;
    QAction *m_notes;
    QAction *m_openUrlAmeli;
    QAction *m_parseCCAM;
    QAction *m_help;
    QAction *m_error;
    QAction *m_regenereconfig;
    QAction *m_recupoldbaseaction;
    QAction *m_sauvegarder;
    QAction *m_exporterfichierpourimpots;
    QAction *m_alertesaction;
};

#endif

#ifndef SAUVEDIALOG_H
#define SAUVEDIALOG_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_sauvedialog.h"
#include <common/configure/medintux/connexion_medintux.h>
#include <common/configure/medintux/constants_medintux.h>

//class recherchechemin;

class sauvedialogue : public QWidget, public Ui::SauveDialog
{
    Q_OBJECT
    enum RythmEnum
    {
        NOAUTO=0,
        VINGTQUATREHEURES,
        DOUZEHEURES,
        SIXHEURES,
        HEURE,
        DIXMINUTES,
        RythmEnumMaxParam 
        };
    enum SavingChoicesEnum
    {
        DRTUXTEST_COMPTA_ALARMES_CHOICE=0,
        COMPTA_CHOICE,
        ALARME_CHOICE,
        DRTUX_CHOICE,
        ALL_CHOICES,
        SavingChoicesEnumMaxParam        
        };
    public:
        sauvedialogue(QWidget *parent = 0);
        ~sauvedialogue();        
    protected slots:
        void fonctionrouge();
        void mafonctionsauvegarde();
        int fonctioncouleur();
        void fermer();
        void trouverchemin();
        void recuperer();
        void searchMysqldump();        
        void readout();
    private:
        bool removeExceeds(); 
        bool checkfilessizes();
        void readerror(QProcess::ProcessError error,QString line);
        QProcess *m_processsauvecompta;
        QProcess *m_processsauvedrtux;
        QProcess *m_processsauveAlarmes;
        QTimer *m_minuteur;
        
        MedintuxConnexionSpace::ConnexionMedintux * m_connexionmedintux;
        
        QHash<int,QString> m_hashsavingchoices;
        
        QString m_chemindump;
        QString m_pass;
        QString m_host;
        QString m_utilisateur;
        QString m_ligne;
        QString m_ligne2;
        QString m_datesauve;
        QStringList m_listechemins;
        QString m_cheminaffiche;
        QString m_medintuxsauveDirPath;
        QString m_cheminafficheplus;
        int m_basesauve;
        QString m_savingpath;

};

#endif

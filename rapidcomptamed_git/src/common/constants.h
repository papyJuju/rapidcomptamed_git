/***************************************************************************
 *   Copyright (C) 2009 by Eric MAEKER, MD.                                *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This code is free and open source .                                   *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#ifndef COMMON_CONSTANTS_H
#define COMMON_CONSTANTS_H

#include "tablesdiagrams.h"
#include <QObject>
#include <QHash>

namespace Common {
/**
  \brief All constants of the project.
  All Settings keys starts with 'S_'.\n
  S_DEF_ for default settings
  A_ pour les actions
  M_ pour les menus
  I_ pour les icons
  All Database name starts with 'DB_'.\n
*/
namespace Constants {

    // Settings
    const char * const  S_FIRSTTIME = "Config/FirstTime";
    const char * const  NO_MORE_THESAURUS_MESSAGE = "Mess_thesaurus/nomoremessage";
    
    const char * const  S_MEDINTUX_CCAM = "Config/CCAMMedintux";

    const char * const  S_APP_LOGIN = "General/Login";
    const char * const  S_APP_PASSWORD = "General/Password";

    const char * const  S_DRTUX_DB_LOGIN = "connect_DB/user";
    const char * const  S_DRTUX_DB_PASSWORD = "connect_DB/password";
    const char * const  S_DRTUX_DB_HOST = "connect_DB/host";
    const char * const  S_DRTUX_DB_PORT = "connect_DB/port";

    const char * const  S_COMPTA_DB_LOGIN = "connect_DB/user";
    const char * const  S_COMPTA_DB_PASSWORD = "connect_DB/password";
    const char * const  S_COMPTA_DB_HOST = "connect_DB/host";
    const char * const  S_COMPTA_DB_PORT = "connect_DB/port";
    const char * const  S_COMPTA_DRIVER = "Database-Accountancy/Driver";
    const char * const  S_COMPTA_NBR_OF_TABLES = "number_of_Tables/nbrTables";
    
    const char * const  S_PATH_TO_MYSQLDUMP = "Database-Accountancy/MysqlDump";
    
    const char * const  S_ACTE_COURANT = "valeur_acte_courant/acte";
    const char * const  S_VALEUR_ACTE_COURANT = "valeur_acte_courant/valeur";
    
    const char * const  S_TYPE_OF_MONEY = "money/name";
    
    const char * const  S_UTILISATEUR_CHOISI = "choix_utilisateur_preferentiel/utilisateur";
    const char * const  S_ID_UTILISATEUR_CHOISI = "choix_utilisateur_preferentiel/id_utilisateur";
    const char * const  S_COMPTE_PREFERENTIEL = "choix_utilisateur_preferentiel/compte_preferentiel";
    
    const char * const S_ID_SITE_PREFERENTIEL = "choix_site_preferentiel/id_site_preferentiel";
    
    const char * const  S_CLOSING_CHOICE = "auto_closing/choixouvert";
    const char * const  S_CLOSING_TIME = "auto_closing/temps";
    
    const char * const  S_REGLE_HABITUELLE = "ccam_rules/habituelle";
    const char * const  S_REGLE_RADIO = "ccam_rules/radiologie";
    const char * const  S_REGLE_TRAUMATO = "ccam_rules/traumatologie";
    
    const char * const  S_POURCENTAGES_DEPENSES = "pourcentages_depenses/taux";
    
    const char * const  X_2035 = "2035_pdf/xpdf";
    const char * const  Y_2035 = "2035_pdf/ypdf";
    
    const char * const  S_EXISTS = "exchange_file_path/exists";
    const char * const  S_EXCHANGE_PATH = "exchange_file_path/path";
    const char * const  CODEC_LINUX = "codec_program/codec_linux";
    const char * const  CODEC_WINDOWS = "codec_program/codec_windows";
    const char * const  TEST = "debug/test";
    
    const char * const  MAX_COLORED_MODEL_ROWS = "modifierrecettes/max_colored_model_rows";
    
    
    //position of widgets
    const char * const X_POSITION = "position/pos_x";
    const char * const Y_POSITION = "position/pos_y";
    const char * const MIN_SIZE_RECETTE = "size/min_size_recettedialog";
    const char * const PREF_SIZE_RECETTE = "size/pref_size_recettedialog";
    
    //program names
    const char * const MEDINTUX = "Medintux";

    // Database's names
    const char * const  DB_DRTUX = "DrTuxTest";
    const char * const  DB_CCAM = "CCAMTest";
    const char * const  DB_ACCOUNTANCY = "rapidecomptabilite";
    const char * const  DB_ALARMES = "alarmes";
    const char * const  DB_FIRST_CONNECTION = "mysql";
    // Driver's names
    const char * const  MYSQL = "QMYSQL";
    const char * const  SQLITE = "QSQLITE";
    const char * const  POSTGRESQL = "QPSQL";
    //config files hidden repertory 
    const char * const  HIDDEN_REPERTORY = ".rapidcomptamednew";
    //tables names
    const char * const TABLE_SEANCES = "seances";
    const char * const TABLE_HONORAIRES = "honoraires";
    const char * const TABLE_PAIEMENTS = "paiements";
    const char * const TABLE_FACTURATION = "facturation";
    const char * const TABLE_UTILISATEURS = "utilisateurs";
    const char * const TABLE_SITES= "sites";
    const char * const TABLE_PAYEURS= "payeurs";
    const char * const TABLE_ALERTES = "alertes";
    
    //valeurs de remboursements
    const char * const REIMBURSMENT_AMO = "pourcentages_AMO_AMC/AMO";
    const char * const REIMBURSMENT_AMC = "pourcentages_AMO_AMC/AMC";
    
    //facturation//////////////////////////
    /////////////////////////////////////
    //statuts
    const char * const NO_VALIDATED = "Non validé";
    const char * const VALIDATED = "Validé";
    const char * const RECORDED = "Enregistré";
    //type payment
    const QString CASH_STR = QObject::tr("Espèce");
    const QString CHECK_STR = QObject::tr("Chèque");
    const QString CREDIT_CARD_STR = QObject::tr("Carte bancaire");
    const QString BANKING_STR = QObject::tr("Virement");
    const QString DAF_STR = QObject::tr("DAF");
    const QString DU_STR = QObject::tr("Du");
    
    ///////////sauvegarde///////////////////////
    //////////////////////////////////////////
    const char * const MAXSAVINGNUMBER = "max_number_of_saved_files/number";
    
    enum PaymentsEnum
    {
        CASH =0,
        CHECK,
        CREDIT_CARD,
        BANKING,
        DAF,
        DU,
        OTHER,
        PaymentsEnum_MaxParam
        };
    enum seances_fields {
      ID_PAIEMENT = 0,
      SEANCES_ID_HONO,
      SEANCES_DATE,
      SEANCES_ID_PAYEURS,
      SEANCES_ID_USR,
      SEANCES_ID_SITE,
      SEANCES_ACTE,
      SEANCES_ESP,
      SEANCES_CHQ,
      SEANCES_CB,
      SEANCES_VIR,
      SEANCES_DAF,
      SEANCES_AUTRE,
      SEANCES_DU,
      SEANCES_VALIDE,
      seance_maxParam
      };
    enum RapidModelEnum
    {
        RM_HONO = 0,
        RM_PATIENT,
        RM_GUID,
        RM_PRATICIEN,
        RM_DATE,
        RM_ACTE,
        RM_REMARQUE,
        RM_ESP,
        RM_CHQ,
        RM_CB,
        RM_VIR,
        RM_DAF,
        RM_DU,
        RM_TRACABILITE,
        RM_DIVERS,
        RapidModelEnum_MaxParam    
        };
    static QHash<int,QString> modifRecettesColumnsHash()
    {
        QHash<int,QString> hash;
        hash.insert(RM_HONO,"id_hono");
        hash.insert(RM_PATIENT,"patient");
        hash.insert(RM_GUID,"GUID");
        hash.insert(RM_PRATICIEN,"praticien");
        hash.insert(RM_DATE,"date");
        hash.insert(RM_ACTE,"acte");
        hash.insert(RM_REMARQUE,"remarque");
        hash.insert(RM_ESP,"esp");
        hash.insert(RM_CHQ,"chq");
        hash.insert(RM_CB,"cb");
        hash.insert(RM_VIR,"vir");
        hash.insert(RM_DAF,"daf");
        hash.insert(RM_DU,"du");
        hash.insert(RM_TRACABILITE,"tracabilite");
        hash.insert(RM_DIVERS,"divers");
        return hash;
        }
    static QHash<int,QString> modifRecettesTitlesColumnsHash()
    {
        QHash<int,QString> hash;
        hash.insert(RM_HONO,QObject::tr("Numéro"));
        hash.insert(RM_PATIENT,"Patient");
        hash.insert(RM_GUID,"GUID");
        hash.insert(RM_PRATICIEN,"Praticien");
        hash.insert(RM_DATE,"Date");
        hash.insert(RM_ACTE,"Acte");
        hash.insert(RM_REMARQUE,"Remarque");
        hash.insert(RM_ESP,QObject::tr("Espèces"));
        hash.insert(RM_CHQ,QObject::tr("Chèques"));
        hash.insert(RM_CB,"Carte Bancaire");
        hash.insert(RM_VIR,"Virement");
        hash.insert(RM_DAF,"DAF");
        hash.insert(RM_DU,"Du");
        hash.insert(RM_TRACABILITE,QObject::tr("Tracabilité"));
        hash.insert(RM_DIVERS,"Divers");
        return hash;
        }
    static QHash<int,QString> tablesHash()
    {
        QHash<int,QString> hash;
        hash.insert(TablesSpace::ACTES_DISPONIBLES,"actes_disponibles" );//TODO
        hash.insert(TablesSpace::CCAM,"ccam" );
        hash.insert(TablesSpace::COMPTES_BANCAIRES,"comptes_bancaires" );
        hash.insert(TablesSpace::DATES_COMPTABLE, "dates_comptable");
        hash.insert(TablesSpace::DEPOTS,"depots" );
        hash.insert(TablesSpace::FACTURATION,"facturation" );
        hash.insert(TablesSpace::FORFAITS,"forfaits" );
        hash.insert(TablesSpace::HEADINGS,"headings" );
        hash.insert(TablesSpace::HONORAIRES,"honoraires" );
        hash.insert(TablesSpace::IMMOBILISATIONS,"immobilisations" );
        hash.insert(TablesSpace::INDEMNITES_DEPLACEMENT,"indemnites_deplacement" );
        hash.insert(TablesSpace::INFORMATIONS,"informations" );
        hash.insert(TablesSpace::MOUVEMENTS,"mouvements" );
        hash.insert(TablesSpace::MOUVEMENTS_DISPONIBLES,"mouvements_disponibles");
        hash.insert(TablesSpace::NAMEINDEX, "nameindex");
        hash.insert(TablesSpace::NOTES,"notes" );
        hash.insert(TablesSpace::PAIEMENTS,"paiements" );
        hash.insert(TablesSpace::PAYEURS,"payeurs" );
        hash.insert(TablesSpace::POURCENTAGES,"pourcentages" );
        hash.insert(TablesSpace::RAPPROCHEMENT_BANCAIRE,"rapprochement_bancaire" );
        hash.insert(TablesSpace::SEANCES,"seances" );
        hash.insert(TablesSpace::SITES,"sites" );
        hash.insert(TablesSpace::THESAURUS,"thesaurus" );
        hash.insert(TablesSpace::UTILISATEURS,"utilisateurs" );
        hash.insert(TablesSpace::Z_VERSION,"z_version" );
        return hash;
        }   
}  // End namespace Constants
}  // End namespace Common

#endif   // COMMON_CONSTANTS_H

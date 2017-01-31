#include "depensestriees.h"

#include <common/icore.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/constants.h>
#include <common/databasemanager.h>

#include <QtGui>
#include <QtCore>

using namespace Common;
using namespace Constants;
using namespace EtatsConstants;

static inline Common::Settings *settings() {return Common::ICore::instance()->settings();}
static inline Common::CommandLine *commandLine() {return Common::ICore::instance()->commandLine();}
static inline Common::DatabaseManager *databaseManager() {return Common::ICore::instance()->databaseManager();}
static inline QString patientUUID() {return commandLine()->value(Common::CommandLine::CL_PatientUUID).toString();}
depensestriees::depensestriees(){
//    databaseManager()->connectDrTux(DatabaseManager::Driver_MySQL);
    databaseManager()->connectAccountancy();
    //databaseManager()->connectAccountancy(DatabaseManager::Driver_MySQL);
    //----------------------------parametrage, les items doivent etre colles aux virgules--------------------------------------------------------------------
    m_appelsqlchamps        = "libelle,date,date_valeur,remarque,montant"; 	// champs appeles
    m_parametrestableformat = "250,150,150,80,80"; 				// largeur des champs de la table
    m_tablesql              = "mouvements";					//table appelee
    m_nombrechamps          =  4; 					//champs a concatener avant especes pour ecrire les mois.
    m_itemspourcombobox.insert(PRATICIEN_INDEX,"Mouvements par mois");
    m_itemspourcombobox.insert(TOUS_PRAT,"Tous mouvements");
    m_itemspourcombobox.insert(ACTEETPRAT,"Par type et praticien");
    m_itemspourcombobox.insert(TOUSACTES,"Tous types");
    //-----------------------------------------------------------------------------------------------------------
    w = new etatstries( m_appelsqlchamps ,m_tablesql,m_nombrechamps,m_parametrestableformat,m_itemspourcombobox,MOUVEMENT_TABLE);
}

depensestriees::~depensestriees(){}

void depensestriees::execRec(){
    w->show();
}

QWidget * depensestriees::widgetdeptrie()
{
    return w;
}

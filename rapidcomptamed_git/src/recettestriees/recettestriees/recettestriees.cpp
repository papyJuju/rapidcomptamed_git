#include "recettestriees.h"

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
recettestriees::recettestriees(){
//    databaseManager()->connectDrTux(DatabaseManager::Driver_MySQL);
    databaseManager()->connectAccountancy();
    //databaseManager()->connectAccountancy(DatabaseManager::Driver_MySQL);
    //---------------------parametrage--------------------------------------------------------------------------------------
    m_appelsqlchamps        = "patient,praticien, date,acte,esp,chq,cb,vir,daf,autre,du"; 	// champs appeles
    m_parametrestableformat = "190,140,80,250,60,60,60,60,60,60,60"; 			// largeur des champs de la table
    m_tablesql              = "honoraires";						//table appelee
    m_nombrechamps          =  4; //champs a concatener avant especes pour ecrire les mois.
    m_itemspourcombobox.insert(PRATICIEN_INDEX,"recettes praticien");
    m_itemspourcombobox.insert(TOUS_PRAT,"tous praticiens");
    m_itemspourcombobox.insert(ACTEETPRAT,"par acte et praticien");
    m_itemspourcombobox.insert(TOUSACTES,"Tous actes de tous praticiens");
    //----------------------------------------------------------------------------------------------------------------------
     w = new etatstries( m_appelsqlchamps ,m_tablesql,m_nombrechamps,m_parametrestableformat,m_itemspourcombobox,RECETTE_TABLE);
    
}

recettestriees::~recettestriees(){}

void recettestriees::execRec(){
  
   w->show();
}

QWidget * recettestriees::widgetrectrie()
{
    return w;
}

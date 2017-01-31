#include "core.h"
#include "settings.h"
#include "databasemanager.h"
#include "commandlineparser.h"
#include "position.h"
#include "configure/util.h"

#include <QMessageBox>
#include <QtDebug>

using namespace Common;

// Ici on va limiter le plus possible la visibilité du singleton global
// Pour s'assurer que personne n'y accède sans passer par notre fonction singleton
namespace Common {
    namespace Internal {
        ICore *m_instance = 0;
    }
}

ICore *ICore::instance()
{
    if (!Internal::m_instance)
        Internal::m_instance = new PrivateCore();
    return Internal::m_instance;
}

// Ici tu initialises les variables de ICore de sorte qu'elles soient uniques
PrivateCore::PrivateCore() :
        m_settings(0), m_db(0),m_pos(0),m_security(0)
{
}

// Tu n'oublies surtout pas de faire le ménage
PrivateCore::~PrivateCore()
{
    if (m_settings) {
        delete m_settings;
        m_settings = 0;
    }
    if (m_db) {
        delete m_db;
        m_db = 0;
    }
    if (m_commandLine){
        delete m_commandLine;
        m_commandLine = 0;
    }
    if(m_pos){
        delete m_pos;
        m_pos = 0;
    }
    if (m_security)
    {
    	  delete m_security;
    	  m_security = 0;
        }
}


bool PrivateCore::initialize()
{
    m_settings = new Settings(this);   // Un seul ICore == UN seul Settings
    m_db = new DatabaseManager(this);  // Idem
    m_commandLine = new CommandLine;
    m_pos = new Position;
    m_security = Common::Security::instance(this);
    return true;
}

Settings *PrivateCore::settings()
{
    return m_settings;
}

DatabaseManager *PrivateCore::databaseManager()
{
    return m_db;
//    if (!m_db->connectDrTux(DatabaseManager::Driver_MySQL)) {
//        QMessageBox::warning(0, tr("Database Error"),
//                             tr("Unable to connect DrTux database."),
//                             QMessageBox::Ok);
//    }
//    if (!m_db->connectAccountancy(DatabaseManager::Driver_SQLite)) {
//        QMessageBox::warning(0, tr("Database Error"),
//                             tr("Unable to connect Accountancy database."),
//                             QMessageBox::Ok);
//    }
}

CommandLine *PrivateCore::commandLine()
{
    return m_commandLine;
}

Position *PrivateCore::position(){
    return m_pos;
}

Security * PrivateCore::security(){
    return m_security;
}

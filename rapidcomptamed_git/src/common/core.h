#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "icore.h"

using namespace Common;

class PrivateCore : public ICore
{
    Q_OBJECT
public:
    PrivateCore();
    ~PrivateCore();

    bool initialize();
    Settings *settings();
    DatabaseManager *databaseManager();
    CommandLine *commandLine();
    Position *position();
    Security * security();

private:
    Settings *m_settings;
    DatabaseManager *m_db;
    CommandLine *m_commandLine;
    Position *m_pos;
    Security * m_security;

};

#endif



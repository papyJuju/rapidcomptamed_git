#include "depensetools.h"
#include <common/constants.h>
#include <common/tablesdiagrams.h>
using namespace Common;
using namespace Constants;
using namespace TablesDiagramsMouvements;

Depensetools::Depensetools(QObject * parent)
{
    Q_UNUSED(parent);
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString table_mouvementsdispo = tablesHash().value(TablesSpace::MOUVEMENTS_DISPONIBLES);
    
    m_modelmouvdispo = new QSqlTableModel(this,m_db);
    m_modelmouvdispo->setTable(table_mouvementsdispo);
    m_modelmouvdispo->select();
}

Depensetools::~Depensetools(){}

QMap<int,QString> Depensetools::getLibelleFromDatabase()
{
    QMap<int,QString> map;
    for ( int row = 0; row < m_modelmouvdispo->rowCount(); ++ row)
    {
          int id = m_modelmouvdispo->data(m_modelmouvdispo->index(row,TablesDiagramsMouvements::ID_MOUVEMENT_DISPO),Qt::DisplayRole).toInt();
          QString libelle = m_modelmouvdispo->data(m_modelmouvdispo->index(row,TablesDiagramsMouvements::LIBELLE_MOUVEMENT_DISPO),Qt::DisplayRole).toString();
          map.insert(id,libelle);
        }
    return map;
}


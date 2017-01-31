#include "tools.h"
#include "../constants.h"


using namespace ToolsSpace;
using namespace Common;
using namespace Constants;

Tools::Tools(QObject * parent){}

Tools::~Tools(){}

QString Tools::modifyCase(const QString & text)
{
    QString minimizedtext;
    //TODO
    return minimizedtext;
}

QHash<int,QString> Tools::getHashOfPaymentsType() const
{
    QHash<int,QString> hash;
    hash.insert(CASH,CASH_STR);
    hash.insert(CHECK,CHECK_STR);
    hash.insert(CREDIT_CARD,CREDIT_CARD_STR);
    hash.insert(BANKING,BANKING_STR);
    hash.insert(DAF,DAF_STR);
    hash.insert(DU,DU_STR);
    return hash;
}

QHash<int,QString> Tools::hashEnumPrograms() const
{
    QHash<int,QString> hash;
    hash.insert(MEDINTUX_TYPE,"Medintux");
    hash.insert(FREEMEDFORMS_TYPE,"FreeMedForms");
    return hash;
}



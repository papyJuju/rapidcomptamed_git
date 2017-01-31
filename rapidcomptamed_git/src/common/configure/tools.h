#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QHash>

namespace ToolsSpace
{
        enum ProgramsEnum
        {
            MEDINTUX_TYPE=0,
            FREEMEDFORMS_TYPE
            };
class Tools:public QObject  
{
    Q_OBJECT
    public:
        Tools (QObject * parent = 0);
        ~Tools();
        QString modifyCase(const QString & text);
        QHash<int,QString> getHashOfPaymentsType() const;
        QHash<int,QString> hashEnumPrograms() const;
    private:
        
};  

};

#endif


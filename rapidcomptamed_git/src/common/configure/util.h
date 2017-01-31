#ifndef UTIL_H
#define UTIL_H

#include <QObject>

namespace Common {
class Security : public QObject
{
    Q_OBJECT
    protected :
        Security(QObject * parent = 0);   
    public :
        ~Security();
        static Security * instance(QObject * parent = 0);
        bool isUserLocked(QString user);
    private :
        static Security * m_singleton;
        
};

};

#endif


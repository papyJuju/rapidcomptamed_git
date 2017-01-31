
#ifndef CONNEXIONMEDINTUX_H
#define CONNEXIONMEDINTUX_H

#include <QObject>

namespace MedintuxConnexionSpace 
{

class ConnexionMedintux : public QObject
{
    Q_OBJECT
    public:
        ConnexionMedintux(QObject *parent = 0);
        ~ConnexionMedintux();
        bool connectToDrTux();
        QString medintuxbasename();
        QString medintuxhost();
        QString medintuxlogin();
        QString medintuxpassword();
        QString medintuxport();
    private:
        void manageSettingsForMysql();
        bool m_IsDBConfigured;
        QString m_base;
    
};

} // End namespace Common

#endif  

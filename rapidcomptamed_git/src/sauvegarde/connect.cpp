#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "connect.h"

using namespace ConnexionsDatasForSauvegarde;

ConnexionsDatas::ConnexionsDatas(QObject * parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    hostEdit->setText("localhost");
    userEdit->setText("root");
    portEdit->setText("3306");
    connect(okButton,  SIGNAL(pressed()),this,SLOT(accept()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(reject()));
}

ConnexionsDatas::~ConnexionsDatas(){}

QString ConnexionsDatas::host()
{
    return hostEdit->text();
}

QString ConnexionsDatas::user()
{
    return userEdit->text();
}

QString ConnexionsDatas::password()
{
    return passEdit->text();
}

int ConnexionsDatas::port()
{
    return portEdit->text().toInt();
}




  

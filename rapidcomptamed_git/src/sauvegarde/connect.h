#ifndef CONNECT_H
#define CONNECT_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_ConnectDialog.h"
namespace ConnexionsDatasForSauvegarde
{
class ConnexionsDatas : public QDialog , public Ui::ConnectDialog{
    Q_OBJECT
    public :
        ConnexionsDatas(QObject * parent = 0);
        ~ConnexionsDatas();
        QString host();
        QString user();
        QString password();
        int port();
};
};

#endif

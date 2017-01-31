#ifndef PASSDIALOG_H
#define PASSDIALOG_H

#include "ui_passdialog.h"

#include <QDialog>

class PassDialog : public QDialog,public Ui::PassDialog
{
    Q_OBJECT
    public :
        PassDialog(QObject * parent = 0, QString login = QString());
        ~PassDialog();
        QString getPassword();
};

#endif


#include "passdialog.h"
#include <QDebug>


PassDialog::PassDialog(QObject * parent,QString login)
{
    setupUi(this);
    QString explanation = "<html><body><font size=3 color=red>Inscrivez votre mot de passe pour "+login+""
                          "</font></body></html>";
    label->setText(explanation);
    label->setWordWrap(true);
}

PassDialog::~PassDialog(){}

QString PassDialog::getPassword()
{
    QString pass;
    pass = lineEdit->text().trimmed();
    return pass;
}



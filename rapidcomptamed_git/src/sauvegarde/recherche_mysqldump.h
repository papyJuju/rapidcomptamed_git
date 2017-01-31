#ifndef RECHERCHE_MYSQLDUMP
#define RECHERCHE_MYSQLDUMP

#include "ui_recherche_mysqldump.h"
#include <QDialog>

class recherchedump : public QDialog, public Ui::DrivesDialog
{
    Q_OBJECT
    public:
        recherchedump(QWidget *parent = 0);
        ~recherchedump();
        QString getPathOfMysqldump();
    protected slots:
        void fonctioninsertion();
};
#endif

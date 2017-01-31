#ifndef BANKMAINWINDOW_H
#define BANKMAINWINDOW_H

#include "ui_mainWindowBank.h"
#include <QMainWindow>


class MainWindowBank : public QMainWindow,public Ui::MainWindowBankMovements
{
    Q_OBJECT
    public :
        MainWindowBank(QWidget * parent = 0);
        ~MainWindowBank();
    private slots:
        void pointMouvements();
        void pointRecettes();
        void depotsEtBalance();
};

#endif


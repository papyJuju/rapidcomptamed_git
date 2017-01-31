#ifndef ANALYSE_H
#define ANALYSE_H

#include "ui_analysedialog.h"
#include <QDialog>

class AnalyseMovements:public QDialog,public Ui::AnalyseDialog
{
    public:
        AnalyseMovements (QWidget * parent = 0);
        virtual ~AnalyseMovements ();
        void getSums(const QString & textOfSums);
    private:
        QString m_text;
        /* data */
};

#endif


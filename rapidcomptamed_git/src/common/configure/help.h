#ifndef HELP_h
#define HELP_H
#include <QtGui>
#include <QtCore>
#include "ui_helpForm.h"
class helpWidget : public QWidget, Ui::helpForm {
    Q_OBJECT
    public :
    helpWidget(QWidget *);
    ~helpWidget();
    };

#endif


#ifndef AUTOMATISME_H
#define AUTOMATISME_H

#include<QtGui>
#include<QtCore>
#include "ui_automatisme.h"
class automatismeDialog : public QWidget,public Ui::AutoDialog{
  Q_OBJECT
  public :
    automatismeDialog();
    ~automatismeDialog();
  protected slots :
    void enregistre();

};

#endif

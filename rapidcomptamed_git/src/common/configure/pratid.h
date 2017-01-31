#ifndef PRATID_H
#define PRATID_H

#include <QtGui>
#include <QtCore>
#include <QtSql>
#include "ui_PratIdDialog.h"

class pratid:public QDialog,public Ui::PratIdDialog{
  Q_OBJECT
  public :
  pratid();
  ~pratid();
  QString recupPrat();
  private :
  QMap <QString,QString> m_map;
  private slots :
  void forLineEdit(const QString &);
};
#endif

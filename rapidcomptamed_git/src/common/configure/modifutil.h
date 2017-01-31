#ifndef MODIFUTIL_H
#define MODIFUTIL_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_modifUtil.h"

class modifutil : public QWidget , public Ui::ModifUtilDialog
{
  Q_OBJECT
public:
  modifutil(QWidget * parent = 0);
  ~modifutil();
protected :
  QSqlDatabase m_dbUtil;
  QSqlTableModel *m_modelUtil;

public slots :
  void enregistrerUtil();
  void effacerLigne();

};

#endif

#ifndef NOUVELLEIMMOBILISATION_H
#define NOUVELLEIMMOBILISATION_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_NouvelleImmobilisation.h"
#include "message.h"

class nouvelleImmobilisation : public QWidget , public Ui::NewImmobDialog {
  Q_OBJECT
public :
  nouvelleImmobilisation();
  ~nouvelleImmobilisation();
  QSqlDatabase    m_db;
  QSqlQueryModel *m_model;
  QSqlQueryModel *m_modele;
  messageBox     *m_message;
  int             m_count;
  int             m_newcount;
  double          m_valResid;
  QString         calculLineaire();
  QString         calculDegressif();
private slots :
  void            enregistrer();
  void            setWhatsThisMode();
  void            changeCombo_3(const QString &);
};

#endif



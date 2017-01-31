#ifndef TABLEAU_H
#define TABLEAU_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_tableaudialog.h"

class tabledialog:public QDialog,public Ui::TableauDialog
{
  Q_OBJECT
  public :
    tabledialog(QSqlTableModel *);
    ~tabledialog();

    QString recupPatient();

  private:
    QSqlTableModel *m_model;
    QSortFilterProxyModel *m_proxymodel;
    QString m_texte;
    void textenonchange();

  private slots :
    void textechange(const QString&);

};

class tableList:public QDialog,public Ui::TableauDialog
{
  Q_OBJECT
  public :
  tableList(QStringList &);
  ~tableList();
  QString findPatientName();
};
  

#endif

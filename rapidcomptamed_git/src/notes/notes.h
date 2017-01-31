#ifndef NOTES_H
#define NOTES_H

#include "ui_Notes.h"
#include <QtGui>
#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>

class Notes : public QWidget, public Ui::NotesWidget{
  Q_OBJECT
  public :
    Notes(QWidget *parent =0);
    ~Notes();
  private :
    bool refreshTextBrowser();
    QSqlDatabase m_accountancyDb;
  private slots :
    void recordAndClose();
};
#endif

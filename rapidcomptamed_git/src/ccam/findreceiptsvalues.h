/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.                  *
 *   pm.desombre@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef  FINDRECEIPTSVALUES_H
#define  FINDRECEIPTSVALUES_H
#include "ui_findvaluesgui.h"

#include <QtGui>
#include <QtCore>
#include <QSqlDatabase>

//using namespace AccountDB;
namespace Ui{
  class findValueDialog;
}
class  FindReceiptsValues:public QDialog
{
  Q_OBJECT
  enum LabelsData
  {
      NAME = 0,
      AMOUNT,
      EXPLANATION,
      OTHERS,
      DATE,
      LabelsData_MaxParam
      };
  enum FatherSon
  {
      FATHER = 0,
      SON
      };
  enum StandardItem
  {
      NAME_ITEM = 0,
      AMOUNT_ITEM,
      DATE_ITEM
      };
public:
    QHash<QString,QString> returnValuesHash();
    FindReceiptsValues(QWidget * parent = 0,QSqlDatabase db = QSqlDatabase());
    ~FindReceiptsValues();
    QHash<QString,QString> getchosenValues();
    void clear();
private:
    Ui::findValueDialog * ui;
    QSqlDatabase m_db;
    double m_modifier;
    QHash<QString,QByteArray> m_hashExplanations;
    QHash<QString,QString> m_otherInformations;
    QHash<QString,QString> m_hashValueschosen;
    QHash<int,QString> m_hashCategories;
    QHash<int,QString> m_hashTables;
    void initialize();
    void fillComboCategories();
    bool tableViewIsFull(QAbstractItemModel * model);
    void enableShowNextTable();
    QHash<QString,QString> getHashFatherSonFromOthers(const QModelIndex & index);
    QStandardItemModel * fillCcamList(int category);
    QStandardItemModel * fillNgapList(int category);
    QStandardItemModel * fillForfaitsList(int category);
    QString lastDateFoundInTable(int category,QString code);
    //void setFlagAccordingToMProcedureCountry();

private Q_SLOTS:
    void fillListViewValues(int);
    void chooseValue();
    void deleteValue();
    void showInformations(const QModelIndex & index);
    void showInformations(const QItemSelection & , const QItemSelection &);
    void on_lineEditFilter_textChanged(const QString & text);
    //void showNext();
    void setModifSpinBox(QWidget*,QWidget*);
    void setModifier(double);
    void wipeFilterEdit(bool b);
    void wipeFilterEdit();
    void setLessButtonEnabled(QListWidgetItem * item);
    void openAmeliFile();
    void writeTheCommandInDoc();
};

#endif

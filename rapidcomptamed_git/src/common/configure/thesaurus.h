#ifndef THESAURUS_H
#define THESAURUS_H

#include "ui_thesaurusWidget.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

class thesaurusModify : public QWidget,public Ui::thesaurusWidget {
    Q_OBJECT
    public :
        thesaurusModify(QWidget * parent =0);
        ~thesaurusModify();
    private slots :
        void addRow();
        void deleteRow();
        void recordAndClose();
    private :
        QSqlDatabase m_db;
        QSqlTableModel * m_model;
};

#endif

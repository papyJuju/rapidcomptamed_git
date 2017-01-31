#ifndef ALERTES_H
#define ALERTES_H

#include "ui_alertes.h"

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>


class Alertes:public QWidget,public Ui::AlertesWidget
{
	Q_OBJECT
	public:
		Alertes (QWidget * parent = 0);
		~Alertes ();
        protected slots:
                void recordAlerte();
                void deleteAlerte();
                void fillTable(const QString& user);
	private:
	        void fillUsersBox();
	        QString findUserPref();
	        QSqlDatabase m_db;
	        QSqlTableModel * m_model;
			
};

#endif

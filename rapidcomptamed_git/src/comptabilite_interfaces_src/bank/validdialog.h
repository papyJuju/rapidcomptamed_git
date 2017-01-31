#ifndef VALIDDIALOG_H
#define VALIDDIALOG_H

#include "ui_validDialog.h"
#include <QDialog>
#include <QCheckBox>
#include <QDate>

class ValidDialog : public QDialog, public Ui::ValidDialog
{
	public:
		ValidDialog (QWidget * parent=0 , QDate begin = QDate(), QDate end = QDate());
		~ValidDialog ();
		QList<int> checksBoxesReturn();
		
	enum boxes{
		    CASHBOX = 0,
		    CHECKSBOX,
		    VISABOX,
		    OTHERSBOX,
		    boxes_maxParams
		};
		QHash<int,QCheckBox*> m_hashCheckBoxes;
};

#endif


#include "validdialog.h"
#include <QDebug>


ValidDialog::ValidDialog(QWidget * parent,QDate begin,QDate end){
    Q_UNUSED(parent);
    setupUi(this);
    const QString beginDateStr = begin.toString("dd/MM/yyyy");
    const QString endDateStr = end.toString("dd/MM/yyyy");
    const QString explicativeText = 
      "<html><body><font color = red size = 3>"
      ""+trUtf8("Pour valider ces versements effectués\n"
      "entre le ")+beginDateStr+trUtf8(" et le ")+endDateStr+trUtf8("\n"
      "veuillez cocher les types de versement")+""
      " </font></body></html>";
    label->setText(explicativeText);
    label->setWordWrap(true);
    m_hashCheckBoxes.insert(CASHBOX,cashBox);
    m_hashCheckBoxes.insert(CHECKSBOX,checksBox);
    m_hashCheckBoxes.insert(VISABOX,cardsBox);
    m_hashCheckBoxes.insert(OTHERSBOX,othersBox);
}

ValidDialog::~ValidDialog(){}

QList<int> ValidDialog::checksBoxesReturn(){
   QList<int> list;
   for (int i = 0; i < boxes_maxParams; i += 1)
   {
   	if (m_hashCheckBoxes.value(i)->isChecked())
   	{
   		  list << i;
   	    }
       }
   return list;
}


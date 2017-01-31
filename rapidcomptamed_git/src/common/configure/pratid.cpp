#include "pratid.h"
#include <QtGui>
#include <QtCore>
#include <QtSql>
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>

using namespace Common;
using namespace Constants;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

pratid::pratid(){
  QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  setupUi(this);
  QString req = QString("SELECT %1,%2,%3 FROM %4").arg("id_usr","nom_usr","login","utilisateurs");
  QSqlQuery q(db);
  if(!q.exec(req)){
      qWarning() << __FILE__ << QString::number(__LINE__) << " " << q.lastError().text();
  }
  QStringList list;
  while(q.next()){
      QString s0 = q.value(0).toString();
      QString s1 = q.value(1).toString();
      QString s2 = q.value(2).toString();
      QString join = s1+","+s2;
      list << s0;
      m_map.insert(s0,join);
  }
  QString idpref = settings()->value(S_ID_UTILISATEUR_CHOISI).toString();
  list.prepend(idpref);
  list.removeDuplicates();
  comboBox->setEditable(false);
  comboBox->setInsertPolicy(QComboBox::NoInsert);
  comboBox->addItems(list);
  forLineEdit(comboBox->currentText());
  connect(comboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(forLineEdit(const QString &)));
}

pratid::~pratid(){}

QString pratid::recupPrat(){
  QString prat = comboBox->currentText();
  close();
  return prat;
}

void pratid::forLineEdit(const QString & combo){
  lineEdit->setText(m_map.value(combo));
}

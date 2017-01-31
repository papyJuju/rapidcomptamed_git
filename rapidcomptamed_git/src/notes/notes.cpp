#include "notes.h"
#include <common/icore.h>
#include <common/constants.h>
#include <common/position.h>
#include <QSqlError>

using namespace Common;
using namespace Constants;
static inline Common::Position *position() {return Common::ICore::instance()->position();}
Notes::Notes(QWidget *parent):QWidget(parent){
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  setAttribute( Qt::WA_AlwaysShowToolTips);
  position()->centralPosition(this,width(),height());
  recordButton-> setShortcut(QKeySequence::InsertParagraphSeparator);
  m_accountancyDb = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);//comptabilite
  textBrowser->setReadOnly(false);
  if (!refreshTextBrowser())
  {
  	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Ne peut pas prendre les notes ! Si c'est la première installation, "
  	  "lancez recettes pour compléter la base."),QMessageBox::Ok);
      }
  connect(recordButton,SIGNAL(pressed()),this,SLOT(recordAndClose()));
}

Notes::~Notes()
{
    recordAndClose();
}

void Notes::recordAndClose(){
  QString text;
  text = textBrowser->toPlainText();
  //text.replace("é","&#233;");
  
  qDebug() << __FILE__ << QString::number(__LINE__) << " text =" << text ;
  QSqlQuery q(m_accountancyDb);
  QString record = QString("INSERT INTO %1(%2) VALUES('%3')").arg("notes","notes",text);
  if (!q.exec(record))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() << "\n" << q.lastQuery();
      }
  //close();
}

bool Notes::refreshTextBrowser(){
    bool ret = true;
    QString text;
    QSqlQuery q(m_accountancyDb);
    QString req = QString("SELECT notes FROM notes");
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
    	  ret = false;
    	  
        }
    while (q.next())
    {
    	text = q.value(0).toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " text in while  =" << text ;
        }
    text.replace("é","&#233;");
    text.replace("\n","<br/>");
    QTextDocument *d = new QTextDocument;
    d->setHtml("<html><body>"+text+"</body></html>");
    qDebug() << __FILE__ << QString::number(__LINE__) << "text"<< text ;
    //text.replace("&#130;","é");
    textBrowser->setDocument(d);
    return ret;
}

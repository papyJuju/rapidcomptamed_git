#include "help.h"
#include <common/icore.h>
#include <common/settings.h>
#include <common/constants.h>
#include <common/position.h>
using namespace Common;
using namespace Constants;
static inline Common::Position *position() {return Common::ICore::instance()->position();}
static inline QString docPath() {return Common::ICore::instance()->settings()->applicationResourcesPath()+"/doc";}
static inline QString resourcesPath() {return Common::ICore::instance()->settings()->applicationResourcesPath();}
helpWidget::helpWidget(QWidget *parent){
  qDebug() << __FILE__;
  setupUi(this);
  qDebug() << __FILE__;
  position()->centralPosition(this,width(),height());
  QString htmlText;
  QFile file(docPath()+"/documentation.html");
  if(!file.open(QIODevice::ReadOnly)){
      qWarning() << __FILE__ << " " << QString::number(__LINE__) << " " << docPath()+"/documentation.html"+" cannot open";
      }
  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  while(!stream.atEnd()){
      QString br = "<br/>";
      QString str = stream.readLine();
      if(!str.contains("</p>")){
          str += br;
      }
      if(str.contains("<img")){
          str.replace("""../resources",resourcesPath()+"");
          }
      htmlText += str;
      }
  htmlText.replace("\n","<br/>");
  //textBrowser->setScrollAnchorMove();
  textBrowser->setHtml(htmlText);
  pushButton->setText(trUtf8("Fermer"));
  pushButton->setShortcut(QKeySequence::InsertParagraphSeparator);
  connect(pushButton,SIGNAL(pressed()),this,SLOT(close()));
}

helpWidget::~helpWidget(){}

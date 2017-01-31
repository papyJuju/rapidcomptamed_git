#include "position.h"
#include "constants.h"
#include "settings.h"
#include "icore.h"
#include <QtGui>
#include <QtCore>

using namespace Common;
using namespace Constants;
Position::Position(){

}

Position::~Position(){}

void Position::centralPosition(QWidget * w ,int width, int height){
  int varX = Common::ICore::instance()->settings()->value(Constants::X_POSITION).toInt();
  int varY = Common::ICore::instance()->settings()->value(Constants::Y_POSITION).toInt();
  QDesktopWidget *desktopwidget = new QDesktopWidget;
  QRect rect                    = desktopwidget->availableGeometry(desktopwidget);
  int desktopHeight = rect.height();
  int desktopWidth  = rect.width();
  int posX = int((desktopWidth - width)/2)-varX;
  int posY = int((desktopHeight - height)/2)-varY;
  
  w -> move(posX,posY);
}


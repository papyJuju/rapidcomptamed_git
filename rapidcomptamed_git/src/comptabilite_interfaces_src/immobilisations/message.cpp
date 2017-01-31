#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QDesktopWidget>


#include "message.h"

messageBox::messageBox(QString & titre,QString & corps){
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::WindowStaysOnTopHint);
  QString title                  =  titre;
  QString messageCorps           =  "<html><body><font size = 3 color = #CC0000 ><bold>"+corps+"<bold>"
                                    "</font></body></html>";
  QDesktopWidget *desktopwidget  = new QDesktopWidget;
  QRect rect                     = desktopwidget->availableGeometry(this);
  int bottomrightx               = rect.bottomRight().x();
  int bottomrighty               = rect.bottomRight().y();
  int w = 300;
  int h = 200;
  setGeometry(QRect(bottomrightx/2 - w/2 , bottomrighty/2 -h/2 , w, h));
  setWindowTitle(title);
  setMinimumSize ( w , h); 
  
  QIcon icon("deb");
  label                          = new QLabel;
  label                         -> setText(messageCorps);
  label                         ->setMinimumSize(278,150);
  /*bool wordwrap = label->textFormat() == Qt::RichText
                    || (label->textFormat() == Qt::AutoText);*/
    label->setWordWrap(true);
  button                         = new QPushButton(icon,"Ok");
  button                        -> setAutoDefault(true);
  
  grid                          =  new QGridLayout;
  grid                          -> addWidget(label,1,1,1,1,Qt::AlignCenter);
  grid                          -> addWidget(button,2,1,2,1);
  setLayout                       (grid);
  adjustSize();

  connect(button,SIGNAL(pressed()),this,SLOT(close()));

}

messageBox::~messageBox(){
     title                  =  "";
     messageCorps           =  "";
     delete button;
     delete grid;
     delete label;
  }

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDialog>


class messageBox : public QDialog
{

public :
  messageBox(QString &,QString &);
  ~messageBox();
  QDialog *message;
  QString title;
  QString messageCorps;
  QLabel *label;
  QPushButton *button;
  QGridLayout *grid;
  QSize sizeHint();

};

#endif

#ifndef FINDFILE_H
#define FINDFILE_H

#include <QtGui>
#include <QtCore>
namespace CCAMfindSpace{
class FindFile : public QDialog{
  Q_OBJECT
  public :
     FindFile(QObject * object,int fileType);
     ~FindFile();
     QString fileRecord();
     QString findFilePath();
     QDialog *dialog();
  private Q_SLOTS :
    void recupere(const QModelIndex &);
  private :
    QDialog          *m_dialog;
    QTreeView        *m_view ;
    QFileSystemModel *m_sys  ;
    QPushButton      *m_pushButton;
    QPushButton      *m_cancelButton;
    QGridLayout      *m_grid;
    QString           m_fileName;
    QString           m_filePath;
};
};

#endif

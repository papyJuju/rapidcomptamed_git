#ifndef FINDFILESAUVE_H
#define FINDFILESAUVE_H

#include <QDialog>
#include <QTreeView>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QPushButton>

namespace SauvegardeSpace{

class findFileSauve : public QDialog{
    Q_OBJECT
    public :
        findFileSauve(QWidget * parent = 0, QString nameFilter = QString());
        virtual ~findFileSauve();
         QString fileRecord();
         QString findFileSauvePath();
    private slots :
        void recupere(const QModelIndex &);
    private :
        QStringList findpathwithfilter(const QString & filter);
        QTreeView        *m_view ;
        QFileSystemModel *m_sys  ;
        QPushButton      *m_quitButton;
        QPushButton      *m_okButton;
        QGridLayout      *m_grid;
        QString           m_fileName;
        QString           m_filePath;
};
};

#endif

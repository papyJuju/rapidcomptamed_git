#include "findFileSauve.h"
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>



using namespace SauvegardeSpace;

findFileSauve::findFileSauve(QWidget *parent, QString nameFilter ) : QDialog(parent){
    setParent(parent);
    m_view = new QTreeView(this);
    m_view-> setGeometry(10,10,500,500);
    m_view->setMinimumSize(500,500);
    m_sys  = new QFileSystemModel;
    m_sys -> setRootPath(QDir::rootPath());
    m_sys->sort(0);
    m_view-> setModel(m_sys);
    m_view->setSortingEnabled(true);    
    m_view->header()->setSortIndicator(0,Qt::AscendingOrder);
    m_view->header()->setMinimumSectionSize ( 300 );
    m_view->header()->resizeSection(0,300);
    m_view->header()->setResizeMode(QHeaderView::Interactive);
    QModelIndex index;
    index = m_sys->index(0,0,QModelIndex());
    m_view-> setExpanded(index,true);
    m_quitButton = new QPushButton;
    m_quitButton-> setMinimumSize(50,30);
    m_quitButton-> setText(trUtf8("&Quitter"));
    m_okButton = new QPushButton;
    m_okButton-> setMinimumSize(50,30);
    m_okButton->setText(tr("&Choisir"));
    m_grid = new QGridLayout;
    m_grid-> addWidget(m_view,0,0,1,3);
    m_grid-> addWidget (m_quitButton,1,1,1,1);
    m_grid-> addWidget (m_okButton,2,1,1,1);  
    setLayout(m_grid);
    connect(m_view,SIGNAL(pressed(const QModelIndex &)),this,SLOT(recupere(const QModelIndex &)));
    connect(m_quitButton,SIGNAL(pressed()),this,SLOT(reject()));
    connect(m_okButton,SIGNAL(pressed()),this,SLOT(accept()));    
}

findFileSauve::~findFileSauve(){
    delete m_grid;
}

void findFileSauve::recupere(const QModelIndex &index){
    m_fileName = m_sys->fileName(index);
    m_filePath = m_sys->filePath(index);
    qDebug() << __FILE__ << QString::number(__LINE__) <<  "m_filePath = "+m_filePath;
    //emit accept();
    QMessageBox::information(0,trUtf8("Information"),trUtf8("Le dossier ")+m_fileName+trUtf8(" a été récupéré"),QMessageBox::Ok);
  //emit close();
}

QString findFileSauve::fileRecord()
{
    return m_fileName;
}

QString findFileSauve::findFileSauvePath(){
  return m_filePath;
}

/*QStringList findFileSauve::findpathwithfilter(const QString & filter)
{
    QStringList pathlist;
    QModelIndex parent = m_sys->index(0,0);
    qDebug() << __FILE__ << QString::number(__LINE__) << " fileName =" << m_sys->fileName(parent) ;
    m_view-> setExpanded(parent,true);
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_sys->rowCount(parent) =" << QString::number(m_sys->rowCount(parent));
    for ( int row = 0; row < m_sys->rowCount(parent); ++ row)
    {
          QModelIndex nextparent = m_sys->index(row,0,parent); 
          int rowofparent = 0;         
          while (m_sys->hasChildren(nextparent))
          {
              m_view-> setExpanded(nextparent,true);
              QString datafile = m_sys->data(m_sys->index(rowofparent,0,nextparent),Qt::DisplayRole).toString();
              //qDebug() << __FILE__ << QString::number(__LINE__) << " datafile =" << datafile ;
              if (datafile.contains(filter))
              {
                  pathlist << datafile;
                  }
              //nextparent = m_sys->index(rowofparent,0,nextparent);
              ++rowofparent;
              }

        }
    return pathlist;
}*/

QStringList findFileSauve::findpathwithfilter(const QString & filter)
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    QStringList list;
    QStringList listmajorsdirs;
    QDir rootdir("/");
    listmajorsdirs = rootdir.entryList();
    foreach(QString majordir,listmajorsdirs){
        if (majordir!="." && majordir!="..")
        {
            //qDebug() << __FILE__ << QString::number(__LINE__) << " root name =" << rootdir.dirName() ;
            //qDebug() << __FILE__ << QString::number(__LINE__) << " majordir =" << rootdir.dirName()+QDir::separator()+majordir ;
            QDirIterator it(rootdir.dirName()+QDir::separator()+majordir,QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
            while (it.hasNext())
            {
                //it.next();
                //qDebug() << __FILE__ << QString::number(__LINE__) << " result =" << it.next() ;
                
                if (it.filePath().contains(filter))
                {
                      qDebug() << __FILE__ << QString::number(__LINE__) << "it.filePath()  =" << it.filePath() ;
                      list << it.filePath();
                    }
                } 
            }

        }
    QApplication::restoreOverrideCursor();
    qDebug() << __FILE__ << QString::number(__LINE__) << "list[0]  =" << list[0] ;
    return list;
}



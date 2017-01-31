#include <QtGui>
#include <QtCore>

#include "findFile.h"
#include "parseconstants.h"

using namespace ParseConstants;
using namespace CCAMfindSpace;

FindFile::FindFile(QObject *parent,int fileType) {
  //setParent(parent);
  QString textsDir;
  switch(fileType){
      case XLS :
          textsDir = QString(qApp->applicationDirPath()+"/../resources/ameli/");
          break;
      case SOFFICE :
          textsDir = QString(qApp->applicationDirPath());
          break;
      default :
          break;    
      }  
  qDebug() << __FILE__ << QString::number(__LINE__) << " textsDir =" << textsDir ;
  m_dialog = new QDialog;
  m_view = new QTreeView(this);
  m_view-> setGeometry(10,10,500,500);
  m_view->setMinimumSize(500,500);
  m_view->header()->setResizeMode(QHeaderView::Interactive);
  m_view->header()->resizeSection(0,200);
  m_sys  = new QFileSystemModel;
  QModelIndex index = m_sys -> setRootPath(QDir::currentPath());
  m_view-> setModel(m_sys);
  const QString home = QDir::homePath();
  QModelIndex indexRoot = m_sys->index(0,0,QModelIndex());
  QModelIndex indexHome = m_sys->index(home);
  QModelIndex indexTexts = m_sys->index(textsDir);
  QModelIndex indexSearch = indexRoot;
  //TODO trouver le .csv et exclure le new et le xls
  while (indexSearch != indexTexts)
  {
  	m_view-> setExpanded(indexSearch,true);
  	indexSearch = m_view->indexBelow(indexSearch);
      }
  m_view-> setExpanded(indexTexts,true);
  m_view->setColumnHidden(1,true);
  m_view->setColumnHidden(2,true);
  m_view->setColumnHidden(3,true);
  m_pushButton = new QPushButton;
  m_pushButton-> setMinimumSize(50,30);
  m_pushButton-> setText(tr("&Ok"));
  m_pushButton->setEnabled(true);
  m_cancelButton = new QPushButton;
  m_cancelButton-> setMinimumSize(50,30);
  m_cancelButton-> setText(tr("&Cancel"));
  m_cancelButton->setEnabled(true);
  m_grid = new QGridLayout(m_dialog);
  m_grid-> addWidget(m_view,0,0,1,3);
  m_grid-> addWidget (m_pushButton,1,0,1,1);
  m_grid-> addWidget (m_cancelButton,1,1,1,1);
  m_dialog->setLayout(m_grid);
  connect(m_view,SIGNAL(clicked(const QModelIndex &)),this,SLOT(recupere(const QModelIndex &)));
  connect(m_pushButton,SIGNAL(pressed()),m_dialog,SLOT(accept()));
  connect(m_cancelButton,SIGNAL(pressed()),m_dialog,SLOT(reject()));
}

FindFile::~FindFile(){
  delete m_dialog;
  delete m_grid;
}

void FindFile::recupere(const QModelIndex &index){
  m_fileName = m_sys->fileName(index);
  m_filePath = m_sys->filePath(index);
  if (m_fileName.contains(".xls") && !m_fileName.contains(".csv"))
  {
      QFile readmefile(qApp->applicationDirPath()+"/../resources/ameli/doc/readme");
      if (!readmefile.open(QIODevice::ReadOnly))
      {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file readme" ;
          }
      QString text;
      QTextStream stream(&readmefile);
      text = stream.readAll();
      qDebug() << __FILE__ << QString::number(__LINE__) << text ;
      QByteArray array = text.toUtf8();
      const char * textchar = array.data();
      QString finetext = tr(textchar);
      QMessageBox::warning(0,tr("Attention"),finetext,QMessageBox::Ok);
      m_pushButton->setEnabled(false);
      return;
      }
  qDebug() << __FILE__ << QString::number(__LINE__) << " m_fileName =" << m_fileName ;
  qDebug() << "m_filePath = "+m_filePath;
}

/*void FindFile::openDir()
{
    QDir dir(m_filePath);
    if (dir.exists())
    {
        QMessageBox::warning(0,tr("Erreur"),m_filePath+tr(" inconnu."),QMessageBox::Ok);
        return;
        }
     QFileDialog::
}*/

QString FindFile::fileRecord(){
  return m_fileName;
 }

QString FindFile::findFilePath(){
  return m_filePath;
}

QDialog *FindFile::dialog(){
    return m_dialog;
}

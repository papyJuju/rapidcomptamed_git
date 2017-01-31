#include "configdatas.h"
#include <common/settings.h>
#include <common/icore.h>


#include <QDebug>
#include <QMessageBox>

using namespace Common;
using namespace ConfigSpace;

static inline Common::Settings * settings(){return Common::ICore::instance()->settings(); }

ConfigDatas::ConfigDatas(QWidget * parent):ui(new Ui::ConfigWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_frame = new QFrame(ui->scrollArea);
    m_layout = new QGridLayout(m_frame);    
    m_listofconfigkeys = settings()->allKeys();
    for ( int i = 0; i < m_listofconfigkeys.size(); ++ i)
    {          
          QString text = m_listofconfigkeys[i];
          QString value = settings()->value(text).toString();
          m_layout->addWidget(new QLabel(text,m_frame),i,0);
          m_layout->addWidget(new QLineEdit(value,m_frame),i,1); 
        }        
    m_frame->resize(ui->scrollArea->width(),30*m_listofconfigkeys.size());    
    m_frame->setLayout(m_layout); 
    ui->scrollArea->setWidget(m_frame);
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->okButton,SIGNAL(pressed()),this,SLOT(record()));  
}

ConfigDatas::~ConfigDatas()
{
}

void ConfigDatas::record()
{
    for ( int i = 0; i < m_listofconfigkeys.size(); ++ i)
    {
          QLayoutItem * item = m_layout->itemAtPosition(i,1);
          QString key = m_listofconfigkeys[i];
          QString value = qobject_cast<QLineEdit*>(item->widget())->text();
          settings()->setValue(key,value);
        }
    QMessageBox::information(0,tr("Information"),tr("Les valeurs ont été intégrées."),QMessageBox::Ok);
}



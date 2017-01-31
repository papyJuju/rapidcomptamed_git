#include "sitesdialog.h"
#include "vision.h"

#include <common/constants.h>
#include <common/settings.h>
#include <common/icore.h>
#include <common/tablesdiagrams.h>

#include <QtCore>
#include <QtGui>
#include <QtSql>

using namespace Common;
using namespace Constants;
using namespace SitesDiagrams;

static inline Common::Settings *settings() {return Common::ICore::instance()->settings();}

sites::sites(QWidget *parent){
  setupUi(this);
  setAttribute    ( Qt::WA_DeleteOnClose);
  QPoint parentPosition = parent->parentWidget()->pos();
  QPoint plusPoint(50,50);
  move(parentPosition+plusPoint);
  //setWindowModality ( Qt::ApplicationModal );
  //m_databaseUn    = QSqlDatabase::database(Constants::DB_DRTUX);
  m_database	  = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  m_nomsite 	  = "";
  m_adressesite   = "";
  m_codepostal	  = "";
  m_ville	  = "";
  m_telsite	  = "";
  m_faxsite	  = "";
  m_mailsite	  = "";
  m_id = "";
  recordButton -> setEnabled(false);
  //---------comboBox des sites----------------------------

  QStringList adresses;
  m_smileIcon . addFile(settings()->iconPath()+"/face-smile.png");
  comboBox                 -> setEditable(false);
  comboBox                 -> setInsertPolicy(QComboBox::InsertAtTop);
  setComboBox();

   if(m_listeDesSites.size() == 0)
   {
       //QIcon smileNo(settings()->iconPath()+"/face-crying.png");
       QString nomPrenom = tr("Pas de site créé");
       m_current_id_site = "0";
       m_listeDesSites << nomPrenom;
       comboBox        -> addItem(QIcon() , nomPrenom , QVariant());
       }
    integrer(0);

  connect(recordButton,SIGNAL(pressed())       ,this,SLOT(inseredanstablesites()));
  connect(closeButton,SIGNAL(pressed())       ,this,SLOT(close()));
  connect(modifyButton,SIGNAL(pressed())       ,this,SLOT(voir()));
  connect(comboBox ,SIGNAL(highlighted(int)),this,SLOT(integrer(int)));
  connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(integrer(int)));  
  connect(addButton,SIGNAL(pressed()),this,SLOT(add()));
  connect(delButton,SIGNAL(pressed()),this,SLOT(del())); 
  connect(preferedButton,SIGNAL(pressed()),this,SLOT(chooseAsPreferential()));  
}

sites::~sites(){
  m_listeDesSites               .clear();
}

void sites::inseredanstablesites(){
  m_nomsite 	= nameEdit->text();
  m_adressesite = textEdit->toPlainText();
  m_codepostal	= cpEdit->text();
  m_ville	= townEdit->text();
  m_telsite	= phoneEdit->text();
  m_faxsite	= faxEdit->text();
  m_mailsite	= mailEdit->text();
  m_nomsite 	.replace("'","''");
  m_adressesite .replace("'","''");
  m_ville	.replace("'","''");
  m_mailsite	.replace("'","''");
  m_id = newsiteid();
  qDebug() << __FILE__ << QString::number(__LINE__) << " m_id =" << m_id ;
  QString requeteSites = QString("INSERT INTO sites (id_site,site,adresse_site,cp_site,ville_site,telsite,faxsite,mailsite)"
		        "VALUES("
		    "'"+m_id+"',"
			"'"+m_nomsite+"',"
			"'"+m_adressesite+"',"
			"'"+m_codepostal+"',"
			"'"+m_ville+"',"
			"'"+m_telsite+"',"
			"'"+m_faxsite+"',"
			"'"+m_mailsite+"')");
  QSqlQuery query(m_database);
  if (!query.exec(requeteSites))
  {
        qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
      }
		if(query.numRowsAffected()== 1){
			QMessageBox::information(0,tr("information"),trUtf8("Les informations\nont été intégrées."),QMessageBox::Ok);
			//emit close();
			}
		else {
			QMessageBox::warning(0,tr("attention"),trUtf8("Echec insertion !"),QMessageBox::Ok);
			}
	recordButton -> setEnabled(false);
	setComboBox();
}

void sites::voir(){
  QString         base         = "sites";
  QList<int>      listtohide         ;
                  listtohide        << ID_PRIMKEY << ID_SITE << DIVERS;
  vision         *v            = new vision(base , listtohide,DIVERS,this);
                  v           -> show();
}

void sites::integrer(int index)
{
  Q_UNUSED(index);
  QString site = comboBox->currentText();
  QSqlQuery  querySites(m_database);
  QString    requeteSites   = "SELECT ID_Primkey, id_site, site , adresse_site , cp_site , ville_site, telsite , faxsite , mailsite FROM sites";
  requeteSites += QString(" where %1 = '%2'").arg("site",site);
    if (!querySites.exec(requeteSites))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << querySites.lastError().text() ;
        }
    while(querySites.next()){
        m_current_id_site = querySites.value(ID_SITE).toString();
        QString lineSite    = querySites.value(SITE).toString();
        nameEdit -> setText(lineSite);
        QString lineAdr     = querySites.value(ADRESSE_SITE).toString();
        textEdit -> setPlainText(lineAdr);
        QString lineCP      = querySites.value(CP_SITE).toString();
        cpEdit -> setText(lineCP);
        QString lineVille   = querySites.value(VILLE_SITE).toString();
        townEdit -> setText(lineVille);
        QString lineTel = querySites.value(TELSITE).toString();
        phoneEdit -> setText(lineTel);
        QString lineFax     = querySites.value(FAXSITE).toString();
        faxEdit -> setText(lineFax);
        QString lineMail    = querySites.value(MAILSITE).toString();
        mailEdit     -> setText(lineMail);
        }
    if (site.contains("Docteur Administrateur"))
    {
        nameEdit->setText("");
        textEdit->setPlainText("");
        cpEdit->setText("");
        townEdit->setText("");
        phoneEdit->setText("");
        faxEdit->setText("");
        mailEdit->setText("");
        }
}

void sites::add()
{
    comboBox -> setEditable(true);
    comboBox->setItemText(0,"");
    comboBox->setItemIcon(0,QIcon());
    nameEdit->setText("");
    textEdit->setPlainText("");
    cpEdit->setText("");
    townEdit->setText("");
    phoneEdit->setText("");
    faxEdit->setText("");
    mailEdit->setText("");
    nameEdit->setFocus();
    comboBox -> setEditable(false);
    recordButton -> setEnabled(true);
}

void sites::del()
{
    voir();
    setComboBox();
}

void sites::setComboBox()
{
    enum Combo {SITE_COMBO=0,IDSITE_COMBO};
    if (comboBox->count()>0)
    {
          comboBox->clear();
        }
    QString prefId = preferentialSiteId();
    QSqlQuery qy(m_database);
    QString req = QString("select %1,%2 from %3").arg("site","id_site","sites");
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        QIcon icon = QIcon();
        QString site = qy.value(SITE_COMBO).toString();
        QString idsite = qy.value(IDSITE_COMBO).toString();
        if (idsite==prefId)
        {
              icon = m_smileIcon ;
            }
        m_listeDesSites << site;
        comboBox->addItem(icon,site,QVariant());
        }
}

void sites::chooseAsPreferential()
{
    settings()->setValue(S_ID_SITE_PREFERENTIEL,m_current_id_site);
    setComboBox();
}

QString sites::preferentialSiteId()
{
    return settings()->value(S_ID_SITE_PREFERENTIEL).toString();
}

QString sites::newsiteid()
{
    return QUuid::createUuid ();
}

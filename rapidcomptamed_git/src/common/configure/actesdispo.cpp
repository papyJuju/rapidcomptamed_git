#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <common/icore.h>
#include <common/settings.h>
#include "actesdispo.h"
#include "../constants.h"
#include <common/tablesdiagrams.h>

using namespace Common;
using namespace Constants;
using namespace TablesDiagramsHonoraires;
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

actesdispo::actesdispo(QWidget * parent)
{
  Q_UNUSED(parent);
  m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  setupUi(this);
  m_reimbursment_AMO = settings()->value(Constants::REIMBURSMENT_AMO).toString();
  dateEdit->setDisplayFormat("yyyy-MM-dd");
  actsComboBox->setEditable(false);
  actsComboBox->setInsertPolicy(QComboBox::NoInsert);
  filltypecombobox();
  QString currentType = tableComboBox->currentText();
  fillactsComboBox(currentType);
  QString actsComboText = actsComboBox->currentText();
  fillLabelsAndCombo(actsComboText);
  
  connect(recordButton,SIGNAL(pressed()),this,SLOT(enregistreacte()));
  connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
  connect(addButton,SIGNAL(pressed()),this,SLOT(add()));
  connect(deleteButton,SIGNAL(pressed()),this,SLOT(deleteact()));  
  connect(tableComboBox,SIGNAL(activated(const QString &)),this,SLOT(fillactsComboBox(const QString &)));  
  connect(actsComboBox,SIGNAL(activated(const QString &)),this,SLOT(fillLabelsAndCombo(const QString &)));
}

actesdispo::~actesdispo(){}

void actesdispo::enregistreacte(){
	QString nomacte 	= codeEdit->text().trimmed();
	QString description 	= descriptionEdit->text().trimmed();
	if (description.contains("'"))
	{
	      description.replace("'","''");
	    }
	QString type 		    = modeEdit->text().trimmed();
	QString valeurtotale	= totalValueEdit->text().trimmed();
	QString valeurtiers     = partialValueEdit->text().trimmed();
	QString date            = dateEdit->date().toString("yyyy-MM-dd");
	
	QString table = tableComboBox->currentText();
    if (!insertintable(NGAP,nomacte,description,type,valeurtotale,date))
    {
        QMessageBox::warning(0,tr("Warning"),tr("Impossible d'insérer dans la table : ")+table,QMessageBox::Ok);
        }
}

bool actesdispo::insertintable(int tablenumber,const QString & act,const QString & description,const QString & type,const QString & valeurtotale,const QString & date)
{
    QSqlQuery qyselect(m_db);
    QSqlQuery qyinsert(m_db);
    QSqlQuery qyupdate(m_db);
    QString table = m_hashtablename.value(m_hashtypetable.value(tablenumber));
    QString fields = getfieldsfromtablenumber();
    QString valuestoinsert;
    QString valeurtiers;
    QString valuestoupdate;
    QString fieldact = getcodenamebytablenumber();
    QString select = QString("select %1 from %2");
    QString insert = QString("insert into %1 (%2) values(%3)");
    QString update = QString("update %1 set %2 where %3 = '%4'");
    switch(tablenumber){
        case NGAP :
            valeurtiers = QString::number(valeurtotale.toDouble()*0.3);
            valuestoinsert = QString("'%1','%2','%3','%4','%5','%6'").arg(act,description,type,valeurtotale,valeurtiers,date);
            valuestoupdate = QString("nom_acte='%1',desc_acte='%2',type='%3',montant_total='%4',montant_tiers='%5',date_effet='%6'")
                             .arg(act,description,type,valeurtotale,valeurtiers,date);
            break;
        case FORFAITS :
            valuestoinsert =QString("'%1','%2','%3','%4','%5'").arg(act,description,valeurtotale,date,type);
            valuestoupdate = QString("forfaits='%1',desc_acte='%2',valeur='%3',date='%4',type='%5'")
                             .arg(act,description,valeurtotale,date,type);
            break;
        case CCAM :
            valuestoinsert = QString("'%1','%2','%3','%4','%5','%6','%7'").arg(act,description,type,valeurtotale,m_reimbursment_AMO,date);
            valuestoupdate = QString("code='%1',abstract='%2',type='%3',amount='%4',reimbursment='%5',date='%6'")
                             .arg(act,description,type,valeurtotale,m_reimbursment_AMO,date);
            break;
        default :
            break;    
        }
    bool test = false;
    if (!qyselect.exec(select.arg(act,table)))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qyselect.lastError().text() ;          
        }
    while (qyselect.next())
    {
        QString actselect = qyselect.value(0).toString();
        if(actselect == act){
          test = true;
          }
        }
    if (!test) //insert
    {
          if (!qyinsert.exec(insert.arg(table,fields,valuestoinsert)))
          {
                qWarning() << __FILE__ << QString::number(__LINE__) << qyinsert.lastError().text() ;
              }
        }
    else //update
    {
        if (!qyupdate.exec(update.arg(table,valuestoupdate,fieldact,act)))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << qyupdate.lastError().text() ;
            }
        }
    QString typetable = tableComboBox->currentText();
    fillactsComboBox(typetable);
    return true;
}


void actesdispo::on_totalValueEdit_textChanged(const QString & text){
    partialValueEdit->setText(QString::number((text.toDouble())*0.3));
}

void actesdispo::fillLabelsAndCombo(const QString & act){
    int typenumber = m_hashtypetable.key(tableComboBox->currentText());
    int descriptionnum = 1;
    int typenum = 2;
    int amountnum = 3;
    if (typenumber == FORFAITS)
    {
          amountnum = 2;
          typenum = 4;
        }
    QString strAct = act;
    QString code = getcodenamebytablenumber();
    QString description;
    QString type;
    QString price;
    //QString thirdPrice;
    QString date;
    QString fields = getfieldsfromtablenumber();
    QString table = m_hashtablename.value(tableComboBox->currentText());
    QString select = QString("select %1 from %2 where %3 = '%4'")
                    .arg(fields,table,code,strAct);
    QSqlQuery qy(m_db);
    if (!qy.exec(select))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        description = qy.value(descriptionnum).toString();
        type = qy.value(typenum).toString();
        price = qy.value(amountnum).toString();
        }
    codeEdit->setText(strAct);
    descriptionEdit->setText(description);
    modeEdit->setText(type);
    totalValueEdit->setText(price);
    dateEdit->setDate(QDate::fromString(date,"yyyy-MM-dd"));
    //fillTypeComboBox(act);
}

void actesdispo::filltypecombobox()
{
    m_hashtypetable.insert(NGAP,"ngap");
    m_hashtypetable.insert(FORFAITS,"forfaits");
    m_hashtypetable.insert(CCAM,"ccam");
    m_hashtablename.insert("ngap","actes_disponibles");
    m_hashtablename.insert("forfaits","forfaits");
    m_hashtablename.insert("ccam","ccam");
    QStringList list;
    list = m_hashtypetable.values();
    tableComboBox->addItems(list);
}

void actesdispo::add()
{
    actsComboBox->clear();
    codeEdit->setText("");
    descriptionEdit->setText("");
    modeEdit->setText("");
    totalValueEdit->setText("");
    partialValueEdit->setText("");
    dateEdit->setDate(QDate::currentDate());
}

void actesdispo::deleteact()
{
    QString act = actsComboBox->currentText();
    QString code = getcodenamebytablenumber();
    QString table = m_hashtablename.value(tableComboBox->currentText());
    QString req = QString("delete from %1 where %2 = '%3'")
                  .arg(table,code,act);
    QSqlQuery qy(m_db);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
          QMessageBox::warning(0,tr("Warning"),tr("Impossible de supprimer : ")+act,QMessageBox::Ok);
        }
    else
    {
        QMessageBox::information(0,tr("Information"),tr("L'acte a été effacé : ")+act,QMessageBox::Ok);
        QString typetable = tableComboBox->currentText();
        fillactsComboBox(typetable);
        }
}

void actesdispo::fillactsComboBox(const QString & type)
{
    actsComboBox->clear();
    QSqlQuery qy(m_db);
    int typenumber = m_hashtypetable.key(type);
    QStringList listofacts;
    QString code = getcodenamebytablenumber();
    QString table;
    switch(typenumber){
        case NGAP :
            table = "actes_disponibles";
            break;
        case FORFAITS :
            table = "forfaits";
            break;
        case CCAM :
            table = "ccam";
            break;
        default :
            break;    
        }
    QString req = QString("select %1 from %2").arg(code,table);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    while (qy.next())
    {
        listofacts << qy.value(0).toString();
        }
    listofacts.sort();
    actsComboBox->addItems(listofacts);
    if (listofacts.size()>0)
    {
        fillLabelsAndCombo(listofacts[0]);
        }
}

QString actesdispo::getcodenamebytablenumber()
{
    int typetable = m_hashtypetable.key(tableComboBox->currentText());
    QString code;
    switch(typetable){
        case NGAP :
            code = "nom_acte";
            break;
        case FORFAITS :
            code = "forfaits";
            break;
        case CCAM :
            code = "code";
            break;
        default :
            break;    
        }
    return code;
}

QString actesdispo::getfieldsfromtablenumber()
{
    QString fields;
    int typetable = m_hashtypetable.key(tableComboBox->currentText());
    switch(typetable){
        case NGAP :
            fields = "nom_acte,desc_acte,type,montant_total,montant_tiers,date_effet";
            break;
        case FORFAITS :
            fields = "forfaits,desc_acte,valeur,date,type";
            break;
        case CCAM :
            fields = "code,abstract,type,amount,reimbursment,date";
            break;
        default :
            break;    
        }
    return fields;
}


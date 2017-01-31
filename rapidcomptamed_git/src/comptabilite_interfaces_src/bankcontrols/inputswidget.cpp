#include "inputswidget.h"
#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>
#include <common/constants.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlTableModel>

using namespace Common;
using namespace Constants;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

Inputs::Inputs(QWidget * parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    initialise();
    connect(addButton,SIGNAL(pressed()),this,SLOT(addValue()));
    connect(deleteButton,SIGNAL(pressed()),this,SLOT(deleteValue()));
}

Inputs::~Inputs(){}

void Inputs::initialise()
{
    //database
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    //type Of Money 
    m_typeOfMoney = QString("euros");
    //dates
    m_dateBegin = calendarWidget->selectedDate();
    m_dateEnd = QDate(m_dateBegin.year(),12,31);
    //radioButtons
    m_hashRadioButtons.insert(ESPECE,especeButton);
    m_hashRadioButtons.insert(CHEQUES,checkButton);
    m_hashRadioButtons.insert(CB,cbButton);
    m_hashRadioButtons.insert(VIREMENT,virButton);
    //types
    m_hashType.insert(ESPECE,"espece");
    m_hashType.insert(CHEQUES,"cheques");
    m_hashType.insert(CB,"cb");
    m_hashType.insert(VIREMENT,"virement");
    //users
    //pref users
    QString prefLogin = settings()->value(Constants::S_UTILISATEUR_CHOISI).toString();
    QString prefId = settings()->value(Constants::S_ID_UTILISATEUR_CHOISI).toString();
    QString prefUserLogin;
    QSqlQuery qPrefUser(m_db);
    const QString reqPrefUser = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("login",
                                                                                 "utilisateurs",
                                                                                 "id_usr",
                                                                                 prefId);
    if (!qPrefUser.exec(reqPrefUser))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qPrefUser.lastError().text() ;
        }
    while (qPrefUser.next())
    {
    	prefUserLogin = qPrefUser.value(0).toString();
        }
    //combo users
    QSqlQuery queryUsers(m_db);
    const QString reqUsers = QString("SELECT %1,%2 FROM %3").arg("login","id_usr","utilisateurs");
    if (!queryUsers.exec(reqUsers))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << queryUsers.lastError().text() ;
        }
    while (queryUsers.next())
    {
    	QString userLogin = queryUsers.value(0).toString();
    	QString userId = queryUsers.value(1).toString();
    	m_hashUsers.insert(userId,userLogin);
        }
    QStringList usersList;
    usersList = m_hashUsers.values();
    usersList.prepend(prefUserLogin);
    usersList.removeDuplicates();
    userComboBox->addItems(usersList);
    //bank accounts
    QString prefBankLibelle = settings()->value(Constants::S_COMPTE_PREFERENTIEL).toString();
    QSqlQuery qBanks(m_db);
    const QString reqBanks = QString("SELECT %1,%2 FROM %3").arg("id_compte","libelle","comptes_bancaires");
    if (!qBanks.exec(reqBanks))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qBanks.lastError().text() ;
        }
    while (qBanks.next())
    {
    	QString idBank = qBanks.value(0).toString();
    	QString libelle = qBanks.value(1).toString();
    	m_hashBanks.insert(idBank,libelle);
        }
    QStringList bankList;
    bankList = m_hashBanks.values();
    bankList.prepend(prefBankLibelle);
    bankList.removeDuplicates();
    bankComboBox->addItems(bankList);
    //model
    QSqlTableModel * depotsModel = new QSqlTableModel(this,m_db);
    depotsModel->setTable("depots");
    depotsModel->setEditStrategy (QSqlTableModel::OnFieldChange);
    depotsModel->select();
    tableView->setModel(depotsModel);
    tableView->setSortingEnabled ( true );
    resultLabel->setText(textOfSums());
}

void Inputs::addValue()
{
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(tableView->model());
    //id_depot 	id_usr 	id_compte 	type_depot 	date 	periode_deb 	periode_fin 	blob_depot 	remarque 	valide
    int rows = model->rowCount();
    int lastid = lastDepotsId();
    int iddepot = lastid + 1;
    int idusr = m_hashUsers.key(userComboBox->currentText()).toInt();
    int idCompte = m_hashBanks.key(bankComboBox->currentText()).toInt();
    int type = getType();
    QString typedepot = m_hashType.value(type);
    QDate date = calendarWidget->selectedDate();
    QDate begin = QDate(date.year(),01,01);
    QDate end = QDate(date.year(),12,31);
    QString inputValue = inputsEdit->text();
    if (inputValue.contains(","))
    {
          QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Utilisez . et pas ,"),QMessageBox::Ok);  
          return;
        }
    QString remarque = "NULL";
    int valid = 0;
    //record
    QSqlRecord record = model->record();
    /*for (int f = 0; f < DepotsFieldsMaxParam; ++f)
    {
          record.append(QSqlField());
        }*/
    record.setValue(IDDEPOT,iddepot);
    record.setValue(IDUSR,idusr);
    record.setValue(IDCOMPTE,idCompte);
    record.setValue(TYPEDEPOT,typedepot);
    record.setValue(DATEDEPOT,date);
    record.setValue(DATEBEGIN,begin);
    record.setValue(DATEEND,end);
    record.setValue(VALUEDEPOT,inputValue);
    record.setValue(REMARQUEDEPOT,remarque);
    record.setValue(VALIDDEPOT,valid);
    //insert in model
    if (!model->insertRecord(rows,record))
    {
          QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Impossible inserer enregistrement"),QMessageBox::Ok);
        }
    resultLabel->setText(textOfSums());
    inputsEdit->setFocus();
    inputsEdit->selectAll();
}

void Inputs::deleteValue()
{
  QSqlTableModel * model = qobject_cast<QSqlTableModel*>(tableView->model());
  QItemSelectionModel *selectionModel = tableView->selectionModel();
  if (selectionModel->selectedRows().count() < 2)
  {      
      QModelIndex index = tableView->QAbstractItemView::currentIndex();
      if(!index.isValid()){
          QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("vous avez oublié de sélectionner une ligne à effacer"),QMessageBox::Ok);
          }
      int row = index.row();      
      if(!model -> removeRows(row,1,QModelIndex())){
          qWarning() << __FILE__ << QString::number(__LINE__) << "lastError = " << model->lastError().text();
          }
      if (!model -> submitAll())
      {
          QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Impossible de soustraire cette ligne."),QMessageBox::Ok);
          }
      else
     {
         QMessageBox::information(0,trUtf8("Information"),trUtf8("Ligne effacée"),QMessageBox::Ok);
        }
    }
  else{
      QModelIndexList listOfIndex = selectionModel->selectedRows(0);
      QList<int> listOfRows;
      for (int i = 0; i < listOfIndex.size() ; ++i)
      {
          int row = listOfIndex[i].row();
          if (!listOfRows.contains(row))
          {
                listOfRows << row;
              }
          }
       for (int r = 0; r < listOfRows.size(); ++r)
       {
             if (!model -> removeRows(r,1,QModelIndex()))
             {
                   qWarning() << __FILE__ << QString::number(__LINE__) << "lastError = " << model->lastError().text();
                 }
           }
    }
    resultLabel->setText(textOfSums());
}

int Inputs::lastDepotsId()
{
    QList<int> list;
    int ret = -1;
    QSqlQuery q(m_db);
    QString req = QString("select %1 from %2").arg("id_depot","depots");
    if (!q.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    while (q.next())
    {
        int v = q.value(0).toInt();
        for (int i = 0; i < list.count(); ++i)
        {
              if (v >= list[i])
              {
                    ret = v;
                  }
            }
        }
    return ret;
}

QString Inputs::textOfSums(){
    QString labelText;
    QString labelTextStr;
    double esp = 0.00;
    double chq = 0.00;
    double visa = 0.00;
    double vir = 0.00;
    double totaux = 0.00;
    QSqlTableModel * model = qobject_cast<QSqlTableModel*>(tableView->model());
    int modelRowCount = model->rowCount(QModelIndex());
    for(int i = 0; i < modelRowCount ; i ++){
       QSqlRecord rowRecord = model->record(i);//ligne d'enregistrement
       QString typeString = rowRecord.value(TYPEDEPOT).toString();
       int type = m_hashType.key(typeString);
       switch(type){
           case ESPECE :
               esp  += rowRecord.value(VALUEDEPOT).toDouble();
               break;
           case CHEQUES :
               chq += rowRecord.value(VALUEDEPOT).toDouble();
               break;
           case CB :
               visa += rowRecord.value(VALUEDEPOT).toDouble();
               break;
           case VIREMENT :
               vir += rowRecord.value(VALUEDEPOT).toDouble();
           default :
               break;    
           }
       }
    totaux = esp + chq + visa + vir;
    QString totauxStr = "<font size = 3 color = ""blue"">Totaux = </font><font size = 3 color = ""red"">"+QString::number(totaux)+" "+m_typeOfMoney+" </font><br/>";
    QString sumsStr = "<font size = 3 color = ""blue"">Esp = </font><font size = 3 color = ""red"">"+QString::number(esp)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Chq = </font><font size = 3 color = ""red"">"+QString::number(chq)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">CB = </font><font size = 3 color = ""red"">"+QString::number(visa)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">VIR = </font><font size = 3 color = ""red"">"+QString::number(vir)+" "+m_typeOfMoney+"  </font>";
    labelTextStr = totauxStr+sumsStr;
    labelText = "<html><body>"+labelTextStr+"</body></html>";
    return labelText;
}

int Inputs::getType()
{
    int ret = 0;
    if (especeButton->isChecked()) ret = int(ESPECE);
    if (checkButton->isChecked()) ret = int(CHEQUES);
    if (cbButton->isChecked()) ret = int(CB);
    if (virButton->isChecked()) ret = int(VIREMENT);
    return ret;
    
}

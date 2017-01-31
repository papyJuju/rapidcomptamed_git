/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.                  *
 *   pm.desombre@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "findreceiptsvalues.h"
#include "constants.h"
#include "receipstools.h"

#include <common/constants.h>

#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>

enum { WarnDebugMessage = true };

using namespace Common;
using namespace Constants;
using namespace ReceiptsConstants;

FindReceiptsValues::FindReceiptsValues(QWidget * parent,QSqlDatabase db):QDialog(parent)
{
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  ui->flagPixmapLabel->setText("");
  ui->flagLabel->hide();
  ui->nextButton->hide();
  ui->nameRadioButton->setChecked(true);
  ui->modifSpinBox->setValue(1.0);
  ui->modifSpinBox->setSingleStep(0.1);
  ui->abstractTextEdit->setText("");
  ui->othersTextEdit->setText("");
  ui->abstractTextEdit->setReadOnly(true);
  ui->othersTextEdit->setReadOnly(true);
  QString explanationNewCommand = tr("Pour enregistrer une nouvelle commande appuyez sur la touche Entrée (Return).");
  ui->commandeAmeliLabel->setToolTip(explanationNewCommand);
  QFile commandeamelifile(qApp->applicationDirPath()+"/../resources/ameli/doc/commande_ameli_xls");
  if (!commandeamelifile.open(QIODevice::ReadOnly))
  {
      qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file : " << commandeamelifile.fileName();
      }
  QTextStream commandamelistream(&commandeamelifile);
  QString openexplorator;
  while (!commandamelistream.atEnd())
  {
      openexplorator = commandamelistream.readLine();
      }
  ui->commandeAmeliXlsEdit->setText(openexplorator);
  m_db = db; //QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  m_modifier = 1.0;
  m_hashCategories.insert(NGAP,"ngap");
  m_hashCategories.insert(FORFAITS,"forfaits");
  m_hashCategories.insert(CCAM,"ccam");
  m_hashTables.insert(NGAP,"actes_disponibles");
  m_hashTables.insert(FORFAITS,"forfaits");
  m_hashTables.insert(CCAM,"ccam");
  fillComboCategories();
  ui->comboBoxCategories->setEditable(false);
  initialize();
  QString comboValue = ui->comboBoxCategories->currentText().trimmed();
  for (int i = 0; i < ui->buttonBox->buttons().size(); ++i)
  {
  	  ui->buttonBox->buttons()[i]->setMinimumSize(100,50);
      }
  //icons and shortcuts
  //ui->plusButton->setIcon(QIcon(qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/next.png"));
  //ui->lessButton->setIcon(QIcon(qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/previous.png"));
  ui->plusButton->setShortcut(QKeySequence("CTRL+p"));
  ui->lessButton->setShortcut(QKeySequence("CTRL+l"));
  ui->plusButton->setToolTip(QKeySequence("CTRL+p").toString());
  ui->lessButton->setToolTip(QKeySequence("CTRL+l").toString());
  ui->buttonBox->button(QDialogButtonBox::Save)->setShortcut(QKeySequence("CTRL+s"));
  ui->buttonBox->button(QDialogButtonBox::Save)->setToolTip("ctrl+s");
  ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Enregistrer."));
  ui->buttonBox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("CTRL+q"));
  ui->buttonBox->button(QDialogButtonBox::Cancel)->setToolTip("ctrl+q");
  ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Quitter."));
  //ui->buttonBox->button(QDialogButtonBox::Save)->setIcon(theme()->icon(Core::Constants::ICONSAVE));
  //ui->buttonBox->button(QDialogButtonBox::Cancel)->setIcon(theme()->icon(Core::Constants::ICONEXIT));
  ui->tableViewOfValues->setMouseTracking(true);
  //flag
  //setFlagAccordingToMProcedureCountry();
  //disable lessButton
  ui->lessButton->setEnabled(false);
  fillListViewValues(NGAP);

  connect(ui->comboBoxCategories,SIGNAL(activated(int)),this,SLOT(fillListViewValues(int)));
  connect(ui->tableViewOfValues,SIGNAL(pressed(const QModelIndex&)),this,SLOT(showInformations(const QModelIndex&)));
  connect(ui->tableViewOfValues,SIGNAL(entered(const QModelIndex&)),this,SLOT(showInformations(const QModelIndex&)));
  
  //connect(ui->tableViewOfValues,SIGNAL(selectionChanged ( const QItemSelection & , const QItemSelection &)),this,SLOT(showInformations(const QItemSelection & , const QItemSelection &)));
  
  connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(chooseValue()));
  connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(deleteValue()));
  //connect(ui->listchosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(supprItemchosen(QListWidgetItem *)));
  //connect(ui->nextButton,SIGNAL(pressed()),this,SLOT(showNext()));
  connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(setModifSpinBox(QWidget*,QWidget*)));
  connect(ui->modifSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setModifier(double)));
  //connect(ui->userRadioButton,SIGNAL(clicked(bool)),this,SLOT(chooseUserModel(bool)));
  //connect(ui->datapackRadioButton,SIGNAL(clicked(bool)),this,SLOT(chooseDatapackModel(bool)));
  connect(ui->nameRadioButton,SIGNAL(toggled(bool)),this,SLOT(wipeFilterEdit(bool)));
  connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(wipeFilterEdit()));
  connect(ui->listchosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(setLessButtonEnabled(QListWidgetItem *)));
  connect(ui->commandeAmeliXlsEdit,SIGNAL(returnPressed()),this,SLOT(writeTheCommandInDoc()));
  connect(ui->ameliButton,SIGNAL(pressed()),this,SLOT(openAmeliFile()));
}

FindReceiptsValues::~FindReceiptsValues()
{
  ui->listchosenWidget->clear();
}

void FindReceiptsValues::initialize(){
    if(m_hashValueschosen.size()>0){
        m_hashValueschosen.clear();
        }
}

void FindReceiptsValues::clear(){
    ui->listchosenWidget->clear();
    m_hashValueschosen.clear();
}

void FindReceiptsValues::fillComboCategories(){
    QStringList choiceList ;
    choiceList = m_hashCategories.values();
    ui->comboBoxCategories->clear();
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBoxCategories->addItems(choiceList);
}

void FindReceiptsValues::fillListViewValues(int category){
    ui->abstractTextEdit->setText("");
    ui->othersTextEdit->setText("");
    QStandardItemModel *model = new QStandardItemModel;
    switch(category){
        case CCAM :
            model = fillCcamList(category);
            break;
        case NGAP :
            model = fillNgapList(category);
            break;
        case FORFAITS :
            model = fillForfaitsList(FORFAITS);
            break;
        default :
            break;    
        }  
        qDebug() << __FILE__ << QString::number(__LINE__) << " model =" << QString::number(model->rowCount()) ;
    ui->tableViewOfValues->setModel(model);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);

}

void FindReceiptsValues::showInformations(const QModelIndex & index)
{
    int row = index.row();
    ui->abstractTextEdit->clear();
    ui->othersTextEdit->clear();
    QAbstractItemModel *model = ui->tableViewOfValues->model();
    QString code = model->data(model->index(row,NAME_ITEM),Qt::DisplayRole).toString();
    QByteArray explanation = m_hashExplanations.value(code);
    const char * explchar = explanation.data();
    QString explanationstr = tr(explchar);
    explanationstr = "<html><body><font color=blue size=3>"+explanationstr+"</font></body></html>";
    QString othersText;
    othersText = "<html><body><font color=blue size=3>";
    QHash<QString,QString> hash = getHashFatherSonFromOthers(index);
    for (int i = 0; i < hash.count(); ++i)
    {
          QString father = hash.keys()[i];
          QString son = hash.values()[i];
          if (!father.isEmpty())
          {
              othersText += father+":"+son+"<br/>";
              }
        }
    othersText += "</font></body></html>";
    ui->abstractTextEdit->insertHtml(explanation);
    ui->othersTextEdit->insertHtml(othersText);
}

void FindReceiptsValues::showInformations(const QItemSelection & selected, const QItemSelection & deselected)
{
    QModelIndexList listofselectedindexes = selected.indexes();
    int row = 0;
    QModelIndex index ;
    if (listofselectedindexes.size()>0)
    {
          index = listofselectedindexes[0];
          row = listofselectedindexes[0].row();
        }
    ui->abstractTextEdit->clear();
    ui->othersTextEdit->clear();
    QAbstractItemModel *model = ui->tableViewOfValues->model();
    QString code = model->data(model->index(row,NAME_ITEM),Qt::DisplayRole).toString();
    QByteArray explanation = m_hashExplanations.value(code);
    const char * explchar = explanation.data();
    QString explanationstr = tr(explchar);
    explanationstr = "<html><body><font color=blue size=3>"+explanationstr+"</font></body></html>";
    QString othersText;
    othersText = "<html><body><font color=blue size=3>";
    QHash<QString,QString> hash = getHashFatherSonFromOthers(index);
    for (int i = 0; i < hash.count(); ++i)
    {
          QString father = hash.keys()[i];
          QString son = hash.values()[i];
          if (!father.isEmpty())
          {
              othersText += father+":"+son+"<br/>";
              }
        }
    othersText += "</font></body></html>";
    ui->abstractTextEdit->insertHtml(explanation);
    ui->othersTextEdit->insertHtml(othersText);
}


void FindReceiptsValues::chooseValue()
{
    //get data
    QAbstractItemModel * model = ui->tableViewOfValues->model();
    QModelIndex inIndex = ui->tableViewOfValues->currentIndex();
    if (!inIndex.isValid()) {
        QMessageBox::warning(0,tr("Attention"),tr("Valeur non choisie"),QMessageBox::Ok);
        return;
    }
    int row = inIndex.row();
    QModelIndex indexData = model->index(row,0,QModelIndex());
    QModelIndex indexAmount = model->index(row,1,QModelIndex());
    QString data = model->data(indexData,Qt::DisplayRole).toString();//NAME
    QString amount = model->data(indexAmount,Qt::DisplayRole).toString();//AMOUNT
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " data = " << data;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " amount = " << amount;
    if (m_modifier < 1.0 || m_modifier > 1.0)
    {
          data = data+"*"+QString::number(m_modifier);
          double amountDouble = amount.toDouble();
          if (amountDouble == 0.0)
             {
              qWarning() << __FILE__ << QString::number(__LINE__) << "value null" ;
              if (amount.contains(","))
              {
                  if (WarnDebugMessage)
                  qDebug() << __FILE__ << QString::number(__LINE__) << " in , "  ;
                  amount.replace(",",QLocale::c().decimalPoint ());
                  amountDouble = amount.toDouble();
                  }
              else if (amount.contains("."))
              {
                  amount.replace(".",QLocale::c().decimalPoint ());
                  amountDouble = amount.toDouble();
                  }
            }
          amount = QString::number(amountDouble * m_modifier);
        }

    ui->listchosenWidget->addItem(data);
    m_hashValueschosen.insert(data,amount);
}

void FindReceiptsValues::deleteValue()
{
    QListWidgetItem * item;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " in deleteValue " ;
    if (!ui->listchosenWidget->currentIndex().isValid()) {
        QMessageBox::warning(0,tr("Attention"),tr("Valeur non choisie"),QMessageBox::Ok);
        return;
    }
    item = ui->listchosenWidget->currentItem();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " item = " << item->text();
    QString dataToRemove = item->data(Qt::DisplayRole).toString();
    m_hashValueschosen.remove(dataToRemove);
    delete item;
    ui->lessButton->setEnabled(false);
}

void FindReceiptsValues::setModifSpinBox(QWidget*old,QWidget*newWidget){
    if (newWidget == ui->modifSpinBox)
    {
          ui->modifSpinBox->setValue(0.5);
 
        }
    if (old == ui->modifSpinBox && newWidget != ui->plusButton)
    {
          ui->modifSpinBox->setValue(1.0);
        }
    if (old == ui->plusButton && newWidget != ui->modifSpinBox  )
    {
          ui->modifSpinBox->setValue(1.0);
        }

}

void FindReceiptsValues::setModifier(double modif)
{
    m_modifier = modif;
}

QHash<QString,QString> FindReceiptsValues::getchosenValues(){
    return m_hashValueschosen;
}

void FindReceiptsValues::on_lineEditFilter_textChanged(const QString & text){
    int comboCategoryValue = ui->comboBoxCategories->currentIndex();
    QString table = m_hashTables.value(comboCategoryValue);
    QString filterText ;
    QString filter;
    QString name;
    QString amount;
    QString abstract;
    QString date ;
    switch(comboCategoryValue){
        case NGAP :
            name = "nom_acte";
            amount = "montant_total";
            date = "date_effet";
            abstract = "desc_acte";
            break;
        case CCAM :
            name = "code";
            amount = "amount";
            abstract = "abstract";
            date = "date";
            break;
        case FORFAITS :
            name = "forfaits";
            amount = "valeur";
            abstract = "";
            date = QDate::currentDate().toString("yyyy-MM-dd");
            break;
        default :
            break;    
        }
    
        
    if (ui->nameRadioButton->isChecked())
    {
          filterText = ""+text+"%";
          filter = QString("WHERE %1 LIKE '%2'").arg(name,filterText);
        }
    else if (ui->abstractRadioButton->isChecked())
    {
          filterText = "%"+text+"%";
          filter = QString("WHERE %1 LIKE '%2'").arg(abstract,filterText);
        }
    else{
        QMessageBox::warning(0,tr("Attention"),tr("Choisissez un bouton"),QMessageBox::Ok);
        //Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Check a button."));
    }
    QString req = QString("SELECT %1,%2,%3 FROM %4 ").arg(name,amount,date,table )+filter;
    QStandardItemModel *model = new QStandardItemModel(1,3,this);
    int row = 0;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
         qWarning() << __FILE__ << QString::number(__LINE__)
                                << "Error __FILE__ "+QString::number(__LINE__) << ":" << q.lastError().text() ;
        }
    while (q.next())
    {
        QString n = q.value(NAME_ITEM).toString();
        QString a = q.value(AMOUNT_ITEM).toString();
        QString d = q.value(DATE_ITEM).toString();
        //if (WarnDebugMessage)
              //qDebug() << __FILE__ << QString::number(__LINE__) << " n and a	= " << n << a;
        model->insertRows(row,1,QModelIndex());
        model->setData(model->index(row,NAME_ITEM),n,Qt::EditRole);
        model->setData(model->index(row,AMOUNT_ITEM),a,Qt::EditRole);
        model->setData(model->index(row,DATE_ITEM),d,Qt::EditRole);
        model->submit();
        ++row;
        }
    ui->tableViewOfValues->setModel(model);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}

bool FindReceiptsValues::tableViewIsFull(QAbstractItemModel * model){
    bool ret = false;
    int count = model->rowCount();
    if (count > 255)
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "table view is full" ;
          ret = true;
        }
    return ret;
}

void FindReceiptsValues::enableShowNextTable(){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " enableshownet "   ;
    ui->nextButton->show();
}

/*void FindReceiptsValues::showNext(){
    QAbstractItemModel * abModel = ui->tableViewOfValues->model();
    int rows = abModel->rowCount();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(rows) ;
    int numberOfLastRow = abModel->headerData(rows -1,Qt::Vertical,Qt::DisplayRole).toInt();
    QString lastData = abModel->data(abModel->index(numberOfLastRow -1,0 ),Qt::DisplayRole).toString();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfLastRow =" << QString::number(numberOfLastRow) ;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " shownext data =" <<  lastData;
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString afterSqlFilter = QString("%1 LIKE '%2' AND %3 >= '%4'").arg("type",comboChoice,"NAME",lastData);
    model->setFilter(afterSqlFilter);
    int count =   model->rowCountWithFilter(QModelIndex(),afterSqlFilter);
    for (int i = 0; i < count; i += 1)
    {
        QString name = model->dataWithFilter(model->index(i,MP_NAME),Qt::DisplayRole,afterSqlFilter).toString();
        //if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " names =" << name ;
        QString value = model->dataWithFilter(model->index(i,MP_AMOUNT),Qt::DisplayRole,afterSqlFilter).toString();
        QStandardItem *itemName = new QStandardItem(name);
        QStandardItem *itemValue = new QStandardItem(value);
        QList<QStandardItem*> list;
        list << itemName << itemValue;
        itemModel->appendRow(list);
        }
    model->setFilter("");
    QVariant act = QVariant(tr("Name"));
    QVariant value = QVariant(tr("Value"));
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
          }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        }
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}*/

/*void FindReceiptsValues::showNext(){//the button is hiden
    QAbstractItemModel * abModel = ui->tableViewOfValues->model();
    int rows = abModel->rowCount();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(rows) ;
    int numberOfLastRow = abModel->headerData(rows -1,Qt::Vertical,Qt::DisplayRole).toInt();
    QString lastData = abModel->data(abModel->index(numberOfLastRow -1,0 ),Qt::DisplayRole).toString();
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString afterSqlFilter = QString("%1 LIKE '%2' AND %3 >= '%4'").arg("type",comboChoice,"name",lastData);
    MedicalProcedureModel *model = new MedicalProcedureModel(this);
    model->setFilter(afterSqlFilter);
    ui->tableViewOfValues->setModel(model);
    ui->tableViewOfValues->setColumnHidden(MP_ID,true);
    ui->tableViewOfValues->setColumnHidden(MP_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_USER_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_INSURANCE_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_REIMBOURSEMENT,true);
    ui->tableViewOfValues->setColumnHidden(MP_ABSTRACT,true);
    ui->tableViewOfValues->setColumnHidden(MP_TYPE,true);
    ui->tableViewOfValues->setColumnHidden(MP_DATE,true);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}*/

/*QString FindReceiptsValues::getDateWhereClause()
{
    QString whereClause;
    receiptsEngine io;
    QString dateOfLastReleaseLessOneDay = io.getJustDayBeforeLastRelease();
    whereClause = QString(" AND WHERE %1 > '%2'").arg(dateOfLastReleaseLessOneDay,"DATE");
    return whereClause;
}*/

/*bool FindReceiptsValues::datapackIsAvalaible()
{
    bool b = true;
    QString datapack = databaseFileName();
    QFile file(datapack);
    if (!file.exists())
    {
          b = false;
        }
    return b;
}*/

/*void FindReceiptsValues::chooseUserModel(bool b)
{
    if (b)
    {
          m_db = QSqlDatabase::database(DB_ACCOUNTANCY);
          fillComboCategories();
          const QString comboText = ui->comboBoxCategories->currentText();
          fillListViewValues(comboText);
        }
}*/

/*void FindReceiptsValues::chooseDatapackModel(bool b)
{
    if (b)
    {
          m_db = QSqlDatabase::database(DATAPACK_DB);
          qDebug() << __FILE__ << QString::number(__LINE__) << " database name =" << m_db.databaseName() ;
          fillComboCategories();
          const QString comboText = ui->comboBoxCategories->currentText();
          fillListViewValues(comboText);
        }
}*/

QHash<QString,QString> FindReceiptsValues::getHashFatherSonFromOthers(const QModelIndex & index)
{
    QHash<QString,QString> hash;
    int row = index.row();
    QString codecOfApplication = ""+QString(QTextCodec::codecForTr()->name())+"";
    qDebug() << __FILE__ << "repere : " << QString::number(__LINE__) << "codec" << codecOfApplication ;
    QAbstractItemModel *model = ui->tableViewOfValues->model();
    QString code = model->data(model->index(row,NAME_ITEM),Qt::DisplayRole).toString();
    QString xmlbegin = "'<\?xml version=\"1.0\" encoding="+codecOfApplication+"\?>'";
    QString othersString = m_otherInformations.value(code);
    qDebug() << __FILE__ << QString::number(__LINE__) << "othersString = " << othersString;
    othersString.replace(xmlbegin,"");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " othersString =" << othersString ;
    othersString.replace("</",";");
    QRegExp exp(";[a-z]*>");
    othersString.replace(exp,";");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " othersString =" << othersString ;
    QStringList list;
    list = othersString.split(";");
    foreach(QString part,list){
        part.replace("<","");
        part.replace(">",";");
        QStringList partList;
        partList = part.split(";");
        if (partList.size()==2)
        {
              hash.insert(partList[FATHER],partList[SON]);
            }
        if(partList.size()==1)
        {
            hash.insert(partList[FATHER],"NULL");
            }
        }
        for (int i = 0; i < hash.count(); i += 1)
        {
              if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " father =" << hash.keys()[i] ;
              if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " son =" << hash.values()[i] ;
            }
    return hash;
}

void FindReceiptsValues::wipeFilterEdit(bool b)
{
    Q_UNUSED(b);
    ui->lineEditFilter->setText("");
}

void FindReceiptsValues::wipeFilterEdit()
{
    ui->lineEditFilter->setText("");
}

void FindReceiptsValues::setLessButtonEnabled(QListWidgetItem * item)
{
    Q_UNUSED(item);
    ui->lessButton->setEnabled(true);
}

QStandardItemModel * FindReceiptsValues::fillCcamList(int category)
{
    QString table = m_hashTables.value(category);
    const QString name = "code";
    const QString amount = "amount";
    const QString explanation = "abstract";
    const QString others = "others";
    const QString type = "type";    
    const QString date = "date";
    QList<int> counterList;
    QString req = QString("SELECT %1,%2,%3,%4,%5 FROM %6 ")
                 .arg(name,amount,explanation,others,date,table);
    QStandardItemModel *model = new QStandardItemModel(0,3,this);
    int row = 0;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
         qWarning() << __FILE__ << QString::number(__LINE__)
                                << "Error __FILE__"+QString::number(__LINE__)+q.lastError().text() ;
        }
    while (q.next())
    {
        QString n = q.value(NAME_ITEM).toString();
        QString a = q.value(AMOUNT_ITEM).toString();
        QString date = q.value(DATE_ITEM).toString();
        QByteArray expl = q.value(EXPLANATION).toByteArray();
        QString otherInformation = q.value(OTHERS).toString();
        model->insertRows(row,1,QModelIndex());
        model->setData(model->index(row,NAME_ITEM),n,Qt::EditRole);
        model->setData(model->index(row,AMOUNT_ITEM),a,Qt::EditRole);
        model->setData(model->index(row,DATE_ITEM),a,Qt::EditRole);
        model->submit();
        m_hashExplanations.insertMulti(n,expl);
        m_otherInformations.insertMulti(n,otherInformation);
        ++row;
        counterList << row;
        }
    if (counterList.size()<1) {
        const QString explanationText = tr("The ccam database is empty.");
        qWarning() << __FILE__ << QString::number(__LINE__) << explanationText ;
        }

    return model;
}

QStandardItemModel *  FindReceiptsValues::fillNgapList(int category)
{
    QStandardItemModel *modelNgap = new QStandardItemModel(0,3,this);
    QString table = m_hashTables.value(category);
    QString code = "nom_acte";
    QString amount = "montant_total";
    QString date = "date_effet";
    QString req = QString("SELECT %1,%2,%3 FROM %4 ").arg(code,amount,date,table);
    qDebug() << __FILE__ << QString::number(__LINE__) << " req ngap =" << req ;
    QStringList names;
    QList<int> counterList;
    int row = 0;
    QSqlQuery query(m_db);
    if (!query.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
        }
    while (query.next())
    {
        QString n = query.value(NAME_ITEM).toString();
        names << n;
        QString a = query.value(AMOUNT_ITEM).toString();
        QString d = query.value(DATE_ITEM).toString();
        QByteArray expl = "";
        QString otherInformation = "";
        if (!modelNgap->insertRows(row,1,QModelIndex()))
        {
              qDebug() << __FILE__ << QString::number(__LINE__) << "unable to insert row =" << QString::number(row) ;
            }
        if (!modelNgap->setData(modelNgap->index(row,NAME_ITEM),n,Qt::EditRole))
        {
              qDebug() << __FILE__ << QString::number(__LINE__) << " unable to setdata for =" << n ;
            }
        if (!modelNgap->setData(modelNgap->index(row,AMOUNT_ITEM),a,Qt::EditRole))
        {
              qDebug() << __FILE__ << QString::number(__LINE__) << " unable to setdata for =" << a ;
            }
        if (!modelNgap->setData(modelNgap->index(row,DATE_ITEM),d,Qt::EditRole))
        {
              qDebug() << __FILE__ << QString::number(__LINE__) << " unable to setdata for =" << d ;
            }
        if (!modelNgap->submit())
        {
              qDebug() << __FILE__ << QString::number(__LINE__) << "unable to submit model for ngap";
            }
        m_hashExplanations.insertMulti(n,expl);
        m_otherInformations.insertMulti(n,otherInformation);
        ++row;
        counterList << row;
        }
    if (counterList.size()<1) {
        const QString explanationText = tr("The database is empty : ")+table;
        qWarning() << __FILE__ << QString::number(__LINE__) << explanationText ;
        }
    names.removeDuplicates();
    foreach(QString code, names)
    {
        QList<QStandardItem*> list = modelNgap->findItems(code,Qt::MatchExactly,NAME_ITEM);
        if (list.count()>1)
        {qDebug() << __FILE__ << QString::number(__LINE__) << " code ngap en double =" << code ;
              for (int i = 0; i < list.count(); ++i)
              {
                    QString lastDateStr = lastDateFoundInTable(category,code);
                    int row = modelNgap->indexFromItem(list[i]).row();
                    QString dateStr = modelNgap->data(modelNgap->index(row,DATE_ITEM),Qt::DisplayRole).toString();
                    QDate lastDate = QDate::fromString(lastDateStr,"yyyy-MM-dd");
                    QDate date = QDate::fromString(dateStr,"yyyy-MM-dd");
                    if (date < lastDate)
                    {
                          modelNgap->removeRows(row,1);
                        }
                  }
            }
        }
        qDebug() << __FILE__ << QString::number(__LINE__) << " model ngap =" << QString::number(modelNgap->rowCount()) ;
        
    return modelNgap;
}

QStandardItemModel *  FindReceiptsValues::fillForfaitsList(int category)
{
    QStandardItemModel *model = new QStandardItemModel(0,3,this);
    QString table = m_hashTables.value(category);
    QString code = "forfaits";
    QString amount = "valeur";
    QString req = QString("SELECT %1,%2 FROM %3 ").arg(code,amount,table);
    QSqlQuery query(m_db);
    QList<int> counterList;
    int row = 0;
    if (!query.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
        }
    while (query.next())
    {
        QString n = query.value(NAME).toString();
        QString a = query.value(AMOUNT).toString();
        QByteArray expl = "";
        QString otherInformation = "";
        model->insertRows(row,1,QModelIndex());
        model->setData(model->index(row,0),n,Qt::EditRole);
        model->setData(model->index(row,1),a,Qt::EditRole);
        model->submit();
        m_hashExplanations.insertMulti(n,expl);
        m_otherInformations.insertMulti(n,otherInformation);
        ++row;
        counterList << row;
        }
    if (counterList.size()<1) {
        const QString explanationText = tr("The database is empty : ")+table;
        qWarning() << __FILE__ << QString::number(__LINE__) << explanationText ;
        }
    return model;
}

QString FindReceiptsValues::lastDateFoundInTable(int category,QString code)
{
    QDate lastDate(1900,01,01);
    QSqlQuery query(m_db);
    QString date;
    QString fieldCode;
    QString table = m_hashTables.value(category);
    switch(category){
        case NGAP :            
            date = "date_effet";
            fieldCode = "nom_acte";
            break;
        case CCAM :
            date = "date";
            fieldCode = "code";
            break;
        default :
            break;    
        }
    QString req = QString("select %1 from %2 where %3 = '%4'").arg(date,table,fieldCode,code);
    if (!query.exec(req))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << query.lastError().text() ;
        }
    while (query.next())
    {
        QString dateStr = query.value(0).toString();
        QDate date = QDate::fromString(dateStr);
        if (date >= lastDate)
        {
              lastDate = date;
            }
        }
     return lastDate.toString("yyyy-MM-dd");
}

void FindReceiptsValues::openAmeliFile()
{
    QProcess *process = new QProcess(this);
    QString command = ui->commandeAmeliXlsEdit->text();
    if (command.contains("nautilus ../resources/ameli"))
    {
    	command = "nautilus "+qApp->applicationDirPath()+"/../resources/ameli/";
        }
    if (command.contains("\\"))
    {
        command.replace("\\","\\\\");
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << "commande = " << command ;
    process->start(command);
}

void FindReceiptsValues::writeTheCommandInDoc()
{
    QString command = ui->commandeAmeliXlsEdit->text();
    QFile commandeamelifile(qApp->applicationDirPath()+"/../resources/ameli/doc/commande_ameli_xls");
    if (!commandeamelifile.open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file : " << commandeamelifile.fileName();
       }
    QTextStream commandamelistream(&commandeamelifile);
    commandamelistream << command;
    QMessageBox::information(0,tr("Information"),tr("La commande a été mémorisée."),QMessageBox::Ok);
}

/*void FindReceiptsValues::setFlagAccordingToMProcedureCountry()
{
  int counterId = 0;
  QString reqCountry; 
  QString flagText;  
  while(reqCountry.isEmpty())
  {
      ++counterId;
      reqCountry = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                      .arg("COUNTRY","ccam","MP_ID",QString::number(counterId));
      if (WarnDebugMessage)
      qDebug() << __FILE__ << QString::number(__LINE__) << " reqCountry =" << reqCountry ;
  }
  QSqlQuery queryCountry(m_db);
  if (!queryCountry.exec(reqCountry))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << queryCountry.lastError().text() ;
      }
  while (queryCountry.next())
  {
  	flagText = queryCountry.value(0).toString();
      }
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " flagText =" << flagText ;
  ui->flagLabel->setText(flagText);
  Tools::ReceiptsTools rt;
  ui->flagPixmapLabel->setPixmap(rt.flagsHash().value(flagText));
}*/


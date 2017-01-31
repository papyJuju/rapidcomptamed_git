#include "facturation.h"
#include <common/tablesdiagrams.h>
#include <common/databasemanager.h>
#include <common/icore.h>
#include <common/settings.h>
#include <common/constants.h>
#include <common/commandlineparser.h>
#include <common/configure/delegate.h>
#include <common/print/qprintereasy.h>
#include <QDebug>
#include <QMessageBox>

using namespace Common;
using namespace Constants;
using namespace FacturationDiagrams;
using namespace DatesDiagrams;

static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

Facture::Facture(QWidget * parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    quitButton->hide();
    m_programName    = dataIn()->value(Common::CommandLine::CL_ProgramName).toString();
    m_nomRecupere    = dataIn()->value(Common::CommandLine::CL_PatientName).toString();
    m_prenomRecupere = dataIn()->value(Common::CommandLine::CL_PatientFirstName).toString();
    m_guidEntre      = dataIn()->value(Common::CommandLine::CL_PatientUUID).toString();
    m_dateOfBirth    = dataIn()->value(Common::CommandLine::CL_DateOfBirth).toString();
    m_tarif          = dataIn()->value(Common::CommandLine::CL_Price).toString();
    m_codeTarif      = dataIn()->value(Common::CommandLine::CL_PriceCode).toString();
    m_user           = dataIn()->value(Common::CommandLine::CL_CallingProgramUser).toString();//inutilisable ici
    qDebug() << __FILE__ << QString::number(__LINE__) << " nom =" << m_nomRecupere << m_prenomRecupere ;
    m_prefuserid = settings()->value(Constants::S_ID_UTILISATEUR_CHOISI).toString();
    dateEdit->setDate(QDate::currentDate());
    if (!Common::ICore::instance()->databaseManager()->connectAccountancy())
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect Accountancy !" ;
      }
    
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_db constructeur =" << m_db.databaseName() ;
    
    m_modelseances = new QSqlTableModel(this,m_db);
    m_modelseances->setTable(Constants::TABLE_SEANCES);
    m_modelhonoraires = new QSqlTableModel(this,m_db);
    m_modelhonoraires->setTable(Constants::TABLE_HONORAIRES);
    m_modelfacture = new FactureModel(this);
    m_io = new FacturationIO(this);
    listFacturesIdComboBox->addItems(setFacturesIdComboBox());
    setTableView();
    //connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(addButton,SIGNAL(pressed()),this,SLOT(addrow()));
    connect(deleteButton,SIGNAL(pressed()),this,SLOT(delRow()));
    connect(newButton,SIGNAL(pressed()),this,SLOT(newBill()));
    //connect(m_modelfacture,SIGNAL(dataChanged(const QModelIndex & , const QModelIndex &  )),this,SLOT(setValueToValueDelegate(const QModelIndex & , const QModelIndex & )));
    connect(recordButton,SIGNAL(pressed()),this,SLOT(record()));
    connect(eraseButton,SIGNAL(pressed()),this,SLOT(deleteBill()));
    connect(printButton,SIGNAL(pressed()),this,SLOT(printBill()));
    connect(listFacturesIdComboBox,SIGNAL(highlighted(const QString &)),this,SLOT(setFactureFromId(const QString &)));
}

Facture::~Facture(){}

void Facture::setTableView()
{
    m_modelfacture->setHeaderData(FacturationSpace::PATIENT,Qt::Horizontal,tr("Patient"));
    m_modelfacture->setHeaderData(FacturationSpace::PRATICIEN,Qt::Horizontal,tr("Praticien"));
    m_modelfacture->setHeaderData(FacturationSpace::SITE,Qt::Horizontal,tr("Site"));
    m_modelfacture->setHeaderData(FacturationSpace::PAYEUR,Qt::Horizontal,tr("Payeur"));
    m_modelfacture->setHeaderData(FacturationSpace::DATE_OPERATION,Qt::Horizontal,tr("Date opération"));
    m_modelfacture->setHeaderData(FacturationSpace::DATE_PAIEMENT,Qt::Horizontal,tr("Date paiement"));
    m_modelfacture->setHeaderData(FacturationSpace::DATE_VALEUR,Qt::Horizontal,tr("Date valeur\nen banque"));
    m_modelfacture->setHeaderData(FacturationSpace::DATE_VALIDATION,Qt::Horizontal,tr("Date validation\nde la facture"));
    m_modelfacture->setHeaderData(FacturationSpace::PRESTATION,Qt::Horizontal,tr("Prestation"));
    m_modelfacture->setHeaderData(FacturationSpace::VALEUR,Qt::Horizontal,tr("Valeur"));
    m_modelfacture->setHeaderData(FacturationSpace::MODE_PAIEMENT,Qt::Horizontal,tr("Mode paiement"));
    m_modelfacture->setHeaderData(FacturationSpace::STATUT,Qt::Horizontal,tr("Statut"));
    tableView->setModel(m_modelfacture);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    setdatedelegate();
    setbuttondelegate();
    setvaleurdelegate();
    setmodepaymentdelegate();
    setusersdelegate();
    setstatutpayementdelegate();
    setpatientdelegate();
    setsitedelegate();
    setpayeurdelegate();
    int diff = 930-912 +8;
    tableView->resizeColumnsToContents();
    int width = 0;
    for (int c  = 0 ; c < FacturationSpace::FacturationHeadersModel_MaxParam; ++c)
    {
          width += tableView->columnWidth(c);
        }
    resize(width+diff,height());
}

void Facture::addrow()
{
    QString name;
    name = m_nomRecupere+","+m_prenomRecupere;
    QString editedName = nameEdit->text();
    QString editedFirstname = firstnameEdit->text();
    if (!editedName.isEmpty()|| !editedFirstname.isEmpty())
    {
          if (editedName.isEmpty()||editedFirstname.isEmpty())
          {
                QMessageBox::warning(0,tr("Warning"),tr("Veuillez renseigner le nom et le prénom."),QMessageBox::Ok);
                return;
              }
        name = editedName+","+editedFirstname;
        }
    if (!m_modelfacture->insertRows(m_modelfacture->rowCount(),1))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << m_modelfacture->lastError().text() ;
        }
    QModelIndex indexpatient = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::PATIENT);
    QModelIndex indexdateop = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::DATE_OPERATION);
    QModelIndex indexprestation = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::PRESTATION);
    QModelIndex indexdatepaiement = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::DATE_PAIEMENT);
    QModelIndex indexvaleurbanque = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::DATE_VALEUR);
    QModelIndex indexvalidation = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::DATE_VALIDATION);
    QModelIndex indexstatut = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::STATUT);
    QModelIndex indexpaymentprestation = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::MODE_PAIEMENT);
    QString date = dateEdit->date().toString("yyyy-MM-dd");
    QString yeardatebegin = QString::number(dateEdit->date().year())+"-01-01";
    m_modelfacture->setData(indexpatient,name,Qt::EditRole);
    m_modelfacture->setData(indexdateop,date,Qt::EditRole);
    m_modelfacture->setData(indexdatepaiement,date,Qt::EditRole);
    m_modelfacture->setData(indexvaleurbanque,yeardatebegin,Qt::EditRole);
    m_modelfacture->setData(indexvalidation,yeardatebegin,Qt::EditRole);
    m_modelfacture->setData(indexstatut,tr("Non validé"),Qt::EditRole);
    m_modelfacture->setData(indexpaymentprestation,tr("Espèces"),Qt::EditRole);
    tableView->openPersistentEditor(indexprestation);
    tableView->openPersistentEditor(indexpatient);
}

void Facture::delRow()
{
    QItemSelectionModel *model = tableView->selectionModel();
    QList<QModelIndex> listofindexes = model->selectedIndexes();
    int numberofselectedrows = listofindexes.count();
    int minrow = 0;
    QStringList listOfRows;
    if (numberofselectedrows < 1)
    {
          QMessageBox::warning(0,tr("Attention"),tr("Veuillez sélectionner un item."),QMessageBox::Ok);
          return;
        }
    for (int i = 0; i < numberofselectedrows;++i)
    {
        QModelIndex index = listofindexes[i];
        if (!index.isValid())
        {
            QMessageBox::warning(0,tr("Attention"),tr("Veuillez sélectionner une ligne à effacer."),QMessageBox::Ok);
            return;
            }  
        int row = index.row();
        listOfRows << QString::number(row);
        if (i==0)
        {
              minrow = row;
            }
        else
        {
            if (row <= minrow)
            {
                  minrow = row;
                }
            }
        }
        listOfRows.removeDuplicates();
        int countofrowsselected = listOfRows.size();
        if (!m_modelfacture->removeRows(minrow,countofrowsselected))
        {
            qWarning() << __FILE__ << QString::number(__LINE__) << m_modelfacture->lastError().text() ;
            }
}

void Facture::newBill()
{
    listFacturesIdComboBox->clear();
    nameEdit->setText("");
    firstnameEdit->setText("");
    dateEdit->setDate(QDate::currentDate());
    tableView->model()->removeRows(0,tableView->model()->rowCount());
    addrow();
}

void Facture::setdatedelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::DATE_OPERATION,new DateDelegate(this));
    tableView->setItemDelegateForColumn(FacturationSpace::DATE_PAIEMENT,new DateDelegate(this));
    tableView->setItemDelegateForColumn(FacturationSpace::DATE_VALEUR,new DateDelegate(this));
    tableView->setItemDelegateForColumn(FacturationSpace::DATE_VALIDATION,new DateDelegate(this));
}

void Facture::setvaleurdelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::VALEUR,new DoubleSpinBoxDelegate(this));
    tableView->resizeColumnToContents(FacturationSpace::VALEUR);
}

void Facture::setmodepaymentdelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::MODE_PAIEMENT,new ComboPayDelegate(this));
    tableView->resizeColumnToContents(FacturationSpace::MODE_PAIEMENT);
}

void Facture::setbuttondelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::PRESTATION,new PrestationEdit(this,m_db));
    tableView->resizeColumnToContents(FacturationSpace::PRESTATION);    
}

void Facture::setusersdelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::PRATICIEN,new ComboUsersDelegate(this,m_prefuserid,m_db));
    tableView->resizeColumnToContents(FacturationSpace::PRATICIEN);
}

void Facture::setsitedelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::SITE,new ComboSitesDelegate(this));
    tableView->resizeColumnToContents(FacturationSpace::SITE);
}

/*void Facture::setValueToValueDelegate(const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    //QAbstractItemDelegate * delegate = new QAbstractItemDelegate(this);
    FacturationIO io(this);
    int columntopleft = topLeft.column();
    int rowtopleft = topLeft.row();
    int columbottomright = bottomRight.column();
    int rowbottomright = bottomRight.row();
    qDebug() << __FILE__ << QString::number(__LINE__) << " columntopleft =" << QString::number(columntopleft) ;
    if (columntopleft == FacturationSpace::PRESTATION  && columbottomright == FacturationSpace::PRESTATION)
    {
          QString act = m_modelfacture->data(m_modelfacture->index(rowtopleft,FacturationSpace::PRESTATION),Qt::DisplayRole).toString();
          double value = io.getValueFromCcamAct(act);
          qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
          m_modelfacture->setData(m_modelfacture-> index(rowtopleft,FacturationSpace::VALEUR),value,Qt::EditRole);
        }
}*/

void Facture::setstatutpayementdelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::STATUT,new ComboStatutPaymentDelegate(this));
    tableView->resizeColumnToContents(FacturationSpace::STATUT);
}

void Facture::setpatientdelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::PATIENT,new ButtonPatientDelegate(this,m_db,m_programName));
    tableView->resizeColumnToContents(FacturationSpace::PATIENT);
}

void Facture::setpayeurdelegate()
{
    tableView->setItemDelegateForColumn(FacturationSpace::PAYEUR,new ComboPayeursDelegate(this));
    tableView->resizeColumnToContents(FacturationSpace::PAYEUR);
}


void Facture::record()
{
    QString factureid = listFacturesIdComboBox->currentText();
    if (!m_io->record(m_modelfacture,factureid,m_programName))
    {
          QMessageBox::warning(0,tr("Warning"),tr("Impossible d'enregistrer la facture"),QMessageBox::Ok);
        }
    else
    {
        if (listFacturesIdComboBox->count()>0)
        {
              listFacturesIdComboBox->clear();
            }
        QStringList listIndexFactures;
        listIndexFactures = setFacturesIdComboBox();            
        listFacturesIdComboBox->addItems(listIndexFactures);
        listFacturesIdComboBox->setCurrentIndex(listIndexFactures.count()-1);
        QMessageBox::information(0,tr("Information"),tr("La facture a été enregistrée"),QMessageBox::Ok);
        }
}

void Facture::deleteBill()
{
    QString idfacture = listFacturesIdComboBox->currentText();
    if (!m_io->deleteBill(idfacture))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "impossible de supprimer la facture" ;
          QMessageBox::warning(0,tr("Warn"),tr("impossible de supprimer la facture"),QMessageBox::Ok);
        }
    else
    {
        if (listFacturesIdComboBox->count()>0)
        {
              listFacturesIdComboBox->clear();
            }
        listFacturesIdComboBox->addItems(setFacturesIdComboBox());
        QMessageBox::information(0,tr("Information"),tr("La facture a été supprimée."),QMessageBox::Ok);
        }
}

QStringList Facture::setFacturesIdComboBox()
{
    return m_io->listOfFacturesId();
}

void Facture::setFactureFromId(const QString & factureid)
{
    m_modelfacture = m_io->getModelFromFactureId(factureid);
    setTableView();
    for (int row  = 0; row <m_modelfacture->rowCount() ;++row )//to avoid sigfault
    {
        QModelIndex indexpatient = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::PATIENT);
        QModelIndex indexprestation = m_modelfacture->index(m_modelfacture->rowCount()-1,FacturationSpace::PRESTATION);
        tableView->openPersistentEditor(indexprestation);
        tableView->openPersistentEditor(indexpatient);
        }
}

void Facture::printBill(){
  QString   pHeadText = "tete";
  QString   pFootText = "foot";
  QString totalDoc = documentToPrint();
  //-------head------------------
  if (m_user.isEmpty())
  {
        m_user = "admin";
      }
  QStringList headList;
  QFile fileHead(Common::ICore::instance()->settings()->applicationResourcesPath()+"/txt/"+m_user+"_headUtf8.txt");
  if(!fileHead.open(QIODevice::ReadOnly)){
     QMessageBox::warning(0,tr("Erreur"),trUtf8("headUtf8.txt ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream streamHead(&fileHead);
    while(!streamHead.atEnd()){
       QString ligneH = streamHead.readLine();
       headList        << ligneH+"<br/>";
    }
  pHeadText = headList.join("<br/>");
  //-------foot---------------
  QStringList listePied;
  QFile fileFoot(Common::ICore::instance()->settings()->applicationResourcesPath()+"/txt/"+m_user+"_footUtf8.txt");
    if(!fileFoot.open(QIODevice::ReadOnly)){
        QMessageBox::warning(0,tr("Erreur"),m_user+trUtf8("_footUtf8.txt ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream streamFoot(&fileFoot);
    while(!streamFoot.atEnd()){
       QString lignepied = streamFoot.readLine();
       listePied        << lignepied+"<br/>";
    }
  pFootText                = listePied.join("<br/>");
  QPrinterEasy pe;
  pe.askForPrinter();
  pe.setHeader(pHeadText);
  pe.setContent(totalDoc);
  pe.setFooter(pFootText);
  pe.previewDialog();
}

QString Facture::documentToPrint()
{
    QString docHtml;
    docHtml = "<html><body><font size=3>";
    docHtml += "<TABLE BORDER=1>";
    docHtml += "<CAPTION><font color = red size = 6>"+trUtf8("Facture.")+"</font></CAPTION>";
    docHtml += "<TR>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Nom,Prenom")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Praticien")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Site")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Payeur")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Date d''opération")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Date de paiement")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Date de valeur")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Date de validation")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Prestation")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Valeur")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Mode de paiement")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Statut")+"</bold></font></TH>";
    docHtml += "</TR>";    
 /*     FacturationSpace::PATIENT,
        FacturationSpace::PRATICIEN,
        FacturationSpace::SITE,
        FacturationSpace::PAYEUR,
        FacturationSpace::DATE_OPERATION,
        FacturationSpace::DATE_PAIEMENT,
        FacturationSpace::DATE_VALEUR,
        FacturationSpace::DATE_VALIDATION, 
        FacturationSpace::PRESTATION,
        FacturationSpace::VALEUR,
        FacturationSpace::MODE_PAIEMENT,
        FacturationSpace::STATUT,
        FacturationSpace::FacturationHeadersModel_MaxParam*/ 
    int numberOfRows = m_modelfacture->rowCount();
    for(int r = 0 ; r < numberOfRows ; r++)
    {
        docHtml += "<TR>";
        for (int col = 0; col < FacturationSpace::FacturationHeadersModel_MaxParam; ++col)
        {
            QModelIndex index = m_modelfacture->index(r,col,QModelIndex());
            QString dataMap = m_modelfacture->data(index).toString();
            docHtml += "<TD>"+dataMap+"</TD>";
            }
        docHtml += "</TR>";
          }
    return docHtml;
}

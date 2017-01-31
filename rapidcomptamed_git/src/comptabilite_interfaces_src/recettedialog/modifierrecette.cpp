#include "modifierrecette.h"
#include "../../recettestriees/etatdesdus/etatdesdus.h"
#include <common/configure/pratid.h>
#include <common/constants.h>
#include <common/tablesdiagrams.h>
#include <common/settings.h>
#include <common/icore.h>
#include <common/commandlineparser.h>
#include <common/print/qprintereasy.h>
#include <common/configure/util.h>

#include <QtGui>
#include <QtCore>

using namespace Common;
using namespace Constants;
using namespace TablesDiagramsHonoraires;
using namespace RapidModelNamespace;

static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

modifierrecette * modifierrecette::m_singleton = NULL;


modifierrecette::~modifierrecette() {
    if(m_singleton){
        m_singleton = NULL;
       }
}

modifierrecette::modifierrecette(){}

void modifierrecette::executeAndShow(){
  m_accountancyDb = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  m_typeOfMoney = Common::ICore::instance()->settings()->value(Constants::S_TYPE_OF_MONEY).toString();
  m_test = settings()->value(TEST).toBool();
  setupUi(this);
  //m_modelHonoraires = new customModel(this,m_accountancyDb);
 // m_modelHonoraires->setTable("honoraires");  
  //m_modelSeances = new customModel(this,m_accountancyDb);
  //m_modelSeances->setTable("seances");
  m_modelfiltre = new customModel(this,m_accountancyDb);
  m_rapidmodel = new RapidModel(this);
  beginDateEdit->setDate(QDate::currentDate());
  endDateEdit->setDate(QDate::currentDate());
  setAttribute(Qt::WA_DeleteOnClose);
  lineEdit   -> setWhatsThis(trUtf8("Vous pouvez écrire ici le nom d'un patient.\nModulo (%) est le caractère jocker."));
  pushButton -> setWhatsThis(trUtf8("Lance le logiciel de gestion des dus."));
  pushButton_5 -> setToolTip(trUtf8("Pour l'aide cliquez sur ce bouton,\ndéplacez la souris sur la zone voulue,\ncliquez "
                                    "pour afficher l'aide."));
  sumsLabel->setWordWrap(true);
  sumsLabel->setText("");
  radioButton->setChecked(true);
  honorairesIsChecked();
  if(m_test)
  qDebug() << __FILE__ << " " << QString::number(__LINE__) << " doc = " <<  m_documentTotal ;
  //construction comboBox "selon"
  QStringList usersList;
  QStringList sitesList;
  usersList = getUsersDatas();
  if (m_test)
    qDebug() << __FILE__ << QString::number(__LINE__) << " usersList =" << usersList.join(";") ;
  sitesList = getSitesDatas();
  usersComboBox->setEditable(false);
  usersComboBox->setInsertPolicy(QComboBox::NoInsert);
  usersComboBox->addItems(usersList);
  sitesComboBox->setEditable(false);
  sitesComboBox->setInsertPolicy(QComboBox::NoInsert);
  sitesComboBox->addItems(sitesList);
  //rechercher();
  connect(pushButton,   SIGNAL(pressed()),this,SLOT(lancerGestionDus()));
  connect(pushButton_2, SIGNAL(pressed()),this,SLOT(effacer()));
  connect(pushButton_3, SIGNAL(pressed()),this,SLOT(close()));
  connect(searchButton, SIGNAL(pressed()),this,SLOT(rechercher()));
  connect(pushButton_5, SIGNAL(pressed()),this,SLOT(setWhatsThisMode()));
  connect(pushButton_6, SIGNAL(pressed()),this,SLOT(imprimer()));
  connect(radioButton,  SIGNAL(clicked()),this,SLOT(honorairesIsChecked()));
  connect(seancesRadioButton,SIGNAL(clicked()),this,SLOT(seancesIsChecked()));
  connect(tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(sortTable(int,Qt::SortOrder)));   
  connect(usersComboBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(changeUser(const QString &)));
  
  show();
}

void modifierrecette::setDatas(const QString & guid, const QString & user,const QString & site){
    m_user = user;
    m_site = site;
    m_guid = guid;
    if(m_test)
    qDebug() << __FILE__ << " " << QString::number(__LINE__)
             << " m_user = "+m_user
             << " m_site = "+m_site
             << " m_guid = "+m_guid;
}

void modifierrecette::lancerGestionDus(){
  QString nom    = dataIn()->value(Common::CommandLine::CL_PatientName).toString();
  QString prenom = dataIn()->value(Common::CommandLine::CL_PatientFirstName).toString();
  etatdesdus *dus = new etatdesdus(nom,prenom,m_guid,m_user);
              dus-> show();
}

void modifierrecette::effacer()
{
    QString textOnLabelFive ;
    QModelIndex index = tableView->currentIndex();
    if(!index.isValid()){
      QMessageBox::warning(0,tr("Erreur"),tr("Veuillez sélectionner une ligne à effacer."),QMessageBox::Ok);
      qWarning() << __FILE__ << QString::number(__LINE__) << "index = " 
                 << QString::number(index.row()) << ":" << QString::number(index.column());
      rechercher();
      return;
      }
    int i = index.row();
    if (tableView->model() == m_modelfiltre)
    {
        if(m_modelfiltre->removeRows(i,1,QModelIndex()))
        {
            coloringDoubles();
            textOnLabelFive = textOfSums(m_modelfiltre);
            sumsLabel->setText(textOnLabelFive);
            QMessageBox::information(0,tr("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
            }
        else
        {
            QMessageBox::warning(0,tr("Attention"),trUtf8("Impossible d'effacer la ligne !"),QMessageBox::Ok);
            return;
            }
        rechercher();
        }
    else if (tableView->model()== m_rapidmodel)
    {
          if (m_rapidmodel->removeRows(i,1,QModelIndex()))
          {
                //m_rapidmodel->submit();
                QMessageBox::information(0,tr("Information"),trUtf8("La ligne rapid a été effacée."),QMessageBox::Ok);
              }
        else
        {
            QMessageBox::warning(0,tr("Attention"),trUtf8("Impossible d'effacer la ligne !"),QMessageBox::Ok);
            return;
            }
        }

    else {QMessageBox::warning(0,tr("Erreur"),trUtf8("Strange thing"),QMessageBox::Ok);}
    
}
///////////////////////////////////////////////////////////////////////////////
//---------------TRI ET RECHERCHE----------------------------------------------
//////////////////////////////////////////////////////////////////////////////

void modifierrecette::rechercher()
{
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  m_documentTotal = "";
  QString champ;
  QString filtre;
  QString filtremodulo;
  filtre = lineEdit->text();
  if(filtre == ""){
      filtremodulo = "%";
      }
  else filtremodulo = filtre+"%";

  QDate datedebut = beginDateEdit->date();
  QString datedebutstring = datedebut.toString("yyyy-MM-dd");
  QDate datefin = endDateEdit->date();
  QString datefinstring = datefin.toString("yyyy-MM-dd");
  //la base filtrée
  if(radioButton->isChecked()){
      m_modelfiltre->setTable(TABLE_HONORAIRES);
      champ = "patient";
      }
  else if (seancesRadioButton->isChecked()){
      m_modelfiltre->setTable(TABLE_SEANCES);
      }
  else {QMessageBox::warning(0,tr("Erreur"),trUtf8("Strange thing ")+QString(__FILE__)+" "+trUtf8("line = ")
        +QString::number(__LINE__),QMessageBox::Ok);
        }
  QString datasUser = usersComboBox->currentText();
  QStringList datasUserList = datasUser.split(",");
  QString idusr;
  if (datasUserList.size()>0)
  {
        idusr = datasUserList[0];
      }
  QString datasSites = sitesComboBox->currentText();
  QString idsite;
  idsite = m_hashsites.key(datasSites);
  //le code du filtre
  QString filtremodel ;
  filtremodel = QString("%1 like '%2' and %1 not like '%3%' and date between '%4' and '%5'").
                arg(champ,
                    filtremodulo,
                    QString::number(0),
                    datedebutstring,
                    datefinstring);
  filtremodel += QString(" and %1 = '%2'").arg("id_usr",idusr);
  filtremodel += QString(" and %1 = '%2'").arg("id_site",idsite);
  if (m_test)
  qWarning() << __FILE__ << QString::number(__LINE__) << " filter  =" << filtremodel ;
  m_modelfiltre->setFilter(filtremodel);
  if (!security()->isUserLocked(m_user))
  {
  	  m_modelfiltre->select();
  	  if (m_modelfiltre->rowCount()>=settings()->value(MAX_COLORED_MODEL_ROWS).toInt())
  	  {
  	       qWarning() << __FILE__ << QString::number(__LINE__) << "m_modelfiltre->canFetchMore(QModelIndex())" ;
  	       resetTableViewForHonoraires();
  	       getRapidModel(filtremodel);
  	       QApplication::restoreOverrideCursor();  	       
  	       return;  	       
  	      }
  	  else
  	  {
  	      tableView->setModel(m_modelfiltre);
  	      setTableViewForHonoraires();
  	      coloringDoubles();
  	      }  	  

      }

  QStringList m_listechamps;
  int numberOfRows = 0;
  if(m_modelfiltre->tableName().contains(TABLE_HONORAIRES)){
      prepareTextOfSums();
      QString textOfLabel = textOfSums(m_modelfiltre);
      sumsLabel->setText(textOfLabel);
      }
  if(m_modelfiltre-> tableName() . contains(TABLE_SEANCES)){//FIXME
      m_mapColumns.clear();
      m_documentTotal = headDocumentToPrint();
      tableView->setColumnHidden(ID_PAIEMENT,true);
      tableView->setColumnHidden(SEANCES_ID_HONO,true);
      tableView->setColumnHidden(SEANCES_ID_PAYEURS,true);
      tableView->setColumnHidden(SEANCES_AUTRE,true);
      //recuperer donnees pour impression
       m_mapColumns.insert(SEANCES_DATE,"date");
       m_mapColumns.insert(SEANCES_ACTE,"acte");
       m_mapColumns.insert(SEANCES_ESP,"esp");
       m_mapColumns.insert(SEANCES_CHQ,"chq");
       m_mapColumns.insert(SEANCES_CB,"cb");
       m_mapColumns.insert(SEANCES_DAF,"daf");
       m_mapColumns.insert(SEANCES_DU,"du");
       numberOfRows = m_modelfiltre->rowCount();
      for(int r = 0 ; r < numberOfRows ; r++){
          m_documentTotal += "<TR>";
          QMapIterator<int,QString> iMap(m_mapColumns);
          while(iMap.hasNext()){
              iMap.next();
              QModelIndex index = m_modelfiltre->index(r,iMap.key(),QModelIndex());
              QString dataMap = m_modelfiltre->data(index).toString();
              m_documentTotal += "<TD>"+dataMap+"</TD>";                  
              }
          m_documentTotal += "</TR>";
          }
      }
  tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
  tableView->horizontalHeader()->setStretchLastSection ( true );
  tableView->resizeColumnsToContents();
  QApplication::restoreOverrideCursor();
}


void modifierrecette::errorMessage(QString & error)
{
    QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("Erreur lancement processus :")+error,QMessageBox::Ok);
}

void modifierrecette::setWhatsThisMode(){
    QWhatsThis::enterWhatsThisMode();
}

void modifierrecette::imprimer(){
  QString   pHeadText = "tete";
  QString   pFootText = "foot";
  //-------tete------------------
  QStringList headList;
  QFile fileHead(Common::ICore::instance()->settings()->applicationResourcesPath()+"/txt/"+m_user+"_headUtf8.txt");
  if(m_test)
  qDebug() << __FILE__ << " " << QString::number(__LINE__) << " "
           << Common::ICore::instance()->settings()->applicationResourcesPath()+"/txt/"+m_user+"_headUtf8.txt";
  if(!fileHead.open(QIODevice::ReadOnly)){
     QMessageBox::warning(0,tr("Erreur"),trUtf8("headUtf8.txt ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream streamHead(&fileHead);
    while(!streamHead.atEnd()){
       QString ligneH = streamHead.readLine();
       headList        << ligneH+"<br/>";
    }
  pHeadText = headList.join("<br/>");

  //-------pied---------------
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
  pe.setContent(m_documentTotal);
  pe.setFooter(pFootText);
  pe.previewDialog();
}

void modifierrecette::honorairesIsChecked()
{
    //rechercher();//FIXME
}

void modifierrecette::seancesIsChecked()
{
    rechercher();
}

QString modifierrecette::textOfSums(customModel * model){
    QString labelText;
    QString labelTextStr;
    double esp = 0.00;
    double chq = 0.00;
    double visa = 0.00;
    double vir = 0.00;
    double dus = 0.00;
    double daf = 0.00;
    double totalRecu = 0.00;
    double totaux = 0.00;
    int modelRowCount = model->rowCount(QModelIndex());
    if(m_test)
     qDebug() << __FILE__ << QString::number(__LINE__) << " modelRowCount = " << QString::number(modelRowCount);
    for(int i = 0; i < modelRowCount ; i ++){
       QSqlRecord rowRecord = model->record(i);//ligne d'enregistrement
       esp  += rowRecord.value(TablesDiagramsHonoraires::ESP).toDouble();
       chq  += rowRecord.value(TablesDiagramsHonoraires::CHQ).toDouble();
       visa += rowRecord.value(TablesDiagramsHonoraires::CB).toDouble();
       vir  += rowRecord.value(TablesDiagramsHonoraires::VIR).toDouble();
       daf  += rowRecord.value(TablesDiagramsHonoraires::DAF).toDouble();
       dus  += rowRecord.value(TablesDiagramsHonoraires::DU).toDouble();
       }
    totaux = esp + chq + visa + vir + dus + daf;
    totalRecu = esp + chq + visa+vir;
    QString totauxStr = "<font size = 3 color = ""blue"">Totaux = </font><font size = 3 color = ""red"">"+QString::number(totaux)+" "+m_typeOfMoney+" </font><br/>";
    QString totRecus = "<font size = 3 color = ""blue"">Totaux re&#231;us = </font><font size = 3 color = ""red"">"+QString::number(totalRecu)+" "+m_typeOfMoney+" </font><br/>";
    QString sumsStr = "<font size = 3 color = ""blue"">Esp = </font><font size = 3 color = ""red"">"+QString::number(esp)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Chq = </font><font size = 3 color = ""red"">"+QString::number(chq)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">CB = </font><font size = 3 color = ""red"">"+QString::number(visa)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">VIR = </font><font size = 3 color = ""red"">"+QString::number(vir)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Tiers-payants = </font><font size = 3 color = ""red"">"+QString::number(daf)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Dus = </font><font size = 3 color = ""red"">"+QString::number(dus)+" "+m_typeOfMoney+"</font>";
    labelTextStr = totauxStr+totRecus+sumsStr;
    labelText = "<html><body>"+labelTextStr+"</body></html>";
    return labelText;
}

QString modifierrecette::textOfSums(RapidModel * model)
{
    QString labelText;
    QString labelTextStr;
    double esp = 0.00;
    double chq = 0.00;
    double visa = 0.00;
    double vir = 0.00;
    double dus = 0.00;
    double daf = 0.00;
    double totalRecu = 0.00;
    double totaux = 0.00;
    int modelRowCount = model->rowCount(QModelIndex());
    if(m_test)
     qDebug() << __FILE__ << QString::number(__LINE__) << " modelRowCount = " << QString::number(modelRowCount);
    for(int row = 0; row < modelRowCount ; ++row)
    {
       esp  += model->data(model->index(row,RM_ESP),Qt::DisplayRole).toDouble();
       chq  += model->data(model->index(row,RM_CHQ),Qt::DisplayRole).toDouble();
       visa += model->data(model->index(row,RM_CB),Qt::DisplayRole).toDouble();
       vir  += model->data(model->index(row,RM_VIR),Qt::DisplayRole).toDouble();
       daf  += model->data(model->index(row,RM_DAF),Qt::DisplayRole).toDouble();
       dus  += model->data(model->index(row,RM_DU),Qt::DisplayRole).toDouble();
       }
    totaux = esp + chq + visa + vir + dus + daf;
    totalRecu = esp + chq + visa+vir;
    QString totauxStr = "<font size = 3 color = ""blue"">Totaux = </font><font size = 3 color = ""red"">"+QString::number(totaux)+" "+m_typeOfMoney+" </font><br/>";
    QString totRecus = "<font size = 3 color = ""blue"">Totaux re&#231;us = </font><font size = 3 color = ""red"">"+QString::number(totalRecu)+" "+m_typeOfMoney+" </font><br/>";
    QString sumsStr = "<font size = 3 color = ""blue"">Esp = </font><font size = 3 color = ""red"">"+QString::number(esp)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Chq = </font><font size = 3 color = ""red"">"+QString::number(chq)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">CB = </font><font size = 3 color = ""red"">"+QString::number(visa)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">VIR = </font><font size = 3 color = ""red"">"+QString::number(vir)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Tiers-payants = </font><font size = 3 color = ""red"">"+QString::number(daf)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">Dus = </font><font size = 3 color = ""red"">"+QString::number(dus)+" "+m_typeOfMoney+"</font>";
    labelTextStr = totauxStr+totRecus+sumsStr;
    labelText = "<html><body>"+labelTextStr+"</body></html>";
    return labelText;
}

void  modifierrecette::coloringDoubles()
{
    int rowCount = m_modelfiltre->rowCount();
    int columnCount =m_modelfiltre->columnCount();
    if(m_test)
    qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(rowCount) << " " << QString::number(columnCount);
    QStringList listRowsStrings;
    QStringList listRowsSamePatientsString;
    for (int i = 0; i < rowCount; ++i) //each row
    {
    	QList<QVariant> dataRow ;
    	QStringList dataRowString;
        QVariant patientName;
        QVariant guid;
        QModelIndex indexPatient = m_modelfiltre->index(i,PATIENT,QModelIndex());
    	patientName = m_modelfiltre->data(indexPatient,Qt::DisplayRole);
    	QModelIndex indexguid = m_modelfiltre->index(i,TablesDiagramsHonoraires::GUID,QModelIndex());
    	guid = m_modelfiltre->data(indexguid,Qt::DisplayRole);
    	for (int c = ID_USR; c < columnCount; ++c) //all the row
    	{
    	    QModelIndex index = m_modelfiltre->index(i,c,QModelIndex());
    	    dataRow << m_modelfiltre->data(index,Qt::DisplayRole);
    		dataRowString << m_modelfiltre->data(index,Qt::DisplayRole).toString();
    	    }
    	   if(m_test)
           qDebug() << __FILE__ << QString::number(__LINE__) << " dataRow =" << dataRowString.join(",");
    	for (int j = i; j < rowCount ; ++j) //rows after
    	{
    	    if(j!=i)
    	    {
    		    QList<QVariant> dataAfterRow ;
    		    QStringList dataAfterRowString;
    		    QVariant patientNameAfter;
    		    QModelIndex indexPatientAfter = m_modelfiltre->index(j,PATIENT,QModelIndex());
    		    patientNameAfter = m_modelfiltre->data(indexPatientAfter,Qt::DisplayRole);
    		    if(m_test)
                    qDebug() << __FILE__ << QString::number(__LINE__) << " patientNameAfter =" << patientNameAfter.toString() ;
    	        for (int c = ID_USR; c < columnCount; ++c)
    	        {
    	            QModelIndex indexAfter = m_modelfiltre->index(j,c,QModelIndex());
    		        dataAfterRow << m_modelfiltre->data(indexAfter,Qt::DisplayRole);
    		        dataAfterRowString << m_modelfiltre->data(indexAfter,Qt::DisplayRole).toString();
    	            }
    	        if (dataAfterRow == dataRow)
    	        {
    	            listRowsStrings << guid.toString();
    	            }
    	         else if (patientName == patientNameAfter)
    	         {
    	             listRowsSamePatientsString << guid.toString();
                     }
    	         else{}
    	         }
    	    }

        }//for
    m_modelfiltre->getDoublesRows = listRowsStrings;
    m_modelfiltre->getDoublesNames = listRowsSamePatientsString;
    m_modelfiltre->submit();
}

QString modifierrecette::headDocumentToPrint()
{
    QString docHtml;
    docHtml = "<html><body><font size=3>";
    docHtml += "<TABLE BORDER=1>";
    docHtml += "<CAPTION><font color = red size = 6>"+trUtf8("Extrait des recettes.")+"</font></CAPTION>";
    docHtml += "<TR>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Nom,Prenom")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Date")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Especes")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Cheques")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Cartes de credit")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Virements")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Dispense avance des frais")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Dus")+"</bold></font></TH>";
    docHtml += "</TR>";
    
    return docHtml;
}

QStringList modifierrecette::getUsersDatas()
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    enum e{IDUSR=0,NOMUSER,LOGINUSER};
    QString req = QString("SELECT %1,%2,%3 FROM %4").arg("id_usr","nom_usr","login","utilisateurs");
    QSqlQuery q(db);
    if(!q.exec(req)){
       qWarning() << __FILE__ << QString::number(__LINE__) << " " << q.lastError().text();
       }
    QStringList list;
    while(q.next()){
      QString id = q.value(IDUSR).toString();
      QString nom = q.value(NOMUSER).toString();
      QString login = q.value(LOGINUSER).toString();
      list << id+","+nom+","+login;
      qDebug() << __FILE__ << QString::number(__LINE__) << " list users =" << id+","+nom+","+login ;
      }
    list.prepend(getUsersDatas(settings()->value(S_ID_UTILISATEUR_CHOISI).toString()).join(","));
    list.removeDuplicates();
    qDebug() << __FILE__ << QString::number(__LINE__) << " list users =" << list.join(":") ;
    return list;
}

QStringList modifierrecette::getUsersDatas(const QString & iduserpref)
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    enum e{IDUSR=0,NOMUSER,LOGINUSER};
    QString req = QString("SELECT %1,%2,%3 FROM %4 ").arg("id_usr","nom_usr","login","utilisateurs");
    if (!iduserpref.isEmpty())
    {
          req += QString("where %1 = '%2'").arg("id_usr",iduserpref);
        }
    QSqlQuery q(db);
    if(!q.exec(req)){
       qWarning() << __FILE__ << QString::number(__LINE__) << " " << q.lastError().text();
       }
    QStringList list;
    while(q.next()){
      QString id = q.value(IDUSR).toString();
      QString nom = q.value(NOMUSER).toString();
      QString login = q.value(LOGINUSER).toString();
      list << id+","+nom+","+login;
      }
     return list;
}

QStringList modifierrecette::getSitesDatas()
{
    enum e{IDSITE=0,SITE};
    QString idsitepref = settings()->value(S_ID_SITE_PREFERENTIEL).toString();
    QStringList list;
    QSqlQuery qSite(m_accountancyDb);
    if (!qSite.exec("SELECT id_site,site FROM sites"))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qSite.lastError().text() ;
        }
     while(qSite.next()){
          QString idSite = qSite.value(IDSITE).toString();
          QString site = qSite.value(SITE).toString();
          list << site;
          m_hashsites.insert(idSite,site);
          }
      QSqlQuery qyPrefSite(m_accountancyDb);
      QString reqPrefsite = QString("select %1,%2 from %3 where %4 = '%5'").arg("id_site","site","sites","id_site",idsitepref);
      if (!qyPrefSite.exec(reqPrefsite))
      {
            qWarning() << __FILE__ << QString::number(__LINE__) << qyPrefSite.lastError().text() ;
          }
      while (qyPrefSite.next())
      {
          list .prepend(qyPrefSite.value(SITE).toString());
          }
      list.removeDuplicates();
      return list;
}

void modifierrecette::sortTable(int column,Qt::SortOrder order)
{
    m_modelfiltre->sort(column,order);
}

void modifierrecette::changeUser(const QString & text)
{
    enum ChangeUserEnum {IDUSER=0,NAME,LOGIN};
    QStringList ltext;
    if (text.isEmpty())
    {
          QMessageBox::warning(0,tr("Erreur"),tr("Mauvaise configuration de l'utilisateur."),QMessageBox::Ok);
          return;
        }
    ltext = text.split(",");
    if (ltext.size()<3)
    {
          QMessageBox::warning(0,tr("Erreur"),tr("Mauvaise configuration de l'utilisateur ou absence de login."),QMessageBox::Ok);
          return;
        }
    m_user = ltext[LOGIN];
}

void modifierrecette::getRapidModel(const QString & filter)
{
    m_rapidmodel->setRapidModelNotReady();
    if (m_rapidmodel->rowCount(QModelIndex())>0)
    {
         if (!m_rapidmodel->clear())
          {
                qWarning() << __FILE__ << QString::number(__LINE__) << "unable to clear RapidModel" ;
              }
           tableView->verticalScrollBar ()->setValue(0);
        }
    QHash<int,QString> hashColumns;
    hashColumns = Constants::modifRecettesColumnsHash();
    QString columnsstring;
    QStringList listcolumns;
    for ( int i = 0; i < RapidModelEnum_MaxParam; ++ i)
    {
          listcolumns << hashColumns.value(i);
        }
    columnsstring = listcolumns.join(",");
    QSqlQuery qy(m_accountancyDb);
    QString req = QString("select %1 from %2 where %3").arg(columnsstring,"honoraires",filter);
    if (!qy.exec(req))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        }
    int row = 0;
    while (qy.next())
    {      
        QVariant value = QVariant();
        //qDebug() << __FILE__ << QString::number(__LINE__);
        if (!m_rapidmodel->insertRows(row,1))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "unable to insert row" ;
              return;
            }
        for ( int col = 0; col < RapidModelEnum_MaxParam; ++ col)
        {
              //qDebug() << __FILE__ << QString::number(__LINE__) << "row & col =" << QString::number(row) << ":" << QString::number(col) ;
              value = qy.value(col);
              if (value.isNull())
              {
                    value = QVariant("NULL");
                  }
              //qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value.toString() ;
              if (!m_rapidmodel->setData(m_rapidmodel->index(row,col),value,Qt::EditRole))
              {
                    qWarning() << __FILE__ << QString::number(__LINE__) << "unable to set data in big model at row & col " << QString::number(row) << QString::number(col);
                  }
            }
        //qDebug() << __FILE__ << QString::number(__LINE__);
        if (!m_rapidmodel->submit())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "unable to submit value" ;
            }
        //qDebug() << __FILE__ << QString::number(__LINE__);
        ++row;
        } 
        //qDebug() << __FILE__ << QString::number(__LINE__);
    if (!m_rapidmodel->submit())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << " unable to submit m_rapidmodel "  ;
        }
    //qDebug() << __FILE__ << QString::number(__LINE__) << " rapidecomptabilite rowCount =" << QString::number(m_rapidmodel->rowCount(QModelIndex())) ;
    if (m_rapidmodel)
    {
          if (tableView)
          {
               tableView->setModel(m_rapidmodel);
              }          
        }
    setTableViewHeadersForLargeAmounts(m_rapidmodel);         
    QString textOfLabel = textOfSums(m_rapidmodel);//FIXME
    sumsLabel->setText(textOfLabel);
    m_rapidmodel->setRapidModelReady();
}

void modifierrecette::setTableViewForHonoraires()
{
      tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
      tableView->setSelectionMode(QAbstractItemView::SingleSelection);
      tableView->setSortingEnabled(true);
      tableView->setColumnHidden(ID_HONO,true);
      tableView->setColumnHidden(ID_USR,true);
      tableView->setColumnHidden(ID_DRTUX_USR,true);
      tableView->setColumnHidden(ID_SITE,true);
      tableView->setColumnHidden(ID_PAYEURS,true);
      tableView->setColumnHidden(ACTECLAIR,true);
      tableView->setColumnHidden(AUTRE,true);
      tableView->setColumnHidden(DU_PAR,true);
      tableView->setColumnHidden(VALIDE,true); 
      tableView->resizeColumnsToContents();
}

void modifierrecette::resetTableViewForHonoraires()
{
      tableView->setSortingEnabled(true);
      tableView->setColumnHidden(ID_HONO,false);
      tableView->setColumnHidden(ID_USR,false);
      tableView->setColumnHidden(ID_DRTUX_USR,false);
      tableView->setColumnHidden(ID_SITE,false);
      tableView->setColumnHidden(ID_PAYEURS,false);
      tableView->setColumnHidden(ACTECLAIR,false);
      tableView->setColumnHidden(AUTRE,false);
      tableView->setColumnHidden(DU_PAR,false);
      tableView->setColumnHidden(VALIDE,false); 
      tableView->resizeColumnsToContents();
}


void modifierrecette::setTableViewHeadersForLargeAmounts(RapidModel * model)
{
    QHash<int,QString> hash;
    hash = modifRecettesTitlesColumnsHash();    
    for ( int headercolumn = 0; headercolumn < RapidModelEnum_MaxParam; ++ headercolumn)
    {
          QString value = hash.value(headercolumn);
          model->setHeaderData(headercolumn, Qt::Horizontal, value, Qt::EditRole);
        }
}

void modifierrecette::prepareTextOfSums()
{
      m_mapColumns.clear();
      m_documentTotal = headDocumentToPrint();
      //recuperer les donnees pour l'impression
      m_mapColumns.insert(TablesDiagramsHonoraires::PATIENT,"Nom,Prenom");
      m_mapColumns.insert(TablesDiagramsHonoraires::DATE,"Date");
      m_mapColumns.insert(TablesDiagramsHonoraires::ESP,"esp");
      m_mapColumns.insert(TablesDiagramsHonoraires::CHQ,"chq");
      m_mapColumns.insert(TablesDiagramsHonoraires::CB,QString("CB"));
      m_mapColumns.insert(TablesDiagramsHonoraires::VIR,QString("VIR"));
      m_mapColumns.insert(TablesDiagramsHonoraires::DAF,QString("DAF"));
      m_mapColumns.insert(TablesDiagramsHonoraires::DU,"Du");
      int numberOfRows = m_modelfiltre->rowCount();
      for(int r = 0 ; r < numberOfRows ; r++){
          m_documentTotal += "<TR>";
          QMapIterator<int,QString> iMap(m_mapColumns);
          while(iMap.hasNext()){
              iMap.next();
              QModelIndex index = m_modelfiltre->index(r,iMap.key(),QModelIndex());
              QString dataMap = m_modelfiltre->data(index).toString();
              m_documentTotal += "<TD>"+dataMap+"</TD>";
              }
          m_documentTotal += "</TR>";
          }
}



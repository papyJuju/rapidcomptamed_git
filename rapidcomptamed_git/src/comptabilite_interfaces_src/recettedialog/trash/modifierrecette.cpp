#include "modifierrecette.h"
#include "../../recettestriees/etatdesdus/etatdesdus.h"
#include "../../common/configure/pratid.h"
#include <common/constants.h>
#include <common/settings.h>
#include <common/icore.h>
#include <common/commandlineparser.h>
#include <common/print/qprintereasy.h>
#include <common/configure/util.h>

#include <QtGui>
#include <QtCore>

using namespace Common;
using namespace Constants;
static inline Common::CommandLine *dataIn(){ return Common::ICore::instance()->commandLine();}
static inline Common::Security * security() {return Common::ICore::instance()->security();}
enum WarnDebugEnum {WarnDebug=false};

modifierrecette * modifierrecette::m_singleton = NULL;



modifierrecette::~modifierrecette() {
  if(m_singleton){
     //delete m_singleton;
     m_singleton = NULL;
  }
}

modifierrecette::modifierrecette(){}

void modifierrecette::executeAndShow(){
  //m_db1 = QSqlDatabase::database(Constants::DB_DRTUX);
  m_accountancyDb = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);

  m_typeOfMoney = Common::ICore::instance()->settings()->value(Constants::S_TYPE_OF_MONEY).toString();
  setupUi(this);
  dateEdit->setDate(QDate::currentDate());
  dateEdit_2->setDate(QDate::currentDate());
  setAttribute(Qt::WA_DeleteOnClose);
  lineEdit   -> setWhatsThis(trUtf8("Vous pouvez écrire ici le nom d'un patient.\nModulo (%) est le caractère jocker."));
  comboBox   -> setWhatsThis(trUtf8("Vous pouvez choisir le type de filtrage"));
  pushButton -> setWhatsThis(trUtf8("Lance le logiciel de gestion des dus."));
  pushButton_5 -> setToolTip(trUtf8("Pour l'aide cliquez sur ce bouton,\ndéplacez la souris sur la zone voulue,\ncliquez "
                                    "pour afficher l'aide."));
  label_5->setWordWrap(true);
  label_5->setText("");
  QString reqIdUsr = QString("SELECT %1 FROM utilisateurs WHERE %2 = '%3'").arg("id_usr","login",m_user);
  QSqlQuery qIdUsr(m_accountancyDb);
  if(!qIdUsr.exec(reqIdUsr)){
      qWarning() << __FILE__ << QString::number(__LINE__) << qIdUsr.lastError().text();
      }
  QStringList listIdUsrTest;
  while(qIdUsr.next()){
      QString str = qIdUsr.value(0).toString();
      listIdUsrTest << str;
      m_id_usr = str;
      }
  if (listIdUsrTest.size()>1)
  {
      qWarning() << __FILE__ << QString::number(__LINE__) << " two id user for the same user !";
  }
  radioButton->setChecked(true);
  if(radioButton->isChecked()){
    emit honorairesIsChecked();
    }
  if(radioButton_2->isChecked()){
    seancesIsChecked();
   }
  if(WarnDebug)
  qDebug() << __FILE__ << " " << QString::number(__LINE__) << " doc = " <<  m_documentTotal ;
  //construction comboBox "selon"
  comboBox->setEditable(false);
  comboBox->setInsertPolicy(QComboBox::NoInsert);
  comboBox->addItems(m_listechamps);

  connect(pushButton,   SIGNAL(pressed()),this,SLOT(lancerGestionDus()));
  connect(pushButton_2, SIGNAL(pressed()),this,SLOT(effacer()));
  connect(pushButton_3, SIGNAL(pressed()),this,SLOT(close()));
  connect(pushButton_4, SIGNAL(pressed()),this,SLOT(rechercher()));
  connect(pushButton_5, SIGNAL(pressed()),this,SLOT(setWhatsThisMode()));
  connect(pushButton_6, SIGNAL(pressed()),this,SLOT(imprimer()));
  connect(radioButton,  SIGNAL(clicked()),this,SLOT(honorairesIsChecked()));
  connect(radioButton_2,SIGNAL(clicked()),this,SLOT(seancesIsChecked()));
  show();
}

void modifierrecette::setDatas(const QString & guid, const QString & user,const QString & site){
    m_user = user;
    m_site = site;
    m_guid = guid;
    if(WarnDebug)
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
  QModelIndex index = tableView->QAbstractItemView::currentIndex();
  if(!index.isValid()){
      QMessageBox::warning(0,tr("Erreur"),tr("Veuillez sélectionner une ligne à effacer."),QMessageBox::Ok);
      return;
      }
  int i = index.row();
  if(tableView->model() == m_modelHonoraires){
      if(m_modelHonoraires->removeRows(i,1,QModelIndex())){
          textOnLabelFive = textOfSums(m_modelHonoraires);
          label_5->setText(textOnLabelFive);
          QMessageBox::information(0,tr("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
      }
  }
  else if(tableView->model() == m_modelSeances){
      if(m_modelSeances->removeRows(i,1,QModelIndex())){
          textOnLabelFive = textOfSums(m_modelSeances);
          label_5->setText(textOnLabelFive);
          QMessageBox::information(0,tr("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
      }
  }
  else if(tableView->model() == m_modelfiltre){
     if(m_modelfiltre->getDoublesRows.contains(i)){
          m_modelfiltre->getDoublesRows.removeAll(i);
          if (m_modelfiltre->getDoublesRows.size()<1)
          {
          	m_modelfiltre->getDoublesRows.clear();
                }
          }
      if(m_modelfiltre->removeRows(i,1,QModelIndex())){
          coloringDoubles();
          textOnLabelFive = textOfSums(m_modelfiltre);
          label_5->setText(textOnLabelFive);
          QMessageBox::information(0,tr("Information"),trUtf8("La ligne a été effacée."),QMessageBox::Ok);
      }
  }
  else {QMessageBox::warning(0,tr("Erreur"),trUtf8("Strange thing"),QMessageBox::Ok);}
}
///////////////////////////////////////////////////////////////////////////////
//---------------TRI ET RECHERCHE----------------------------------------------
//////////////////////////////////////////////////////////////////////////////
void modifierrecette::rechercher()
{
  m_documentTotal = "";
  QString filtre;
  QString filtremodulo;
  filtre = lineEdit->text();
  QString champ = comboBox->currentText();
  if(filtre == ""){
      filtremodulo = "%";
      }
  else filtremodulo = filtre+"%";

  QDate datedebut = dateEdit->date();
  QString datedebutstring = datedebut.toString("yyyy-MM-dd");
  QDate datefin = dateEdit_2->date();
  QString datefinstring = datefin.toString("yyyy-MM-dd");
  //la base filtrée
  QString tableName = "";
  if(radioButton->isChecked()){
      tableName = "honoraires";
      }
  else if (radioButton_2->isChecked()){
      tableName = "seances";
      champ = "acte";
      }
  else {QMessageBox::warning(0,tr("Erreur"),trUtf8("Strange thing ")+QString(__FILE__)+" "+trUtf8("line = ")
        +QString::number(__LINE__),QMessageBox::Ok);
        }

  //le code du filtre
  QString filtremodel ;
  filtremodel = QString("%1 LIKE '%2' AND %1 NOT LIKE '%3%' AND DATE BETWEEN '%4' AND '%5'").
                arg(champ,
                    filtremodulo,
                    QString::number(0),
                    datedebutstring,
                    datefinstring);
  if(comboBox->currentText() == "praticien"){
      QString id_usr;
      pratid *prat = new pratid();
      if(prat->exec() == QDialog::Accepted){
          id_usr = prat->recupPrat();
          m_user = prat->lineEdit->text().split(",")[1];
          if(WarnDebug)
qDebug() << __FILE__ << QString::number(__LINE__) << " id_usr et m_user = " << id_usr << " " << m_user;
      }
      champ = "id_usr";

     /* QSqlQuery qPrat(m_accountancyDb);
                qPrat.exec("SELECT id_usr FROM utilisateurs WHERE login = '"+m_user+"'");
      while(qPrat.next()){
          id_usr = qPrat.value(0).toString();
      }*/
     filtremodel = QString("%1 LIKE '%2' AND DATE BETWEEN '%3' AND '%4'").
                arg(champ,
                    id_usr,
                    datedebutstring,
                    datefinstring);
  }
  if(comboBox->currentText() == "site"){
      champ = "id_site";
      QString idSite;
      QSqlQuery qSite(m_accountancyDb);
                qSite.exec("SELECT id_site FROM sites WHERE site = '"+m_site+"'");
      while(qSite.next()){
          idSite = qSite.value(0).toString();
      }
      filtremodel = QString("%1 LIKE '%2' AND DATE BETWEEN '%3' AND '%4'").
                    arg(champ,
                    idSite,
                    datedebutstring,
                    datefinstring);
  }
  m_modelfiltre = new customModel(this,m_accountancyDb);
  m_modelfiltre->setTable(tableName);
  m_modelfiltre->setFilter(filtremodel);
  if (!security()->isUserLocked(m_user))
  {
  	  m_modelfiltre->select();
  	  coloringDoubles();
  	  tableView->setModel(m_modelfiltre);
      }
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
 // paintCororedItems();
  QStringList m_listechamps;
  int numberOfRows = 0;
  if(tableName == "honoraires"){
      m_mapColumns.clear();
      m_documentTotal = headDocumentToPrint();
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
      tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
      tableView->horizontalHeader()->setStretchLastSection ( true );
      QString textOfLabel = textOfSums(m_modelfiltre);
      label_5->setText(textOfLabel);
      //recuperer les donnees pour l'impression
      m_mapColumns.insert(PATIENT,"Nom,Prenom");
      m_mapColumns.insert(DATE,"Date");
      m_mapColumns.insert(ESP,"esp");
      m_mapColumns.insert(CHQ,"chq");
      m_mapColumns.insert(CB,"CB");
      m_mapColumns.insert(VIR,"VIR");
      m_mapColumns.insert(DAF,"DAF");
      m_mapColumns.insert(DU,"Du");
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
      if(WarnDebug)
      qDebug() << "in docHono =" << m_documentTotal << __LINE__;
      }
  if(tableName == "seances"){
      m_mapColumns.clear();
      m_documentTotal = headDocumentToPrint();
      tableView->setSortingEnabled(true);
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
  tableView->resizeColumnsToContents();

  if(WarnDebug)
   qDebug() << __FILE__ << __LINE__ << "doc = " << m_documentTotal;

}

/*QString modifierrecette::adapteCheminWindows(QString & chemin){
    QString str           = chemin;
    QString nouveauChemin = "";
    QStringList listStr   = str.split("/");
    for(int i = 0;i < listStr.size();i++){
        QString partie    = listStr[i];
        if(listStr[i].contains(" ")){
             partie = "\""+listStr[i]+"\"";
            }
        QString apres = "\\";
        if(i == listStr.size()-1){
            apres     = ".exe";
            }
            nouveauChemin += partie+apres;
        }

    return nouveauChemin;
}*/

void modifierrecette::errorMessage(QString & error){
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
  if(WarnDebug)
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
    /*QPrinter printer;
    QPrintDialog *printdialog = new QPrintDialog(&printer,this);
    printdialog               ->setWindowTitle(tr("Imprimer"));
    printdialog               ->exec();
    if(WarnDebug)
qDebug() << "print"<< __LINE__;
    m_affichage               ->print(&printer);
    if(WarnDebug)
qDebug() << "print"<< __LINE__;*/
}

void modifierrecette::honorairesIsChecked(){
      int numberOfRows = 0;
      m_mapColumns.clear();
      m_documentTotal = "";
      if (!security()->isUserLocked(m_user))
      {
      	  qWarning() << __FILE__ << QString::number(__LINE__) << m_user+" is not locked." ;
      	  m_modelHonoraires->select();
      	  tableView->setModel(m_modelHonoraires);
          }
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
      tableView->setColumnHidden(TRACABILITE,true);
      tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
      tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
      tableView->resizeColumnsToContents();
      QString textOfLabel = textOfSums(m_modelHonoraires);
      label_5->setText(textOfLabel);
      m_listechamps.clear();
      //pour comboBox
      m_listechamps << "patient" << "praticien" << "site" << "remarque" << "esp" << "chq"
                    << "cb" << "daf" << "autre" << "du" << "valide" ;
      //recuperer les donnees pour l'impression
      m_mapColumns.insert(PATIENT,"Nom,Prenom");
      m_mapColumns.insert(DATE,"Date");
      m_mapColumns.insert(ESP,"esp");
      m_mapColumns.insert(CHQ,"chq");
      m_mapColumns.insert(CB,"CB");
      m_mapColumns.insert(VIR,"VIR");
      m_mapColumns.insert(DAF,"DAF");
      m_mapColumns.insert(DU,"Du");
      numberOfRows = m_modelHonoraires->rowCount();
      if(WarnDebug)
      qDebug() << __FILE__ << QString::number(__LINE__) << " total rowcount = " << QString::number(numberOfRows);
      for(int r = 0 ; r < numberOfRows ; r++){
          QString documentStr ;
          QMapIterator<int,QString> iMap(m_mapColumns);
          while(iMap.hasNext()){
              iMap.next();
              QModelIndex index = m_modelHonoraires->index(r,iMap.key(),QModelIndex());
              documentStr += iMap.value()+" = "+m_modelHonoraires->data(index).toString()+"   ";
          }
          m_documentTotal += documentStr+"<br/>";
      }
}

void modifierrecette::seancesIsChecked(){
       int numberOfRows = 0;
       m_mapColumns.clear();
       m_documentTotal = "";
       if (!security()->isUserLocked(m_user))
       {
       	  m_modelSeances->select();
       	  tableView->setModel(m_modelSeances);
           }
       
       
       tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
       tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
       tableView->setSortingEnabled(true);
       tableView->resizeColumnsToContents();
       QString textOfLabel = textOfSums(m_modelHonoraires);
       label_5->setText(textOfLabel);
       m_listechamps.clear();
       m_listechamps << "acte" << "praticien"  << "site" <<  "esp" << "chq" << "cb" << "daf" << "autre" << "du"
                     << "valide" ;
       m_mapColumns.insert(SEANCES_DATE,"date");
       m_mapColumns.insert(SEANCES_ACTE,"acte");
       m_mapColumns.insert(SEANCES_ESP,"esp");
       m_mapColumns.insert(SEANCES_CHQ,"chq");
       m_mapColumns.insert(SEANCES_CB,"cb");
       m_mapColumns.insert(SEANCES_DAF,"daf");
       m_mapColumns.insert(SEANCES_DU,"du");
       numberOfRows = m_modelSeances->rowCount();
       for(int r = 0 ; r < numberOfRows ; r++){
          QString documentStr ;
          QMapIterator<int,QString> iMap(m_mapColumns);
          while(iMap.hasNext()){
              iMap.next();
              QModelIndex index = m_modelSeances->index(r,iMap.key(),QModelIndex());
              documentStr += iMap.value()+" = "+m_modelSeances->data(index).toString()+"   ";
          }
          m_documentTotal += documentStr+"<br/>";

        }
}

bool modifierrecette::isItShown(){
   show();
   return true;
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
   if(WarnDebug)
qDebug() << __FILE__ << QString::number(__LINE__) << " modelRowCount = " << QString::number(modelRowCount);
   for(int i = 0; i < modelRowCount ; i ++){
       QSqlRecord rowRecord = model->record(i);//ligne d'enregistrement
       esp  += rowRecord.value(ESP).toDouble();
       chq  += rowRecord.value(CHQ).toDouble();
       visa += rowRecord.value(CB).toDouble();
       vir  += rowRecord.value(VIR).toDouble();
       daf  += rowRecord.value(DAF).toDouble();
       dus  += rowRecord.value(DU).toDouble();
       }
    totaux = esp + chq + visa + vir + dus + daf;
    totalRecu = esp + chq + visa;
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

void  modifierrecette::coloringDoubles(){
    int rowCount = m_modelfiltre->rowCount();
    int columnCount =m_modelfiltre->columnCount();
    if(WarnDebug)
    qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(rowCount) << " " << QString::number(columnCount);
    QList<int> listRows;
    QList<int> listRowsSamePatients;
    for (int i = 0; i < rowCount; i += 1)
    {
    	QList<QVariant> dataRow ;
        QVariant patientName;
    	for (int c = 1; c < columnCount; c += 1)
    	{
    	        QModelIndex index = m_modelfiltre->index(i,c,QModelIndex());
    		dataRow << m_modelfiltre->data(index,Qt::DisplayRole);
    		QModelIndex indexPatient = m_modelfiltre->index(i,3,QModelIndex());
    		patientName = m_modelfiltre->data(indexPatient,Qt::DisplayRole);
    		if(WarnDebug)
            qDebug() << __FILE__ << QString::number(__LINE__) << " patientName =" << patientName.toString() ;    		
    	    }
    	if(WarnDebug)
           qDebug() << __FILE__ << QString::number(__LINE__) << " dataRow.size =" << QString::number(dataRow.size());
    	for (int j = i; j < rowCount ; j += 1)
    	{
    	           if(j!=i){
    		    	QList<QVariant> dataAfterRow ;
    		    	QVariant patientNameAfter;
    		    	QModelIndex indexPatientAfter = m_modelfiltre->index(j,3,QModelIndex());
    		    	patientNameAfter = m_modelfiltre->data(indexPatientAfter,Qt::DisplayRole);
    		    	if(WarnDebug)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " patientNameAfter =" << patientNameAfter.toString() ;    		    	
    	                for (int c = 1; c < columnCount; c += 1)
    	                {
    	                    QModelIndex indexAfter = m_modelfiltre->index(j,c,QModelIndex());
    		                dataAfterRow += m_modelfiltre->data(indexAfter,Qt::DisplayRole);
    	                }
    	                if(WarnDebug)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " dataRow.size =" << QString::number(dataRow.size());
    	                if(WarnDebug)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " dataAfterRow.size =" << QString::number(dataAfterRow.size());
    	                if (dataAfterRow == dataRow)
    	                {
    	                        if(WarnDebug)
                                qDebug() << __FILE__ << QString::number(__LINE__) << " dataAfterRow= "<< dataAfterRow;
    	                        if(WarnDebug)
                                qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(i) << QString::number(j);
    	                	listRows << i << j;
    	                }
    	                else if (patientName == patientNameAfter)
    	                {
    	                	 listRowsSamePatients << i << j; 
    	                	    	                	 
    	                    }
    	                else{}
    	           }
    	}

    }
    m_modelfiltre->getDoublesRows = listRows;
    m_modelfiltre->getDoublesNames = listRowsSamePatients;
    if(WarnDebug)
    qDebug() << __FILE__ << QString::number(__LINE__) << " listRows = " << QString::number(listRows.size());
    m_modelfiltre->submit();

}
/*    m_mapColumns.insert(PATIENT,"Nom,Prenom");
      m_mapColumns.insert(DATE,"Date");
      m_mapColumns.insert(ESP,"esp");
      m_mapColumns.insert(CHQ,"chq");
      m_mapColumns.insert(CB,"CB");
      m_mapColumns.insert(VIR,"VIR");
      m_mapColumns.insert(DAF,"DAF");
      m_mapColumns.insert(DU,"Du");*/
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
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Dispense avance des frais'")+"</bold></font></TH>";
    docHtml += "<TH><font color = blue><bold>"+trUtf8("Dus")+"</bold></font></TH>";
    docHtml += "</TR>";
    
    return docHtml;
}


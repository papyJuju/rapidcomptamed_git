#include "etatstries.h"
#include "pratid.h"
#include <common/constants.h>
#include <common/position.h>
#include <QtGui>
#include <QtCore>
#include <QtSql>

using namespace Common;
using namespace Constants;
using namespace EtatsConstants;
enum { WarnDebugMessage = false };
etatstries::etatstries(	QString & appelsqlchamps,
			QString & tablesql,
			int & champsaconcatener,
			QString & parametrestableformat,
			QHash<int,QString> & comboitem,
			int TABLE)
    
{
    setupUi(this);
    Position *pos = new Position;
    pos->centralPosition(this,width(),height());
    QPalette p=palette();
        p                        .setColor(QPalette::Active, QPalette::Base, QColor ("#DDDDDD"));
        textEdit               -> setPalette(p);
    
    m_phrasesql                 = appelsqlchamps ;
       
    m_listforquery              = appelsqlchamps.split(",");
    
    m_phrasetable               = tablesql;
    
    m_TABLETYPE = TABLE;
    
    m_champsamerger             = champsaconcatener;//champs a concatener pour écrire total mois.
    m_parametrespourtableformat = parametrestableformat;
    label_2->setText("<html><body><font size = 5 color = ""blue""><bold>Ann&#233;e</bold></font></body></html>");
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    dateEdit->setDisplayFormat("yyyy");
    
    dateEdit->setToolTip(trUtf8("Année."));
    dateEdit->setDate(QDate::currentDate());// date de fin,mise au jour courant. 
    m_date = dateEdit->date();
    
    m_hashComboItems = comboitem;
    
    comboBox                     ->setEditable(false);
    comboBox                     ->setInsertPolicy(QComboBox::NoInsert);
    for (int CHOICE = 0; CHOICE < ComboFieldsMaxParam; ++CHOICE)
    {
          comboBox->addItem(comboitem.value(CHOICE));
        }
    m_progressbar                = new QProgressBar;
    Position position;
    QTextDocument *document      = new QTextDocument;
    QTextCursor *cursor          = new QTextCursor(document);      
    
    QString dateyeardebut        = QDate::currentDate().toString("yyyy")+"-01-01";
    QString dateyearfin          = QDate::currentDate().toString("yyyy")+"-01-31";
    
    	m_model                  = new QSqlQueryModel;
    	m_model                  ->setQuery("SELECT "+m_phrasesql+" FROM "+m_phrasetable+" "
    			                    "WHERE date BETWEEN '"+dateyeardebut+"' AND '"+dateyearfin+"' ",m_db);
    
    QTextTableFormat tableFormat;
     monformat(tableFormat,m_parametrespourtableformat);
    QTextBlockFormat   centrerHead ;
         centrerHead              .setAlignment(Qt::AlignCenter);
         cursor                 -> insertBlock(centrerHead);
         cursor                 -> insertHtml ("<font size = 6 color = #3300FF><bold>"
                                               "<br/><br/>Etats tri&#233;s.<br/>Mois de janvier<bold></font><br/><br/>");
       
    QTextTable *tableDeTitres = cursor->insertTable(1,m_listforquery.size(),tableFormat);
    for(int j= 0;j<m_listforquery.size();j++){
            QTextTableCell cell = tableDeTitres->cellAt(0,j);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(m_listforquery[j]);
           }
    cursor->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    QTextTable *table            = cursor->insertTable(m_model->rowCount(QModelIndex()),
                                                                         m_listforquery.size(),
                                                                         tableFormat);
    m_progressbar                ->resize(200,30);
    position.centralPosition(m_progressbar ,200, 30);
    //m_progressbar                ->setAlignment(Qt::AlignCenter);
    m_progressbar                ->setRange(0,m_model->rowCount(QModelIndex()));
    m_progressbar                ->show();
              
    for(int i=0;i< m_model->rowCount(QModelIndex());i++){
        m_progressbar->setValue(i);
        QStringList liste; // liste des données de la ligne
    	for (int a = 0;a < m_listforquery.size(); a++){
            QString str = m_model->data(m_model->index(i,a)).toString();
            if(str.contains("/")){//a cause de la methode Maeker d'entree des recettes.
                QStringList listetrois = str.split("//");
                str = listetrois[0];
            }
            liste << ""+str+"";
        }
    	for(int j= 0;j<m_listforquery.size();j++){
            QTextTableCell cell = table->cellAt(i,j);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(liste[j]);
        }
    }
       textEdit->setDocument(document);
       
       if((m_progressbar->value())>( m_model->rowCount(QModelIndex())-2)){
            fermeprogressbar();
       }
    QSqlQuery queryActs(m_db);
    QString reqActs = QString("SELECT %1 FROM %2").arg("acte","honoraires");
    if(!queryActs.exec(reqActs)){
        qWarning() << __FILE__ << QString::number(__LINE__) << queryActs.lastError().text();
    }
    while(queryActs.next()){
        QString strAct = queryActs.value(0).toString();
        m_listOfActs << strAct;
    }
    m_listOfActs.removeDuplicates();
    
    QSqlQuery queryType(m_db);
    QString reqType = QString("SELECT %1,%2 FROM %3 WHERE type = '1'").arg("id_mvt_dispo","libelle","mouvements");
    if(!queryType.exec(reqType)){
        qWarning() << __FILE__ << QString::number(__LINE__) << queryType.lastError().text();
    }
    while(queryType.next()){
        QString idMvtDispo = queryType.value(0).toString();
        QString libelle    = queryType.value(1).toString();
        m_hashOfType.insertMulti(idMvtDispo,libelle);
    }
    
    QSqlQuery qPrat(m_db);
    if(!qPrat.exec(QString("SELECT %1,%2 FROM %3").arg("id_usr",
                                                       "nom_usr",
                                                       "utilisateurs"))){
        qWarning() << __FILE__ << QString::number(__LINE__) << qPrat.lastError().text();
    }
    while(qPrat.next()){
        QString idUsr  = qPrat.value(0).toString();
        QString nomUsr = qPrat.value(1).toString();
        m_hashLoginUser.insertMulti(idUsr,nomUsr);
    }    
    connect(pushButton,  SIGNAL(clicked()),this,SLOT(trier()));   
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(imprimer()));  
    connect(pushButton_3,SIGNAL(clicked()),this,SLOT(close()));
    connect(pushButton_4,SIGNAL(clicked()),this,SLOT(avancerdate()));
    connect(pushButton_5,SIGNAL(clicked()),this,SLOT(reculerdate()));
    
}

etatstries::~etatstries()
{
    delete m_progressbar ;
    m_idUsr = "";
    m_SumUpOfActs.clear();
}

void etatstries::fermeprogressbar(){

  m_progressbar->close();
}

void etatstries::imprimer(){
  QPrinter printer;
           printer.setOrientation(QPrinter::Landscape);
  QPrintDialog *printdialog = new QPrintDialog(&printer,this);
                printdialog ->setWindowTitle(tr("Imprimer"));
                printdialog ->exec();
                textEdit    ->print(&printer);
}
//-------------------------fonction trier------------------------------------------------
void etatstries::trier(){
  QString comboChoice = comboBox->currentText();
  int comboIndex = comboBox->currentIndex();
  m_date = dateEdit->date();
  QTextDocument *documenttrie = new QTextDocument;
  QTextCursor   *cursortrie   = new QTextCursor(documenttrie);
  m_idUsr = "";
  if(comboIndex != TOUS_PRAT && comboIndex != TOUSACTES && comboIndex != TOUSACTES){
         pratid *id = new pratid;
        if(id->exec() == QDialog::Accepted){
           m_idUsr = id->recupPrat();
           if (WarnDebugMessage)
           qDebug() << "m_idUsr = "+m_idUsr << " " << __FILE__ << QString::number(__LINE__);
           QTextBlockFormat title ;
                             title        .setAlignment(Qt::AlignCenter);
                             cursortrie -> insertBlock(title);
                             cursortrie -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                                 "<br/>"+m_hashLoginUser.value(m_idUsr)+""
                                                                 "<bold></font><br/><br/>");
        }
  }
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  QTextTableFormat tableFormat;
  monformat(tableFormat,m_parametrespourtableformat);
  int nombrejoursannee = m_date.daysInYear();


  //janvier
  QString unjanv         = m_date.toString("yyyy")+"-01-01";
  QString trenteetunjanv = m_date.toString("yyyy")+"-01-31";
  QString janvier        = trUtf8("janvier");
  QTextTable *tableDeTitres = cursortrie->insertTable(1,m_listforquery.size(),tableFormat);
    for(int j= 0;j<m_listforquery.size();j++){
            QTextTableCell cell = tableDeTitres->cellAt(0,j);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(m_listforquery[j]);
           }
    cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
  modele3031(janvier, unjanv , trenteetunjanv ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);

  cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
     if (WarnDebugMessage)
qDebug() << __FILE__ << QString::number(__LINE__);
    //fevrier------------mois particulier !
    QString unfev          = m_date.toString("yyyy")+"-02-01";
    QString vingthuitfev   = m_date.toString("yyyy")+"-02-28";
    QString fevrier         = trUtf8("février");
        if(nombrejoursannee == 366){
            vingthuitfev = m_date.toString("yyyy")+"-02-29";
        }
    modele3031(fevrier, unfev , vingthuitfev ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);

    cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    if (WarnDebugMessage)
qDebug() << __FILE__ << QString::number(__LINE__);
    //mars
    	QString unmars = m_date.toString("yyyy")+"-03-01";
	QString trenteetunmars = m_date.toString("yyyy")+"-03-31";
	QString mars = trUtf8("mars");
	modele3031(mars,unmars , trenteetunmars ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //avril
    	QString unavril = m_date.toString("yyyy")+"-04-01";
	QString trenteavril = m_date.toString("yyyy")+"-04-30";
	QString avril = trUtf8("avril");
	modele3031(avril,unavril , trenteavril ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //mai
    	QString unmai = m_date.toString("yyyy")+"-05-01";
	QString trenteetunmai = m_date.toString("yyyy")+"-05-31";
	QString mai = trUtf8("mai");
	modele3031(mai, unmai , trenteetunmai ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //juin
    	QString unjuin = m_date.toString("yyyy")+"-06-01";
	QString trentejuin = m_date.toString("yyyy")+"-06-30";
	QString juin = trUtf8("juin");
	modele3031(juin,unjuin , trentejuin ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //juillet
     	QString unjuillet = m_date.toString("yyyy")+"-07-01";
	QString trenteetunjuillet = m_date.toString("yyyy")+"-07-31";
	QString juillet = trUtf8("juillet");
	modele3031(juillet, unjuillet , trenteetunjuillet ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //aout
    	QString unaout = m_date.toString("yyyy")+"-08-01";
	QString trenteetunaout = m_date.toString("yyyy")+"-08-31";
	QString aout = trUtf8("août");
	modele3031(aout, unaout , trenteetunaout ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //septembre
    	QString unsept = m_date.toString("yyyy")+"-09-01";
	QString trentesept = m_date.toString("yyyy")+"-09-30";
	QString sept = trUtf8("septembre");
	modele3031(sept, unsept , trentesept ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //octobre
    	QString unoct = m_date.toString("yyyy")+"-10-01";
	QString trenteetunoct = m_date.toString("yyyy")+"-10-31";
	QString oct = trUtf8("octobre");
	modele3031(oct, unoct , trenteetunoct ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //novembre
    	QString unnov = m_date.toString("yyyy")+"-11-01";
	QString trentenov = m_date.toString("yyyy")+"-11-30";
	QString nov = trUtf8("novembre");
	modele3031(nov, unnov , trentenov ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    //decembre
    	QString undec = m_date.toString("yyyy")+"-12-01";
	QString trenteetundec = m_date.toString("yyyy")+"-12-31";
	QString dec = trUtf8("décembre");
	modele3031(dec, undec , trenteetundec ,tableFormat,cursortrie,m_listforquery,comboIndex,m_TABLETYPE);
	cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);

     //toute annee 
      if (WarnDebugMessage)
      qDebug() << __FILE__ << QString::number(__LINE__) << "toute annee" ;
      QHash<QString,QString> hashTypeValues = getHashTypeValues();
      if (WarnDebugMessage)
      qDebug() << __FILE__ << QString::number(__LINE__) << "hashTypeValues size"
      << QString::number(hashTypeValues.size()) ;
      insertEndOfTable(cursortrie, hashTypeValues); 
      if (WarnDebugMessage)
      qDebug() << __FILE__ << QString::number(__LINE__) << "toute annee 2" ;
      cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);

      //total annee
      double totalValueOfTotalYear = getTotalValueOfYear();
      insertTotalOfTable(cursortrie,totalValueOfTotalYear);
      cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
		
      textEdit->setDocument(documenttrie);
      if(comboChoice == "par acte"|| comboChoice == "par acte et praticien"){
          insertTableOfSumOfActs(cursortrie);
          m_SumUpOfActs.clear();
      }
      if(comboChoice == "par type"|| comboChoice == "par type et praticien"){
          insertTableOfSumOfTypes(cursortrie);
          m_SumUpOfType.clear();
      }
      
      QApplication::restoreOverrideCursor();
}
//----------fonction de construction du tableau type (sauf fevrier)-------------------------------------------
QTextTableFormat etatstries::monformat(QTextTableFormat &tableFormat,QString & parametrespourtableformat){
     QString parametrespourfonction = parametrespourtableformat;
     QStringList listeparametres = parametrespourfonction.split(",");
     tableFormat.setBackground(QColor("#e0e0e0"));
     tableFormat.setAlignment(Qt::AlignCenter);
     tableFormat.setCellPadding(2);
     tableFormat.setCellSpacing(2);
     QVector<QTextLength> contraintes;
     for(int i = 0;i < listeparametres.size() ; i++){
         contraintes << QTextLength(QTextLength::FixedLength, listeparametres[i].toInt());
     }
    
     tableFormat.setColumnWidthConstraints(contraintes);
     return tableFormat;
}
//-------------fonction de calcul des colonnes recettes---------------------------
QString etatstries::calcul(QString & x, QString & datedebut, QString & datefin){
  QString type = x;
  QString date1 = datedebut;
  QString date2 = datefin;
  QStringList listetype;
  QSqlQuery query(m_db);
  query.exec("SELECT "+type+" FROM "+m_phrasetable+" WHERE date BETWEEN '"+datedebut+"' AND '"+datefin+"'");

  while(query.next()){
      QString string = query.value(0).toString();
      listetype << ""+string+"";
  }

  QString result;
  double total;
  total = 0;

  for(int i=0; i< listetype.size();i++){
      total += listetype[i].toDouble();
  }

  result = QString::number(total);
  return result;
}
//----------------------fonction d'insertion des resultats de calcul des recettes dans chaque colonne.
void etatstries::calculparmois(QStringList & liste,QTextTable *tableau, QString & datedebut, QString & datefin){
  QStringList listepourfonction;
  listepourfonction = liste;
  QTextTable *tablepourfonction;
  tablepourfonction = tableau;
  QString date1 = datedebut;
  QString date2 = datefin;

    int b = m_champsamerger;//4 car 5°cellule;
    QString str = "";
    for(int a = 0; a< listepourfonction.size()-b; a++){
    QString str = listepourfonction[a+b];
    
    QTextTableCell cellesp = tablepourfonction->cellAt(tablepourfonction->rows()-1,a+b);
    QTextCursor cellCursor1 = cellesp.firstCursorPosition();
    QString calc = calcul(str,date1,date2);
    
    cellCursor1.insertText(calc);
    
    }
    
}
//--------------------fonction de remplissage de tableau par mois-------------------------------------------
void etatstries::modele3031(QString & mois,
                            QString & jour1 , 
                            QString & dernierjour ,
                            QTextTableFormat & tableFormattableau,
                            QTextCursor *cursortriefonction,
                            QStringList & listefonction,
                            int choice,
                            int TABLETYPE ){
  QString cemois                     = mois;
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << "MOIS = " << cemois ;
  QString un                         = jour1;
  QString trenteetquelque            = dernierjour;
  QTextTableFormat tableFormat       = tableFormattableau;
  QTextCursor *cursortrieinfonction  = cursortriefonction;
  QStringList listforquery           = listefonction;
  QString requete = "";
  /*"SELECT "+m_phrasesql+" FROM "+m_phrasetable+" "
	                  " WHERE date BETWEEN '"+un+"'AND '"+trenteetquelque+"'"*/
  if(choice == TOUS_PRAT && TABLETYPE == RECETTE_TABLE) {
      requete = QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'")
                .arg (m_phrasesql,
                      m_phrasetable,
                      un,
                      trenteetquelque);
      if(!showInUsualCase(requete,mois,un,trenteetquelque,tableFormat,cursortrieinfonction,listforquery)){
          qWarning() << "Error req in " << __FILE__ << QString::number(__LINE__);
      }
  }
  else if(choice == TOUSACTES && TABLETYPE == RECETTE_TABLE) {
      if (WarnDebugMessage)
      qDebug() << "par acte" << __FILE__ << QString::number(__LINE__);
      if(!showByAct(mois,un,trenteetquelque,cursortrieinfonction)){
          qWarning() << "Error insert in table " << __FILE__ << QString::number(__LINE__);
      }
  
  }
  else if(choice == PRATICIEN_INDEX ) {
          qDebug() << __FILE__ << QString::number(__LINE__) << " PRATICIEN =" << QString::number(PRATICIEN) ;
          requete = QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'AND id_usr = '%5'")
                      .arg (m_phrasesql,
                            m_phrasetable,
                            un,
                            trenteetquelque,
                            m_idUsr);
      if(!showInUsualCase(requete,mois,un,trenteetquelque,tableFormat,cursortrieinfonction,listforquery)){
          qWarning() << "Error req in " << __FILE__ << QString::number(__LINE__);
      }
  }
  else if(choice == ACTEETPRAT && TABLETYPE == RECETTE_TABLE){
      if (WarnDebugMessage)
      qDebug() << "par acte et praticien" << __FILE__ << QString::number(__LINE__);
      if(!showByAct(mois,un,trenteetquelque,cursortrieinfonction)){
          qWarning() << "Error insert in table " << __FILE__ << QString::number(__LINE__);
      }
  }
  else if(choice == TOUSACTES && TABLETYPE == MOUVEMENT_TABLE) {//mouvements
      if (WarnDebugMessage)
      qDebug() << "par type" << __FILE__ << QString::number(__LINE__);
      if(!showByType(mois,un,trenteetquelque,cursortrieinfonction)){
          qWarning() << "Error insert in table " << __FILE__ << QString::number(__LINE__);
      }
  }
  else if(choice == ACTEETPRAT && TABLETYPE == MOUVEMENT_TABLE) {//mouvements
      if (WarnDebugMessage)
      qDebug() << "par type et praticien" << __FILE__ << QString::number(__LINE__);
      if(!showByType(mois,un,trenteetquelque,cursortrieinfonction)){
          qWarning() << "Error insert in table " << __FILE__ << QString::number(__LINE__);
      }
  }
  else{
      qWarning() << __FILE__ << QString::number(__LINE__) << "no choice";
  }
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__);
}

void etatstries::avancerdate(){
  QDate date = dateEdit->date();
  dateEdit->setDate(date.addYears(1));
}

void etatstries::reculerdate(){
  QDate date = dateEdit->date();
  dateEdit->setDate(date.addYears(-1));
}

bool etatstries::showByAct(QString & mois,
                           QString & un,
                           QString & trenteetquelque,
                           QTextCursor *cursortrieinfonction){
    bool boule = true;
            QTextBlockFormat title ;
                             title               .setAlignment(Qt::AlignCenter);
                             cursortrieinfonction -> insertBlock(title);
                             cursortrieinfonction -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                                 "<br/>Mois de  "+mois+"<bold></font><br/><br/>");
       
            QTextTableFormat tableFormat;
             tableFormat.setBackground(QColor("#e0e0e0"));
             tableFormat.setAlignment(Qt::AlignCenter);
             tableFormat.setCellPadding(2);
             tableFormat.setCellSpacing(2);
             QVector<QTextLength> contraintes;
             QList<int> listSizeOfCells;
                        listSizeOfCells << 150 << 150 ;
                  for(int i= 0;i< listSizeOfCells.size();i++){
                      contraintes << QTextLength(QTextLength::FixedLength, listSizeOfCells[i]);
                  }
            tableFormat.setColumnWidthConstraints(contraintes);
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << un << trenteetquelque;
    QString strAct = "x";
    
    foreach(strAct,m_listOfActs){
        double sum = 0.00;
        if(strAct.contains("/")){//a cause de la methode Maeker d'entree des recettes.
                     QStringList listetrois = strAct.split("//");
                     strAct = listetrois[0];
        }
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " " << strAct;
        QString requete =  QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'AND acte = '%5'")
                           .arg (m_phrasesql,
                                 m_phrasetable,
                                 un,
                                 trenteetquelque,
                                 strAct);
        if(comboBox->currentText() == "par acte et praticien"){
            requete = QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'AND acte = '%5'AND id_usr = '%6'")
                      .arg (m_phrasesql,
                            m_phrasetable,
                            un,
                            trenteetquelque,
                            strAct,
                            m_idUsr);
        }
        QSqlQuery query(m_db);
        if(!query.exec(requete)){
            qWarning()  << __FILE__ << QString::number(__LINE__) << query.lastError().text();
           
        }
        while(query.next()){
            double esp = query.value(ESP).toDouble();
            double chq = query.value(CHQ).toDouble();
            double cb  = query.value(CB).toDouble();
            double vir = query.value(VIR).toDouble();
            sum += esp+chq+cb+vir;
        }
            
            m_SumUpOfActs.insertMulti(strAct,sum);
            if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "sum ="+QString::number(sum);
            QTextTable *table = cursortrieinfonction->insertTable(1,
                                                                  2,
                                                                  tableFormat);
             QTextTableCell cell = table->cellAt(0,0);
             QTextCursor cellCursor = cell.firstCursorPosition();
                         cellCursor.insertText(strAct);
             QTextTableCell cell1 = table->cellAt(0,1);
             QTextCursor cellCursor1 = cell1.firstCursorPosition();
                         cellCursor1.insertText(QString::number(sum));

             cursortrieinfonction->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);

   }
   return boule;
}   

bool etatstries::showByType(QString & mois,
                           QString & un,
                           QString & trenteetquelque,
                           QTextCursor *cursortrieinfonction){
    bool boule = true;
    m_SumUpOfType.clear();
            QTextBlockFormat title ;
                             title               .setAlignment(Qt::AlignCenter);
                             cursortrieinfonction -> insertBlock(title);
                             cursortrieinfonction -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                                 "<br/>Mois de  "+mois+"<bold></font><br/><br/>");
       
            QTextTableFormat tableFormat;
             tableFormat.setBackground(QColor("#e0e0e0"));
             tableFormat.setAlignment(Qt::AlignCenter);
             tableFormat.setCellPadding(2);
             tableFormat.setCellSpacing(2);
             QVector<QTextLength> contraintes;
             QList<int> listSizeOfCells;
                        listSizeOfCells << 150 << 150 ;
                  for(int i= 0;i< listSizeOfCells.size();i++){
                      contraintes << QTextLength(QTextLength::FixedLength, listSizeOfCells[i]);
                  }
            tableFormat.setColumnWidthConstraints(contraintes);
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << un << trenteetquelque;
    QString idMvtDispo = "x";
    QStringList listOfMvts;
                listOfMvts = m_hashOfType.keys();
                listOfMvts.removeDuplicates();
    foreach(idMvtDispo,listOfMvts){
        double sum = 0.00;
        QString requete =  QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'AND id_mvt_dispo = '%5'")
                           .arg (m_phrasesql,
                                 m_phrasetable,
                                 un,
                                 trenteetquelque,
                                 idMvtDispo);
        if(comboBox->currentText() == "par type et praticien"){
            requete = QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'AND id_mvt_dispo = '%5'"
                              " AND id_usr = '%6'")
                      .arg (m_phrasesql,
                            m_phrasetable,
                            un,
                            trenteetquelque,
                            idMvtDispo,
                            m_idUsr);
                            
        }
        QSqlQuery query(m_db);
        if(!query.exec(requete)){
            qWarning()  << __FILE__ << QString::number(__LINE__) << query.lastError().text();
           
        }
        while(query.next()){
            double montant = query.value(4).toDouble();
            sum += montant;
        }
            if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " lastQuery = "<< query.lastQuery();
            m_SumUpOfType.insertMulti(idMvtDispo,sum);
            QTextTable *table = cursortrieinfonction->insertTable(1,
                                                                  2,
                                                                  tableFormat);
             QTextTableCell cell = table->cellAt(0,0);
             QTextCursor cellCursor = cell.firstCursorPosition();
                         cellCursor.insertText(m_hashOfType.value(idMvtDispo));
             QTextTableCell cell1 = table->cellAt(0,1);
             QTextCursor cellCursor1 = cell1.firstCursorPosition();
                         cellCursor1.insertText(QString::number(sum));

             cursortrieinfonction->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);

   }
   return boule;
}
bool etatstries::showInUsualCase(
                     QString & requete ,
                     QString & mois,
                     QString & un,
                     QString & trenteetquelque,
                     QTextTableFormat & tableFormat,
                     QTextCursor *cursortrieinfonction,
                     QStringList & listforquery){
     bool but = true;
     m_model->setQuery(requete,m_db);
     if(m_model->rowCount(QModelIndex())!= 0){
           QTextBlockFormat centrer1 ;
                          //centrer1               .setBackground(Qt::yellow) ;
                          //centrer1               .setForeground(Qt::red) ;
                            centrer1               .setAlignment(Qt::AlignCenter);
                            cursortrieinfonction -> insertBlock(centrer1);
                            cursortrieinfonction -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                                "<br/>Mois de  "+mois+"<bold></font><br/><br/>");
       
           QTextTable *table = cursortrieinfonction->insertTable(m_model->rowCount(QModelIndex()),
                                                                                   listforquery.size(),
                                                                                   tableFormat);
           for(int i=0;i< m_model->rowCount(QModelIndex());i++){
             QStringList liste; // liste des données de la ligne
    	     for (int a = 0;a < listforquery.size(); a++){
                 QString str = m_model->data(m_model->index(i,a)).toString();
                 if(str.contains("/")){//a cause de la methode Maeker d'entree des recettes.
                     QStringList listetrois = str.split("//");
                     str = listetrois[0];
                 }
              liste << str;
             }
           for(int j= 0;j<listforquery.size();j++){
            QTextTableCell cell = table->cellAt(i,j);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(liste[j]);
               }
           }
    table->insertRows(table->rows(),1);
    table->mergeCells(table->rows()-1,0,1,m_champsamerger);//-1 car part de zero
    QTextTableCell cell = table->cellAt(table->rows()-1,0);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText("Total "+mois+"");
    calculparmois(listforquery,table, un,trenteetquelque);//calcul par type recette et mois
    cursortrieinfonction->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
   }
   else{but = false;}
   return but;
}
void etatstries::insertTableOfSumOfActs(QTextCursor *cursortrieinfonction){
    QStringList listOfActs;
                listOfActs = m_SumUpOfActs.uniqueKeys();
    QTextBlockFormat title ;
                     title                  .setAlignment(Qt::AlignCenter);
                     cursortrieinfonction -> insertBlock(title);
                     cursortrieinfonction -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                                 "<br/>Totaux par acte<bold></font><br/><br/>");
    QTextTableFormat tableFormat;
             tableFormat.setBackground(QColor("#e0e0e0"));
             tableFormat.setAlignment(Qt::AlignCenter);
             tableFormat.setCellPadding(2);
             tableFormat.setCellSpacing(2);
             QVector<QTextLength> contraintes;
             QList<int> listSizeOfCells;
                        listSizeOfCells << 150 << 150 ;
                  for(int i= 0;i< listSizeOfCells.size();i++){
                      contraintes << QTextLength(QTextLength::FixedLength, listSizeOfCells[i]);
                  }
            tableFormat.setColumnWidthConstraints(contraintes);
    QString act = "cs";
    foreach(act,listOfActs){
        double sumOfAct = 0.00;
        QList<double> valuesList;
                      valuesList = m_SumUpOfActs.values(act);
        for(int i = 0;i < valuesList.size(); i++){
           sumOfAct += valuesList.at(i);
        }
        if(sumOfAct !=0){
            QTextTable *table = cursortrieinfonction->insertTable(1,
                                                                  2,
                                                                  tableFormat);
            QTextTableCell cell = table->cellAt(0,0);
            QTextCursor cellCursor = cell.firstCursorPosition();
                        cellCursor.insertText(act);
            QTextTableCell cell1 = table->cellAt(0,1);
            QTextCursor cellCursor1 = cell1.firstCursorPosition();
                        cellCursor1.insertText(QString::number(sumOfAct));
        }
        cursortrieinfonction->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    }
    
}

void etatstries::insertTableOfSumOfTypes(QTextCursor *cursortrieinfonction){
    QStringList listOfActs;
                listOfActs = m_SumUpOfActs.uniqueKeys();
    QTextBlockFormat title ;
                     title                  .setAlignment(Qt::AlignCenter);
                     cursortrieinfonction -> insertBlock(title);
                     cursortrieinfonction -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                         "<br/>"+trUtf8("Totaux par dépenses")+"<bold>"
                                                         "</font><br/><br/>");
    QTextTableFormat tableFormat;
             tableFormat.setBackground(QColor("#e0e0e0"));
             tableFormat.setAlignment(Qt::AlignCenter);
             tableFormat.setCellPadding(2);
             tableFormat.setCellSpacing(2);
             QVector<QTextLength> contraintes;
             QList<int> listSizeOfCells;
                        listSizeOfCells << 150 << 150 ;
                  for(int i= 0;i< listSizeOfCells.size();i++){
                      contraintes << QTextLength(QTextLength::FixedLength, listSizeOfCells[i]);
                  }
            tableFormat.setColumnWidthConstraints(contraintes);
    QString idMvt = "x";
    QStringList listOfMvts;
                listOfMvts = m_hashOfType.keys();
                listOfMvts.removeDuplicates();
    foreach(idMvt,listOfMvts){
        double sumOfAct = 0.00;
        QList<double> valuesList;
                      valuesList = m_SumUpOfType.values(idMvt);
        
        for(int i = 0;i < valuesList.size(); i++){
           sumOfAct += valuesList.at(i);
        }
        if(sumOfAct !=0){
            QTextTable *table = cursortrieinfonction->insertTable(1,
                                                                  2,
                                                                  tableFormat);
            QTextTableCell cell = table->cellAt(0,0);
            QTextCursor cellCursor = cell.firstCursorPosition();
                        cellCursor.insertText(m_hashOfType.value(idMvt));
            QTextTableCell cell1 = table->cellAt(0,1);
            QTextCursor cellCursor1 = cell1.firstCursorPosition();
                        cellCursor1.insertText(QString::number(sumOfAct));
        }
        cursortrieinfonction->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    }
    
}

QTextTableFormat etatstries::formatFinAnnee(){
    QTextTableFormat tableFormat;
    QStringList listeparametres ;
    listeparametres << "190" << "190";
     tableFormat.setBackground(QColor("#e0e0e0"));
     tableFormat.setAlignment(Qt::AlignCenter);
     tableFormat.setCellPadding(2);
     tableFormat.setCellSpacing(2);
     QVector<QTextLength> contraintes;
     for(int i = 0;i < listeparametres.size() ; i++){
         contraintes << QTextLength(QTextLength::FixedLength, listeparametres[i].toInt());
     }
    
     tableFormat.setColumnWidthConstraints(contraintes);
    return tableFormat;
}

void etatstries::insertEndOfTable(QTextCursor *cursortrieinfonction, const QHash<QString,QString> & hashTypeValues){
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << "insertEndOfTable1" ;
    const QString totalAnneeText = trUtf8("TOTAUX DE L'ANNEE");
    QTextBlockFormat totalAnneBlock ;
    totalAnneBlock               .setAlignment(Qt::AlignCenter);
    cursortrieinfonction -> insertBlock(totalAnneBlock);
    cursortrieinfonction -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                        "<br/>"+totalAnneeText+"<bold></font><br/><br/>");
    QTextTableFormat format = formatFinAnnee();
    int column = 2;
    int row = 0;
    int numberOfRows = 0;
    if (m_phrasetable=="honoraires")
    {
    	numberOfRows = int(RECETTES_MaxParams)-int(ESP);
        }
    if (m_phrasetable=="mouvements")
    {
    	numberOfRows = hashTypeValues.size();   	
        }
    QHashIterator<QString,QString> it(hashTypeValues);
    QTextTable * table = cursortrieinfonction->insertTable(numberOfRows,column,format);
    while(it.hasNext())
    {           	    it.next();
            QString key = it.key();
            QString value = it.value();
            QTextTableCell cell = table->cellAt(row,0);
            QTextCursor cellCursor = cell.firstCursorPosition();
                        cellCursor.insertText(key);
            QTextTableCell cell1 = table->cellAt(row,1);
            QTextCursor cellCursor1 = cell1.firstCursorPosition();
                        cellCursor1.insertText(value);
            ++row;
            cursortrieinfonction->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    }
}

void etatstries::insertTotalOfTable(QTextCursor *cursortrieinfonction, double value){
    QTextTableFormat format = formatFinAnnee();
    int column = 2;
    int rows = 1;
    const QString text = trUtf8("Total année sauf dus");
    const QString valueStr = QString::number(value);
    QTextTable * table = cursortrieinfonction->insertTable(rows,column,format);
    QTextTableCell cell = table->cellAt(0,0);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(text);
    QTextTableCell cell1 = table->cellAt(0,1);
    QTextCursor cellCursor1 = cell1.firstCursorPosition();
    cellCursor1.insertText(valueStr);
}

QHash<QString,QString> etatstries::getHashTypeValues(){
    QHash<QString,QString> hash;
    QHash<QString,double> hashForMovements;
    QString un = m_date.toString("yyyy")+"-01-01";
    QString trenteetquelque = m_date.toString("yyyy")+"-12-31";
    //honoraires
    double esp = 0.00;
    double chq = 0.00;
    double cb  = 0.00;
    double vir = 0.00;
    double daf = 0.00;
    double autre = 0.00;
    double du  = 0.00;
    QSqlQuery q(m_db);
    QString req = QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'")
                           .arg (m_phrasesql,
                                 m_phrasetable,
                                 un,
                                 trenteetquelque);
    if ( comboBox->currentIndex() == ACTEETPRAT || comboBox->currentIndex() == PRATICIEN_INDEX)
    {
    	  req += QString(" AND id_usr = '%1'").arg(m_idUsr);
        }    
    if (!q.exec(req))
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text();
        }
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << "query" << q.lastQuery() ;
    QStringList listOfValues;
    listOfValues = m_phrasetable.split(",");
    while(q.next()){
      if (m_phrasetable=="honoraires")
      {
    	esp += q.value(ESP).toDouble();
    	chq += q.value(CHQ).toDouble();
    	cb  += q.value(CB).toDouble();
    	vir += q.value(VIR).toDouble();
    	daf += q.value(DAF).toDouble();
    	autre += q.value(AUTRE).toDouble();  
    	du  += q.value(DU).toDouble();
        }
      else if (m_phrasetable == "mouvements")
      {//libelle,date,date_valeur,remarque,montant
    	QString libelle = q.value(LIBELLE).toString();
    	double value = q.value(MONTANT).toDouble();
    	hashForMovements.insertMulti(libelle,value);
    	}
      else{
        qWarning() << __FILE__ << QString::number(__LINE__) << "no phrase table 1";
       }
     }
    if (m_phrasetable=="honoraires")
    {
    	hash.insert(trUtf8("Espèces"),QString::number(esp));
    	hash.insert(trUtf8("Chèques"),QString::number(chq));
    	hash.insert(trUtf8("Cartes"),QString::number(cb));
    	hash.insert(trUtf8("Virements"),QString::number(vir));
    	hash.insert(trUtf8("DAF"),QString::number(daf));
    	hash.insert(trUtf8("Autres"),QString::number(autre));
    	hash.insert(trUtf8("Dus"),QString::number(du));
        }
    else if (m_phrasetable == "mouvements")
    {
        QStringList listOfLibelles = hashForMovements.keys();
        listOfLibelles.removeDuplicates();
        QString libelle;
        foreach(libelle,listOfLibelles){
            QList<double> values = hashForMovements.values(libelle);
            double valueForLibelle = 0.00;
            for (int i = 0; i < values.size(); i += 1)
            {
            	valueForLibelle += values[i];
                }
            hash.insert(libelle,QString::number(valueForLibelle));
            }        
        }
     else{
         qWarning() << __FILE__ << QString::number(__LINE__) << "no phrase table 2";
         }
    return hash;
}

double etatstries::getTotalValueOfYear(){
    double value = 0.00;
    double esp = 0.00;
    double chq = 0.00;
    double cb  = 0.00;
    double vir = 0.00;
    double daf = 0.00;
    double autre = 0.00;
    double du  = 0.00;
    Q_UNUSED(daf);
    Q_UNUSED(du);
    QString un = m_date.toString("yyyy")+"-01-01";
    QString trenteetquelque = m_date.toString("yyyy")+"-12-31";
    QSqlQuery q(m_db);
    QString req = QString("SELECT %1 FROM %2 WHERE date BETWEEN '%3' AND '%4'")
                           .arg (m_phrasesql,
                                 m_phrasetable,
                                 un,
                                 trenteetquelque);
    if (comboBox->currentIndex() == ACTEETPRAT || comboBox->currentIndex() == PRATICIEN_INDEX)
    {
    	  req += QString(" AND id_usr = '%1'").arg(m_idUsr);
        }
    if (!q.exec(req))
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text();
        }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "query" << q.lastQuery() ;
    while(q.next()){
      if (m_phrasetable=="honoraires")
      {
    	esp = q.value(ESP).toDouble();
    	chq = q.value(CHQ).toDouble();
    	cb  = q.value(CB).toDouble();
    	vir = q.value(VIR).toDouble();
    	daf = q.value(DAF).toDouble();
    	autre = q.value(AUTRE).toDouble();  
    	du  = q.value(DU).toDouble();
    	value += esp+chq+cb+vir+autre;
          }
      else if (m_phrasetable == "mouvements")
      {
          value += q.value(MONTANT).toDouble();
          }
      else{
          qWarning() << __FILE__ << QString::number(__LINE__) << "no phrase table 3";
          }
       }
                  
    return value;
}

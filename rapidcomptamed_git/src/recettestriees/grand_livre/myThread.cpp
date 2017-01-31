#include "myThread.h"

#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/constants.h>
#include <common/icore.h>

#include <QtGui>
#include <QtCore>
#include <QtSql>

using namespace Common;
using namespace Constants;


extern QDate   g_date;
extern QString g_utilisateur;

produceDoc::produceDoc(QObject *parent) : QThread(parent){
    m_db               = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    
/*int*/         m_champsaconcatener     = 0;
/*QString*/     m_parametrestableformat = "200,50,50,200,170";
/*QString*/     m_parametresTablesRecap = "400,400";

    }
    
produceDoc::~produceDoc(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in ~produceDoc" ;
   /* if (m_documenttrie) delete m_documenttrie;
    if (m_doctrieJanvRecT) delete m_doctrieJanvRecT ;
    if (m_doctrieJanvDepT) delete m_doctrieJanvDepT ;   
    if (m_doctrieFevRecT) delete m_doctrieFevRecT  ;  
    if (m_doctrieFevDepT) delete m_doctrieFevDepT  ; 
    if (m_doctrieMarRecT) delete m_doctrieMarRecT  ;
    if (m_doctrieMarDepT) delete m_doctrieMarDepT  ;
    if (m_doctrieAvrRecT) delete m_doctrieAvrRecT  ;
    if (m_doctrieAvrDepT) delete m_doctrieAvrDepT  ;
    if (m_doctrieMaiRecT) delete m_doctrieMaiRecT  ;
    if (m_doctrieMaiDepT) delete m_doctrieMaiDepT  ;
    if (m_doctrieJuinRecT) delete m_doctrieJuinRecT ;
    if (m_doctrieJuinDepT) delete m_doctrieJuinDepT ;
    if (m_doctrieJuilRecT) delete m_doctrieJuilRecT ;
    if (m_doctrieJuilDepT) delete m_doctrieJuilDepT ;
    if (m_doctrieAouRecT) delete m_doctrieAouRecT  ;
    if (m_doctrieAouDepT) delete m_doctrieAouDepT  ;
    if (m_doctrieSeptRecT) delete m_doctrieSeptRecT ;
    if (m_doctrieSeptDepT) delete m_doctrieSeptDepT ;
    if (m_doctrieOctRecT) delete m_doctrieOctRecT  ;
    if (m_doctrieOctDepT) delete m_doctrieOctDepT  ;
    if (m_doctrieNovRecT) delete m_doctrieNovRecT  ;
    if (m_doctrieNovDepT) delete m_doctrieNovDepT  ;
    if (m_doctrieDecRecT) delete m_doctrieDecRecT  ;
    if (m_doctrieDecDepT) delete m_doctrieDecDepT  ;*/
    
}

void produceDoc::run(){
    m_mutex.lock();
    m_date = g_date;
    m_documenttrie       = new QTextDocument;  
    m_doctrieJanvRecT    = new QTextDocument;
    m_doctrieJanvDepT    = new QTextDocument;
    m_doctrieFevRecT     = new QTextDocument;
    m_doctrieFevDepT     = new QTextDocument;
    m_doctrieMarRecT     = new QTextDocument;
    m_doctrieMarDepT     = new QTextDocument;
    m_doctrieAvrRecT     = new QTextDocument;
    m_doctrieAvrDepT     = new QTextDocument;
    m_doctrieMaiRecT     = new QTextDocument;
    m_doctrieMaiDepT     = new QTextDocument;
    m_doctrieJuinRecT    = new QTextDocument;
    m_doctrieJuinDepT    = new QTextDocument;
    m_doctrieJuilRecT    = new QTextDocument;
    m_doctrieJuilDepT    = new QTextDocument;
    m_doctrieAouRecT     = new QTextDocument;
    m_doctrieAouDepT     = new QTextDocument;
    m_doctrieSeptRecT    = new QTextDocument;
    m_doctrieSeptDepT    = new QTextDocument;
    m_doctrieOctRecT     = new QTextDocument;
    m_doctrieOctDepT     = new QTextDocument;
    m_doctrieNovRecT     = new QTextDocument;
    m_doctrieNovDepT     = new QTextDocument;
    m_doctrieDecRecT     = new QTextDocument;
    m_doctrieDecDepT     = new QTextDocument;
    trier();
    m_mutex.unlock();
}

void produceDoc::trier(){
    QStringList utilNbre;
                utilNbre  = g_utilisateur.split(",");
    qDebug() << __FILE__ << QString::number(__LINE__) << "g_utilisateur = "+g_utilisateur ;
    QString login = utilNbre[1];
    QList<int> listUsr;
    QSqlQuery queryUsr(m_db);
    if(!queryUsr.exec(QString("SELECT %1 FROM %2 WHERE login = '%3'").arg("id_usr",
                                                                  "utilisateurs",
                                                                  login))){
        qWarning() << __FILE__ << QString::number(__LINE__) << queryUsr.lastError().text();
    }
    while(queryUsr.next()){
        m_usr = queryUsr.value(0).toInt();
        listUsr << m_usr;
    }
    if(listUsr.size()> 1){
       QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Deux utilisateurs ont le même login !"),QMessageBox::Ok);
    }
    
   // qDebug() << "m_usr = "+QString::number(m_usr)+" " << __FILE__;
    QString parametrespourtableformat = m_parametrestableformat;
    QTextTableFormat tableFormat;
    monformat(tableFormat,parametrespourtableformat);
    int nombrejoursannee              = m_date.daysInYear();
    QTextCursor   *cursortrie         = new QTextCursor(m_documenttrie);
    
//-----------------------insertion du titre du document-----------------------------------

    QString titreDoc                   = "<font size = 10 color = #3300FF><bold>Grand Livre<bold></font>"
                                         "<br/><br/><br/>";
    //QFont   titreFont                  ("Times", 40, QFont::Bold);
    //QTextCharFormat titreFormat ;
    //titreFormat                       . setFont(titreFont);
    QTextBlockFormat centrer ;
       //centrer                       .setBackground(Qt::yellow) ;
       centrer                        .setForeground(Qt::red) ;
       centrer                        .setAlignment(Qt::AlignCenter) ;
       centrer                        .setLeftMargin(24.0) ;
       centrer                        .setRightMargin(32.0) ;
       centrer                        .setTopMargin(16.0) ;
       centrer                        .setBottomMargin(24.0) ;
   
    cursortrie                        ->insertBlock(centrer);
    //cursortrie                        ->insertFragment(titredoc);
    cursortrie                        ->insertHtml(titreDoc);
    cursortrie                        ->movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,1);
//janvier
    QString unjanv                    = m_date.toString("yyyy")+"-01-01";
    QString trenteetunjanv            = m_date.toString("yyyy")+"-01-31";
    QString janvier                   = trUtf8("janvier");
    QString janvList;
    janvList                          = unjanv +","+ trenteetunjanv;
    

//fevrier------------mois particulier !
    QString unfev                     = m_date.toString("yyyy")+"-02-01";
    QString vingthuitfev              = m_date.toString("yyyy")+"-02-28";
        if(nombrejoursannee == 366){
            vingthuitfev  = m_date.toString("yyyy")+"-02-29";
            }
    QString fevrier                   =  trUtf8("février");
    QString fevList;
    fevList                           = unfev +","+ vingthuitfev;
//mars
    	QString unmars                = m_date.toString("yyyy")+"-03-01";
	QString trenteetunmars        = m_date.toString("yyyy")+"-03-31";
	QString mars = trUtf8("mars");
	QString marsList;
	marsList                      = unmars +","+ trenteetunmars;
 //avril
    	QString unavril               = m_date.toString("yyyy")+"-04-01";
	QString trenteavril           = m_date.toString("yyyy")+"-04-30";
	QString avril                 = trUtf8("avril");
	QString avrList;
	avrList                       = unavril +","+ trenteavril;
 //mai
    	QString unmai                 = m_date.toString("yyyy")+"-05-01";
	QString trenteetunmai         = m_date.toString("yyyy")+"-05-31";
	QString mai                   = trUtf8("mai");
	QString maiList;
	maiList                       = unmai +","+ trenteetunmai;
 //juin
    	QString unjuin                = m_date.toString("yyyy")+"-06-01";
	QString trentejuin            = m_date.toString("yyyy")+"-06-30";
	QString juin                  = trUtf8("juin");
	QString juinList;
	juinList                     = unjuin +","+ trentejuin;
 //juillet
     	QString unjuillet            = m_date.toString("yyyy")+"-07-01";
	QString trenteetunjuillet    = m_date.toString("yyyy")+"-07-31";
	QString juillet              = trUtf8("juillet");
	QString juillList;
	juillList                    = unjuillet +","+ trenteetunjuillet;
 //aout
    	QString unaout               = m_date.toString("yyyy")+"-08-01";
	QString trenteetunaout       = m_date.toString("yyyy")+"-08-31";
	QString aout                 = trUtf8("août");
	QString aouList;
	aouList                      = unaout +","+ trenteetunaout;
 //septembre
    	QString unsept               = m_date.toString("yyyy")+"-09-01";
	QString trentesept           = m_date.toString("yyyy")+"-09-30";
	QString sept                 = trUtf8("septembre");
	QString sepList;
	sepList                      = unsept +","+ trentesept;
 //octobre
    	QString unoct                = m_date.toString("yyyy")+"-10-01";
	QString trenteetunoct        = m_date.toString("yyyy")+"-10-31";
	QString oct                  = trUtf8("octobre");
	QString octList;
	octList                      = unoct +","+ trenteetunoct;
 //novembre
    	QString unnov                = m_date.toString("yyyy")+"-11-01";
	QString trentenov            = m_date.toString("yyyy")+"-11-30";
	QString nov                  = trUtf8("novembre");
	QString novList;
	novList                      = unnov +","+ trentenov;
 //decembre
    	QString undec                = m_date.toString("yyyy")+"-12-01";
	QString trenteetundec        = m_date.toString("yyyy")+"-12-31";
	QString dec                  = trUtf8("décembre");
	//modele(dec, undec , trenteetundec ,tableFormat,cursortrie,m_listforquery);
	//cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
	QString decList;
	decList                      = undec +","+ trenteetundec;
	
//annee
        QString annee                = trUtf8("année entière");
	QString anneeList;	             
	anneeList                    = unjanv+","+ trenteetundec;
//--------------construction de listes des types----------------------------------------------------------
        QFile file(Common::ICore::instance()->settings()->applicationBundlePath()+"/log1");
        if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
            return;
        QTextStream stream(&file);
        QSqlQuery queryTypeHono(m_db);
                  queryTypeHono.exec("SELECT acte FROM honoraires WHERE id_usr = '"+QString::number(m_usr)+"'; ");
                  while(queryTypeHono.next()){
                      QString strtype = queryTypeHono.value(0).toString();
                        if(!m_typesRecettes.contains(strtype)){
                            m_typesRecettes << strtype;
                        }
                  }
                  for (int i = 0 ; i < m_typesRecettes.size() ; i++){
                      stream << m_typesRecettes[i]+"\n";
                  }

    QSqlQuery queryTypeDepense(m_db);
              queryTypeHono.exec("SELECT libelle FROM mouvements WHERE id_usr = '"+QString::number(m_usr)+"'; ");
              while(queryTypeHono.next()){
                  QString type   = queryTypeHono.value(0).toString();
                    if(!m_typesDepenses.contains(type)){
                        m_typesDepenses << type;
                        }
              }
              for (int i = 0 ; i < m_typesDepenses.size() ; i++){
                  stream << m_typesDepenses[i]+"\n";
              }
//------------boucles mois ---------------------------------------------------------------------
    QStringList listeMois;
        listeMois << janvier << fevrier << mars << avril << mai << juin 
                  << juillet << aout << sept << oct << nov << dec << annee ;
    QStringList datesMois;
        datesMois << janvList << fevList << marsList << avrList << maiList << juinList << juillList
                  << aouList << sepList << octList << novList << decList << anneeList;
    for (int i = 0 ; i < listeMois.size() ; i++)
    {
        QString mois      = listeMois[i];
        QString dates     = datesMois[i];
        QStringList list  = dates.split(",");
        QString debutMois = list[0];
        QString finMois   = list[1];
        
        modele(listeMois[i], debutMois , finMois ,tableFormat,cursortrie);
        cursortrie->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    }
       		

}
//----------fonction de construction du tableau type -------------------------------------------
QTextTableFormat produceDoc::monformat(QTextTableFormat &tableFormat,QString & parametrespourtableformat){
     QTextTableFormat table         = tableFormat;
     QString parametrespourfonction = parametrespourtableformat;
     QStringList listeparametres    = parametrespourfonction.split(",");
     tableFormat                     .setBackground(QColor("#C0C0C0"));
     tableFormat                     .setAlignment(Qt::AlignCenter);
     tableFormat                     .setCellPadding(2);
     tableFormat                     .setCellSpacing(2);
     QVector<QTextLength> contraintes;
         for(int i = 0;i < listeparametres.size() ; i++){
             contraintes << QTextLength(QTextLength::FixedLength, listeparametres[i].toInt());
             }

     tableFormat                     .setColumnWidthConstraints(contraintes);
     return table;
}


//-------------fonction de calcul des colonnes recettes---------------------------
QStringList produceDoc::calculRecettes( QString & datedebut, QString & datefin){
    qDebug() << "in calculrecettes";
    QString date1                    = datedebut;
    QString date2                    = datefin;
   // qDebug() << date1 << date2;
    double sommetotale               = 0;
    double sommeEsp                  = 0;
    double sommeChq                  = 0;
    double sommeCB                   = 0;
    double sommeVir                  = 0;
    QStringList listetype;
    QSqlQuery query(m_db);
        if(!query.exec("SELECT esp,chq,cb,vir FROM honoraires "
                   " WHERE id_usr = '"+QString::number(m_usr)+"' AND date BETWEEN '"+datedebut+"' AND '"+datefin+"';")){
                   qDebug() << query.lastError().text() << __FILE__ << __LINE__;
                   }
                   //
        while(query.next()){
            double esp   = query.value(0).toDouble();
            double chq   = query.value(1).toDouble();
            double cb    = query.value(2).toDouble();
            double vir   = query.value(3).toDouble();
            sommetotale += esp+chq+cb+vir;
            sommeEsp    += esp;
            sommeChq    += chq;
            sommeCB     += cb;
            sommeVir    += vir;
        }
    QString totalSumReceipts = QString::number(sommetotale);
    QString sommeEspStr  = QString::number(sommeEsp);
    QString sommeChqStr  = QString::number(sommeChq);
    QString sommeCBStr   = QString::number(sommeCB);
    QString sommeVirStr  = QString::number(sommeVir);
    QStringList sommeList;
        sommeList << totalSumReceipts << sommeEspStr << sommeChqStr << sommeCBStr << sommeVirStr;
        
    return sommeList;
}
//----------------fonction de calcul des depenses--------------------------------------------------
QStringList produceDoc::calculDepenses(QString & dateun, QString & datedeux){
    qDebug() << "in calcul depenses";
    QString debutMois    = dateun;
    QString finMois      = datedeux;
    QStringList sommeParType;
    QSqlQuery query(m_db);
    for (int i = 0 ; i< m_typesDepenses.size() ; i++){
       double somme    = 0;
       QString requete = "SELECT  montant FROM mouvements "
                         " WHERE id_usr = '"+QString::number(m_usr)+"' AND libelle = '"+m_typesDepenses[i]+"' AND "
                         "date BETWEEN '"+debutMois+"' AND '"+finMois+"'; ";
                         
          if(!query.exec(requete)){
              qDebug() << query.lastError().text() << __LINE__;
          }
          while(query.next()){
          double montant = query.value(0).toDouble();
                 somme  += montant;
          }
          QString paire = m_typesDepenses[i]+"="+QString::number(somme);
          sommeParType << paire;
    }
    return sommeParType;
}
//----------------------fonction d'insertion des resultats de calcul des recettes dans chaque colonne.
/*void grandlivre::calculparmois(QStringList & liste,QTextTable *tableau, QString & datedebut, QString & datefin){
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

    cellCursor1.insertText(""+calc+"");

    }

}*/
//--------------------fonction de remplissage de tableau par mois-------------------------------------------
bool produceDoc::modele(QString & mois,QString & jour1 , QString & dernierjour ,QTextTableFormat & tableFormattableau,
                        QTextCursor *cursortriefonction ){
    if(!m_db.isOpen()){
        qDebug() << m_db.lastError().text() << __LINE__;
    }
    QString cemois                       = mois;
    QString un                           = jour1;
    QString trenteetquelque              = dernierjour;
    QTextTableFormat tableFormatModele   = tableFormattableau;
    QTextCursor *cursortrieinfonctionMod = cursortriefonction;



    QList<QVector<QString> > tableauLivreMois;
    QString requeteModele = "SELECT date,patient,esp,chq,cb,vir,praticien,acte FROM honoraires "
	                    " WHERE id_usr = '"+QString::number(m_usr)+"' AND "
	                    "date BETWEEN '"+un+"' AND '"+trenteetquelque+"';";
	                   // qDebug() << QString::number(m_usr) << un << trenteetquelque;
    QSqlQuery queryRequeteModele(m_db);
        if(!queryRequeteModele.exec(requeteModele)){
            qDebug() << queryRequeteModele.lastError().text();
        }
        
            while(queryRequeteModele.next()){
                QString vect0 = queryRequeteModele.value(0).toString();//date
                QString vect1 = queryRequeteModele.value(1).toString();       
                QString vect2 = queryRequeteModele.value(2).toString();//esp
                QString vect3 = queryRequeteModele.value(3).toString();//chq
                QString vect4 = queryRequeteModele.value(4).toString();//cb
                QString vect5 = queryRequeteModele.value(5).toString();//vir
                QString vect6 = queryRequeteModele.value(6).toString();//praticien
                QString vect7 = queryRequeteModele.value(7).toString();//acte
                QVector<QString> vector;
                vector       << vect0 << vect1 << vect2 << vect3 << vect4 << vect5 << vect6 << vect7;
                tableauLivreMois << vector;//on a le tableau date,patient,esp,chq,cb,vir,praticien,acte du mois
            }
	     //qDebug() << "tableauLivreMois.size()= "+ QString::number(tableauLivreMois.size()) << __LINE__;                 

//---------------itération recettes par type-----------------------------------------------------------------
    QList<QVector<QString> > tableauLivreTypeRecette;
        
        for(int i = 0; i <  m_typesRecettes.size(); i++){
            QString typeRecettes = m_typesRecettes[i];
            //qDebug() << "type recettes ="+m_typesRecettes[i];
            QStringList listeLibelle;
            listeLibelle << "esp" << "chq" << "cb" << "vir";
            for (int j = 0 ; j < listeLibelle.size() ; j++){
               QVector<QString> vector;
               foreach(vector,tableauLivreMois){
                  if(vector[7]== typeRecettes && ((vector[j+2] == QString::number(0)) == false))
                  {
                      QVector<QString> vectorLibelle;
                      recupSlot(cemois+" : "+vector[0]);//renseigne le QLabel label_5
                      vectorLibelle           << vector[0] << vector[1] << vector[j+2] << vector[6] << vector[7];
                      tableauLivreTypeRecette << vectorLibelle;
                  }
                }
            }
       }
    QStringList sommeRecettes;
                sommeRecettes   = calculRecettes( un, trenteetquelque);
    //qDebug() << "tableauLivreTypeRecette.size()= "+ QString::number(tableauLivreTypeRecette.size()) << __LINE__;
    fonctionremplir(tableauLivreTypeRecette,tableFormatModele,cursortrieinfonctionMod,cemois,sommeRecettes,1);
//----------------tableau depenses---------------------------------------------------------
    QList<QVector<QString> > tableauLivreMoisDepenses;
    QString requeteModDepense = "SELECT date,libelle,montant,remarque,detail FROM mouvements "
	                        " WHERE id_usr = '"+QString::number(m_usr)+"' AND "
	                        "date BETWEEN '"+un+"' AND '"+trenteetquelque+"' ;";
    QSqlQuery queryModDepense(m_db);
        queryModDepense.exec(requeteModDepense);
        while(queryModDepense.next()){
            QString vect0 = queryModDepense.value(0).toString();
            QString vect1 = queryModDepense.value(1).toString();    //libelle   
            QString vect2 = queryModDepense.value(2).toString();
            QString vect3 = queryModDepense.value(3).toString();
            QString vect4 = queryModDepense.value(4).toString();
            QVector<QString> vector;
                vector    << vect0 << vect1 << vect2 << vect3 << vect4 ;
            tableauLivreMoisDepenses << vector;
        }
       // qDebug() << "tableauLivreMoisDepenses= "+ QString::number(tableauLivreMoisDepenses.size()) << __LINE__;
//----------------iteration des depenses par type------------------------------------------------------
    QList<QVector<QString> > tableauLivreTypeDepense;
   
            
    for(int i = 0 ; i < m_typesDepenses.size() ; i++){
            QVector<QString> vector;
            foreach(vector,tableauLivreMoisDepenses){
                 if(vector[1] == m_typesDepenses[i]){
                     tableauLivreTypeDepense << vector;
                     }
            }
        }
    QStringList sommedepenses;
                sommedepenses = calculDepenses( un, trenteetquelque);
    //qDebug() << "tableauLivreTypeDepense= "+ QString::number(tableauLivreTypeDepense.size()) << __LINE__;
    fonctionremplir(tableauLivreTypeDepense,tableFormatModele,cursortrieinfonctionMod,cemois,sommedepenses,2);
    return true;
}
///////////////////////////////////////////////////////////////////////////////////
//fin modele-----------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////
void produceDoc::fonctionremplir(QList<QVector<QString> > &tableau,
                                 QTextTableFormat & tableFormattableau,
                                 QTextCursor *cursortrieinfonctionMod,
                                 QString& cemois, 
                                 QStringList & sommeList, 
                                 int choix){
    qDebug() << "in fonction remplir";
    QList<QVector<QString> > tableauInFonction;
                         tableauInFonction         = tableau;
        int              nbreLignesTableau         = tableauInFonction.size();
        int              nbreColonnesTableau       = 5;
        int              tailleTableau             = nbreLignesTableau*nbreColonnesTableau;
        QTextTableFormat tableFormat               = tableFormattableau;
        QTextCursor     *cursortrieinfonction      = cursortrieinfonctionMod;
        QString          cemoisfonction            = cemois;
        int              choixfonction             = choix;
        QString          type                      = "";
        QStringList      totaux                    = sommeList;
        if(choix == 1){
            type = trUtf8("recettes");
            
                        }
        if(choix == 2){
            type = trUtf8("dépenses");
            }
        QTextBlockFormat centrerHead ;
       //centrer                       .setBackground(Qt::yellow) ;
           centrerHead                    .setAlignment(Qt::AlignCenter) ;
           cursortrieinfonction         -> insertBlock(centrerHead);
           cursortrieinfonction         -> insertHtml("<font size = 6 color = #3300FF><bold><br/>"
                                                      "<br/>Mois de "+cemoisfonction+" = "+type+"<bold>"
                                                      "</font><br/><br/>");
        
        QTextTableFormat tableFormatRecapitule;   
        monformat(tableFormatRecapitule,m_parametresTablesRecap);
        
        if(tailleTableau!= 0){
            if((cemoisfonction == trUtf8("année entière")) == false){
                QTextTable * table = cursortrieinfonction->insertTable(nbreLignesTableau,
                                                                       nbreColonnesTableau,
                                                                       tableFormat);
            qDebug() << "tableau.size() ="+QString::number(tableauInFonction.size());
            for(int i=0 ; i< nbreLignesTableau ; i++){
                QVector<QString> vectorString;
                    vectorString = tableauInFonction[i];
                QStringList liste; // liste des données de la ligne
    	        for (int a = 0 ;a < nbreColonnesTableau ; a++){
                    QString str = vectorString[a];
                    
                         if(str.contains("/")){//a cause de la methode Maeker d'entree des recettes.
                             QStringList listetrois = str.split("//");
                             str                    = listetrois[0];
                         }

                         liste << str;
                   }
                   double s = liste[2].toDouble();
                   if(s > 0){
                      for(int j= 0 ; j < nbreColonnesTableau ; j++){
                          QTextTableCell cell    = table->cellAt(i,j);
                          QTextCursor cellCursor = cell.firstCursorPosition();
                          cellCursor             . insertText(liste[j]);
                      }
                   }
            }
      
    QTextBlockFormat centrer ;
       //centrer                       .setBackground(Qt::yellow) ;
       centrer                        .setAlignment(Qt::AlignCenter) ;
       cursortrieinfonction         -> insertBlock(centrer);
       cursortrieinfonction         -> insertText("\n \n");
//----------------insertion fin de table-------------------------------------------
    table                           -> insertRows(table->rows(),1);
    table                           -> mergeCells(table->rows()-1,0,1,2);//-1 car part de zero
    QTextTableCell cell              = table->cellAt(table->rows()-1,0);
     QTextCursor cellCursor          = cell.firstCursorPosition();
     cellCursor                       .insertText(trUtf8("Total ")+cemoisfonction+"");
    QTextTableCell cell2             = table->cellAt(table->rows()-1,2);
     QTextCursor cellCursor2         = cell2.firstCursorPosition();
     table                          -> mergeCells(table->rows()-1,2,1,3);
     cellCursor2                      .insertText(totaux[0]);
    cursortrieinfonction            -> movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    
    QTextBlockFormat centrer1 ;
       //centrer1                       .setBackground(Qt::yellow) ;
       centrer1                        .setForeground(Qt::red) ;
       centrer1                        .setAlignment(Qt::AlignCenter);
       cursortrieinfonction          -> insertBlock(centrer1);
       cursortrieinfonction          -> insertHtml ("<font size = 6 color = #3300FF><bold><br/>"
                                                    "<br/>Cumul des "+type+" de "+cemoisfonction+"<bold></font>"
                                                    "<br/><br/>");       
    }
//---------------insertion table recapitulative----------------------------------
    QTextTable *tableRecap;
    if(choixfonction == 1){
        QString totalSums = totaux[TOTAL_SUM];
        QString esp               = totaux[ESP_SUM];
        QString chq               = totaux[CHQ_SUM];
        QString cb                = totaux[CB_SUM];
        QString vir               = totaux[VIR_SUM];
        nbreLignesTableau         = TotalSumsMaxParam;
        tableRecap                = cursortrieinfonction->insertTable(nbreLignesTableau,2,tableFormatRecapitule);
        QTextTableCell cell00     = tableRecap->cellAt(0,0);
         QTextCursor cellCursor00 = cell00.firstCursorPosition();
         cellCursor00             . insertText(trUtf8("Total Espèces"));
        QTextTableCell cell01     = tableRecap->cellAt(0,1);
         QTextCursor cellCursor01 = cell01.firstCursorPosition();
         cellCursor01             . insertText(esp);
        QTextTableCell cell10     = tableRecap->cellAt(1,0);
         QTextCursor cellCursor10 = cell10.firstCursorPosition();
         cellCursor10             . insertText(trUtf8("Total chèques"));
        QTextTableCell cell11     = tableRecap->cellAt(1,1);
         QTextCursor cellCursor11 = cell11.firstCursorPosition();
         cellCursor11             . insertText(chq);
        QTextTableCell cell20     = tableRecap->cellAt(2,0);
         QTextCursor cellCursor20 = cell20.firstCursorPosition();
         cellCursor20             . insertText(trUtf8("Total CB"));
        QTextTableCell cell21     = tableRecap->cellAt(2,1);
         QTextCursor cellCursor21 = cell21.firstCursorPosition();
         cellCursor21             . insertText(cb);
        QTextTableCell cell30     = tableRecap->cellAt(3,0);
         QTextCursor cellCursor30 = cell30.firstCursorPosition();
         cellCursor30             . insertText("Total virements");
        QTextTableCell cell31     = tableRecap->cellAt(3,1);
         QTextCursor cellCursor31 = cell31.firstCursorPosition();
         cellCursor31             . insertText(vir);
        QTextTableCell cell40     = tableRecap->cellAt(4,0);
         QTextCursor cellCursor40 = cell40.firstCursorPosition();
         cellCursor40             . insertText("Totaux");
        QTextTableCell cell41     = tableRecap->cellAt(4,1);
         QTextCursor cellCursor41 = cell41.firstCursorPosition();
         cellCursor41             . insertText(totalSums);
    }
    
    if(choixfonction == 2){
        nbreLignesTableau = m_typesDepenses.size();
        tableRecap        = cursortrieinfonction->insertTable(nbreLignesTableau,2,tableFormatRecapitule);
        for(int i = 0 ; i < nbreLignesTableau ; i++){
            QStringList paireDepenseMontant = totaux[i].split("=");
            QTextTableCell cellDep          = tableRecap->cellAt(i,0);
             QTextCursor cellCursorDep      = cellDep.firstCursorPosition();
             cellCursorDep                  . insertText(paireDepenseMontant[0]);
            QTextTableCell cellDep1         = tableRecap->cellAt(i,1);
             QTextCursor cellCursorDep1     = cellDep1.firstCursorPosition();
             cellCursorDep1                 . insertText(paireDepenseMontant[1]);
        }
    }
     

    //calculparmois(listforquery,table, un,trenteetquelque);//calcul par type recette et mois
    cursortrieinfonction  ->movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    }
   qDebug() << "FIN fonction remplir";
}

void produceDoc::recupSlot(const QString &text){
    //qDebug() << text;
    emit outThread(text);
}
QTextDocument *produceDoc::recupTextDoc(){

//QTextDocument *doc = new QTextDocument("essai");
//doc->moveToThread(QApplication::instance()->thread());
//return doc;
  return m_documenttrie;
}

void produceDoc::dateChosen(QDate &date){
  m_mutex.lock();
  m_date1 = date;
  m_mutex.unlock();
}

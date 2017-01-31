#include "frenchTax.h"

#include <common/icore.h>
#include <common/settings.h>
#include <common/commandlineparser.h>
#include <common/constants.h>
#include <common/databasemanager.h>
#include <common/position.h>
#include <QtGui>
#include <QtCore>
#include <QtSql>

using namespace Common;
using namespace Constants;
static inline QString htmlPath(){return Common::ICore::instance()->settings()->applicationResourcesPath()+"/html";}
static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}
frenchTax::frenchTax(){
  setupUi(this);
  Position *position = new Position;
  position->centralPosition(this,width(),height());
  setAttribute(Qt::WA_DeleteOnClose);
  m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  enumMapping();
  dateEdit->setDisplayFormat("yyyy");
  dateEdit->setToolTip(trUtf8("Année."));
  dateEdit->setDate(QDate::currentDate());
  m_beginYear = "";
  m_endYear = "";
  userComboBox->setEditable(false);
  userComboBox->setInsertPolicy(QComboBox::NoInsert);
  QStringList loginList;
  QSqlQuery queryUser(m_db);
            if(!queryUser.exec("SELECT login FROM utilisateurs")){
                messNoQuery(queryUser,__LINE__);
                emit close();
                return;
                }
            while(queryUser.next()){
                QString loginStr = queryUser.value(0).toString();
                loginList << loginStr;
                }
  m_dirForHtmlFile = QDir::homePath()+QDir::separator()+"rapidcompta2035";
  QDir dir2035(m_dirForHtmlFile);
  if (!dir2035.exists())
  {
      dir2035.mkdir(m_dirForHtmlFile);
      }
  //prefered user
  QString prefUser = Common::ICore::instance()->settings()->value(S_UTILISATEUR_CHOISI).toString();
  if(!loginList.isEmpty()){
      loginList.prepend(prefUser);
      }
  else{
      loginList << prefUser;
      }
  loginList.removeDuplicates();
  userComboBox->addItems(loginList);
  //hash
  m_hashEnum.insert(AA_recettesEncaissees,"AA_recettesEncaissees");
  m_hashEnum.insert(AB_deboursPourClients,"AB_deboursPourClients");
  m_hashEnum.insert(AC_honorairesRetrocedes,"AC_honorairesRetrocedes");
  m_hashEnum.insert(AD_montantNetRecettes,"AD_montantNetRecettes");
  m_hashEnum.insert(AE_produitsFinanciers,"AE_produitsFinanciers");
  m_hashEnum.insert(AF_gainsDivers,"AF_gainsDivers");
  m_hashEnum.insert(AG_totalRecettes,"AG_totalRecettes");
  m_hashEnum.insert(BA_achats,"BA_achats");
  m_hashEnum.insert(BB_salairesNetPersonnels,"BB_salairesNetPersonnels");
  m_hashEnum.insert(BC_chargesSurSalairesPersonnels,"BC_chargesSurSalairesPersonnels");
  m_hashEnum.insert(BD_TVA,"BD_TVA");
  m_hashEnum.insert(BE_taxeProfessionnelle,"BE_taxeProfessionnelle");
  m_hashEnum.insert(BS_autresImpots,"BS_autresImpots");
  m_hashEnum.insert(BF_loyersEtChargesLocatives,"BF_loyersEtChargesLocatives");
  m_hashEnum.insert(BG_locationMateriels,"BG_locationMateriels");
  m_hashEnum.insert(BH_entretienReparations,"BH_entretienReparations");
  m_hashEnum.insert(BH_personnelInterimaire,"BH_personnelInterimaire");
  m_hashEnum.insert(BH_petitOutillage,"BH_petitOutillage");
  m_hashEnum.insert(BH_chauffageEauGazElectricite,"BH_chauffageEauGazElectricite");
  m_hashEnum.insert(BH_honorairesNonRetrocedes,"BH_honorairesNonRetrocedes");
  m_hashEnum.insert(BH_assurances,"BH_assurances");
  m_hashEnum.insert(BH_totalTravauxFournitureServicesExterieurs,"BH_totalTravauxFournitureServicesExterieurs");
  m_hashEnum.insert(BJ_fraisVehiculesAuto,"BJ_fraisVehiculesAuto");
  m_hashEnum.insert(BJ_fraisVehiculesMoto,"BJ_fraisVehiculesMoto");
  m_hashEnum.insert(BJ_fraisDeplacementsAutres,"BJ_fraisDeplacementsAutres");
  m_hashEnum.insert(BJ_totalFraisDeplacements,"BJ_totalFraisDeplacements");
  m_hashEnum.insert(BK_chargesSocialePersonnelles,"BK_chargesSocialePersonnelles");
  m_hashEnum.insert(BL_fraisCongres,"BL_fraisCongres");
  m_hashEnum.insert(BL_fraisReceptionRepresentations,"BL_fraisReceptionRepresentations");
  m_hashEnum.insert(BL_fraisReceptionRepresentationsCongres,"BL_fraisReceptionRepresentationsCongres");
  m_hashEnum.insert(BM_bureauDocPTT,"BM_bureauDocPTT");
  m_hashEnum.insert(BM_fraisActesContentieux,"BM_fraisActesContentieux");
  m_hashEnum.insert(BM_CotisationsSyndicalesProfessionnelles,"BM_CotisationsSyndicalesProfessionnelles");
  m_hashEnum.insert(BM_autresFraisGestion,"BM_autresFraisGestion");
  m_hashEnum.insert(BM_totalFraisDiversGestion,"BM_totalFraisDiversGestion");
  m_hashEnum.insert(BN_fraisFinanciers,"BN_fraisFinanciers");
  m_hashEnum.insert(BP_pertesDiverses,"BP_pertesDiverses");
  m_hashEnum.insert(BR_TotalDepenses,"BR_TotalDepenses");
  m_hashEnum.insert(CA_excedent,"CA_excedent");
  m_hashEnum.insert(CB_plusValuesCourtTerme,"CB_plusValuesCourtTerme");
  m_hashEnum.insert(CC_DiversAReintegrer,"CC_DiversAReintegrer");
  m_hashEnum.insert(CD_beneficeSCM,"CD_beneficeSCM");
  m_hashEnum.insert(CE_totalExcedentReintegre,"CE_totalExcedentReintegre");
  m_hashEnum.insert(CF_insuffisance,"CF_insuffisance");
  m_hashEnum.insert(CG_fraisEtablissement,"CG_fraisEtablissement");
  m_hashEnum.insert(CH_dotationAmortissements,"CH_dotationAmortissements");
  m_hashEnum.insert(CJ_provisionPourDepreciationImmobilisationsNonAmortissable,"CJ_provisionPourDepreciationImmobilisationsNonAmortissable");
  m_hashEnum.insert(CK_moinsValuesCourtTerme,"CK_moinsValuesCourtTerme");
  m_hashEnum.insert(CL_diversADeduire,"CL_diversADeduire");
  m_hashEnum.insert(CM_deficitSCM,"CM_deficitSCM");
  m_hashEnum.insert(CN_totalDeficitReintegre,"CN_totalDeficitReintegre");
  m_hashEnum.insert(CP_benefice,"CP_benefice");
  m_hashEnum.insert(CR_deficit,"CR_deficit");
  m_hashEnum.insert(DA_montantImmobilisationHorsTVAdeductible,"DA_montantImmobilisationHorsTVAdeductible");
  m_hashEnum.insert(DB_montantTVAafferenteRecettes,"DB_montantTVAafferenteRecettes");
  m_hashEnum.insert(DC_montantTVAafferenteDepenses,"DC_montantTVAafferenteDepenses");
  m_hashEnum.insert(DD_montantTVAafferenteRecettes,"DD_montantTVAafferenteRecettes");
  m_hashEnum.insert(DE_montantTVAafferenteDepenses,"DE_montantTVAafferenteDepenses");

  m_textDoc = new QTextDocument;
  buildMapping();
  connect(pushButton  ,SIGNAL(pressed()),    this,SLOT(close()));
  connect(printButton,SIGNAL(pressed()),    this,SLOT(print()));
  connect(pushButton_3,SIGNAL(pressed()),    this,SLOT(pdf()));
  connect(userComboBox,  SIGNAL(highlighted(int)),this,SLOT(buildMapping()));
  connect(userComboBox,  SIGNAL(activated(int)),this,SLOT(buildMapping()));
  connect(dateEdit,SIGNAL(dateChanged(const QDate &)),this, SLOT(buildMapping()));
}

frenchTax::~frenchTax(){}

void frenchTax::buildMapping(){
  m_sumsMap.clear();
  m_itemMap.clear();
  m_lettersMap.clear();
  QDate year = dateEdit->date();
  QString yearStr = year.toString("yyyy");
  qDebug() << yearStr;
  m_beginYear = yearStr+"-01-01";
  m_endYear = yearStr+"-12-31";
  QString user = userComboBox->currentText();//login
  qDebug() << "user ="+user;
  QSqlQuery queryId(m_db);
  QString reqUser = QString("SELECT id_usr FROM utilisateurs WHERE login = '%1'").arg(user);
          if(!queryId.exec(reqUser)){
              messNoQuery(queryId,__LINE__);
              return;
          }
          qDebug() << queryId.lastQuery();
  QString userId;
  QStringList listUserId;
          while(queryId.next()){
              userId = queryId.value(0).toString();
              listUserId << userId;
          }
          if(listUserId.size() > 1){
              QMessageBox::critical(0,trUtf8("Critical"),
                                    trUtf8("Error = ")+"\n"__FILE__+QString::number(__LINE__),QMessageBox::Ok);
          }
          qDebug() << userId;
  //AA,1,AA_recettesEncaissees
  double totalRecettes = 0.00;
  QSqlQuery queryRecettes(m_db);
  QString reqRecettes = QString( "SELECT esp,chq,cb,vir,autre FROM honoraires "
                        "WHERE id_usr = '%1' AND date BETWEEN '%2' AND '%3'").arg(userId,m_beginYear,m_endYear);
          if(!queryRecettes.exec(reqRecettes)){
              messNoQuery(queryRecettes,__LINE__);
              return;
          }
          qDebug() << queryRecettes.lastQuery();
          while(queryRecettes.next()){
              QString espStr = queryRecettes.value(frenchTax::ESP).toString();              
              double esp   = doubleRegul(espStr);
              QString chqStr = queryRecettes.value(frenchTax::CHQ).toString();
              double chq   = doubleRegul(chqStr);
              QString cbStr = queryRecettes.value(frenchTax::CB).toString();
              double cb    = doubleRegul(cbStr);
              QString virStr = queryRecettes.value(frenchTax::VIR).toString();
              double vir   = doubleRegul(virStr);
              QString otherStr = queryRecettes.value(frenchTax::AUTRE).toString();
              double autre = doubleRegul(otherStr);
              double total = esp+chq+cb+vir+autre;
              totalRecettes += total;
              //qDebug() << "total recettes ="+QString::number(totalRecettes);
              }
  m_lettersMap.insert(AA_recettesEncaissees,"AA");
  m_itemMap.insert(AA_recettesEncaissees,trUtf8("Recettes encaissées"));
  m_sumsMap.insert(AA_recettesEncaissees,QString::number(totalRecettes));
  /*negatives sums : fillMap(int & typeEnum ,
                           QString & letterType,
                           QString & explanation,
                           QString & idMvtDispo,
                           QString & userId )*/
  //AB,2,AB_deboursPourClients
  double ABSum = fillMap(AB_deboursPourClients, "AB","Débours payé pour le compte des clients",userId);
  //AC,3,AC_honorairesRetrocedes
  double ACSum = fillMap(AC_honorairesRetrocedes,"AC","Honoraires rétrocédés",userId);
  //AD,4,AD_montantNetRecettes
  //AD=AA-AB-AC
  double ADSum = totalRecettes - ABSum - ACSum;
  m_lettersMap.insert(AD_montantNetRecettes,"AD");
  m_itemMap.insert(AD_montantNetRecettes,trUtf8("Montant net des recettes"));
  m_sumsMap.insert(AD_montantNetRecettes,QString::number(ADSum));
  //AE,5,AE_produitsFinanciers
  double AESum = fillMap(AE_produitsFinanciers,"AE","Produits financiers",userId);
  //AF,6,AF_gainsDivers
  double AFSum = fillMap(AF_gainsDivers,"AF","Gains divers",userId);
  //AG,7,AG_totalRecettes
  double AGSum = ADSum + AESum + AFSum;//Total Recettes
  m_lettersMap.insert(AG_totalRecettes,"AG");
  m_itemMap.insert(AG_totalRecettes,trUtf8("Total(lignes 4 à 6)"));
  m_sumsMap.insert(AG_totalRecettes,QString::number(AGSum));
  // end of RECETTES-------------------------------------------
  //---------------------------------------------------------------
  //beginning of DEPENSES
  //BA,8,BA_achats
  double BASum = fillMap(BA_achats,"BA","Achats",userId);
  //BB,9,BB_salairesNetPersonnels
  double BBSum = fillMap(BB_salairesNetPersonnels,"BB","Salaires nets et avantages en nature",userId);
  //BC,10,BC_chargesSurSalairesPersonnels
  double BCSum = fillMap(BC_chargesSurSalairesPersonnels,"BC",
                         "Charges sociales sur salaires(parts patronale et ouvrière",userId);
  //BD,11,BD_TVA
  double BDSum = fillMap(BD_TVA,"BD","Taxe sur la valeur ajoutée",userId);
  //BE,BE_taxeProfessionnelle,12
  double BESum = fillMap(BE_taxeProfessionnelle,"BE","Taxe professionnelle",userId);
  //BS,BS_autresImpots,13
  double BSSum = fillMap(BS_autresImpots,"BS","Autres impôts",userId);
  //BF,BF_loyersEtChargesLocatives,14
  double BFSum = fillMap(BF_loyersEtChargesLocatives,"BF","Loyer et charges locatives",userId);
  //BG,BG_locationMateriels,15
  double BGSum = fillMap(BG_locationMateriels,"BG",
          "Location de matériel et de mobilier dont redevance au titre d'un contrat de collaboration",userId);
  //BH,
      //BH_entretienReparations,16
      double BH16 = fillMap(BH_entretienReparations,"BH","Entretien et réparations",userId);
      //BH_personnelInterimaire,17
      double BH17 = fillMap(BH_personnelInterimaire,"BH","Personnel intérimaire",userId);
      //BH_petitOutillage,18
      double BH18 =fillMap(BH_petitOutillage,"BH","Petit outillage",userId);
      //BH_chauffageEauGazElectricite,19
      double BH19 = fillMap(BH_chauffageEauGazElectricite,"BH","Chauffage,eau,gaz,électricité",userId);
      //BH_honorairesNonRetrocedes,20
      double BH20 = fillMap(BH_honorairesNonRetrocedes,"BH",
                                             "Honoraires ne constituant pas des rétrocessions",userId);
      //BH_assurances,21
      double BH21 = fillMap(BH_assurances,"BH","Primes d'assurances",userId);
      //BH_totalTravauxFournitureServicesExterieurs
      double BHSum = BH16 + BH17 + BH18 + BH19 + BH20 + BH21;
      m_lettersMap.insert(BH_totalTravauxFournitureServicesExterieurs,"BH");
      m_itemMap.insert(BH_totalTravauxFournitureServicesExterieurs,
                     trUtf8("TOTAL:travaux,fournitures et services extérieurs."));
      m_sumsMap.insert(BH_totalTravauxFournitureServicesExterieurs,QString::number(BHSum));
  //BJ,
      //BJ_fraisVehiculesAuto,22
      double BJSumAuto = fillMap(BJ_fraisVehiculesAuto,"BJ","Frais de véhicules auto",userId);
      //BJ_fraisVehiculesMoto,22
      double BJSumMoto = fillMap(BJ_fraisVehiculesMoto,"BJ","Frais de véhicules auto",userId);
      //BJ_fraisDeplacementsAutres,23
      double BJSumAutresDeplacements = fillMap(BJ_fraisDeplacementsAutres,"BJ",
                                               "Autres frais de déplacements(voyages...)",userId);
      //BJ_totalFraisDeplacements
      double BJSum = BJSumAuto + BJSumMoto + BJSumAutresDeplacements;
      m_lettersMap.insert(BJ_totalFraisDeplacements,"BJ");
      m_itemMap.insert(BJ_totalFraisDeplacements,trUtf8("TOTAL:transports et déplacements"));
      m_sumsMap.insert(BJ_totalFraisDeplacements,QString::number(BJSum));
  //BK_chargesSocialePersonnelles,24
      double BKSum = fillMap(BK_chargesSocialePersonnelles,"BK","Charges sociales personnelles",userId);
  //BL
      //BL_fraisCongres,
      double BLCongres = fillMap(BL_fraisCongres,"BL","Congrès",userId);
      //BL_fraisReceptionRepresentations,
      double BLRepresentation = fillMap(BL_fraisReceptionRepresentations,"BL",
                                       "Réceptions et représentations, cadeaux",userId);

      //BL_fraisReceptionRepresentationsCongres,25
      double BLSum = BLCongres + BLRepresentation;
      qDebug() << __FILE__ << QString::number(__LINE__) << " " << "QString::number(BLSum) = " << QString::number(BLSum);
      m_lettersMap.insert(BL_fraisReceptionRepresentationsCongres,"BL");
      m_itemMap.insert(BL_fraisReceptionRepresentationsCongres,
                     trUtf8("Frais de réception,de représentation et de congrès"));
      m_sumsMap.insert(BL_fraisReceptionRepresentationsCongres,QString::number(BLSum));
  //BM
      //BM_bureauDocPTT,
      double BM1 = fillMap(BM_bureauDocPTT,"BM","Fournitures de bureau, documentation, PTT",userId);
      //BM_fraisActesContentieux,
      double BM2 = fillMap(BM_fraisActesContentieux,"BM","Frais d'actes et de contentieux",userId);
      //BM_CotisationsSyndicalesProfessionnelles,
      double BM3 = fillMap(BM_CotisationsSyndicalesProfessionnelles,"BM","Cotisations syndicales et professionnelles",userId);
      //BM_autresFraisGestion,
      double BM4 = fillMap(BM_autresFraisGestion,"BM","Autres frais divers de gestion",userId);
      //BM_totalFraisDiversGestion
      double BMSum = BM1 + BM2 + BM3 + BM4;
      m_lettersMap.insert(BM_totalFraisDiversGestion,"BM");
      m_itemMap.insert(BM_totalFraisDiversGestion,
                     trUtf8("TOTAL:frais divers de gestion"));
      m_sumsMap.insert(BM_totalFraisDiversGestion,QString::number(BMSum));
  //BN_fraisFinanciers,30
    double BNSum = fillMap(BN_fraisFinanciers,"BN","Frais financiers",userId);
  //BP_pertesDiverses,31
    double BPSum = fillMap(BP_pertesDiverses,"BP","Pertes diverses",userId);
  //BR_TotalDepenses,32
    double BRSum = BASum + BBSum + BCSum + BDSum + BESum + BSSum +
                   BFSum + BGSum + BHSum + BJSum + BKSum + BLSum +BMSum + BNSum + BPSum ;
    m_lettersMap.insert(BR_TotalDepenses,"BR");
    m_itemMap.insert(BR_TotalDepenses,trUtf8("Total(lignes 8 à 31)"));
    m_sumsMap.insert(BR_TotalDepenses,QString::number(BRSum));

    fillTableWidget();

}

void frenchTax::messNoQuery(QSqlQuery & query , const int line){
  QString error = query.lastError().text()+"\n"+QString::number(line)+__FILE__;
  QMessageBox::critical(0,trUtf8("Critical"),trUtf8("Error = ")+error,QMessageBox::Ok);
}

double frenchTax::fillMap(const int typeEnum ,
                          const char * letterType,
                          const char * explanation,
                          QString & userId ){
  double total = 0.00;
  QStringList idMvtDispoList;
  idMvtDispoList = getListOfMovements(typeEnum);
  qDebug() << __FILE__ << QString::number(__LINE__) << " idMvtDispoList =" << idMvtDispoList.join(";") ;
  foreach(QString idMvtDispo,idMvtDispoList){
      QSqlQuery query(m_db);
      QString req = QString("SELECT %1 FROM mouvements WHERE %2 = '%3' AND %4 = '%5' AND date BETWEEN '%6' AND '%7'")
                            .arg("montant",
                                 "id_mvt_dispo",
                                 QString(idMvtDispo),
                                 "id_usr",
                                 userId,
                                 m_beginYear,
                                 m_endYear);
          qDebug() << __FILE__ << QString::number(__LINE__) << "fill map : "<< req ;
          if(!query.exec(req)){
              qDebug() << query.lastQuery();
              messNoQuery(query,__LINE__);
              emit close();
              return 1.00;
          }
          while(query.next()){
              QString montantStr = query.value(0).toString();
              double montant = doubleRegul(montantStr);
              qDebug() << __FILE__ << QString::number(__LINE__) << " montant =" << QString::number(montant) ;
              total += montant;
          }
      }
  m_lettersMap.insert(typeEnum,letterType);
  m_itemMap.insert(typeEnum,trUtf8(explanation));
  m_sumsMap.insert(typeEnum,QString::number(total));
  return total;
}

void frenchTax::fillTableWidget(){
  m_textDoc->clear();
  QTextCursor curs(m_textDoc);
  int r = 0;
  int c = 3;
  r = m_lettersMap.count();
  tableWidget->setRowCount(r);
  tableWidget->setColumnCount(c);
  for(int i = 1;i < BR_TotalDepenses+1  ; i++){
      QTableWidgetItem *itemLetters = new QTableWidgetItem(m_lettersMap.value(i));
      QTableWidgetItem *itemExpl = new QTableWidgetItem(m_itemMap.value(i));
      QTableWidgetItem *itemSum = new QTableWidgetItem(m_sumsMap.value(i));
      tableWidget->setItem(i-1,0,itemLetters);
      tableWidget->setItem(i-1,1,itemExpl);
      tableWidget->setItem(i-1,2,itemSum);
      QString text = m_lettersMap.value(i)+"   "+m_itemMap.value(i)+"   :   "+m_sumsMap.value(i)+" "+settings()->value(S_TYPE_OF_MONEY).toString()+"\n";
      curs.insertText(text);
      curs.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor,1);
      curs.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,1);
      }
}

void frenchTax::print(){
     QPrinter printer;
     QPrintDialog printdialog(&printer,this);
                printdialog .setWindowTitle(tr("Imprimer"));
                printdialog.exec();
                m_textDoc->print(&printer);
}

void frenchTax::pdf(){
  QString x = Common::ICore::instance()->settings()->value(Constants::X_2035).toString();
  QString y = Common::ICore::instance()->settings()->value(Constants::Y_2035).toString();
  qDebug() << "x ="+x;
  QStringList listHtml;
  QPixmap image(htmlPath()+"/resources_html/pg-3001.png");
  QString rapidcompta2035 = "rapidcompta2035";  
  if(modifFileToNewHtml()){
      QFile fileHtml(m_dirForHtmlFile+QDir::separator()+"pageTroisMod.html");
      if(!fileHtml.open(QIODevice::ReadOnly)){
          qWarning() << "pageTroisMod.html cannot open or not found";
          }
      QTextStream streamHtml(&fileHtml);
      streamHtml.setCodec("UTF-8");
      while(!streamHtml.atEnd()){
          QString line = streamHtml.readLine();
          listHtml << line+"\n";
          }
      QPainter *paint = new QPainter;
      if(paint->begin(&image)){
          qDebug() << "Painting image is possible";
          }
      paint->setPen(QPen(Qt::black));
      paint->setFont(QFont("Helvetica",6.5,QFont::Normal));
      //paint->setBackground(image);
      QString str;
      foreach(str,listHtml){
          if(str.contains("<DIV")&& !str.contains("relative")){
              if(str.contains("REVENUS 1998")){
                  str.replace("REVENUS 1998","REVENUS  "+QDate::currentDate().toString("yyyy"));
                  }
          str.remove("<DIV style=\"position:absolute;top:");
          str.remove("left:");
          str.remove("\"><nobr><span class");
          str.remove("</span></nobr></DIV>");
          str.remove("\"ft");
          str.remove("\n");
          str.replace("&nbsp;"," ");
          if(str.contains("<br>")){
              if(str.contains("TOTAL :")){
                  QString rightStr = str.split("<b>")[1];
                  rightStr.remove("<b>");
                  rightStr.remove("</b>");
                  qDebug() << "rightstr = "+rightStr;
                  QString pos = str.split("=")[0];
                  int y = pos.split(";")[0].toInt();
                  int x = pos.split(";")[1].toInt();
                  QStringList list = rightStr.split("<br>");
                  QString str;
                  foreach(str,list){
                     paint->drawText(QPointF(x,y),str);
                     y+= 9;
                     }
                  continue;
                  }
              else{
                  str.remove("<br>");
                  str.replace("<b>","");
                  str.replace("</b>","");
                  QString rightStr = str.split(">")[1];
                  QString pos = str.split("=")[0];
                  int y = pos.split(";")[0].toInt();
                  int x = pos.split(";")[1].toInt();
                  for(QString::iterator it = rightStr.begin();it != rightStr.end() ; it++ ){
                     paint->drawText(QPointF(x,y),QString(*it));
                     y += 11;
                     }
                  continue;
                  }
              }
          if(str.contains(">")){
              str.replace("<b>","");
              str.replace("</b>","");
              QStringList listUn = str.split(">");
              QString sum = listUn[1];
              QStringList listTwo = listUn[0].split("=");
              QString pos = listTwo[0];
              QStringList listThree = pos.split(";");
              int h = 0;
              int w = 0;
              if(y.contains("-")){
                  qDebug() << "-";
                  h = listThree[0].toInt()-y.toInt();
                  }
              else{
                  h = listThree[0].toInt()+y.toInt();
                  }
              if(x.contains("-")){
                  w  = listThree[1].toInt()-x.toInt();
                  }
              else{
                  w  = listThree[1].toInt()+x.toInt();
                  }
              paint->drawText(QPointF(w,h),sum);
              }
            }
         }//foreach
  image.save(m_dirForHtmlFile+QDir::separator()+"image.png");
  QTextDocument *doc = new QTextDocument;
  QTextCursor c(doc);
  c.insertImage(m_dirForHtmlFile+QDir::separator()+"image.png");
  QPrinter *p = new QPrinter();
  QPrintDialog printDialog(p,this);
  if(printDialog.exec() == QDialog::Accepted){
      doc->print(p);
      }
  paint->end();
      }//if
  else{qWarning() << "Erreur __FILE__ "+QString::number(__LINE__);}

}

void frenchTax::enumMapping(){
  m_enumMap.insert(AA_recettesEncaissees ,"AA_recettesEncaissees");
  m_enumMap.insert(      AB_deboursPourClients,"AB_deboursPourClients");
  m_enumMap.insert(      AC_honorairesRetrocedes,"AC_honorairesRetrocedes");
  m_enumMap.insert(      AD_montantNetRecettes,"AD_montantNetRecettes");
  m_enumMap.insert(      AE_produitsFinanciers,"AE_produitsFinanciers");
  m_enumMap.insert(      AF_gainsDivers,"AF_gainsDivers");
  m_enumMap.insert(      AG_totalRecettes,"AG_totalRecettes");
   m_enumMap.insert(     BA_achats,"BA_achats");
  m_enumMap.insert(      BB_salairesNetPersonnels,"BB_salairesNetPersonnels");
  m_enumMap.insert(      BC_chargesSurSalairesPersonnels,"BC_chargesSurSalairesPersonnels");
  m_enumMap.insert(      BD_TVA,"BD_TVA");
   m_enumMap.insert(     BE_taxeProfessionnelle,"BE_taxeProfessionnelle");
    m_enumMap.insert(    BS_autresImpots,"BS_autresImpots");
  m_enumMap.insert(      BF_loyersEtChargesLocatives,"BF_loyersEtChargesLocatives");
  m_enumMap.insert(      BG_locationMateriels,"BG_locationMateriels");
  m_enumMap.insert(      BH_entretienReparations,"BH_entretienReparations");
  m_enumMap.insert(      BH_personnelInterimaire,"BH_personnelInterimaire");
  m_enumMap.insert(      BH_petitOutillage,"BH_petitOutillage");
  m_enumMap.insert(      BH_chauffageEauGazElectricite,"BH_chauffageEauGazElectricite");
  m_enumMap.insert(      BH_honorairesNonRetrocedes,"BH_honorairesNonRetrocedes");
  m_enumMap.insert(      BH_assurances,"BH_assurances");
  m_enumMap.insert(      BH_totalTravauxFournitureServicesExterieurs,"BH_totalTravauxFournitureServicesExterieurs");
  m_enumMap.insert(      BJ_fraisVehiculesAuto,"BJ_fraisVehiculesAuto");
  m_enumMap.insert(      BJ_fraisVehiculesMoto,"BJ_fraisVehiculesMoto");
  m_enumMap.insert(      BJ_fraisDeplacementsAutres,"BJ_fraisDeplacementsAutres");
  m_enumMap.insert(      BJ_totalFraisDeplacements,"BJ_totalFraisDeplacements");
  m_enumMap.insert(      BK_chargesSocialePersonnelles,"BK_chargesSocialePersonnelles");
  m_enumMap.insert(      BL_fraisCongres,"BL_fraisCongres");
  m_enumMap.insert(      BL_fraisReceptionRepresentations,"BL_fraisReceptionRepresentations");
  m_enumMap.insert(      BL_fraisReceptionRepresentationsCongres,"BL_fraisReceptionRepresentationsCongres");
  m_enumMap.insert(      BM_bureauDocPTT,"BM_bureauDocPTT");
  m_enumMap.insert(      BM_fraisActesContentieux,"BM_fraisActesContentieux");
  m_enumMap.insert(      BM_CotisationsSyndicalesProfessionnelles,"BM_CotisationsSyndicalesProfessionnelles");
  m_enumMap.insert(      BM_autresFraisGestion,"BM_autresFraisGestion");
  m_enumMap.insert(      BM_totalFraisDiversGestion,"BM_totalFraisDiversGestion");
  m_enumMap.insert(      BN_fraisFinanciers,"BN_fraisFinanciers");
  m_enumMap.insert(      BP_pertesDiverses,"BP_pertesDiverses");
  m_enumMap.insert(      BR_TotalDepenses,"BR_TotalDepenses");
  m_enumMap.insert(      CA_excedent,"CA_excedent");
  m_enumMap.insert(      CB_plusValuesCourtTerme,"CB_plusValuesCourtTerme");
  m_enumMap.insert(      CC_DiversAReintegrer,"CC_DiversAReintegrer");
  m_enumMap.insert(      CD_beneficeSCM,"CD_beneficeSCM");
  m_enumMap.insert(      CE_totalExcedentReintegre,"CE_totalExcedentReintegre");
  m_enumMap.insert(      CF_insuffisance,"CF_insuffisance");
  m_enumMap.insert(      CG_fraisEtablissement,"CG_fraisEtablissement");
  m_enumMap.insert(      CH_dotationAmortissements,"CH_dotationAmortissements");
  m_enumMap.insert(      CJ_provisionPourDepreciationImmobilisationsNonAmortissable,"CJ_provisionPourDepreciationImmobilisationsNonAmortissable");
  m_enumMap.insert(      CK_moinsValuesCourtTerme,"CK_moinsValuesCourtTerme");
  m_enumMap.insert(      CL_diversADeduire,"CL_diversADeduire");
  m_enumMap.insert(      CM_deficitSCM,"CM_deficitSCM");
  m_enumMap.insert(      CN_totalDeficitReintegre,"CN_totalDeficitReintegre");
  m_enumMap.insert(      CP_benefice,"CP_benefice");
  m_enumMap.insert(      CR_deficit,"CR_deficit");
  m_enumMap.insert(      DA_montantImmobilisationHorsTVAdeductible,"DA_montantImmobilisationHorsTVAdeductible");
  m_enumMap.insert(      DB_montantTVAafferenteRecettes,"DB_montantTVAafferenteRecettes");
  m_enumMap.insert(      DC_montantTVAafferenteDepenses,"DC_montantTVAafferenteDepenses");
  m_enumMap.insert(      DD_montantTVAafferenteRecettes,"DD_montantTVAafferenteRecettes");
  m_enumMap.insert(      DE_montantTVAafferenteDepenses,"DE_montantTVAafferenteDepenses");
}

bool frenchTax::modifFileToNewHtml(){
  QFile fileTrois(htmlPath()+"/pageTrois.html");
  QFile fileTroisMod(m_dirForHtmlFile+QDir::separator()+"pageTroisMod.html");
  if(!fileTrois.open(QIODevice::ReadOnly)){
      qDebug() << "pageTrois not found";
      return false;
      }
    if(!fileTroisMod.open(QIODevice::ReadWrite|QIODevice::Text)){
       qDebug() << "pageTroisMod not found";
       return false;
       }
  QTextStream streamMod(&fileTroisMod);
  QTextStream streamTrois(&fileTrois);
  while(!streamTrois.atEnd()){
      QString line = streamTrois.readLine();
      QMapIterator<int,QString> it(m_enumMap);
      while(it.hasNext()){
          it.next();
          if(line.contains(it.value())){
              if(it.value() != "BL_fraisReceptionRepresentations"){
                  line.replace(it.value(),m_sumsMap.value(it.key()));
              }
          }
      }
      streamMod << line+"\n";
  }
  return true;
}

double frenchTax::doubleRegul(QString text)
{
    if (text.contains(","))
    {
          text.replace(",",".");
        }
    double d = text.toDouble();
    return d;
}

QStringList frenchTax::getListOfMovements(int enumerateur)
{
    QString enumStr = m_hashEnum.value(enumerateur);
    qDebug() << __FILE__ << QString::number(__LINE__) << "enumStr" << enumStr;
    QString choosenline;    
    QFile file(Common::ICore::instance()->settings()->applicationResourcesPath()+"/csv_files/concordance_mov_impots");
    if (!file.open(QIODevice::ReadOnly))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open concordance_mov_impots " ;
        }
    QTextStream stream(&file);
    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        if (line.contains("\""))
        {
        	line.remove("\"");
            }
        qDebug() << __FILE__ << QString::number(__LINE__) << "line"<< line ;
        QStringList list;
        list = line.split(";");
        if (list.size()>0)
        {
              if (list.contains(enumStr))
              {
                    choosenline = line;
                    qDebug() << __FILE__ << QString::number(__LINE__) << "choose" << choosenline ;
                  }
            }
        }
    qDebug() << __FILE__ << QString::number(__LINE__) << "choosenline"<< choosenline ;
    QStringList listToReturn;
    listToReturn = choosenline.split(";");
    if (listToReturn.size()>1)
    {
          listToReturn.removeAt(0);
        }
    return listToReturn;
}

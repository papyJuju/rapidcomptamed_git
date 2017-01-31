#ifndef FRENCHTAX_H
#define FRENCHTAX_H

#include "ui_frenchTaxDialog.h"
#include <QtGui>
#include <QtCore>
#include <QtSql>

class frenchTax : public QWidget , public Ui::frenchTaxDialog
{
  Q_OBJECT
  public :
  enum frenchTaxItems {
      AA_recettesEncaissees = 1,
      AB_deboursPourClients,
      AC_honorairesRetrocedes,
      AD_montantNetRecettes,
      AE_produitsFinanciers,
      AF_gainsDivers,
      AG_totalRecettes,
      BA_achats,
      BB_salairesNetPersonnels,
      BC_chargesSurSalairesPersonnels,
      BD_TVA,
      BE_taxeProfessionnelle,
      BS_autresImpots,
      BF_loyersEtChargesLocatives,
      BG_locationMateriels,
      BH_entretienReparations,
      BH_personnelInterimaire,
      BH_petitOutillage,
      BH_chauffageEauGazElectricite,
      BH_honorairesNonRetrocedes,
      BH_assurances,
      BH_totalTravauxFournitureServicesExterieurs,
      BJ_fraisVehiculesAuto,
      BJ_fraisVehiculesMoto,
      BJ_fraisDeplacementsAutres,
      BJ_totalFraisDeplacements,
      BK_chargesSocialePersonnelles,
      BL_fraisCongres,
      BL_fraisReceptionRepresentations,
      BL_fraisReceptionRepresentationsCongres,
      BM_bureauDocPTT,
      BM_fraisActesContentieux,
      BM_CotisationsSyndicalesProfessionnelles,
      BM_autresFraisGestion,
      BM_totalFraisDiversGestion,
      BN_fraisFinanciers,
      BP_pertesDiverses,
      BR_TotalDepenses,
      CA_excedent,
      CB_plusValuesCourtTerme,
      CC_DiversAReintegrer,
      CD_beneficeSCM,
      CE_totalExcedentReintegre,
      CF_insuffisance,
      CG_fraisEtablissement,
      CH_dotationAmortissements,
      CJ_provisionPourDepreciationImmobilisationsNonAmortissable,
      CK_moinsValuesCourtTerme,
      CL_diversADeduire,
      CM_deficitSCM,
      CN_totalDeficitReintegre,
      CP_benefice,
      CR_deficit,
      DA_montantImmobilisationHorsTVAdeductible,
      DB_montantTVAafferenteRecettes,
      DC_montantTVAafferenteDepenses,
      DD_montantTVAafferenteRecettes,
      DE_montantTVAafferenteDepenses
  };
  
  enum typesOfPayments{
      ESP = 0,
      CHQ,
      CB,
      VIR,
      AUTRE
  };
  frenchTax();
  ~frenchTax();
  private :
  void fillTableWidget();
  void enumMapping();
  bool modifFileToNewHtml();
  void messNoQuery(QSqlQuery & /*query*/, const int /*line*/);
  double fillMap (const int /*typeEnum*/ ,
                  const char * /*letterType*/,
                  const char * /*explanation*/,
                  QString & /*userId*/ );
  double doubleRegul(QString text);
  QStringList getListOfMovements(int enumerateur);
  QSqlDatabase m_db;
  QTextDocument *m_textDoc;
  QMultiMap <int,QString> m_enumMap ,m_lettersMap, m_itemMap, m_sumsMap;
  QHash<int,QString> m_hashEnum;
  QString m_beginYear;
  QString m_endYear;
  QString m_dirForHtmlFile;
  private slots :
  void buildMapping();
  void print();
  void pdf();

};

#endif

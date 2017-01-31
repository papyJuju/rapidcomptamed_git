#ifndef TABLESDIAGRAMS_H
#define TABLESDIAGRAMS_H

#include <QHash>

namespace TablesDiagramsMouvements 
{
    enum MouvementsDisponiblesHeaders
    {
        ID_MOUVEMENT_DISPO = 0,
        ID_MOUVEMENT_PARENT,
        TYPE_MOUVEMENT_DISPO,
        LIBELLE_MOUVEMENT_DISPO,
        REMARQUE_MOUVEMENT_DISPO,
        MouvementsDisponiblesHeaders_MaxParam
        };
    enum MouvementsHeaders
    {
        ID_MOUVEMENT = 0,
        ID_MVT_DISPO,
        ID_USR,
        ID_COMPTE,
        TYPE,
        LIBELLE,
        DATE,
        DATE_VALEUR,
        MONTANT,
        MONTANT_TOTAL,
        REMARQUE,
        VALIDE,
        TRACABILITE,
        VALIDATION,
        DETAIL,
        DIVERS_MOUVEMENTS,
        MouvementsHeaders_MaxParam
        };
    enum LibellesMouvements
    {
        APPORTS_PRATICIEN = 0,
        AUTRES_RECETTES,
        PRODUITS_FINANCIERS,
        TVA_RECUPERABLE,
        PRELEVEMENTS_DU_PRATICIEN,
        ACHATS_FOURNITURES_ET_PHARMACIE,
        FRAIS_DE_PERSONNEL,
        SALAIRES_NETS,
        CHARGES_SOCIALES,
        IMPOTS_ET_TAXES,
        TVA_PAYEE,
        TAXE_PROFESSIONNELLE,
        AUTRES_IMPOTS,
        TRAVAUX_FOURNITURES_SERVICES_EXTERIEURS,
        LOYERS_ET_CHARGES,
        LOCATION_MATERIEL,
        REPARATION_ET_ENTRETIEN,  
        PERSONNEL_INTERIM_SECRETARIAT_TELEPHONIQUE,
        PETIT_OUTILLAGE,
        GAZ_ELECTRICITE_CHAUFFAGE_EAU,
        HONORAIRES_RETROCEDES,
        HONORAIRES_NE_CONSTITUANT_PAS_DE_RETROCESSION,
        ASSURANCES,
        TRANSPORTS_ET_DEPLACEMENTS,
        FRAIS_DE_VOITURE,
        FRAIS_MOTO,
        AUTRES_FRAIS_DE_DEPLACEMENT,
        CHARGES_SOCIALES_DU_PRATICIEN,
        VIEILLESSE_ASSURANCE_MALADIE_ALLOC_FAM,
        FRAIS_DIVERS_DE_GESTION,
        CONGRES,
        CADEAUX_REPRESENTATION_ET_RECEPTION,
        FRAIS_DE_BUREAU_DOCUMENTATION_ET_P_ET_T,
        COTISATION_PROFESSIONNELLE_ET_SYNDICALES,
        DIVERS,
        FRAIS_FINANCIERS,
        PERTES,
        DEBOURS_PAYES_POUR_LE_COMPTE_DES_CLIENTS,
        AUTRES_DEPENSES,
        SCM_OUvPARTAGE_DE_FRAIS,
        IMMOBILISATIONS,
        DIVERS_A_REINTEGRER,
        COMPTES_D_ATTENTE,
        Libelles_MaxParam
        };
        static QHash<int,QString> mouvementsColumnsNamesHash()
        {
            QHash <int,QString> hash;
            hash.insert(ID_MOUVEMENT,"id_mouvement");
            hash.insert(ID_MVT_DISPO,"id_mvt_dispo");
            hash.insert(ID_USR,"id_usr");
            hash.insert(ID_COMPTE,"id_compte");
            hash.insert(TYPE,"type");
            hash.insert(LIBELLE,"libelle");
            hash.insert(DATE,"date");
            hash.insert(DATE_VALEUR,"date_valeur");
            hash.insert(MONTANT,"montant");
            hash.insert(MONTANT_TOTAL,"montant_total");
            hash.insert(REMARQUE,"remarque");
            hash.insert(VALIDE,"valide");
            hash.insert(TRACABILITE,"tracabilite");
            hash.insert(VALIDATION,"validation");
            hash.insert(DETAIL,"detail");
            hash.insert(DIVERS_MOUVEMENTS,"divers");
            return hash;
        };
};
namespace TablesDiagramsHonoraires
{
    enum honoraires_fields{
        ID_HONO = 0,
        ID_USR,
        ID_DRTUX_USR,
        PATIENT,
        ID_SITE,
        ID_PAYEURS,
        GUID,
        PRATICIEN,
        DATE,
        ACTE,
        ACTECLAIR,
        REMARQUE,
        ESP,
        CHQ,
        CB,
        VIR,
        DAF,
        AUTRE,
        DU,
        DU_PAR,
        VALIDE,
        TRACABILITE,
        DIVERS_HONORAIRES,
        honoraires_fiels_maxParam
        };
    enum TypeTables
    {
        NGAP=0,
        FORFAITS,
        CCAM
        };
    static QHash<int,QString> honorairesColumnsNamesHash()
    {
        QHash<int,QString> hash;
        hash.insert(ID_HONO,"id_hono");
        hash.insert(ID_USR,"id_usr");
        hash.insert(ID_DRTUX_USR,"id_drtux_usr");
        hash.insert(PATIENT,"patient");
        hash.insert(ID_SITE,"id_site");
        hash.insert(ID_PAYEURS,"id_payeurs");
        hash.insert(GUID,"GUID");
        hash.insert(PRATICIEN,"praticien");
        hash.insert(DATE,"date");
        hash.insert(ACTE,"acte");
        hash.insert(ACTECLAIR,"acteclair");
        hash.insert(REMARQUE,"remarque");
        hash.insert(ESP,"esp");
        hash.insert(CHQ,"chq");
        hash.insert(CB,"cb");
        hash.insert(VIR,"vir");
        hash.insert(DAF,"daf");
        hash.insert(AUTRE,"autre");
        hash.insert(DU,"du");
        hash.insert(DU_PAR,"du_par");
        hash.insert(VALIDE,"valide");
        hash.insert(TRACABILITE,"tracabilite");
        hash.insert(DIVERS_HONORAIRES,"divers");
        return hash;
    }
};
namespace SitesDiagrams
{
  enum SitesHeaders
  {
     ID_PRIMKEY=0,
     ID_SITE,
     SITE,
     ADRESSE_SITE,
     CP_SITE,
     VILLE_SITE,
     TELSITE,
     FAXSITE,
     MAILSITE,
     DIVERS,
     Headers_MaxParam
     };
};
namespace PayeursDiagrams
{
    enum PayeursHeaders
    {
        ID_PRIMKEY=0,
        ID_PAYEURS,
        NOM_PAYEUR,
        ADRESSE_PAYEUR,
        VILLE_PAYEUR,
        CP_PAYEUR,
        PHONE_PAYEUR,
        DIVERS_PAYEURS,
        Headers_MaxParam
        };
};
namespace PaiementsDiagrams
{
    enum PaiementsHeaders
    {
        ID_PAIEMENT=0,
        ID_HON0,
        DATE,
        ID_PAYEURS,
        ACTE,
        ESP,
        CHQ,
        CB,
        VIR,
        DAF,
        AUTRE,
        DU,
        VALIDE,
        PaiementsHeaders_MaxParam
        };
};
namespace FacturationDiagrams
{
    enum FacturationTableHeaders
    {
        ID_FACTURE=0,
        DIVERS,
        FacturationHeaders_MaxParam
        };
};

namespace DatesDiagrams
{
    enum DatesTableHeaders
    {
        ID_DATES_COMPTABLE=0,
        DATE_OPERATION,
        DATE_PAIEMENT,
        DATE_VALEUR,
        DATE_VALIDATION,
        DatesTableHeaders_MaxParam
        };
};

namespace FacturationSpace
{
    enum FacturationHeadersModel 
    {
        PATIENT=0,
        PRATICIEN,
        SITE,
        PAYEUR,
        DATE_OPERATION,
        DATE_PAIEMENT,
        DATE_VALEUR,
        DATE_VALIDATION, 
        PRESTATION,
        VALEUR,
        MODE_PAIEMENT,
        STATUT,
        FacturationHeadersModel_MaxParam
        };
};

namespace HeadingsSpace
{
    enum HeadingsModel
    {
        ID_HEADINGS=0,
        ID_USER,
        ID_SITE,
        HEADINGS,
        FOOTER,
        HeadingsModel_MaxParam
        };
};

namespace CompteBancairesSpace
{
    enum BankAccountsEnum
    {
        ID_COMPTE_BANK=0,
        ID_USR,
        LIBELLE8ACCOUNT,
        TITULAIRE,
        NOM_BANQUE,
        RIB_CODE_BANQUE,
        RIB_CODE_GUICHET,
        RIB_NUMCOMPTE,
        RIB_CLE,
        SOLDE_INITIAL,
        REMARQUE,
        BankAccountsEnum_MaxParam
    };
};

namespace TablesSpace
{
    enum TablesEnum
    {
        ACTES_DISPONIBLES=0,
        CCAM,
        COMPTES_BANCAIRES,
        DATES_COMPTABLE,
        DEPOTS,
        FACTURATION,
        FORFAITS,
        HEADINGS,
        HONORAIRES,
        IMMOBILISATIONS,
        INDEMNITES_DEPLACEMENT,
        INFORMATIONS,
        MOUVEMENTS,
        MOUVEMENTS_DISPONIBLES,
        NAMEINDEX,
        NOTES,
        PAIEMENTS,
        PAYEURS,
        POURCENTAGES,
        RAPPROCHEMENT_BANCAIRE,
        SEANCES,
        SITES,
        THESAURUS,
        UTILISATEURS,
        Z_VERSION
        };
};

namespace AlertesSpace
{
    enum AlertesEnum	
    {
        ID_ALERTES=0,
        ALERTE_COLUMN,
        DATE_ALERTE,
        ID_USR_ALERTE,
        VALID_ALERTE,
        ISREAD
    };
};



#endif


#ifndef EXPORTFISCAL_H
#define EXPORTFISCAL_H

#include "ui_exportfiscal.h"
#include <QWidget>
#include <QSqlDatabase>

enum ExportFiscalEnum
{
    JOURNALCODE=1,
    JOURNALLIB,
    ECRITURENUM,
    ECRITUREDATE,
    COMPTENUM,
    COMPTELIB,
    COMPAUXNUM,
    COMPAUXLIB,
    PIECEREF,
    PIECEDATE,
    ECRITURELIB,
    DEBIT,
    CREDIT,
    ECRITURELET,
    DATELET,
    VALIDDATE,
    MONTANTDEVISE,
    IDEVISE,
    DATERGLT,
    MODERGLT,
    NATOP,
    IDCLIENT,
    ExportFiscalEnumMaxParam
};

enum ExportFiscalConditionsEnum
{
    EXPORTFISCAL_DEBIT=ExportFiscalEnumMaxParam,
    EXPORTFISCAL_CREDIT,
    EXPORTFISCAL_MODERGLT
};

enum TabExportFiscEnum
{
    NBR_TYPE = 0,
    INFORMATION,
    NOMDUCHAMP,
    TYPEDECHAMP,
    TabExportFiscEnumMaxParam
};

/*const char * const tableauCodesExportationFiscale =
"0|INFORMATION|NOM DU CHAMP|TYPE DE CHAMP$"
"1|Le code journal de l'écriture comptable (à blanc si non utilisé)|JournalCode|Alphanumérique$"
"2|Le libellé journal de l'écriture comptable (à blanc si non utilisé)|JournalLib|Alphanumérique$"
"3|Le numéro sur une séquence continue de l'écriture comptable|EcritureNum|Alphanumérique$"
"4|La date de comptabilisation de l'écriture comptable|EcritureDate|Date$"
"5|Le numéro de compte (à blanc si non utilisé)|CompteNum|Alphanumérique$"
"6|Le libellé de compte|CompteLib|Alphanumérique$"
"7|Le numéro de compte auxiliaire (à blanc si non utilisé)|CompAuxNum|Alphanumérique$"
"8|Le libellé de compte auxiliaire (à blanc si non utilisé)|CompAuxLib|Alphanumérique$"
"9|La référence de la pièce justificative|PieceRef|Alphanumérique$"
"10|La date de la pièce justificative|PieceDate|Date$"
"11|Le libellé de l'écriture comptable|EcritureLib|Alphanumérique$"
"12|Le montant au débit|Debit|Numérique$"
"13|Le montant au crédit|Credit|Numérique$"
"14|Le lettrage de l'écriture (à blanc si non utilisé)|EcritureLet|Alphanumérique$"
"15|La date de lettrage (à blanc si non utilisé)|DateLet|Date$"
"16|La date de validation de l'écriture comptable|ValidDate|Date$"
"17|Le montant en devise (à blanc si non utilisé)|Montantdevise|Numérique$"
"18|L'identifiant de la devise (à blanc si non utilisé)|Idevise|Alphanumérique$"
"19|La date de règlement|DateRglt|Date$"
"20|Le mode de règlement|ModeRglt|Alphanumérique$"
"21|La nature de l'opération (à blanc si non utilisé)|NatOp|Alphanumérique$"
"22|L'identification du client (à blanc si non utilisé)|IdClient|Alphanumérique";*/

const char * const tableauCodesExportationFiscale =
"0|INFORMATION|NOM DU CHAMP|TYPE DE CHAMP$"
"1|Le code journal de l'écriture comptable (à blanc si non utilisé)|JOURNALCODE|Alphanumérique$"
"2|Le libellé journal de l'écriture comptable (à blanc si non utilisé)|JOURNALLIB|Alphanumérique$"
"3|Le numéro sur une séquence continue de l'écriture comptable|ECRITURENUM|Alphanumérique$"
"4|La date de comptabilisation de l'écriture comptable|ECRITUREDATE|Date$"
"5|Le numéro de compte (à blanc si non utilisé)|COMPTENUM|Alphanumérique$"
"6|Le libellé de compte|COMPTELIB|Alphanumérique$"
"7|Le numéro de compte auxiliaire (à blanc si non utilisé)|COMPAUXNUM|Alphanumérique$"
"8|Le libellé de compte auxiliaire (à blanc si non utilisé)|COMPAUXLIB|Alphanumérique$"
"9|La référence de la pièce justificative|PIECEREF|Alphanumérique$"
"10|La date de la pièce justificative|PIECEDATE|Date$"
"11|Le libellé de l'écriture comptable|ECRITURELIB|Alphanumérique$"
"12|Le montant au débit|DEBIT|Numérique$"
"13|Le montant au crédit|CREDIT|Numérique$"
"14|Le lettrage de l'écriture (à blanc si non utilisé)|ECRITURELET|Alphanumérique$"
"15|La date de lettrage (à blanc si non utilisé)|DATELET|Date$"
"16|La date de validation de l'écriture comptable|VALIDDATE|Date$"
"17|Le montant en devise (à blanc si non utilisé)|MONTANTDEVISE|Numérique$"
"18|L'identifiant de la devise (à blanc si non utilisé)|IDEVISE|Alphanumérique$"
"19|La date de règlement|DATERGLT|Date$"
"20|Le mode de règlement|MODERGLT|Alphanumérique$"
"21|La nature de l'opération (à blanc si non utilisé)|NATOP|Alphanumérique$"
"22|L'identification du client (à blanc si non utilisé)|IDCLIENT|Alphanumérique";

class ExportFiscal:public QWidget,public Ui::ExportWidget
{
	Q_OBJECT
	public:
		ExportFiscal (QWidget * parent = 0);
		~ExportFiscal ();
	
	protected slots:
	        void exportfile();

	private:
	        void initialiseDatas();
	        QStringList users();
	        QStringList sites();
	        QStringList comptes();
	        QHash<int,int> dispatchRecettesHash();
	        QHash<int,int> dispatchMouvementsHash();
	        QString honorairesColumns();
	        QString mouvementsColumns();
	        QString headersTax();
	        QString montantavecvirgule(QString & montant);
	        QString get_compte_bancaire_courant(QString & iduser);
	        QString get_numero_siret_user(QString & exportfiledir);
	        QSqlDatabase m_db;
	        QString SEPARATEUR_DE_CHAMP ;
	        QString ENCADREUR_DE_CHAMP ;
	        QString SEPARATEUR_DE_LIGNE ;
	        QString fileexportedpath;
	        QString nom_FEC;
};

#endif

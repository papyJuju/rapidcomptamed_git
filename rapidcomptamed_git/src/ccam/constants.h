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
#ifndef ACCOUNTRECEIPTS_CONTANTS_H
#define ACCOUNTRECEIPTS_CONTANTS_H

#include <QObject>
#include <QHash>

namespace ReceiptsConstants{
    enum Categories
    {
        NGAP=0,
        FORFAITS,
        CCAM,
        CategoriesMaxParam
        };
    enum TypeOfPayment{
        Cash = 0,
        Check,
        Visa,
        Banking,
        Other,
        Due,
        MaxOfTypeOfPayments
    };
    enum HeaderData{
        HDCash = 0,
        HDCheck,
        HDVisa,
        HDBanking,
        HDOther,
        HDDue,
        HDDebtor,
        HDSite,
        HDDistRules,
        HDAct,
        MaxOfHeaderData
    };
    enum ItemsOfTreeView
    {
        THESAURUS_ITEM = 0,
        ALL_VALUES_ITEM,
        PREFERED_VALUE_ITEM,
        ROUND_TRIP_ITEM,
        FREE_VALUE_ITEM,
        DEBTOR_ITEM,
        SITES_ITEM,
        DISTANCE_RULES_ITEM,
        BANK_ITEM,
        RULES_ITEM,
        ItemsOfTreeView_MaxParam
    };
    enum CcamFields
    {
        ID_CCAM=0,
        CODE_CCAM_FIELD,
        ABSTRACT_CCAM_FIELD,
       _CCAM_FIELD,
        AMOUNT_CCAM_FIELD,
        REIMBURSMENT_CCAM_FIELD,
        DATE_CCAM_FIELD,
        OTHERS_CCAM_FIELD
        };
    enum NgapFields
    {
        ID_ACTE_DISPO=0,
        NOM_ACTE,
        DESC_ACTE,
        TYPE,
        MONTANT_TOTAL,
        MONTANT_TIERS,
        DATE_EFFET
        };
}

#endif

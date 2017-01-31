#ifndef PARSECONSTANTS_H
#define PARSECONSTANTS_H

namespace ParseConstants
{
    enum itemsEnum{
        CODE = 0,
        C_CODE,
        TEXT,
        ACTIVITY,
        PHASE,
        PRICE,
        REIMBURSMENT,
        AGREEMENT,
        EXEMPTION,
        REGROUPMENT,
        ITEMS_ENUM_MaxParam
        };
    /*              "id_ccam  int(20) UNSIGNED  NOT NULL  auto_increment ,"
                    "code varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "abstract varchar(200) COLLATE utf8_unicode_ci NULL,"
                    "type varchar(50) COLLATE utf8_unicode_ci NULL,"
                    "amount double NOT NULL,"
                    "reimbursment double NULL,"
                    "date date NOT NULL,"
                    "others blob NULL,"*/
    enum itemsCCAMbase
    {
        IDCCAM = 0,
        CODE_CCAM,
        ABSTRACT,
        TYPE,
        AMOUNT,
        REIMBURSMENT_CCAM,
        DATE,
        OTHERS,
        itemsCCAMbaseMaxParam
        };
    enum XmlItems
    {
        XML_ACTIVITY = 0,
        XML_PHASE,
        XML_REIMBURSMENT,
        XML_AGREEMENT,
        XML_EXEMPTION,
        XML_REGROUPMENT,
        XmlItems_MaxParam
        };
    enum ModelItems
    {
        MODEL_CODE = 0,
        MODEL_TEXT,
        MODEL_PRICE,
        MODEL_BLOB,
        ModelItems_MaxParam 
        };
    enum NGAPitems
    {
        NGAP_CODE = 0,
        NGAP_TEXT,
        NGAP_TYPE,
        NGAP_AMOUNT,
        NGAP_OTHERS,
        NGAPitems_MaxParam
        };
    enum FilesToFind
    {
        XLS=0,
        SOFFICE
        };
};

#endif


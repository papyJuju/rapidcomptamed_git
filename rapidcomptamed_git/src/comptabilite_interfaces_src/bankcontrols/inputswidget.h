#ifndef INPUTSWIDGET_H
#define INPUTSWIDGET_H

#include <ui_inputs.h>
#include <QWidget>
#include <QSqlDatabase>

class Inputs:public QWidget,public Ui::InputsWidget
{
    Q_OBJECT
    enum Types
    {
        ESPECE=0,
        CHEQUES,
        CB,
        VIREMENT,
        TypesMaxParam
        };
    enum DepotsFields
    {
        IDDEPOT=0,
        IDUSR,
        IDCOMPTE,
        TYPEDEPOT,
        DATEDEPOT,
        DATEBEGIN,
        DATEEND,
        VALUEDEPOT,
        REMARQUEDEPOT,
        VALIDDEPOT,
        DepotsFieldsMaxParam
        };
    public:
        Inputs(QWidget* w = 0);
        ~Inputs();
    
    protected slots:
        void addValue();
        void deleteValue();

    private:
        void initialise();
        int lastDepotsId();
        QString textOfSums();
        int getType();
        QSqlDatabase m_db;
        QDate m_dateBegin;
        QDate m_dateEnd;
        QHash<QString,QString> m_hashUsers;
        QHash<QString,QString> m_hashBanks;
        QHash<int,QRadioButton*> m_hashRadioButtons;
        QHash<int,QString> m_hashType;
        QString m_typeOfMoney;
        /* data */
};

#endif


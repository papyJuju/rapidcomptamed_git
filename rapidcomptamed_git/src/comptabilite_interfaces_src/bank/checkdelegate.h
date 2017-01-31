#ifndef CHECKDELEGATE_H
#define CHECKDELEGATE_H

#include <QStyledItemDelegate>
#include <QCheckBox>
#include <QWidget>
#include <QMap>
#include <QMapIterator>
#include <QHash>
#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QString>

class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
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
    REMARQUE,
    VALIDE,
    TRACABILITE,
    VALIDATION,
    DETAIL,
    MouvementsHeaders_MaxParam
    };
    public:
        CheckBoxDelegate(QObject * parent = 0);
        ~CheckBoxDelegate();

        QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

#endif


#ifndef TREEVIEWDELEGATE_H
#define TREEVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QDateEdit>
#include <QWidget>
#include <QMap>
#include <QMapIterator>
#include <QHash>
#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QString>
#include <QComboBox>


class DateEditTreeViewFirstDelegate : public QStyledItemDelegate
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
        DateEditTreeViewFirstDelegate(QObject * parent = 0);
        ~DateEditTreeViewFirstDelegate();

        QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class ComboLibelleDelegate:public QStyledItemDelegate
{
    public:
        ComboLibelleDelegate(QObject * parent = 0);
        ~ComboLibelleDelegate ();
        QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    private:
        QMap<int,QString> getLibelleFromDatabase();
        QMap<int,QString> m_mapLibelle;
};

#endif


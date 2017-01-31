#ifndef PONDERE_H
#define PONDERE_H

#include "ui_pondere.h"
#include <QDialog>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QDoubleSpinBox>
#include <QAbstractItemModel>
#include <QModelIndex>

class VariantModel:public QAbstractItemModel
{
    Q_OBJECT
    enum Table
       {
           ACT = 0,
           VALUE,
           MODIFIER,
           COL_COUNT
           };
    public:
        VariantModel(QObject * parent = 0);
        ~VariantModel();
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const ;
        QModelIndex parent ( const QModelIndex & index ) const ;
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
        bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        bool setHeaderData( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
        Qt::ItemFlags flags(const QModelIndex &index) const;        
    private:
        QVector<QVector<QVariant> > m_itemsVector;
        QStringList m_headersRows;
        QStringList m_headersColumns;
};

class SpinBoxDelegate : public QItemDelegate
 {
     Q_OBJECT

 public:
     SpinBoxDelegate(QObject *parent = 0);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;
 };

class PondereActes : public QDialog, public Ui::ModifActesDialog
{
    Q_OBJECT
    enum Table
       {
           ACT = 0,
           VALUE,
           MODIFIER,
           COL_COUNT
           };
    public:
        PondereActes(QWidget * parent = 0, QHash<QString,QString>  hashOfActs = QHash<QString,QString> ());
        ~PondereActes();
        QHash<QString,QString> getModifiedValues();
    private slots:
        void apply();
    private:
        VariantModel * m_model;
};

#endif


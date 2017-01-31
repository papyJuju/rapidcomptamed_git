#ifndef CUSTOMMODEL_H
#define CUSTOMMODEL_H
#include <QtSql>
#include <QtCore>

class customModel:public QSqlTableModel{
  Q_OBJECT
  public :
    customModel(QObject * object,QSqlDatabase & database);
    ~customModel();
    QString tableName() const;
    void setTable(const QString & tableName);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int arow, const QModelIndex &aparent = QModelIndex())        { return insertRows(arow, 1, aparent); }
    bool insertColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return insertColumns(acolumn, 1, aparent); }
    bool removeRow(int arow, const QModelIndex &aparent = QModelIndex())        { return removeRows(arow, 1, aparent); }
    bool removeColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return removeColumns(acolumn, 1, aparent); }

    QString filter() const;
    void setFilter(const QString &filter);

    QSqlRecord record(int row) const;
    bool select();

public Q_SLOTS:
    bool submit();
    void revert();

private :
    QSqlTableModel * m_model;
};
#endif

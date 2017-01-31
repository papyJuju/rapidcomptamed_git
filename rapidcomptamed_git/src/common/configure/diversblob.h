#ifndef DIVERSBLOB_H
#define DIVERSBLOB_H
#include "tokens.h"
#include "../constants.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDebug>


namespace DiversBlobs
{
    QHash<QString,QString> diversblobsList(const QString & table,QModelIndex & index)
    {
        QSqlDatabase db = QSqlDatabase::database(Common::Constants::DB_ACCOUNTANCY);
        QHash<QString,QString> itemshash;
        QSqlTableModel model(0,db);
        model.setTable(table);
        model.select();
        QString value = model.data(index,Qt::DisplayRole).toString();
        value.remove(QString(Tokens::DIVERSBLOBSBEGIN));
        value.remove(QString(Tokens::DIVERSBLOBSEND));
        Tokens::TokensClass tk;
        QHash<QString,QString> hashTokens = tk.hashTokensForBlobs();
        QHash<QString,QString> hashTokensNames = tk.hashTokensNames();
        QStringList keys = hashTokensNames.keys();
        foreach(QString token,keys){
            QString endtoken = hashTokens.value(token) ;
            int indexendtokenbegin = value.indexOf(token)+token.size();
            int indexbegintokenend = value.indexOf(endtoken);
            QString item;
            for (int i = indexendtokenbegin ; i < indexbegintokenend; ++i)
            {
                  item += value.at(i);
                }
            itemshash.insert(hashTokensNames.value(token),item);
            }
        return itemshash;
    }
};

#endif


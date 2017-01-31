#ifndef TOKENS_H
#define TOKENS_H
#include <QObject>
#include <QHash>

namespace Tokens
{
    const char * const DIVERSBLOBSBEGIN = "[divers_blob_begin]";
    const char * const DIVERSBLOBSEND = "[divers_blob_end]";
    const char * const NUMERO_AMC_BEGIN = "[numero_AMC_begin]";
    const char * const NUMERO_AMC_END = "[numero_AMC_end]";
    //toutes les cles debut et fin des blobs divers
    class TokensClass:public QObject
    {
        Q_OBJECT
        public:
            TokensClass (QObject *parent = 0);
            ~TokensClass ();
        QHash<QString,QString> hashTokensForBlobs()
        {
            QHash<QString,QString> hash;
            hash.insert(QString(DIVERSBLOBSBEGIN),QString(DIVERSBLOBSEND));
            hash.insert(QString(NUMERO_AMC_BEGIN),QString(NUMERO_AMC_END));
            //hash.insert(QString(),QString());
            return hash;
            };

        QHash<QString,QString> hashTokensNames()
        {
            QHash<QString,QString> hash;
            hash.insert(QString(NUMERO_AMC_BEGIN),QObject::tr("numéro AMC"));
            return hash;
            };
    };
};

#endif


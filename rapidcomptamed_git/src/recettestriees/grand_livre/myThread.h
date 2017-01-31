#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

class produceDoc: public QThread {
Q_OBJECT
enum TotalSums{
    TOTAL_SUM=0,
    ESP_SUM,
    CHQ_SUM,
    CB_SUM,
    VIR_SUM,
    TotalSumsMaxParam
    };
public :
    produceDoc(QObject *parent =0);
    ~produceDoc();
    int            m_champsaconcatener;
    int            m_usr;
    QString        m_parametrestableformat;
    QString        m_parametresTablesRecap;
    QMutex         m_mutex;
    QStringList    m_typesRecettes;
    QStringList    m_typesDepenses;
    QTextDocument* recupTextDoc();
    void           dateChosen(QDate & );
private :
    QSqlDatabase   m_db;
    QDate          m_date;
    QDate          m_date1;
    QTextDocument *m_documenttrie;
    QTextDocument *m_doctrieJanvRecT ;
    QTextDocument *m_doctrieJanvDepT ;   
    QTextDocument *m_doctrieFevRecT  ;  
    QTextDocument *m_doctrieFevDepT  ; 
    QTextDocument *m_doctrieMarRecT  ;
    QTextDocument *m_doctrieMarDepT  ;
    QTextDocument *m_doctrieAvrRecT  ;
    QTextDocument *m_doctrieAvrDepT  ;
    QTextDocument *m_doctrieMaiRecT  ;
    QTextDocument *m_doctrieMaiDepT  ;
    QTextDocument *m_doctrieJuinRecT ;
    QTextDocument *m_doctrieJuinDepT ;
    QTextDocument *m_doctrieJuilRecT ;
    QTextDocument *m_doctrieJuilDepT ;
    QTextDocument *m_doctrieAouRecT  ;
    QTextDocument *m_doctrieAouDepT  ;
    QTextDocument *m_doctrieSeptRecT ;
    QTextDocument *m_doctrieSeptDepT ;
    QTextDocument *m_doctrieOctRecT  ;
    QTextDocument *m_doctrieOctDepT  ;
    QTextDocument *m_doctrieNovRecT  ;
    QTextDocument *m_doctrieNovDepT  ;
    QTextDocument *m_doctrieDecRecT  ;
    QTextDocument *m_doctrieDecDepT  ;
    void        run();
    void        trier();
    void        fonctionremplir      (QList<QVector<QString> > &/*tableau*/,QTextTableFormat &, QTextCursor *,QString&, QStringList &, int );
    QStringList calculRecettes       (QString & /*datedebut*/, QString & /*datefin*/);
    QStringList calculDepenses       (QString & /*dateun*/, QString & /*datedeux*/);
    
                         
    QTextTableFormat monformat(QTextTableFormat &/*tableFormat*/,
                               QString & /*parametrespourtableformat*/);
                               
    bool modele               (QString & /*mois*/,
                               QString & /*jour1*/ ,
                               QString & /*dernierjour*/ ,
                               QTextTableFormat & /*tableFormattableau*/,
                               QTextCursor */*cursortriefonction*/ );
    void recupSlot(const QString&);
    
signals :
    void outThread(const QString &);
    void started();
};

#endif

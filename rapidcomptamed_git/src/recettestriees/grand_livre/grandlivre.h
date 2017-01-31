#ifndef GRANDLIVRE_H
#define GRANDLIVRE_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "ui_grandlivredialog.h"
#include "myThread.h"

class grandlivre : public QWidget, public Ui::GrandLivreDialog {
Q_OBJECT

public :
    grandlivre();
    ~grandlivre();
    produceDoc    *m_monThread;
    QTextDocument *m_doctrie;
    QTextDocument *m_doctrieJanvRec ;
    QTextDocument *m_doctrieJanvDep ;   
    QTextDocument *m_doctrieFevRec  ;  
    QTextDocument *m_doctrieFevDep  ; 
    QTextDocument *m_doctrieMarRec  ;
    QTextDocument *m_doctrieMarDep  ;
    QTextDocument *m_doctrieAvrRec  ;
    QTextDocument *m_doctrieAvrDep  ;
    QTextDocument *m_doctrieMaiRec  ;
    QTextDocument *m_doctrieMaiDep  ;
    QTextDocument *m_doctrieJuinRec ;
    QTextDocument *m_doctrieJuinDep ;
    QTextDocument *m_doctrieJuilRec ;
    QTextDocument *m_doctrieJuilDep ;
    QTextDocument *m_doctrieAouRec  ;
    QTextDocument *m_doctrieAouDep  ;
    QTextDocument *m_doctrieSeptRec ;
    QTextDocument *m_doctrieSeptDep ;
    QTextDocument *m_doctrieOctRec  ;
    QTextDocument *m_doctrieOctDep  ;
    QTextDocument *m_doctrieNovRec  ;
    QTextDocument *m_doctrieNovDep  ;
    QTextDocument *m_doctrieDecRec  ;
    QTextDocument *m_doctrieDecDep  ;
    QDate         m_date;

signals :
    void signDoc(const QTextDocument *);
    void deleteThread();

private slots :
    void imprimer();
    void fonctionDeTri();
    void inThis(const QString & text);
    void sortiedoc();
    void inThread();
    void deleteSlotThread();
    
};



#endif

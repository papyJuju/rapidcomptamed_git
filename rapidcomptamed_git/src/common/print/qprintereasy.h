/**********************************************************************************
 *   QPrinterEasy                                                                 *
 *   Copyright (C) 2009                                                           *
 *   Main author : eric.maeker@free.fr                                            *
 *                                                                                *
 * Copyright (c) 1998, Regents of the University of California                    *
 * All rights reserved.                                                           *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted provided that the following conditions are met:    *
 *                                                                                *
 *     * Redistributions of source code must retain the above copyright           *
 *       notice, this list of conditions and the following disclaimer.            *
 *     * Redistributions in binary form must reproduce the above copyright        *
 *       notice, this list of conditions and the following disclaimer in the      *
 *       documentation and/or other materials provided with the distribution.     *
 *     * Neither the name of the University of California, Berkeley nor the       *
 *       names of its contributors may be used to endorse or promote products     *
 *       derived from this software without specific prior written permission.    *
 *                                                                                *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY    *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED      *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE         *
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY   *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES     *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;   *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND    *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS  *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                   *
 **********************************************************************************/
#ifndef QPRINTEREASY_H
#define QPRINTEREASY_H

#include "qprintereasy_global.h"
#include "qtextdocumentextra.h"
class QPrinterEasyPrivate;
class QTextDocumentExtra;
class QPrinterEasyPreviewer;

#include <QObject>
#include <QFlags>
#include <QPixmap>
#include <QTextDocument>
#include <QPrinter>
#include <QFont>
#include <QColor>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

/**
 * \file qprintereasy.h
 * \author QPrinterEasy Team
 * \version 0.0.8
 * \date June 26 2009
*/

class   QPrinterEasy : public QObject
{
    Q_OBJECT
public:
        QPrinterEasy( QObject * parent = 0 );
    ~QPrinterEasy();

    enum Presence {
        EachPages = 0,
        FirstPageOnly,
        SecondPageOnly,
        LastPageOnly,
        ButFirstPage,
        OddPages,  // pages impaires
        EvenPages,  // pages paires
        DuplicataOnly
    };
    static QStringList presencesAvailable()
    {
        return QStringList()
                << tr("Each Pages")
                << tr("First page only")
                << tr("Second page only")
                << tr("Last page only")
                << tr("All pages but first")
                << tr("Odd pages")
                << tr("Even pages")
                << tr("Duplicatas only");
    }

    enum Priority {
        First = 0,
        Second,
        Third,
        Quater
    };


    bool askForPrinter( QWidget *parent = 0 );
    bool useDefaultPrinter();
    void setPrinter( QPrinter * printer );
    QPrinter *printer();

    bool previewDialog( QWidget *parent = 0, bool test = false );

    void setHeader( const QString & html, Presence p = EachPages, QPrinterEasy::Priority prior = First );
    void clearHeaders();

    void setFooter( const QString & html, Presence p = EachPages, QPrinterEasy::Priority prior = First );
    void clearFooters();

    void setContent( const QString & html );

    void setOrientation(QPrinter::Orientation orientation);
    void setPaperSize(QPrinter::PaperSize size);

public Q_SLOTS:
    void addWatermarkPixmap( const QPixmap & pix,
                             const Presence p = EachPages,
                             const Qt::AlignmentFlag alignement = Qt::AlignCenter);

    void addWatermarkText( const QString & plainText,
                           const Presence p = EachPages,
                           const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                           const Qt::Alignment textAlignment = Qt::AlignCenter,
                           const QFont & font = QFont( "Hevetica", 36 ),
                           const QColor & color = QColor("lightgrey"),
                           const int orientation = -1 );

    void addWatermarkHtml( const QString & html,
                           const Presence p = EachPages,
                           const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                           const int orientation = -1 );

    static QPrinterEasyPreviewer *previewer( QWidget *parent );

    void previewToPixmap( QPixmap &drawTo, QPrinter *printer );

    void previewHeaderFooter( QPixmap &drawTo,
                              const QString &headerHtml,
                              const QString &footerHtml );

    static void previewDocumentWatermark( QPixmap &drawTo,
                                       QTextDocument *doc,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );
    static void previewHtmlWatermark( QPixmap &drawTo,
                                      const QString & html,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );
    static void previewTextWatermark( QPixmap &drawTo,
                                      const QString & plainText,
                                      const Presence p = EachPages,
                                      const Qt::Alignment watermarkAlignment = Qt::AlignCenter,
                                      const int orientation = -1 );

    void clearWatermark();

    bool print( const QTextDocument & docToPrint );
    bool printWithDuplicata( bool state = true );
    bool print( const QString &htmlToPrint );

    bool toPdf(const QString &fileName);
    bool toPdf(const QString &fileName, const QString &html);
    bool toPdf(const QString &fileName, const QTextDocument & docToPrint);

protected Q_SLOTS:
    bool print( QPrinter *printer = 0 );  // used by QPrinterEasy PreviewDialog

private:
    QPrinterEasyPrivate *d;
};


class  QPrinterEasyPreviewer : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(QPrinterEasyPreviewer)
    Q_PROPERTY(QString htmlHeader  READ headerToHtml WRITE setHeader USER true)
    Q_PROPERTY(QString htmlFooter  READ footerToHtml WRITE setFooter USER true)

//    Q_PROPERTY(QVariant extraDoc  READ extraDocument  WRITE setExtraDocument   USER true)

public:
    explicit QPrinterEasyPreviewer(QWidget *parent = 0) : QWidget(parent) {}
    virtual void initialize() = 0;

    virtual void setHeader(const QString &html, QPrinterEasy::Presence p = QPrinterEasy::EachPages) = 0;
    virtual void setFooter(const QString &html, QPrinterEasy::Presence p = QPrinterEasy::EachPages) = 0;
    virtual void setWatermark(const QString &html, QPrinterEasy::Presence p = QPrinterEasy::EachPages) = 0;

    virtual void setHeader(const QTextDocumentExtra *extra) = 0;
    virtual void setFooter(const QTextDocumentExtra *extra) = 0;
    virtual void setWatermark(const QTextDocumentExtra *extra) = 0;

    virtual QTextEdit *headerEditor() = 0;
    virtual QTextEdit *footerEditor() = 0;
    virtual QTextEdit *watermarkEditor() = 0;

    virtual QString headerToHtml() = 0;
    virtual QString footerToHtml() = 0;
    virtual QString watermarkToHtml() = 0;
    virtual int headerPresence() = 0;
    virtual int footerPresence() = 0;
    virtual int watermarkPresence() = 0;

    virtual void headerToPointer(QTextDocumentExtra *extra) = 0;
    virtual void footerToPointer(QTextDocumentExtra *extra) = 0;
    virtual void watermarkToPointer(QTextDocumentExtra *extra) = 0;
};

#endif // QPRINTEREASY_H

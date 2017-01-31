/**********************************************************************************
 *   QPrinterEasy                                                                 *
 *   Copyright (C) 2009                                                           *
 *   eric.maeker@free.fr, wathek@gmail.com, aurelien.french@gmail.com             *
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
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QString>
#include <QTextFrame>
#include <QAbstractTextDocumentLayout>
#include <QTextTable>
#include <QtDebug>

#include "qprintereasy.h"

QString document;
QString header;
QString header2;
QString footer;
QString footer2;
QString watermark;


QByteArray readEntireFile(const QString &fileName)
{
	QFile f(fileName);
	if (f.open(QIODevice::ReadOnly))
		return f.readAll();

	qCritical("Error in loading %s", qPrintable(fileName));
	return QByteArray();
}

void example1()
{
    qWarning() << "example 1 : One header, one footer on each pages";
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header );
    pe.setFooter( footer );
    pe.setContent( document );
    pe.previewDialog();
}

void example2()
{
    qWarning() << "example 2 : One header on first page only, one footer on each pages";
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header, QPrinterEasy::FirstPageOnly );
    pe.setFooter( footer );
    pe.setContent( document );
    pe.previewDialog();
}

void example3()
{
    qWarning() << "example 3 : One header on first page only, one footer on the second page only";
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header, QPrinterEasy::FirstPageOnly );
    pe.setFooter( footer, QPrinterEasy::SecondPageOnly );
    pe.setContent( document );
    pe.previewDialog();
}

void example4()
{
    qWarning() << "example 4 : Header and footer with centered plain text Watermark";
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header );
    pe.setFooter( footer );
    pe.setContent( document );
    pe.addWatermarkText( "Adding a plain text\nWATERMARK" );
    pe.previewDialog();
}


void example5()
{
    qWarning() << "example 5 : Header and footer with centered plain text Watermark on Even Pages";
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header );
    pe.setFooter( footer );
    pe.setContent( document );
    pe.addWatermarkText( "Adding a plain text\nWATERMARK", QPrinterEasy::EvenPages );
    pe.previewDialog();
}

void example6()
{
    qWarning() << "example 6 : Header and footer with a pixmap watermark on EachPages";
    QDir dir(qApp->applicationDirPath());
    QPixmap pixWatermark;
    pixWatermark.load( dir.filePath("pixmapWatermark.png") );
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header );
    pe.setFooter( footer );
    pe.setContent( document );
    pe.addWatermarkPixmap( pixWatermark, QPrinterEasy::EachPages );
    pe.previewDialog();
}

void example7()
{
    qWarning() << "example 7 : MultiHeaders, one footer, watermark on EvenPages";
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header, QPrinterEasy::FirstPageOnly );
    pe.setHeader( header2, QPrinterEasy::EachPages );
    pe.setFooter( footer, QPrinterEasy::ButFirstPage );
    pe.setContent( document );
    pe.addWatermarkText( "Adding a plain text\nWATERMARK", QPrinterEasy::EvenPages );
    pe.previewDialog();
}

void warnDocumentBlockCount()
{
    QTextDocument td;
    td.setHtml(document);
    QPrinter p;
    p.setPaperSize( QPrinter::A4 );
//    td.setPageSize( p.pageRect().size() );
    td.setTextWidth( p.pageRect().width());

    qWarning() << "document blockCount" << td.blockCount() << "doc size" << td.size();

    QTextFrame::iterator it;
    int i = 0;
    for (it = td.rootFrame()->begin(); !(it.atEnd()); ++it) {
        if (it.currentFrame()) {
            qWarning() << "Frame : number of children" << it.currentFrame()->childFrames().count();
            QTextTable *table = qobject_cast<QTextTable*>( it.currentFrame() );
            if (table) {
                qWarning() << "   --> Frame : table" << table->firstPosition() << table->lastPosition();
            }
        }
        else {
            QTextLayout *layout = it.currentBlock().layout();
            qWarning() << "Block" << it.currentBlock().blockNumber()
                       << td.documentLayout()->blockBoundingRect( it.currentBlock() )
                       << "block line count" << it.currentBlock().lineCount()
                       << "layout line count" << layout->lineCount();
            int l = 0;
            for(l=0;l<layout->lineCount();++l) {
                QTextLine line = layout->lineAt(l);
                if (line.isValid())
                    qWarning() << "            Line" << line.rect();
            }
        }
        ++i;
    }
    qWarning() << "document frameCount" << i;

//    int i = 0;
//    QTextFrame *f = td.rootFrame();
//    qWarning() << f->childFrames().count();
//
//    foreach( QTextFrame *c, f->childFrames() )  {
//        qWarning() << "x";
//        warnFrameChildren(c , 1);
//        ++i;
//    }
    qWarning() << "document frameCount" << i;
}

void example8()
{
    qWarning() << "example 8 : MultiHeaders, one footer, multiwatermarks";
    /** \todo If multiwatermarks, and watermarks does not have the same presence --> BUG need to store watermarks into a hash<presence,pixmap>. */
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header, QPrinterEasy::FirstPageOnly );
    pe.setHeader( header2, QPrinterEasy::EachPages );
    pe.setFooter( footer, QPrinterEasy::ButFirstPage );
    pe.setContent( document );
    pe.addWatermarkText( "Adding a plain text\nWATERMARK", QPrinterEasy::EachPages );
    pe.addWatermarkText( "Second watermark", QPrinterEasy::EachPages, Qt::AlignBottom, Qt::AlignLeft, QFont(), QColor("lightgray") );
    pe.previewDialog();
}

void example9()
{
    qWarning() << "example 9 : one header, one footer, one watermarks from page 2 to 3";
    /** \todo Does not work with complexDrawing */
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.printer()->setFromTo(2,3);
    pe.setHeader( header );
    pe.setFooter( footer );
    pe.setContent( document );
    pe.addWatermarkText( "Adding a plain text\nWATERMARK", QPrinterEasy::EachPages );
    pe.previewDialog();
}

void examplePdf()
{
    QPrinterEasy pe;
    pe.askForPrinter();
    pe.setHeader( header, QPrinterEasy::FirstPageOnly );
    pe.setHeader( header2, QPrinterEasy::EachPages );
    pe.setFooter( footer, QPrinterEasy::ButFirstPage );
    pe.setContent( document );
    pe.addWatermarkText( "Adding a plain text\nWATERMARK", QPrinterEasy::EvenPages );
    pe.toPdf( qApp->applicationDirPath() + "/pdfSample.pdf" );
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDir dir(app.applicationDirPath());
    header = readEntireFile(dir.filePath("header.html"));
    header2 = readEntireFile(dir.filePath("header_2.html"));
    footer = readEntireFile(dir.filePath("footer.html"));
    footer2 = readEntireFile(dir.filePath("footer_2.html"));
    watermark = readEntireFile(dir.filePath("watermark.html"));

    QPixmap pixWatermark;
    pixWatermark.load( dir.filePath("pixmapWatermark.png") );
    document = QString::fromUtf8(readEntireFile(dir.filePath("document.html")));

//    example1();
//    example2();
//    example3();
//    example4();
//    example5();
//    example6();
//    example7();
//    example8();
    example9();
//    examplePdf();

//    warnDocumentBlockCount();

//    QPrinterEasy pe;
//    pe.askForPrinter();
////    pe.addWatermarkPixmap( pixWatermark, QPrinterEasy::EachPages );
//    pe.addWatermarkText( "Adding a plain text\nWATERMARK", QPrinterEasy::EvenPages, Qt::AlignCenter );
//    pe.setHeader( header );//, QPrinterEasy::FirstPageOnly );
////    pe.setHeader( header2, QPrinterEasy::EachPages );
//    pe.setFooter( footer );
////    pe.setFooter( footer2 );
////    pe.setFooter( footer );
//    pe.setContent( document );
//    pe.setOrientation(QPrinter::Portrait);
//    pe.setPaperSize(QPrinter::A4);
//    pe.previewDialog();

    return 0;
}

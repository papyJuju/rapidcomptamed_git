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
#ifndef QTEXTDOCUMENTEXTRA_H
#define QTEXTDOCUMENTEXTRA_H

#include "qprintereasy_global.h"
#include "qprintereasy.h"

#include <QtGui>
#include <QtCore>
#include <QVariant>



namespace QTextDocumentExtraConstants {
const char* const QDOCUMENT_GENERAL_XML_TAG   = "QTextDocumentExtra";  /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */
const char* const QDOCUMENT_VERSION_XML_TAG   = "Version";             /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */
const char* const QDOCUMENT_PRESENCE_XML_TAG  = "Presence";            /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */
const char* const QDOCUMENT_PRIORITY_XML_TAG  = "Priority";            /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */
const char* const QDOCUMENT_EXTRA_XML_TAG     = "Extras";              /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */
const char* const QDOCUMENT_HTML_XML_TAG      = "QDocumentHtml";       /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */

const char* const QDOCUMENT_XML_ACTUALVERSION = "1.0";                 /*!< \brief QTextDocumentExtra Xml Tag. \sa QTextDocumentExtra::toXml(), QTextDocumentExtra::fromXml() */

}

using namespace QTextDocumentExtraConstants;

class QTextDocumentExtraPrivate;
class QPrinterEasy;

class  QTextDocumentExtra
{
public:
    QTextDocumentExtra();
    QTextDocumentExtra( const QString &html, const int presence = 0/*QPrinterEasy::Presence EachPages*/, const int priority = 0/*QPrinterEasy::Priority First*/, const QString &version = QString::null );
    ~QTextDocumentExtra();

    void setPriority( int );
    void setPresence( int );
    void setHtml( const QString &html);

    /*QPrinterEasy::Priority*/ int priority() const;
    /*QPrinterEasy::Presence*/ int presence() const;

    QTextDocument *document() const;

    void setTextWidth( qreal width );
    bool lessThan( const QTextDocumentExtra *h1, const QTextDocumentExtra *h2 );

    QString toXml() const;
    static QTextDocumentExtra *fromXml(const QString &xml);
    QString toHtml() const;

private:
    QTextDocumentExtraPrivate *d;
};
Q_DECLARE_METATYPE(QTextDocumentExtra)

#endif // QTEXTDOCUMENTEXTRA_H

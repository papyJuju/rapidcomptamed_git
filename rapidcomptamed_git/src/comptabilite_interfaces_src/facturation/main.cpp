#include "facturation.h"
#include <common/icore.h>
#include <common/databasemanager.h>
#include <common/settings.h>
#include <common/constants.h>
#include <QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>


int main (int argc, char * argv[])
{
    QApplication a(argc,argv);
    if (!Common::ICore::instance()->initialize()) 
    {
        qWarning() << "Error : unable to initialize Core";
        return 123;
        }
    #ifdef Q_OS_LINUX
    if (Common::ICore::instance()->settings()->allKeys().contains(Common::Constants::CODEC_LINUX))
    {
        QString codec =  Common::ICore::instance()->settings()->value(Common::Constants::CODEC_LINUX).toString();
        QByteArray codecarray = codec.toLatin1();
        const char * codecchar = codecarray.data();
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codecchar));
        }
    else
    {
        QString warning = QString("<html>Une fonction a &eacute;t&eacute; ajout&eacute;e,"
        "<br/>veuillez r&eacute;g&eacute;n&eacute;rer votre $HOME/.config.ini"
        "<br/>dans comptatrie/configuration</html>");
        QMessageBox::warning(0,"Warning",warning,QMessageBox::Ok);
        }
    #elif Q_OS_WIN
    if (Common::ICore::instance()->settings()->allKeys().contains(Common::Constants::CODEC_WINDOWS))
    {
        QString codec =  Common::ICore::instance()->settings()->value(Common::Constants::CODEC_WINDOWS).toString();
        QByteArray codecarray = codec.toLatin1();
        const char * codecchar = codecarray.data();
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codecchar));
        }
    else
    {
        QString warning = QString("<html>Une fonction a &eacute;t&eacute; ajout&eacute;e,"
        "<br/>veuillez r&eacute;g&eacute;n&eacute;rer votre $HOME/.config.ini"
        "<br/>dans comptatrie/configuration</html>");
        QMessageBox::warning(0,"Warning",warning,QMessageBox::Ok);
        }
    #else
        QMessageBox::warning(0,"Warning","OS non support&eacute;.",QMessageBox::Ok);
    #endif
    Facture *f = new Facture;
    f->show();
    return a.exec();
}

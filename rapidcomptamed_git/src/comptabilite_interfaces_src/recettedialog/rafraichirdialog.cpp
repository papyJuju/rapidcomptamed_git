#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "rafraichirdialog.h"
rafraichirdialog::rafraichirdialog()
{
    setupUi(this);
    lineEdit->setText("mettez ici la valeur de l'acte du type acte=valeur");
    lineEdit_2->setText(trUtf8("mettez ici le type de dépense"));
    connect(pushButton,SIGNAL(pressed()),this,SLOT(fermer()));
    connect(pushButton_2,SIGNAL(pressed()),this,SLOT(fonctionrafraichiracte()));
    connect(pushButton_3,SIGNAL(pressed()),this,SLOT(fonctionrafraichirdepense()));
}

void rafraichirdialog::on_lineEdit_textEdited(){
    pushButton_3->setEnabled(false);
}
void rafraichirdialog::on_lineEdit_2_textEdited(){
    pushButton_2->setEnabled(false);
}
void rafraichirdialog::fonctionrafraichiracte(){
    QStringList liste;
    QString acte = lineEdit->text();
    lineEdit_2->setText("");
    QFile file(m_pathabsolu+"/actes.ini");
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text)){
    QMessageBox::warning(0,tr("erreur"),trUtf8("actes.ini ne peut pas être ouvert"),QMessageBox::Ok);}
    QTextStream stream(&file);
    while(!stream.atEnd())
        {
        QString line = stream.readLine();
        liste << ""+line+"";
        }
    file.remove();
    QFile file2(m_pathabsolu+"/actes.ini");
    if(!file2.open(QIODevice::ReadWrite|QIODevice::Text)){
    QMessageBox::warning(0,tr("erreur"),trUtf8("actes.ini ne peut pas être ouvert"),QMessageBox::Ok);}
    QTextStream stream2(&file2);
    for(int i=0;i<liste.size();i++)
    {
        stream2 << ""+liste[i]+"""\n";
        }
    stream2 << ""+acte+"""\n";
    QMessageBox::information(0,tr("info"),trUtf8("actes.ini a été renseigné"),QMessageBox::Ok);
    emit close();
}

void rafraichirdialog::fonctionrafraichirdepense(){
    QStringList liste;
    QString depense = lineEdit_2->text();
    lineEdit->setText("");
    QFile file(m_pathabsolu+"/depense.ini");
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        QMessageBox::warning(0,tr("erreur"),trUtf8("depense.ini ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream stream(&file);
    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        liste << ""+line+"";
        }
    file.remove();
    QFile file2(""+m_pathabsolu+"/depense.ini");
    if(!file2.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        QMessageBox::warning(0,tr("erreur"),trUtf8("depense.ini ne peut pas être ouvert"),QMessageBox::Ok);
        }
    QTextStream stream2(&file2);
    int j = liste.size();
    for(int i=0;i < j;i++)
    {
        QString ligne = liste[i].toLatin1();
        stream2 << ""+ligne+"""\n";
        }
    stream2 << ""+QString::number(j+1)+"""="""+depense+"""\n";
    QMessageBox::information(0,tr("info"),trUtf8("depense.ini a été renseigné"),QMessageBox::Ok);
    emit close();
}
void rafraichirdialog::fermer()
{
    emit close();
}

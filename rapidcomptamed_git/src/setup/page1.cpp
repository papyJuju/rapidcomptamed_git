#include "page1.h"

#include <common/icore.h>
#include <common/settings.h>
#include <common/constants.h>
#include "wiz1.h"


#include <QtGui>
#include <QtCore>


static inline QString absolutePath() {return Common::ICore::instance()->settings()->applicationBundlePath();}

page1::page1(QObject * parent){
    setupUi(this);
    QString texte = trUtf8("Ce programme permet l'installation des boutons de la nouvelle comptabilité.<br/>"
                                     "Vous devez d'abord :<br/>"
                                     "  - faire une sauvegarde de votre comptabilité si vous avez déjà une comptabilité MedinTux,<br/>"
                                     "  - copier le répertoire rapidcomptamed dans MedinTux/Programmes,<br/>"
                                     "  - relancer le setup.<br/>"
                                     "Cliquez sur cancel pour arrêter le programme, sur next pour continuer l'installation.");
    QString texteHtml  = "<html><body><font size = 4 color = ""blue"">"+texte+"</font></body></html>";
    QDir dirDrTux               (absolutePath()+"/../../drtux");
    if (!dirDrTux . exists()){
        QMessageBox::warning(0,trUtf8("Erreur"),trUtf8("Le répertoire comptabilité n'est pas placé dans le répertoire Programmes "
                                                     "à côté de DrTux.\n"
                                                     "L'installation ne peut pas se faire correctement. "),QMessageBox::Ok);
        label -> setText("");
        static_cast<wiz1*>(parent)->setfalse(true);
        return;
        }
    else
    {
        label -> setText(texteHtml);
        //isFinalPage();//FIXME    
          
        //static_cast<QWizard*>(parent)->next();
        }  
}

page1::~page1(){}

page2::page2(){
  setupUi(this);
  QString texte             = trUtf8("Installation.<br/>Veuillez cliquer sur next.");
  QString texteHtml         = "<html><body><font size = 6 color = ""green"">"+texte+"</font></body></html>";
  label                     -> setText(texteHtml);
  label                     -> setWordWrap(true);
  installCompta();
}

page2::~page2(){}

pageFin::pageFin(){
  setupUi(this);
  setFinalPage(true);
  QString texte             = trUtf8("Félicitations pour votre installation."
                                     "<br/>Vous avez l'apparition de trois boutons dans le dossier patient : "
                                     "<br/> - un bouton de la fenêtre d'acceuil de tous les programmes,"
                                     "<br/> - un bouton de prise rapide des recettes,"
                                     "<br/> - un bouton de prise rapide des dépenses.");
  QString texteHtml         = "<html><body><font size = 6 color = ""red"">"+texte+"</font></body></html>";
  label                     -> setText(texteHtml);
  label                     -> setWordWrap(true);
  
}

pageFin::~pageFin(){}

void page2::installCompta(){

  //------------creation dossiers dans ../../drtux/bin/Ressources/Menus/---------
  QDir dirMenus              (absolutePath()+"/../../drtux/bin/Ressources/Menus");
       dirMenus             . mkdir("AccesCompta");
       dirMenus             . mkdir("AccesDepense");
       dirMenus             . mkdir("AccesRecette");
       
  QDir dirAccesCompta(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesCompta");
  QDir dirAccesRecette(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesRecette");
  QDir dirAccesDepense(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesDepense");
       
  QFile fileCompta           (absolutePath()+"/../resources/Menus/AccesCompta/definition.txt");
  QFile fileComptaPng        (absolutePath()+"/../resources/Menus/AccesCompta/comptabilite.png");
  QFile fileDepense          (absolutePath()+"/../resources/Menus/AccesDepense/definition.txt");
  QFile fileDepensePng       (absolutePath()+"/../resources/Menus/AccesDepense/creditcards.png");
  QFile fileRecette          (absolutePath()+"/../resources/Menus/AccesRecette/definition.txt");
  QFile fileRecettePng       (absolutePath()+"/../resources/Menus/AccesRecette/recette.png");
  
  QString strFilesToDelete;
  QStringList listOfFilesAccesRecette;
  listOfFilesAccesRecette = dirAccesRecette.entryList();
  foreach(strFilesToDelete,listOfFilesAccesRecette){
      QString fileStr = absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesRecette/"+strFilesToDelete;
      QFile fileToDelete(fileStr);
      fileToDelete.remove();
      }
  QStringList listOfFilesAccesDepense;
  listOfFilesAccesDepense = dirAccesDepense.entryList();
  foreach(strFilesToDelete,listOfFilesAccesDepense){
      QString fileStr = absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesDepense/"+strFilesToDelete;
      QFile fileToDelete(fileStr);
      fileToDelete.remove();
      }
  QStringList listOfFilesAccesCompta;
  listOfFilesAccesCompta = dirAccesCompta.entryList();
  foreach(strFilesToDelete,listOfFilesAccesCompta){
      QString fileStr = absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesCompta/"+strFilesToDelete;
      QFile fileToDelete(fileStr);
      fileToDelete.remove();
      }
  
  /*QFile fileComptaMedintux(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesCompta/definition.txt");
  QFile fileDepenseMedintux(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesDepense/definition.txt");
  QFile fileRecetteMedintux(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesRecette/definition.txt");
  fileComptaMedintux.remove();
  fileDepenseMedintux.remove();
  fileRecetteMedintux.remove();*/
        fileCompta          . copy(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesCompta/definition.txt");
        fileComptaPng       . copy(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesCompta/comptabilite.png");
        fileDepense         . copy(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesDepense/definition.txt");
        fileDepensePng      . copy(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesDepense/creditcards.png");
        fileRecette         . copy(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesRecette/definition.txt");
        fileRecettePng      . copy(absolutePath()+"/../../drtux/bin/Ressources/Menus/AccesRecette/recette.png");
        

}

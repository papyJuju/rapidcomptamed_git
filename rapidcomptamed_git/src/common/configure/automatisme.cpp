#include<QtGui>
#include<QtCore>

#include "automatisme.h"
#include "../constants.h"
#include "../icore.h"
#include "../settings.h"
using namespace Common;
using namespace Constants;

automatismeDialog::automatismeDialog(){
  setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose);
  radioButton                  -> setText("Laisser ouvert.");
  radioButton_2                -> setText("Fermeture automatique");
  radioButton                  -> setChecked(true);
  radioButton                  -> setToolTip("Si vous choisissez cette option,\n"
                                             "la fenêtre de saisie des recettes restera ouverte après saisie.");
  radioButton_2                -> setToolTip("Si vous choisissez cette option,\n"
                                             "la fenêtre de saisie des recettes se fermera automatiquement après saisie.");
  connect(pushButton,  SIGNAL(pressed()),this,SLOT(enregistre()));
  connect(pushButton_2,SIGNAL(pressed()),this,SLOT(close()));

}

automatismeDialog::~automatismeDialog(){}

void automatismeDialog::enregistre(){
  QString choice = "";
  if(radioButton->isChecked()){
      choice = "oui";
  }
  if(radioButton_2->isChecked()){
      choice = "non";
  }
  Common::ICore::instance()->settings()->setValue(Constants::S_CLOSING_CHOICE,choice);
  QMessageBox::information(0,trUtf8("Information"),trUtf8("Le changement de configuration sera pris en compte \n"
                                                          "à la prochaine ouverture de "
                                                          "la fenêtre de saisie des recettes."),QMessageBox::Ok);
  emit close();
}

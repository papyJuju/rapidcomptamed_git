#ifndef PARAMETRAGES_H
#define PARAMETRAGES_H

#include "ui_ParametragesDialog.h"
#include <QWidget>

namespace ParametragesSpace
{
class parametrages : public QWidget , public Ui::ParametrageDialog
{
  Q_OBJECT
public :
  parametrages(QWidget*);
  ~parametrages();
protected slots :
  void utilisateurPreferentiel();
  void utilisateursComptabilite();
  void parametreSite();
  void entrePayeur();
  void entrePourcentage();
  void entreActe();
  void autoFermeture();
  //void completeBase();
  void rapportBug();
  void headingsParams();
  void thesaurusModif();
  void regenereConfigIni();
  void configmedintuxini();
  void configdatasofrapidcomptamed();
};
};

#endif

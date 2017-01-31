#!/bin/bash

echo "Pour compiler vous avez besoin de QT4.
      Si QT 4 n'est pas installe ouvrez un terminal 
      et faites en root (su ou sudo):
      apt-get install qt-sdk
      Pour fonctionner le programme aura besoin de mysql.
      Si MYSQL n'est pas intalle faites dans un terminant en root (su ou sudo) :
      apt-get install mysql-server
      
      Pour continuer tapez y pour abandonner tapez n
      puis touche Entree"
      
read a
if [ $a != 'y' ]
	then echo "Abandon de la compilation."
	exit 1
fi

pathAbsRapidcomptamed=`pwd`
echo "le chemin est : $pathAbsRapidcomptamed"

if [ $1 == ]; then
  LISTE="recettedialog depensedialog comptatriee sauvegarde alertes"
  else
  LISTE=$1
fi

#if [ $LISTE == ]; then
#  exit
#fi

for i in $LISTE
  do
    if [ $i = 'recettedialog' ]  ||  [ $i = 'depensedialog' ] ; then
      pAbs=$pathAbsRapidcomptamed/src/comptabilite_interfaces_src/$i
      echo "$pAbs"
      qmake-qt4 -nocache $pAbs/$i.pro
      make
      make clean
      echo ""
      echo "========== fin de compilation $i  ======================================"
      echo ""
    elif [ $i = 'comptatriee' ] ; then
      pAbs=$pathAbsRapidcomptamed/src/recettestriees/$i
      echo "$pAbs"
      qmake-qt4 -nocache $pAbs/$i.pro
      make
      make clean
      echo ""
      echo "==========fin de compilation $i  ======================================"
      echo ""
    else
      pAbs=$pathAbsRapidcomptamed/src/$i
      echo "$pAbs"
      qmake-qt4 -nocache $pAbs/$i.pro
      make
      make clean
      echo ""
      echo "==========fin de compilation $i  ======================================"
      echo ""
    fi
   done

echo ""
echo "==============fin de la compilation les binaires sont dans /bin================"
echo ""

exit 0
  

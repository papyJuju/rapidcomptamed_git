#!/usr/bin/perl -w

$cheminmedintux = "/home/pmddeb/MedinTux-V2.13-Mac-Intel-105-pmd-130809/Programmes/";
$cheminqmakeqt4 = "qmake-qt4";
$cheminmake     = "make";
@makeclean = ($cheminmake,"clean");
@make = ($cheminmake);
#construction de la liste des make 

if ($ARGV[0] eq "")
{
@listemake = ("parametrages","recettedialog","depensedialog","immobilisations","comptatriee","recettestriees","depensestriees","etatdesdus","grand_livre","setup");
}
else
{
@listemake = ($ARGV[0]);
}

for($i=0;$i<@listemake;$i++){

if($listemake[$i] eq "parametrages" || $listemake[$i] eq "immobilisations" || $listemake[$i] eq "recettedialog" || $listemake[$i] eq "depensedialog"){
#-----------------------------------------------------------------------------------------------
#compilation en QT4 -----------------------------------------------------------------------------
chdir($cheminmedintux."comptabilite/comptabilite_interfaces_src/".$listemake[$i]."/");
@qmakecomptabiliteintefaces = ($cheminqmakeqt4 ,$cheminmedintux."comptabilite/comptabilite_interfaces_src/".$listemake[$i]."/src.pro");
system(@qmakecomptabiliteintefaces) == 0
	or die "system @qmakecomptabiliteintefaces erreur:$?";
system(@make) == 0
	or die "system @make erreur:$?";
system(@makeclean) == 0
	or die "system @make erreur:$?";
		
print "\n====================== compilation ".$listemake[$i]." OK ======================\n";
}
elsif($listemake[$i] eq "setup"){
@qmakesetup = ($cheminqmakeqt4 ,$cheminmedintux."comptabilite/".$listemake[$i]."/src.pro");
chdir($cheminmedintux."comptabilite/".$listemake[$i]."/");
system(@qmakesetup) == 0
	or die "system @qmakesetup erreur:$?";
system(@make) == 0
	or die "system @make erreur:$?";
system(@makeclean) == 0
	or die "system @make erreur:$?";
print "compilation ".$listemake[$i]."\n";
}

else{
@qmakerecettestriees = ($cheminqmakeqt4 ,$cheminmedintux."comptabilite/recettestriees/".$listemake[$i]."/src.pro");
chdir($cheminmedintux."comptabilite/recettestriees/".$listemake[$i]."/");
system(@qmakerecettestriees) == 0
	or die "system @qmakerecettestriees erreur:$?";
system(@make) == 0
	or die "system @make erreur:$?";
system(@makeclean) == 0
	or die "system @make erreur:$?";
	

print "\n=================compilation ".$listemake[$i]."========================================\n";

}
}
print "\n==============================================================\n";
print "-------------compilation at end----------------\n";
print "================================================================\n";






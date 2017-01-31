-- MySQL dump 10.13  Distrib 5.1.47, for debian-linux-gnu (i486)
--
-- Host: localhost    Database: comptabilite
-- ------------------------------------------------------
-- Server version	5.1.47-1
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO,MYSQL40' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `mouvements_disponibles`
--

DROP TABLE IF EXISTS `mouvements_disponibles`;
CREATE TABLE `mouvements_disponibles` (
  `id_mvt_dispo` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `id_mvt_parent` int(11) DEFAULT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `libelle` varchar(100) NOT NULL,
  `remarque` blob,
  PRIMARY KEY (`id_mvt_dispo`)
) TYPE=MyISAM AUTO_INCREMENT=44;

--
-- Dumping data for table `mouvements_disponibles`
--

LOCK TABLES `mouvements_disponibles` WRITE;
/*!40000 ALTER TABLE `mouvements_disponibles` DISABLE KEYS */;
INSERT INTO `mouvements_disponibles` VALUES (1,0,0,'Apports praticien','|ln=AF'),(2,0,0,'Autres recettes','|ln=AF'),(3,0,0,'Produits financiers','|ln=AE'),(4,0,1,'TVA recuperable','NULL'),(5,0,1,'Prelevements du praticien','NULL'),(6,0,1,'Achats : fournitures et pharmacie','NULL'),(7,0,1,'Frais de personnel','NULL'),(8,6,1,'Salaires nets','|ln=BB'),(9,6,1,'Charges sociales','|ln=BC'),(10,0,1,'Impots et Taxes','NULL'),(11,10,1,'TVA Pay','NULL'),(12,10,1,'Taxe Professionnelle','|ln=BE'),(13,10,1,'Autres impots','|ln=BS'),(14,0,1,'Travaux, Fournitures, Services Exterieurs','NULL'),(15,14,1,'Loyers et charges','|ln=BF'),(16,14,1,'Location materiel','|ln=BG'),(17,14,1,'Reparation et entretien','|ln=BH'),(18,14,1,'Personnel interim, secretariat telephonique','|ln=BH'),(19,14,1,'Petit outillage','|ln=BH'),(20,14,1,'Gaz, electricite, chauffage, eau','|ln=BH'),(21,14,1,'Honoraires retrocedes','NULL'),(22,14,1,'Honoraires ne constituant pas de retrocession','|ln=BH'),(23,14,1,'Assurances','|ln=BH'),(24,0,1,'Transports et deplacements','NULL'),(25,24,1,'Frais de voiture','|ln=BJ'),(26,24,1,'Frais moto','|ln=BJ'),(27,24,1,'Autres frais de deplacement','|ln=BJ'),(28,0,1,'Charges sociales du praticien','NULL'),(29,28,1,'Vieillesse, Assurance Maladie, Alloc. Fam.','|ln=BM'),(30,0,1,'Frais divers de gestion','|ln=BM'),(31,30,1,'Congres','NULL'),(32,30,1,'Cadeaux, representation et reception','NULL'),(33,30,1,'Frais de bureau, documentation, et P et T','NULL'),(34,30,1,'Cotisation professionnelle et syndicales','|ln=BM'),(35,30,1,'Divers','|ln=BM'),(36,0,1,'Frais financiers','|ln=BN'),(37,0,1,'Pertes',''),(38,0,1,'Debours payes pour le compte des clients','NULL'),(39,0,1,'Autres depenses','NULL'),(40,39,1,'SCM ou partage de frais','NULL'),(41,39,1,'Immobilisations','NULL'),(42,39,1,'Divers a reintegrer','|ln=CC'),(43,NULL,1,'Comptes d\'attente',NULL);
/*!40000 ALTER TABLE `mouvements_disponibles` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2010-06-30 17:24:00

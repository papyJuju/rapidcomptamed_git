-- MySQL dump 10.13  Distrib 5.1.47, for debian-linux-gnu (i486)
--
-- Host: localhost    Database: comptabilite
-- ------------------------------------------------------
-- Server version	5.1.45-1
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO,MYSQL40' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `actes_disponibles`
--

DROP TABLE IF EXISTS `actes_disponibles`;
CREATE TABLE `actes_disponibles` (
  `id_acte_dispo` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `nom_acte` varchar(50) NOT NULL,
  `desc_acte` varchar(200) NOT NULL,
  `type` varchar(100) NOT NULL,
  `montant_total` double NOT NULL,
  `montant_tiers` double NOT NULL,
  `date_effet` date NOT NULL,
  PRIMARY KEY (`id_acte_dispo`)
) TYPE=MyISAM AUTO_INCREMENT=5;

--
-- Dumping data for table `actes_disponibles`
--

LOCK TABLES `actes_disponibles` WRITE;
/*!40000 ALTER TABLE `actes_disponibles` DISABLE KEYS */;
INSERT INTO `actes_disponibles` VALUES (1,'V','visite','visite',32,0,'2009-07-03'),(2,'CS','consultation spécialisée','spé',23,10,'0000-00-00'),(3,'C','consultation MG','C',22,6.6,'2010-06-01'),(4,'CA','Consultation approfondie MG','C',26,8.66,'2010-06-01');
/*!40000 ALTER TABLE `actes_disponibles` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2010-06-22 13:35:02

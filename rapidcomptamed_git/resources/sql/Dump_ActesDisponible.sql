-- MySQL dump 10.11
--
-- Host: localhost    Database: comptabilite
-- ------------------------------------------------------
-- Server version	5.0.51a-24+lenny1
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
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `actes_disponibles` (
  `id_acte_dispo` int(10) unsigned NOT NULL auto_increment,
  `nom_acte` varchar(50) NOT NULL,
  `desc_acte` varchar(200) NOT NULL,
  `type` varchar(100) NOT NULL,
  `montant_total` double NOT NULL,
  `montant_tiers` double NOT NULL,
  `date_effet` date NOT NULL,
  PRIMARY KEY  (`id_acte_dispo`)
) TYPE=MyISAM AUTO_INCREMENT=9;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `actes_disponibles`
--

LOCK TABLES `actes_disponibles` WRITE;
/*!40000 ALTER TABLE `actes_disponibles` DISABLE KEYS */;
INSERT INTO `actes_disponibles` VALUES (1,'V','visite','visite',32,0,'2009-07-03'),(2,'DEP','depassement','depassement',0,0,'2009-07-03'),(3,'C+MGE','consultation_generaliste_enfant','consultation',25,0,'2009-07-03'),(4,'C+MNO','consultation_generaliste_nourrisson','consultation',27,0,'2009-07-03'),(5,'C+MCG','consultation_generaliste_avec_majoration_coordination','consultation',25,0,'2009-07-03'),(6,'V+MU','visite_MG_urgence','visite',40,0,'2009-07-03'),(7,'MU','majoration_urgence','majoration',26.88,0,'2009-07-03'),(8,'forfait_ALD','forfait_ALD30','forfait',40,0,'2009-07-03');
/*!40000 ALTER TABLE `actes_disponibles` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-08-04 16:39:55

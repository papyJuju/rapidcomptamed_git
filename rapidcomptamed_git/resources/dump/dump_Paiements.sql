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
-- Table structure for table `paiements`
--

DROP TABLE IF EXISTS `paiements`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `paiements` (
  `id_paiement` int(11) unsigned NOT NULL auto_increment,
  `id_hono` int(11) unsigned NOT NULL,
  `date` date NOT NULL,
  `id_payeurs` bigint(20) NOT NULL,
  `acte` blob,
  `esp` double NOT NULL,
  `chq` double NOT NULL,
  `cb` double NOT NULL,
  `daf` double NOT NULL,
  `autre` double NOT NULL,
  `du` double NOT NULL,
  `valide` tinyint(1) default NULL,
  PRIMARY KEY  (`id_paiement`)
) TYPE=MyISAM;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `paiements`
--

LOCK TABLES `paiements` WRITE;
/*!40000 ALTER TABLE `paiements` DISABLE KEYS */;
/*!40000 ALTER TABLE `paiements` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-10-31 17:04:43

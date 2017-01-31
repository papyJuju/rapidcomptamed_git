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
-- Table structure for table `pourcentages`
--

DROP TABLE IF EXISTS `pourcentages`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `pourcentages` (
  `id_pourcent` int(10) unsigned NOT NULL auto_increment,
  `type` varchar(100) default NULL,
  `pourcentage` varchar(6) NOT NULL,
  PRIMARY KEY  (`id_pourcent`)
) TYPE=MyISAM AUTO_INCREMENT=5;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `pourcentages`
--

LOCK TABLES `pourcentages` WRITE;
/*!40000 ALTER TABLE `pourcentages` DISABLE KEYS */;
INSERT INTO `pourcentages` VALUES (1,'AMO_Alsace','90'),(2,'AMO','70'),(3,'AMC','30'),(4,'AMC_Alsace','10');
/*!40000 ALTER TABLE `pourcentages` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-09-02 16:16:23

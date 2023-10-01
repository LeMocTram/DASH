-- MySQL dump 10.13  Distrib 8.0.33, for Linux (x86_64)
--
-- Host: localhost    Database: CHATSERVICE
-- ------------------------------------------------------
-- Server version	8.0.33-0ubuntu0.20.04.2

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `CONVERSATION`
--

DROP TABLE IF EXISTS `CONVERSATION`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `CONVERSATION` (
  `conversationID` int NOT NULL,
  `no_participants` int NOT NULL,
  `no_active_p` int NOT NULL,
  `conversation_name` varchar(20) NOT NULL,
  PRIMARY KEY (`conversationID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `CONVERSATION`
--

LOCK TABLES `CONVERSATION` WRITE;
/*!40000 ALTER TABLE `CONVERSATION` DISABLE KEYS */;
INSERT INTO `CONVERSATION` VALUES (1,2,2,'Con1'),(2,2,2,'Con2');
/*!40000 ALTER TABLE `CONVERSATION` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `MESSAGE`
--

DROP TABLE IF EXISTS `MESSAGE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `MESSAGE` (
  `messageID` char(5) NOT NULL,
  `text` varchar(1024) NOT NULL,
  PRIMARY KEY (`messageID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `MESSAGE`
--

LOCK TABLES `MESSAGE` WRITE;
/*!40000 ALTER TABLE `MESSAGE` DISABLE KEYS */;
INSERT INTO `MESSAGE` VALUES ('00001','Hello, World'),('00002','Hello, World'),('00003','Hello, World'),('00004','Hello, World'),('00005','Hello, World'),('00006','Hello, World'),('00007','Hello, harrygooner');
/*!40000 ALTER TABLE `MESSAGE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PARTICIPATE`
--

DROP TABLE IF EXISTS `PARTICIPATE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `PARTICIPATE` (
  `conversationID` int NOT NULL,
  `username` varchar(30) NOT NULL,
  PRIMARY KEY (`conversationID`,`username`),
  KEY `username` (`username`),
  CONSTRAINT `PARTICIPATE_ibfk_1` FOREIGN KEY (`username`) REFERENCES `USER` (`username`),
  CONSTRAINT `PARTICIPATE_ibfk_2` FOREIGN KEY (`conversationID`) REFERENCES `CONVERSATION` (`conversationID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `PARTICIPATE`
--

LOCK TABLES `PARTICIPATE` WRITE;
/*!40000 ALTER TABLE `PARTICIPATE` DISABLE KEYS */;
INSERT INTO `PARTICIPATE` VALUES (1,'h'),(2,'h'),(2,'harrygooner'),(1,'hello'),(2,'hello'),(2,'khanhcba'),(2,'lemoctram'),(2,'thinhdek297'),(2,'trungcba');
/*!40000 ALTER TABLE `PARTICIPATE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SEND`
--

DROP TABLE IF EXISTS `SEND`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `SEND` (
  `messageID` char(5) NOT NULL,
  `conversationID` int NOT NULL,
  `username` varchar(30) NOT NULL,
  PRIMARY KEY (`messageID`,`conversationID`,`username`),
  KEY `username` (`username`),
  KEY `conversationID` (`conversationID`),
  CONSTRAINT `SEND_ibfk_1` FOREIGN KEY (`username`) REFERENCES `USER` (`username`),
  CONSTRAINT `SEND_ibfk_2` FOREIGN KEY (`conversationID`) REFERENCES `CONVERSATION` (`conversationID`),
  CONSTRAINT `SEND_ibfk_3` FOREIGN KEY (`messageID`) REFERENCES `MESSAGE` (`messageID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SEND`
--

LOCK TABLES `SEND` WRITE;
/*!40000 ALTER TABLE `SEND` DISABLE KEYS */;
INSERT INTO `SEND` VALUES ('00001',2,'khanhcba'),('00002',2,'trungcba'),('00003',2,'thinhdek297'),('00004',2,'lemoctram'),('00005',2,'harrygooner'),('00006',2,'hello'),('00007',2,'h');
/*!40000 ALTER TABLE `SEND` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `USER`
--

DROP TABLE IF EXISTS `USER`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `USER` (
  `username` varchar(25) NOT NULL,
  `password` char(255) NOT NULL,
  `email` varchar(30) NOT NULL,
  `fullname` varchar(30) NOT NULL,
  `status` tinyint NOT NULL,
  PRIMARY KEY (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `USER`
--

LOCK TABLES `USER` WRITE;
/*!40000 ALTER TABLE `USER` DISABLE KEYS */;
INSERT INTO `USER` VALUES ('h','h','h','h',1),('harrygooner','hung123','hung@gmail.com','Hung',1),('hello','hello','hello','hello',1),('khanhcba','hung123','khanh@gmail.com','khanh',1),('lemoctram','tram123','tram@gmail.com','tram',1),('thinhdek297','hung123','thinh@gmail.com','thinh',1),('trungcba','hung123','trungg@gmail.com','trung',1);
/*!40000 ALTER TABLE `USER` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-07-27 17:09:35

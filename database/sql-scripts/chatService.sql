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
  `conversationID` int NOT NULL AUTO_INCREMENT,
  `no_participants` int NOT NULL,
  `no_active_p` int NOT NULL,
  `conversation_name` varchar(20) NOT NULL,
  PRIMARY KEY (`conversationID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
ALTER TABLE CONVERSATION AUTO_INCREMENT=0;
/*!40101 SET character_set_client = @saved_cs_client */;
ALTER TABLE CONVERSATION AUTO_INCREMENT=0;

--
-- Dumping data for table `CONVERSATION`
--

LOCK TABLES `CONVERSATION` WRITE;
/*!40000 ALTER TABLE `CONVERSATION` DISABLE KEYS */;
/*!40000 ALTER TABLE `CONVERSATION` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `MESSAGE`
--

DROP TABLE IF EXISTS `MESSAGE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `MESSAGE` (
  `messageID` int NOT NULL AUTO_INCREMENT,
  `text` varchar(1024) NOT NULL,
  PRIMARY KEY (`messageID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
ALTER TABLE MESSAGE AUTO_INCREMENT=0;
/*!40101 SET character_set_client = @saved_cs_client */;
ALTER TABLE MESSAGE AUTO_INCREMENT=0;

--
-- Dumping data for table `MESSAGE`
--

LOCK TABLES `MESSAGE` WRITE;
/*!40000 ALTER TABLE `MESSAGE` DISABLE KEYS */;
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
/*!40000 ALTER TABLE `PARTICIPATE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SEND`
--

DROP TABLE IF EXISTS `SEND`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `SEND` (
  `messageID` int NOT NULL,
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

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-07-26 13:05:28

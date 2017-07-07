CREATE DATABASE  IF NOT EXISTS `it_step` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `it_step`;
-- MySQL dump 10.13  Distrib 5.7.17, for Win64 (x86_64)
--
-- Host: localhost    Database: it_step
-- ------------------------------------------------------
-- Server version	5.7.18-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `groups`
--

DROP TABLE IF EXISTS `groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `groups` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `teacherId` int(10) unsigned NOT NULL,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_teachedId_idx` (`teacherId`),
  CONSTRAINT `fk_teachedId` FOREIGN KEY (`teacherId`) REFERENCES `teacher` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `groups`
--

LOCK TABLES `groups` WRITE;
/*!40000 ALTER TABLE `groups` DISABLE KEYS */;
INSERT INTO `groups` VALUES (1,1,'ЕКО-16-П3');
/*!40000 ALTER TABLE `groups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `questions`
--

DROP TABLE IF EXISTS `questions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `questions` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `quizId` int(11) NOT NULL,
  `question` varchar(120) COLLATE utf8_unicode_ci NOT NULL,
  `rightAnswer` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `wrongAnswer2` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `wrongAnswer3` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `wrongAnswer4` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `code` longblob,
  PRIMARY KEY (`id`),
  KEY `quizId_idx` (`quizId`),
  CONSTRAINT `quizId` FOREIGN KEY (`quizId`) REFERENCES `quizes` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `questions`
--

LOCK TABLES `questions` WRITE;
/*!40000 ALTER TABLE `questions` DISABLE KEYS */;
INSERT INTO `questions` VALUES (27,4,'Which of the following factors supports the statement that reusability is a desirable feature of a language?','Both A and B.','It reduces the compilation time.','It lowers the maintenance cost.','It decreases the testing time.',NULL),(28,4,'Which of the following is a mechanism of static polymorphism?','All of the above','Templates','Function overloading','Operator overloading','VOID receiveUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo &clientinfo) {\r\n	char szBuff[100];\r\n	int result = recv(clientinfo.socket, szBuff, 100, 0);\r\n	if (!strcmp(szBuff, \"<CHECK_STUDENT>\")) {\r\n		strcpy_s(szBuff, \"<OK>\");\r\n		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n\r\n		result = recv(clientinfo.socket, szBuff, 100, 0);\r\n		name = szBuff;\r\n		strcpy_s(szBuff, \"<OK>\");\r\n		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n\r\n		result = recv(clientinfo.socket, szBuff, 100, 0);\r\n		surname = szBuff;\r\n		strcpy_s(szBuff, \"<OK>\");\r\n		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n\r\n		result = recv(clientinfo.socket, szBuff, 100, 0);\r\n		fatherName = szBuff;\r\n		strcpy_s(szBuff, \"<OK>\");\r\n		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n\r\n		result = recv(clientinfo.socket, szBuff, 100, 0);\r\n		group = szBuff;\r\n	}\r\n}'),(29,4,'What happens if the base and derived class contains definition of a function with same prototype?','Base class object will call base class function and derived class object will ca','Only derived class function will get called irrespective of object.','Only base class function will get called irrespective of object.','Compiler reports an error on compilation.',NULL),(30,4,'Which of the following is not a type of inheritance?','Distributive','Multilevel','Multiple','Hierarchical','BOOL checkUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo & clientinfo) {\r\n	char szBuff[100];\r\n	INT studId = -1;\r\n	INT groupId = -1;\r\n	MyDialog::ptr->ssql << \"select student.id,student.Name,student.groupId from student where name=;\r\n	MyDialog::ptr->ssql << surname <<  << name <<  << fatherName <<  and student.groupId = (select groups.id from groups where groups.name=;\r\n	MyDialog::ptr->ssql << group << );\";\r\n	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();\r\n	MyDialog::ptr->ssql.str(\"\");\r\n	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {\r\n		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());\r\n\r\n		if ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) == NULL) {\r\n			strcpy(szBuff, \"<DOESNT_EXIST>\");\r\n			send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n			shutdown(clientinfo.socket, SD_BOTH); // SD_BOTH cai?auaao eae i?eai, oae e ioi?aaeo aaiiuo\r\n			closesocket(clientinfo.socket);\r\n		}\r\n		else {\r\n			//Student connected\r\n			//If passed already\r\n			studId = atoi(MyDialog::ptr->row[0]);\r\n			groupId = atoi(MyDialog::ptr->row[2]);\r\n			mysql_free_result(MyDialog::ptr->rset);\r\n\r\n\r\n			MyDialog::ptr->ssql << \"select groups.name from groups where groups.id =  << groupId << ;\";\r\n			MyDialog::ptr->sql = MyDialog::ptr->ssql.str();\r\n			MyDialog::ptr->ssql.str(\"\");\r\n			if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {//If a student from the group chosen by the teacher;\r\n				MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());\r\n				if ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {\r\n\r\n					if (!strcmp(MyDialog::ptr->getGroup(), MyDialog::ptr->row[0])) {\r\n						MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n						mysql_free_result(MyDialog::ptr->rset);\r\n\r\n						MyDialog::ptr->ssql << \"select results.date from results where results.idStudent =  << studId <<  \";\r\n						MyDialog::ptr->ssql << \"AND results.idQuiz =  << MyDialog::ptr->quizID << ;\";\r\n						MyDialog::ptr->sql = MyDialog::ptr->ssql.str();\r\n						MyDialog::ptr->ssql.str(\"\");\r\n\r\n						if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {\r\n							MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());\r\n\r\n							if ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) == NULL) {//Never passed no record in results table yet\r\n								MyDialog::ptr->ssql << \"INSERT INTO results(results.idQuiz,results.idStudent) VALUES(;\r\n								MyDialog::ptr->ssql << MyDialog::ptr->quizID << , << studId << );\";\r\n								MyDialog::ptr->sql = MyDialog::ptr->ssql.str();\r\n								MyDialog::ptr->ssql.str(\"\");\r\n								mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());\r\n\r\n								strcpy_s(szBuff, \"<OK>\");\r\n								send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n								MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n								mysql_free_result(MyDialog::ptr->rset);\r\n								return TRUE;\r\n							}\r\n							else if (MyDialog::ptr->row[0] != NULL) {//Record exists, and Date field is filled\r\n								strcpy(szBuff, \"<ALREADY_PASSED>\");\r\n								send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n								shutdown(clientinfo.socket, SD_BOTH); // SD_BOTH cai?auaao eae i?eai, oae e ioi?aaeo aaiiuo\r\n								closesocket(clientinfo.socket);\r\n							}\r\n							else {//Record exists, and Date field is empty\r\n								  //If connected from another pc or erased date field in database\r\n								string lookFor = surname;\r\n								lookFor += \" \";\r\n								lookFor += name;\r\n								lookFor += \" \";\r\n								lookFor += fatherName;\r\n								LVFINDINFO item = { LVFI_STRING, (LPCTSTR)lookFor.c_str() };\r\n								int indexUser = ListView_FindItem(MyDialog::ptr->getUserList(), -1, &item);\r\n\r\n								if (MyDialog::ptr->userListView.at(indexUser)->getStatus() == Status::NOT_CONNECTED) {//Retake\r\n									strcpy_s(szBuff, \"<OK>\");\r\n									send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n									MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n									mysql_free_result(MyDialog::ptr->rset);\r\n									return TRUE;\r\n								}\r\n								else {//Taking right now\r\n									strcpy_s(szBuff, \"<ALREADY_LOGGED>\");\r\n									send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);\r\n									MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n									mysql_free_result(MyDialog::ptr->rset);\r\n									return FALSE;\r\n								}\r\n							}\r\n						}\r\n\r\n					}\r\n					else {\r\n						MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n						mysql_free_result(MyDialog::ptr->rset);\r\n						return FALSE;\r\n					}\r\n				}\r\n			}\r\n		}\r\n	}\r\n	MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n	mysql_free_result(MyDialog::ptr->rset);\r\n	return FALSE;\r\n}'),(31,4,'Which of the following keyword is used to overload an operator?','operator','overload','friend','override',NULL),(32,4,'Which of the following statement is correct?','Object is an instance of a class.','Class is an instance of object.','Class is an instance of data type.','Object is an instance of data type.','INT_PTR CALLBACK MyDialog::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)\r\n{\r\n	switch (message)\r\n	{\r\n		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);\r\n		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);\r\n		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);\r\n		case WM_NOTIFY:\r\n		HandleWM_NOTIFY(lParam);\r\n		break;\r\n	}\r\n	return FALSE;\r\n}'),(33,4,'Which of the following function / type of function cannot be overloaded?','Virtual function','Static function','Member function','Both B and C',NULL),(34,4,'Which of the following statement is incorrect?','The default arguments are given in the function prototype and should be repeated','Compiler uses the prototype information to build a call, not the function defini','The default arguments are given in the function prototype.','The default value for an argument can be a global constant.',NULL),(35,4,'Which of the following statements is correct?1.Once the variable and the reference are linked they are tied together.2.O','Only 1 is correct.','Only 2 is correct.','Both 1 and 2 are correct.','Both 1 and 2 are incorrect.',NULL),(36,4,'Which of the following statement is correct?','A reference is a constant pointer.','A reference is not a constant pointer.','An array of references is acceptable.','It is possible to create a reference to a reference.',NULL),(37,4,'Which of the following statement is correct?','An array of references is not acceptable.','Once a reference variable has been defined to refer to a particular variable it ','An array of references is acceptable.','Reference is like a structure.',NULL),(38,4,'Which of the following statement is correct with respect to the use of friend keyword inside a class?','A class may be declared as a friend.','A private data member can be declared as a friend.','An object may be declared as a friend.','We can use friend keyword as a class name.',NULL),(39,4,'Which of the following statements is correct?','Both data and functions can be either private or public.','Data items in a class must be private.','Member functions of a class must be private.','Constructor of a class cannot be private.',NULL),(40,4,'Which of the following means \"The use of an object of one class in definition of another class\"?','Composition','Inheritance','Encapsulation','Abstraction',NULL),(41,4,'Destructor has the same name as the constructor and it is preceded by ______ .','~','?','!','$',NULL),(42,4,'When are the Global objects destroyed?','When the program terminates.','When the control comes out of the block in which they are being used.','When the control comes out of the function in which they are being used.','As soon as local objects die.',NULL),(43,4,'How many default constructors per class are possible?','Only one','Two','Three','Unlimited',NULL),(44,4,'Which of the following statement is correct about destructors?','A destructor has no return type.','A destructor has void return type.','A destructor has integer return type.','A destructors return type is always same as that of main().',NULL),(45,4,'Which of the following type of class allows only one object of it to be created?','Singleton class','Abstract class','Virtual class','Friend class',NULL),(46,4,'How many instances of an abstract class can be created?','0','5','13','1',NULL),(47,4,'Which of the following statement will be correct if the function has three arguments passed to it?','The trailing argument will be the default argument.','The first argument will be the default argument.','The middle argument will be the default argument.','All the argument will be the default argument.',NULL),(48,4,'Отработает ли программа','Да','Нет','Да/нет','Нет/да','void updateQuizCombo(HWND& h) {\r\n	if (Quiz::ptr !=nullptr && Quiz::ptr->hChooseQuiz == h) {\r\n		Quiz::ptr->m_quizRelation.clear();\r\n	}\r\n	SendMessage(h, CB_RESETCONTENT, NULL, NULL);\r\n\r\n	MyDialog::ptr->ssql << \"select * from quizes;\";\r\n	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();\r\n	MyDialog::ptr->ssql.str(\"\");\r\n	INT counter = 0;\r\n	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {\r\n		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());\r\n		while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {\r\n			if (Quiz::ptr != nullptr && Quiz::ptr->hChooseQuiz == h) {\r\n				Quiz::ptr->m_quizRelation.insert_or_assign(counter++, atoi(MyDialog::ptr->row[0]));\r\n			}\r\n			SendMessage(h, CB_ADDSTRING, 0, (LPARAM)MyDialog::ptr->row[1]);\r\n		}\r\n		MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n	}\r\n	mysql_free_result(MyDialog::ptr->rset);\r\n}\r\nvoid updateQuestionsCombo(HWND& h,INT quizId) {\r\n	Quiz::ptr->m_questionRelation.clear();\r\n	SendMessage(h, CB_RESETCONTENT, NULL, NULL);\r\n\r\n	MyDialog::ptr->ssql << \"select * from questions where quizId = ; \r\n	MyDialog::ptr->ssql << quizId << ;\";\r\n	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();\r\n	MyDialog::ptr->ssql.str(\"\");\r\n	INT counter = 0;\r\n	string buff;\r\n	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {\r\n		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());\r\n		while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {\r\n			Quiz::ptr->m_questionRelation.insert_or_assign(counter, atoi(MyDialog::ptr->row[0]));\r\n			buff = to_string(++counter);\r\n			buff += \".\";\r\n			buff += MyDialog::ptr->row[2];\r\n			SendMessage(h, CB_ADDSTRING, 0, (LPARAM)buff.c_str());\r\n			buff.clear();\r\n		}\r\n		MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);\r\n	}\r\n	mysql_free_result(MyDialog::ptr->rset);\r\n}');
/*!40000 ALTER TABLE `questions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `quizes`
--

DROP TABLE IF EXISTS `quizes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `quizes` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `description` varchar(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `quizes`
--

LOCK TABLES `quizes` WRITE;
/*!40000 ALTER TABLE `quizes` DISABLE KEYS */;
INSERT INTO `quizes` VALUES (2,'quizFirst',NULL),(4,'C++ Programming Test#1',NULL);
/*!40000 ALTER TABLE `quizes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `results`
--

DROP TABLE IF EXISTS `results`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `results` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `idQuiz` int(11) NOT NULL,
  `idStudent` int(11) NOT NULL,
  `score` float DEFAULT '0',
  `time` varchar(6) COLLATE utf8_unicode_ci DEFAULT NULL,
  `date` date DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `quizId_idx` (`idQuiz`),
  KEY `quizId` (`idQuiz`),
  KEY `fk_studentId_idx` (`idStudent`),
  CONSTRAINT `fk_quizId` FOREIGN KEY (`idQuiz`) REFERENCES `quizes` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_studentId` FOREIGN KEY (`idStudent`) REFERENCES `student` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `results`
--

LOCK TABLES `results` WRITE;
/*!40000 ALTER TABLE `results` DISABLE KEYS */;
/*!40000 ALTER TABLE `results` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `student`
--

DROP TABLE IF EXISTS `student`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `student` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(40) CHARACTER SET utf8 NOT NULL,
  `groupId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `fk_groupId_idx` (`groupId`),
  CONSTRAINT `fk_groupId` FOREIGN KEY (`groupId`) REFERENCES `groups` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `student`
--

LOCK TABLES `student` WRITE;
/*!40000 ALTER TABLE `student` DISABLE KEYS */;
INSERT INTO `student` VALUES (1,'Сайдалиев Даниил Сергеевич',1),(2,'Минченко Антон Андреевич',1),(3,'Кравчук Олег Витольтович',1),(4,'Пурдик Аркадий Олегович',1),(5,'Буженко Александр Александрович',1),(6,'Демченко Александр Игоревич',1),(7,'Шулянский Владислав Сергеевич',1),(8,'Дубровский Вадим Русланович',1),(9,'Шевченко Иван Игоревич',1),(10,'Замчий Юлия Михайловна',1),(11,'Скляров Владислав Александрович',1),(12,'Николенко Александр Сергеевич',1),(13,'Казанин Виктор Андреевич',1),(14,'Кичук Андрей Владимирович',1),(15,'Мирошник Владислав Евгеньевич',1),(16,'Мисоцкий Виктор Юрьевич',1),(17,'Шевчук Павел Александрович',1),(18,'Рашковский Никита Алексеевич',1),(19,'Белкин Денис Александрович',1);
/*!40000 ALTER TABLE `student` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `teacher`
--

DROP TABLE IF EXISTS `teacher`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `teacher` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `Password` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `teacher`
--

LOCK TABLES `teacher` WRITE;
/*!40000 ALTER TABLE `teacher` DISABLE KEYS */;
INSERT INTO `teacher` VALUES (1,'Виталий Полянский','vitalyadmin');
/*!40000 ALTER TABLE `teacher` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-07-07 20:06:00

-- phpMyAdmin SQL Dump
-- version 4.7.0
-- https://www.phpmyadmin.net/
--
-- Хост: 127.0.0.1
-- Время создания: Июн 13 2017 г., 00:07
-- Версия сервера: 10.1.22-MariaDB
-- Версия PHP: 7.1.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `it_step`
--

-- --------------------------------------------------------

--
-- Структура таблицы `groups`
--

CREATE TABLE `groups` (
  `id` int(10) UNSIGNED NOT NULL,
  `teacherId` int(10) UNSIGNED NOT NULL,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Дамп данных таблицы `groups`
--

INSERT INTO `groups` (`id`, `teacherId`, `name`) VALUES
(1, 1, 'ЕКО-16-П3');

-- --------------------------------------------------------

--
-- Структура таблицы `questions`
--

CREATE TABLE `questions` (
  `id` int(11) NOT NULL,
  `quizId` int(11) NOT NULL,
  `question` varchar(120) COLLATE utf8_unicode_ci NOT NULL,
  `rightAnswer` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `wrongAnswer2` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `wrongAnswer3` varchar(80) COLLATE utf8_unicode_ci NOT NULL,
  `wrongAnswer4` varchar(80) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Дамп данных таблицы `questions`
--

INSERT INTO `questions` (`id`, `quizId`, `question`, `rightAnswer`, `wrongAnswer2`, `wrongAnswer3`, `wrongAnswer4`) VALUES
(27, 4, 'Which of the following factors supports the statement that reusability is a desirable feature of a language?', 'Both A and B.', 'It reduces the compilation time.', 'It lowers the maintenance cost.', 'It decreases the testing time.'),
(28, 4, 'Which of the following is a mechanism of static polymorphism?', 'All of the above', 'Templates', 'Function overloading', 'Operator overloading'),
(29, 4, 'What happens if the base and derived class contains definition of a function with same prototype?', 'Base class object will call base class function and derived class object will ca', 'Only derived class function will get called irrespective of object.', 'Only base class function will get called irrespective of object.', 'Compiler reports an error on compilation.'),
(30, 4, 'Which of the following is not a type of inheritance?', 'Distributive', 'Multilevel', 'Multiple', 'Hierarchical'),
(31, 4, 'Which of the following keyword is used to overload an operator?', 'operator', 'overload', 'friend', 'override'),
(32, 4, 'Which of the following statement is correct?', 'Object is an instance of a class.', 'Class is an instance of object.', 'Class is an instance of data type.', 'Object is an instance of data type.'),
(33, 4, 'Which of the following function / type of function cannot be overloaded?', 'Virtual function', 'Static function', 'Member function', 'Both B and C'),
(34, 4, 'Which of the following statement is incorrect?', 'The default arguments are given in the function prototype and should be repeated', 'Compiler uses the prototype information to build a call, not the function defini', 'The default arguments are given in the function prototype.', 'The default value for an argument can be a global constant.'),
(35, 4, 'Which of the following statements is correct?1.Once the variable and the reference are linked they are tied together.2.O', 'Only 1 is correct.', 'Only 2 is correct.', 'Both 1 and 2 are correct.', 'Both 1 and 2 are incorrect.'),
(36, 4, 'Which of the following statement is correct?', 'A reference is a constant pointer.', 'A reference is not a constant pointer.', 'An array of references is acceptable.', 'It is possible to create a reference to a reference.'),
(37, 4, 'Which of the following statement is correct?', 'An array of references is not acceptable.', 'Once a reference variable has been defined to refer to a particular variable it ', 'An array of references is acceptable.', 'Reference is like a structure.'),
(38, 4, 'Which of the following statement is correct with respect to the use of friend keyword inside a class?', 'A class may be declared as a friend.', 'A private data member can be declared as a friend.', 'An object may be declared as a friend.', 'We can use friend keyword as a class name.'),
(39, 4, 'Which of the following statements is correct?', 'Both data and functions can be either private or public.', 'Data items in a class must be private.', 'Member functions of a class must be private.', 'Constructor of a class cannot be private.'),
(40, 4, 'Which of the following means \"The use of an object of one class in definition of another class\"?', 'Composition', 'Inheritance', 'Encapsulation', 'Abstraction'),
(41, 4, 'Destructor has the same name as the constructor and it is preceded by ______ .', '~', '?', '!', '$'),
(42, 4, 'When are the Global objects destroyed?', 'When the program terminates.', 'When the control comes out of the block in which they are being used.', 'When the control comes out of the function in which they are being used.', 'As soon as local objects die.'),
(43, 4, 'How many default constructors per class are possible?', 'Only one', 'Two', 'Three', 'Unlimited'),
(44, 4, 'Which of the following statement is correct about destructors?', 'A destructor has no return type.', 'A destructor has void return type.', 'A destructor has integer return type.', 'A destructors return type is always same as that of main().'),
(45, 4, 'Which of the following type of class allows only one object of it to be created?', 'Singleton class', 'Abstract class', 'Virtual class', 'Friend class'),
(46, 4, 'How many instances of an abstract class can be created?', '0', '5', '13', '1'),
(47, 4, 'Which of the following statement will be correct if the function has three arguments passed to it?', 'The trailing argument will be the default argument.', 'The first argument will be the default argument.', 'The middle argument will be the default argument.', 'All the argument will be the default argument.');

-- --------------------------------------------------------

--
-- Структура таблицы `quizes`
--

CREATE TABLE `quizes` (
  `id` int(11) NOT NULL,
  `name` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `description` varchar(45) COLLATE utf8_unicode_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Дамп данных таблицы `quizes`
--

INSERT INTO `quizes` (`id`, `name`, `description`) VALUES
(2, 'quizFirst', NULL),
(4, 'C++ Programming Test#1', NULL);

-- --------------------------------------------------------

--
-- Структура таблицы `results`
--

CREATE TABLE `results` (
  `id` int(10) UNSIGNED NOT NULL,
  `idQuiz` int(11) NOT NULL,
  `idStudent` int(11) NOT NULL,
  `score` float DEFAULT '0',
  `time` varchar(6) COLLATE utf8_unicode_ci DEFAULT NULL,
  `date` date DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Структура таблицы `student`
--

CREATE TABLE `student` (
  `id` int(11) NOT NULL,
  `Name` varchar(40) CHARACTER SET utf8 NOT NULL,
  `groupId` int(10) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Дамп данных таблицы `student`
--

INSERT INTO `student` (`id`, `Name`, `groupId`) VALUES
(1, 'Сайдалиев Даниил Сергеевич', 1),
(2, 'Минченко Антон Андреевич', 1),
(3, 'Кравчук Олег Витольтович', 1),
(4, 'Пурдик Аркадий Олегович', 1),
(5, 'Буженко Александр Александрович', 1),
(6, 'Демченко Александр Игоревич', 1),
(7, 'Шулянский Владислав Сергеевич', 1),
(8, 'Дубровский Вадим Русланович', 1),
(9, 'Шевченко Иван Игоревич', 1),
(10, 'Замчий Юлия Михайловна', 1),
(11, 'Скляров Владислав Александрович', 1),
(12, 'Николенко Александр Сергеевич', 1),
(13, 'Казанин Виктор Андреевич', 1),
(14, 'Кичук Андрей Владимирович', 1),
(15, 'Мирошник Владислав Евгеньевич', 1),
(16, 'Мисоцкий Виктор Юрьевич', 1),
(17, 'Шевчук Павел Александрович', 1),
(18, 'Рашковский Никита Алексеевич', 1),
(19, 'Белкин Денис Александрович', 1);

-- --------------------------------------------------------

--
-- Структура таблицы `teacher`
--

CREATE TABLE `teacher` (
  `id` int(10) UNSIGNED NOT NULL,
  `Name` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `Password` varchar(20) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Дамп данных таблицы `teacher`
--

INSERT INTO `teacher` (`id`, `Name`, `Password`) VALUES
(1, 'Виталий Полянский', 'vitalyadmin');

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `groups`
--
ALTER TABLE `groups`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_teachedId_idx` (`teacherId`);

--
-- Индексы таблицы `questions`
--
ALTER TABLE `questions`
  ADD PRIMARY KEY (`id`),
  ADD KEY `quizId_idx` (`quizId`);

--
-- Индексы таблицы `quizes`
--
ALTER TABLE `quizes`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `name_UNIQUE` (`name`);

--
-- Индексы таблицы `results`
--
ALTER TABLE `results`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id_UNIQUE` (`id`),
  ADD KEY `quizId_idx` (`idQuiz`),
  ADD KEY `quizId` (`idQuiz`),
  ADD KEY `fk_studentId_idx` (`idStudent`);

--
-- Индексы таблицы `student`
--
ALTER TABLE `student`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id_UNIQUE` (`id`),
  ADD KEY `fk_groupId_idx` (`groupId`);

--
-- Индексы таблицы `teacher`
--
ALTER TABLE `teacher`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id_UNIQUE` (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `groups`
--
ALTER TABLE `groups`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT для таблицы `questions`
--
ALTER TABLE `questions`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=48;
--
-- AUTO_INCREMENT для таблицы `quizes`
--
ALTER TABLE `quizes`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;
--
-- AUTO_INCREMENT для таблицы `results`
--
ALTER TABLE `results`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;
--
-- AUTO_INCREMENT для таблицы `student`
--
ALTER TABLE `student`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=20;
--
-- AUTO_INCREMENT для таблицы `teacher`
--
ALTER TABLE `teacher`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- Ограничения внешнего ключа сохраненных таблиц
--

--
-- Ограничения внешнего ключа таблицы `groups`
--
ALTER TABLE `groups`
  ADD CONSTRAINT `fk_teachedId` FOREIGN KEY (`teacherId`) REFERENCES `teacher` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Ограничения внешнего ключа таблицы `questions`
--
ALTER TABLE `questions`
  ADD CONSTRAINT `quizId` FOREIGN KEY (`quizId`) REFERENCES `quizes` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Ограничения внешнего ключа таблицы `results`
--
ALTER TABLE `results`
  ADD CONSTRAINT `fk_quizId` FOREIGN KEY (`idQuiz`) REFERENCES `quizes` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  ADD CONSTRAINT `fk_studentId` FOREIGN KEY (`idStudent`) REFERENCES `student` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Ограничения внешнего ключа таблицы `student`
--
ALTER TABLE `student`
  ADD CONSTRAINT `fk_groupId` FOREIGN KEY (`groupId`) REFERENCES `groups` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

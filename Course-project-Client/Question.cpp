#include "Question.h"
Question::Question(string question, string c1, string c2, string c3, string c4, int answer) {
	m_choices.push_back(c1);
	m_choices.push_back(c2);
	m_choices.push_back(c3);
	m_choices.push_back(c4);

	m_rightAnswer = answer;
	m_question = question;
	m_userAnswer = NOT_CHECKED;
}
void Question::randomizeQuestions(vector<Question*> &q){
	srand(time(NULL));
	int r1;
	int r2;
	for (int i = 0; i < q.size(); i++) {
		r1 = rand() % q.size();
		r2 = rand() % q.size();
		swap(q.at(r1), q.at(r2));
	}
}
void Question::randomizeChoices() {
	srand(time(NULL));
	int r1;
	int r2;
	for (int i = 0; i < 3; i++) {
		r1 = rand() % 4;
		r2 = rand() % 4;
		if (getRightAnswer() == r1) {
			setRightAnswer(r2);
		}
		else if (getRightAnswer() == r2) {
			setRightAnswer(r1);
		}
		swap(m_choices.at(r1), m_choices.at(r2));
	}
}
vector<Question*> Question::parseFile(string check, BOOL randQuestions) {
	vector<Question*>questions;
	rtrim(check);
	int qPos1, qPos2, choicePos1, choicePos2, aPos1, aPos2;
	qPos1 = qPos2 = choicePos1 = choicePos2 = aPos1 = aPos2 = 0;
	while (true) {
		Question* question = new Question();
		qPos1 = check.find("~q", qPos2);
		qPos2 = check.find("q~", qPos1);
		question->setQuestion(check.substr(qPos1 + 2, qPos2 - qPos1 - 2));

		for (int i = 0; i < 3; i++) {
			choicePos1 = check.find("~!", choicePos2);
			choicePos2 = check.find("!~", choicePos1);
			question->addChoices(check.substr(choicePos1 + 2, choicePos2 - choicePos1 - 2));
		}

		aPos1 = check.find("~^", aPos2);
		aPos2 = check.find("^~", aPos1);
		question->addChoices(check.substr(aPos1 + 2, aPos2 - aPos1 - 2));
		question->setRightAnswer(3);

		question->randomizeChoices();

		questions.push_back(question);
		if (check.size() - 2 == aPos2)
			break;
	}
	if (randQuestions) {
		randomizeQuestions(questions);
	}
	return questions;
}

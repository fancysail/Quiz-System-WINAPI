#pragma once
#include "header.h"
#define NOT_CHECKED 99
using namespace std;
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}
class Question {
private:
	vector<string> m_choices;
	int m_rightAnswer;
	string m_question;
	int m_userAnswer;
	string m_code;
public:
	Question() { m_userAnswer = NOT_CHECKED; }
	Question(string question, string c1, string c2, string c3, string c4, int answer);
	Question(string question, string c1, string c2, string c3, string c4, int answer, string code);

	static void randomizeQuestions(vector<Question*> &q);
	string getQuestion() { return m_question; }
	vector<string> getChoices() { return m_choices; }
	string getChoiceAt(int i) { return m_choices.at(i); }
	int getRightAnswer() { return m_rightAnswer; }
	int getUserAnswer() { return m_userAnswer; }
	string getCode() { return m_code; }


	void setQuestion(string question) { m_question = question; }
	void setChoices(int n, string choice) { m_choices.at(n) = choice; }
	void setRightAnswer(int a) { m_rightAnswer = a; }
	void setUserAnswer(int i) { m_userAnswer = i; }
	void setCode(string code) { m_code = code; }
	void addChoices(string choice) { m_choices.push_back(choice); }

	static vector<Question*> parseFile(string check, BOOL randQuestions);
};
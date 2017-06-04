#pragma once
#include "header.h"
struct ClientInfo
{
	sockaddr_in addr;
	SOCKET socket;
};
class MyDialog
{
public:
	MyDialog(VOID);
public:
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	static MyDialog *ptr;
	HANDLE hEvent;
	//HWND buttonChosen;

	vector<std::pair<HWND, BOOL>> markButtons;
	INT markFirst = 0x8824;
	INT markLast = markFirst;
	//HWND testButton;
	HWND hButton;
private:
	//окно
	HWND hDialog;
	//элементы второго окна
	HWND hListBox, hQuestionNumber, groupBox, hQuestion, hTimeLeft, groupBox1, hPercents;
	//элементы второго окна
	HWND hRadios[4], hEditForRadios[4], buttons[5];
	//элементы  первого окна
	HWND hName, hSurname, hFatherName, hGroup, hLogin;
	//Все вопросы и ответы
	vector<Question*> quiz;

	BOOL bconnected, bsubmitted, blogged, blistShown, brandQuestions;
	WSADATA wsd;
	ClientInfo clientInfo;

	HANDLE hConnectThread, hReceiveThread, hStatsThread;

	INT m_countUserAnswered;
	INT horizontal, vertical;
	INT windowMinWidth, winowMinHeight;
	//Текущий вопрос
	INT m_index;
	//время полученное с сервера
	INT m_itime;
	//строка со временем
	string m_stime;
	//При изменении размера экрана используется.Подумать нужно ли мне это?
	RECT rRect1;
	TCHAR m_name[20], m_surname[20], m_group[20], m_fatherName[20];
	CONST TCHAR m_serverIpAddres[10] = "127.0.0.1";
public:
	VOID setQuiz(vector<Question*> q) { quiz = q; }
	VOID setQuizTime(INT m) { m_itime = m; }
	VOID setConnected(BOOL b) { bconnected = b; }
	VOID setSubmitted(BOOL b) { bsubmitted = b; }
	VOID setRandQuestions(BOOL b) { brandQuestions = b; }

	CONST TCHAR* getServerIp() { return m_serverIpAddres; }
	//Номер текущего вопроса
	INT getIndex() { return m_index; }
	INT getCountUserAnswered() { return m_countUserAnswered; }
	INT getQuizTime() { return m_itime; }
	TCHAR* getName() { return m_name; }
	TCHAR* getSurname() { return m_surname; }
	TCHAR* getFatherName() { return m_fatherName; }
	TCHAR* getGroup() { return m_group; }
	HWND& getListBox() { return hListBox; }
	HWND& gethDialogue() { return hDialog; }
	HWND& gethLogin() { return hLogin; }
	HANDLE& gethConnect() { return hConnectThread; }
	ClientInfo& getClientInfo() { return clientInfo; }
	WSADATA& getWSD() { return wsd; }
	vector<Question*> getQuiz() { return quiz; }

	BOOL isConnected() { return bconnected; }
	BOOL isSubmitted() { return bsubmitted; }
	BOOL isLogged() { return blogged; }
	BOOL isRandQuestions() { return brandQuestions; }

	VOID increaseCountUserAnswered() { m_countUserAnswered++; }
	VOID decreaseCountUserAnswered() { m_countUserAnswered--; }

	INT checkResult();
	VOID disconnectMsg();
	VOID disconnect();
private:
	VOID createAllElements(HWND hwnd);
	VOID changePercents();
	VOID changedQuestion();
	VOID toggleListBox();
	VOID submitTest();
	VOID countCurrentAnswer();

	VOID Cls_OnSize(HWND hwnd, UINT State, INT cx, INT cy);
	VOID Cls_OnTimer(HWND hwnd, UINT id);
	VOID Cls_OnCommand(HWND hwnd, INT id, HWND hwndCtl, UINT codeNotify);
	VOID Cls_OnClose(HWND hwnd);
	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
};
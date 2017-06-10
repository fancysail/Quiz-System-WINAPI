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
	HWND& gethTimeLeft() { return hTimeLeft; }
	void setm_stime(char *s) { m_stime = s; }
	void setsubmitted(bool b) { bsubmitted = b; }
	string& getm_stime() { return m_stime; }
	VOID submitTest();
	INT& getSeconds() { return seconds; }
	INT& getm_minutes() { return m_minutes; }
	HWND* gethRadios() { return hRadios; }
	HWND* gethButtons() { return buttons; }
public:
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	static MyDialog *ptr;
	HANDLE hEvent;

	vector<std::pair<HWND, BOOL>> markButtons;
	INT markFirst = 0x8824;
	INT markLast = markFirst;
private:
	HWND hDialog;
	//second window
	HWND hListBox, hQuestionNumber, groupBox, hQuestion, hTimeLeft, groupBox1, hPercents;
	//second window
	HWND hRadios[4], hEditForRadios[4], buttons[5];
	//first window
	HWND hName, hSurname, hFatherName, hGroup, hLogin;
	vector<Question*> quiz;

	BOOL bconnected, bsubmitted, blogged, blistShown, brandQuestions;
	WSADATA wsd;
	ClientInfo clientInfo;

	HANDLE hConnectThread, hReceiveThread, hStatsThread;

	INT m_countUserAnswered;
	INT horizontal, vertical;
	INT windowMinWidth = 383;
	INT winowMinHeight = 252;
	//current question
	INT m_index;
	//time from server
	static INT m_minutes;
	INT m_minutesCopy;
	static INT seconds;
	//time string
	string m_stime;
	TCHAR m_name[20], m_surname[20], m_group[20], m_fatherName[20];
	CONST TCHAR m_serverIpAddres[10] = "127.0.0.1";
public:
	VOID setQuiz(vector<Question*> q) { quiz = q; }
	VOID setQuizTime(INT m) { m_minutes = m; m_minutesCopy = m; }
	VOID setConnected(BOOL b) { bconnected = b; }
	VOID setSubmitted(BOOL b) { bsubmitted = b; }
	VOID setRandQuestions(BOOL b) { brandQuestions = b; }

	CONST TCHAR* getServerIp() { return m_serverIpAddres; }
	//current question number
	INT getIndex() { return m_index; }
	INT getCountUserAnswered() { return m_countUserAnswered; }
	INT getQuizTime() { return m_minutesCopy; }
	INT getSpentTime() { return m_minutes; }
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
	VOID countCurrentAnswer();

	VOID Cls_OnCommand(HWND hwnd, INT id, HWND hwndCtl, UINT codeNotify);
	VOID Cls_OnClose(HWND hwnd);
	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
};
#pragma once
#include "header.h"
#include "UserListView.h"
#include "Database.h"
#include <vector>
//#include "Quiz.h"
#include "headerForQuizCreator.h"
class MyDialog;
class Quiz
{
public:
	MyDialog*ptrMyDialog;
	HWND hCreateQuiz,hChooseQuiz,hQuestion, hAnswer, hChoice2, hChoice3, hChoice4, hAdd, hPlus, hDialog;

	void MessageAboutError(DWORD dwError);
	BOOL Cls_OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT CodeNotify);
	void Cls_OnClose(HWND hWnd);
	string m_path;

	void AddQuestion();
	void addQuiz(char* quizName);
	void enableAdd();
	//void updateQuizCombo(HWND& h);
public:
	static Quiz* ptr;
	
	Quiz();
	Quiz(MyDialog*p);
	~Quiz(void);
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
};

class MyDialog
{
public:
	MyDialog(void);
	~MyDialog(void);

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	static MyDialog*ptr;
	VOID MessageAboutError(DWORD dwError);

	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	VOID Cls_OnClose(HWND hwnd);

	MYSQL_RES* rset;
	MYSQL_ROW row;
	stringstream ssql;
	string sql;
	std::vector<UserListView*> userListView;
private:
	BOOL bStarted = FALSE;

	TCHAR m_group[40];
	TCHAR m_name[20];
	TCHAR m_surname[20];
	TCHAR m_password[20];
	TCHAR m_time[8];

	ifstream ifs;
	string fileData;

	INT horizontal, vertical;
	INT windowWidth, winowHeight;

	HWND hDialog;
	HWND hsGroup, hsTest, hsTime;    //НЕ ИСПОЛЬЗУЮТСЯ!Могут понадобиться, если буду растягивать окно.Можно использовать GetDlgItem и идентификаторы, а HWND удалить
	HWND hGroupCombo, hBrowse, hPath, hTime, hStart, hUserList; //вводимые поля нового окна
	HWND hName, hSurname, hPassword, hLogin;//вводимые поля старого окна
	HWND hCheckBox1;

	Database& db = Database::Instance();

	HANDLE hDbconnect;//HANDLE нити в которой создается подключение к ДБ
	WSADATA wsd;
public:
	TCHAR* getName() { return m_name; }
	TCHAR* getSurname() { return m_surname; }
	TCHAR* getPassword() { return m_password; }
	TCHAR* getGroup() { return m_group; }
	TCHAR* getTime() { return m_time; }

	HWND& gethPath() { return hPath; }
	HWND& gethDialogue() { return hDialog; }
	HWND& gethLogin() { return hLogin; }
	HWND& gethStart() { return hStart; }
	WSADATA& getWSD() { return wsd; }
	Database& getDB() { return db; }
	string getFileData() { return fileData; }
	HWND& getCheckBox1() { return hCheckBox1; }
	HWND& getUserList() { return hUserList; }
	HWND& gethGroupCombo() { return hGroupCombo; }
	HWND& gethTime() { return hTime; }

	BOOL isStarted() { return bStarted; }
	VOID UpdateUserList(char* ListItem);
private:
	VOID createAllElements();
	BOOL loginCheck();
};

struct ClientInfo
{
	sockaddr_in addr;
	SOCKET socket;
};
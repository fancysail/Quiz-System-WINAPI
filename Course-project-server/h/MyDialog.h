#pragma once
#include "header.h"
#include "UserListView.h"
#include "Database.h"
#include <vector>

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

	TCHAR m_path[60];TCHAR m_group[40];
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
	TCHAR* getTime() { return m_time; }
	HWND& gethDialogue() { return hDialog; }
	HWND& gethStart() { return hStart; }
	WSADATA& getWSD() { return wsd; }
	Database& getDB() { return db; }
	string getFileData() { return fileData; }
	HWND& getCheckBox1() { return hCheckBox1; }
	HWND& getUserList() { return hUserList; }
	VOID UpdateUserList(char* ListItem);
private:
	VOID OnBrowse();
	VOID rightExtension(char* dot);
	VOID checkLogin();
	VOID createAllElements();
	VOID checkAllFields();
	BOOL loginCheck();
};

struct ClientInfo
{
	sockaddr_in addr;
	SOCKET socket;
};
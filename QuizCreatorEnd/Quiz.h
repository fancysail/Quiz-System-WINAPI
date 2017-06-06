#pragma once
#include "header.h"
class Quiz
{
public:
	HWND hQuestion, hPath, hAnswer, hChoice2, hChoice3, hChoice4, hAdd, hDialog;

	void MessageAboutError(DWORD dwError);
	BOOL Cls_OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT CodeNotify);
	void Cls_OnClose(HWND hWnd);
	string m_path;

	void rightExtension(char* dot);
	void OnBrowse();
	void AddQuestion();
	void enableAdd();
public:
	static Quiz* ptr;
	Quiz(void);
	~Quiz(void);
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
};
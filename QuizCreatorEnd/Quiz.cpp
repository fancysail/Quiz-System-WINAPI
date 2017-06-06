#include "Quiz.h"

Quiz* Quiz::ptr = NULL;

void Quiz::Cls_OnClose(HWND hWnd) { EndDialog(hWnd, NULL); }
void Quiz::enableAdd() {
	int lP = SendMessage(hPath, WM_GETTEXTLENGTH, 0, 0);
	int lQ = SendMessage(hQuestion, WM_GETTEXTLENGTH, 0, 0);
	int lA = SendMessage(hAnswer, WM_GETTEXTLENGTH, 0, 0);
	int lC2 =SendMessage(hChoice2, WM_GETTEXTLENGTH, 0, 0);
	int lC3 = SendMessage(hChoice3, WM_GETTEXTLENGTH, 0, 0);
	int lC4 = SendMessage(hChoice4, WM_GETTEXTLENGTH, 0, 0);
	if (lP != 0 && lQ != 0 && lA != 0 && lC2 != 0 && lC3 != 0 && lC4 != 0) 
		EnableWindow(hAdd,TRUE);
	else EnableWindow(hAdd, FALSE);
}
void Quiz::rightExtension(char* dot) {
	int len = m_path.size();
	dot[0] = m_path[len - 4];
	dot[1] = m_path[len - 3];
	dot[2] = m_path[len - 2];
	dot[3] = m_path[len - 1];
	dot[4] = '\0';
}
void Quiz::MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;
	TCHAR szBuf[80];
	BOOL fOK = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM//флаг сообщает функции, что нужна строка, соответствующая коду ошибки, определённому в системе
		| FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL,//нужно выделить соответствующий блок памяти для хранения текста
		dwError,//код ошибки
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//язык, на котором выводится описание ошибки (язык пользователя по умолчанию)
		(LPTSTR)&lpMsgBuf,//адрес выделенного блока памяти записывается в эту переменную
		0,//минимальный размер буфера для выделения памяти
		NULL);
	if (lpMsgBuf != NULL)
	{
		wsprintf(szBuf, TEXT("Ошибка %d: %s"), dwError, lpMsgBuf);
		MessageBox(hDialog, szBuf, TEXT("Сообщение об ошибке"), MB_OK | MB_ICONSTOP);
		LocalFree(lpMsgBuf);
	}
}
Quiz::Quiz(void)
{
	ptr = this;
}
Quiz::~Quiz(void)
{
	ptr = nullptr;
}
BOOL Quiz::Cls_OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
	hDialog = hWnd;
	hQuestion = GetDlgItem(hWnd, IDC_QUESTION);
	hPath = GetDlgItem(hWnd, IDC_FILE_PATH);
	hAnswer = GetDlgItem(hWnd, IDC_ANSWER);
	hChoice2 = GetDlgItem(hWnd, IDC_CHOICE2);
	hChoice3 = GetDlgItem(hWnd, IDC_CHOICE3);
	hChoice4 = GetDlgItem(hWnd, IDC_CHOICE4);
	hAdd = GetDlgItem(hWnd, IDC_ADD);

	return TRUE;
}
void Quiz::OnBrowse()
{
	BROWSEINFO binfo = { 0 };
	binfo.hwndOwner = hDialog;
	binfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_BROWSEINCLUDEFILES;
	binfo.lpszTitle = "Выбор файла или папки...";
	LPITEMIDLIST ptr = SHBrowseForFolder(&binfo);
	if (ptr)
	{
		char path[MAX_PATH];
		SHGetPathFromIDList(ptr, path);
		if (strlen(path) > 59) {
			m_path.clear();
			throw std::runtime_error("Слишком длинный путь к файлу.\nДлина имени файла больше 59 символов.");
		}
		if (strlen(path)>4) {
			m_path = path;
			//MessageBox(hDialog, m_path, "Выбор файла или папки", MB_OK | MB_ICONINFORMATION);
		}
	}
}
void Quiz::Cls_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT CodeNotify)
{
	switch (id) {
		case IDC_FILE_PATH:
		case IDC_QUESTION:
		case IDC_ANSWER:
		case IDC_CHOICE2:
		case IDC_CHOICE3:
		case IDC_CHOICE4: {
			enableAdd();
		}
						  break;
		case IDC_ADD:
			AddQuestion();
			break;
		case IDC_BROWSE:
		{
			try {
				ptr->OnBrowse();
			}
			catch (const std::exception& ex) {
				SetWindowText(hPath, "");
				MessageBox(hDialog, TEXT(ex.what()), "Ошибка", MB_ICONERROR);
				enableAdd();
				return;
			}

			int len = m_path.size();
			char dot[5];
			rightExtension(dot);
			string format;
			format += "Неверное расширение файла ";
			if (!strcmp(dot, ".txt")) {
				SetWindowText(hPath, m_path.c_str());
			}
			else {
				format += dot;
				MessageBox(hDialog, TEXT(format.c_str()), "Ошибка", MB_ICONERROR);
				SetWindowText(hPath, "");
			}
			enableAdd();
		}
	}
}


INT_PTR CALLBACK Quiz::DlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		HANDLE_MSG(hWnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hWnd, WM_COMMAND, ptr->Cls_OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, ptr->Cls_OnClose);
	}

	return FALSE;
}
void writeToFile(HWND hwnd,TCHAR help[], std::string buff,ofstream &out,char* pre,char* after) {
	GetWindowText(hwnd, help, 200);
	std::string toCode;
	toCode += pre;
	toCode += help;
	toCode += after;

	out.write(toCode.c_str(), toCode.size());
	out.flush();
}
void Quiz::AddQuestion() {//~q  q~   ~!  !~   ~^  ^~
						  //Получаем путь к файлу
	TCHAR text[200];
	TCHAR help[200];
	std::string buff;

	GetWindowText(hPath, text, 200);

	//Добавляем в файл форматированный вопрос
	std::ofstream out(text, std::ios_base::app | std::ios_base::out);

	writeToFile(hQuestion, help, buff, out,"~q","q~");
	writeToFile(hChoice2, help, buff, out, "~!", "!~");
	writeToFile(hChoice3, help, buff, out, "~!", "!~");
	writeToFile(hChoice4, help, buff, out, "~!", "!~");
	writeToFile(hAnswer, help, buff,out, "~^", "^~");

	SendMessageA(hQuestion, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hAnswer, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice2, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice3, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice4, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hAnswer, WM_SETTEXT, WPARAM(0), LPARAM(""));
}
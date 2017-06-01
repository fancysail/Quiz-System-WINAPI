#include "MyDialog.h"
#define IDC_BROWSE 0x8809
#define IDC_PATH 0x8810
#define IDC_GROUPCOMBO 0x8811
#define IDC_TIME 0x8812
#define IDC_START 0x8813
#define IDC_USERLIST 0x8814
#define IDC_LISTVIEW 0x8815
#define IDC_CHECKBOX1 0x8816
#define IDC_CHECKBOX2 0x8817

//Обслуживание очередного запроса будем выполнять в отдельном потоке
DWORD WINAPI ThreadForReceive(LPVOID lpParam);
//Ожидать запросы на соединение будем в отдельном потоке
DWORD WINAPI ThreadForAccept(LPVOID lpParam);
//Получение информации от пользователя
VOID receiveUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo &clientinfo);
/**
Checks database if user is valid

@param(clientInfo) Socket reference
@return Returns true if user is valid,FALSE if not.
*/
BOOL checkUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo & clientinfo);


MyDialog* MyDialog::ptr = NULL;
BOOL CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, TRUE);
	return TRUE;
}

BOOL InitListViewColumns(HWND hWndListView)
{
	TCHAR szText[256];     // Temporary buffer.
	LVCOLUMN lvc;
	int iCol;

	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text,
	// and subitem members of the structure are valid.
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	// Add the columns.
	for (iCol = 0; iCol < /*C_COLUMNS*/3; iCol++)
	{
		lvc.iSubItem = iCol;
		lvc.pszText = szText;
		switch (iCol) {
		case 0:lvc.cx = 140; break;
		case 1:lvc.cx = 130; break;
		case 2:lvc.cx = 85; break;
		}


		if (iCol < 2)
			lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
		else
			lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

									// Load the names of the column headings from the string resources.
		LoadString(NULL,
			IDS_STRINGNAME + iCol,
			szText,
			sizeof(szText) / sizeof(szText[0]));

		// Insert the columns into the list view.
		if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
			return FALSE;
	}

	return TRUE;
}
////////////Еще не трогал
BOOL InsertListViewItems(HWND hWndListView, int cItems)
{
	LVITEM lvI;

	// Initialize LVITEM members that are common to all items.
	lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	// Initialize LVITEM members that are different for each item.
	for (int index = 0; index < cItems; index++)
	{
		lvI.iItem = index;
		lvI.iImage = index;

		// Insert items into the list.
		if (ListView_InsertItem(hWndListView, &lvI) == -1)
			return FALSE;
	}

	return TRUE;
}
//void HandleWM_NOTIFY(LPARAM lParam)
//{
//	NMLVDISPINFO* plvdi;
//
//	switch (((LPNMHDR)lParam)->code)
//	{
//	case LVN_GETDISPINFO:
//
//		plvdi = (NMLVDISPINFO*)lParam;
//
//		switch (plvdi->item.iSubItem)
//		{
//		case 0:
//			plvdi->item.pszText = rgPetInfo[plvdi->item.iItem].szKind;
//			break;
//
//		case 1:
//			plvdi->item.pszText = rgPetInfo[plvdi->item.iItem].szBreed;
//			break;
//
//		case 2:
//			plvdi->item.pszText = rgPetInfo[plvdi->item.iItem].szPrice;
//			break;
//
//		default:
//			break;
//		}
//
//		break;
//
//	}
//	// NOTE: In addition to setting pszText to point to the item text, you could 
//	// copy the item text into pszText using StringCchCopy. For example:
//	//
//	// StringCchCopy(plvdi->item.pszText, 
//	//                         plvdi->item.cchTextMax, 
//	//                         rgPetInfo[plvdi->item.iItem].szKind);
//
//	return;
//}

///////////////////////////
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
BOOL MyDialog::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCHAR GUID[] = TEXT("{D99CD3E0-670D-4def-9B74-99FD7E793DFB}");
	HANDLE hMutex = CreateMutex(NULL, FALSE, GUID);
	DWORD dwAnswer = WaitForSingleObject(hMutex, 0);
	if (dwAnswer == WAIT_TIMEOUT)
	{
		MessageBox(hwnd, TEXT("Программа уже запущена!"), TEXT("Admin Quiz"), MB_OK | MB_ICONINFORMATION);
		/*HWND window = FindWindow("#32770", "Admin Quiz");
		ShowWindow(window, SW_RESTORE);
		SetForegroundWindow(window);
		ShowWindow(window, SW_NORMAL);
		SendMessage(window, WM_SYSCOMMAND, SC_MAXIMIZE, NULL);*/
		EndDialog(hwnd, 0);
		
	}
	GetDesktopResolution(horizontal, vertical);

	RECT rect;
	GetWindowRect(hwnd, &rect);
	int widthRect = rect.right - rect.left;
	int heightRect = rect.bottom - rect.top;
	windowWidth = widthRect;
	winowHeight = heightRect;
	MoveWindow(hwnd, (horizontal - (horizontal / 2) - widthRect / 2), (vertical - (vertical / 2) - heightRect / 2), widthRect, heightRect, TRUE);
	hDialog = hwnd;

	hName = GetDlgItem(hDialog, IDC_NAME);
	hSurname = GetDlgItem(hDialog, IDC_SURNAME);
	hPassword = GetDlgItem(hDialog, IDC_PASSWORD);
	hLogin = GetDlgItem(hDialog, IDC_LOGIN);

	wsprintf(m_path, "");
	wsprintf(m_group, "");
	wsprintf(m_name, "");
	wsprintf(m_surname, "");
	wsprintf(m_password, "");
	wsprintf(m_time, "");
	return TRUE;
}
VOID MyDialog::Cls_OnClose(HWND hwnd)
{
	WSACleanup();
	//Разорвать соединение с БД
	EndDialog(hwnd, 0);
}
VOID MyDialog::MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;
	char szBuf[300];
	//Функция FormatMessage форматирует строку сообщения
	BOOL fOK = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM /* флаг сообщает функции, что нужна строка, соответствующая коду ошибки, определённому в системе */
		| FORMAT_MESSAGE_ALLOCATE_BUFFER, //нужно выделить соответствующий блок памяти для хранения текста
		NULL, //указатель на строку, содержащую текст сообщения
		dwError, //код ошибки
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // язык, на котором выводится описание ошибки (язык пользователя по умолчанию)
		(LPTSTR)&lpMsgBuf, //указатель на буфер, в который запишется текст сообщения
		0, //минимальный размер буфера для выделения памяти - память выделяет система
		NULL //список аргументов форматирования
	);
	if (lpMsgBuf != NULL)
	{
		wsprintf(szBuf, "Ошибка %d: %s", dwError, lpMsgBuf);
		MessageBox(hDialog, szBuf, "Сообщение об ошибке", MB_OK | MB_ICONSTOP);
		if (strcmp(szBuf, "Ошибка 0: Операция успешно завершена.\r\n")) {//чтобы не мешало, когда пользователь отсоединяется.
		}
		LocalFree(lpMsgBuf); //освобождаем память, выделенную системой
	}
}

BOOL CALLBACK DestoryChildCallback(HWND hwnd, LPARAM lParam) {
	if (hwnd != NULL) {
		if (hwnd != MyDialog::ptr->gethDialogue()) {
			DestroyWindow(hwnd);
		}
	}
	return TRUE;
}
VOID SetView(HWND hWndListView, DWORD dwView)
{
	// Retrieve the current window style. 
	DWORD dwStyle = GetWindowLong(hWndListView, GWL_STYLE);

	// Set the window style only if the view bits changed.
	if ((dwStyle & LVS_TYPEMASK) != dwView)
	{
		SetWindowLong(hWndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	}
}
HWND CreateListView(int x, int y, int width, int height, HWND *hwndParent)
{
	INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	//RECT rcClient;                       // The parent window's client area.

	//GetClientRect(hwndParent, &rcClient);

	// Create the list-view window in report view with label editing enabled.
	HWND hWndListView = CreateWindow(WC_LISTVIEW,
		"",
		WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE | WS_BORDER,
		x, y,
		width,
		height,
		*hwndParent,
		(HMENU)IDC_LISTVIEW,
		NULL,
		NULL);

	return (hWndListView);
}
HWND& createCombo(int x, int y, int width, int height, HWND* parent, int id) {
	HWND combo = CreateWindowEx(WS_EX_STATICEDGE, "COMBOBOX", "Default String", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, x, y, width, height, *parent, (HMENU)id, 0, 0);
	return combo;
}
HWND& createEdit(int x, int y, int width, int height, HWND* parent, int id) {
	HWND editControl = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", nullptr, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY,
		x, y, width, height, *parent, (HMENU)id, nullptr, 0);
	return editControl;
}
HWND& createStatic(const char* text, int x, int y, int width, int height, HWND*parent, int style) {
	HWND hStatic = CreateWindowEx(0, TEXT("STATIC"), TEXT(text), WS_CHILD | WS_VISIBLE |
		WS_EX_CLIENTEDGE | style /*| WS_BORDER*/, /*ptr->windowWidth*/x, /*ptr->winowHeight*/y, width, height, *parent, 0, nullptr, 0);
	return hStatic;//SS_CENTER
}

HWND& createButton(const char* text, int x, int y, int width, int height, HWND* parent, int id, int style) {
	HWND hButton = CreateWindowEx(0, TEXT("Button"), TEXT(text), WS_CHILD | WS_VISIBLE | WS_GROUP | style,
		x, y, width, height, *parent, (HMENU)id, GetModuleHandle(NULL), 0);
	return hButton;
}
HWND& createCheckBox(const char* text, int x, int y, int width, int height, HWND* parent, int id) {
	HWND hCkeckBox = CreateWindowEx(0, TEXT("Button"), TEXT(text), WS_CHILD | WS_VISIBLE | WS_GROUP | BS_CHECKBOX | WS_TABSTOP | BS_MULTILINE | BS_AUTOCHECKBOX,
		x, y, width, height, *parent, (HMENU)id, GetModuleHandle(NULL), 0);
	return hCkeckBox;
}
VOID MyDialog::OnBrowse()
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
			wsprintf(MyDialog::ptr->m_path, TEXT(""));
			throw std::runtime_error("Слишком длинный путь к файлу.\nДлина имени файла больше 59 символов.");
		}
		if (strlen(path)>2) {
			wsprintf(MyDialog::ptr->m_path, TEXT(""));
			wsprintf(MyDialog::ptr->m_path, TEXT("%s"), path);
		}
	}
}
VOID MyDialog::rightExtension(char* dot) {
	int len = lstrlen(MyDialog::ptr->m_path);
	dot[0] = MyDialog::ptr->m_path[len - 4];
	dot[1] = MyDialog::ptr->m_path[len - 3];
	dot[2] = MyDialog::ptr->m_path[len - 2];
	dot[3] = MyDialog::ptr->m_path[len - 1];
	dot[4] = '\0';
}
VOID MyDialog::checkLogin() {
	if (lstrlen(MyDialog::ptr->m_name) > 2 && lstrlen(MyDialog::ptr->m_surname) > 2 && lstrlen(MyDialog::ptr->m_password) > 3)
		EnableWindow(MyDialog::ptr->hLogin, TRUE);
	else
		EnableWindow(MyDialog::ptr->hLogin, FALSE);
}
VOID MyDialog::checkAllFields() {
	if (bStarted)
		return;
	int nIndex = SendMessage(hGroupCombo, CB_GETCURSEL, 0, 0);
	if (nIndex == CB_ERR) {
		EnableWindow(MyDialog::ptr->hStart, FALSE);
		return;
	}
	nIndex = SendMessage(hTime, CB_GETCURSEL, 0, 0);
	if (nIndex == CB_ERR) {
		EnableWindow(MyDialog::ptr->hStart, FALSE);
		return;
	}

	GetWindowText(MyDialog::ptr->hTime, MyDialog::ptr->m_time, 8);

	if (lstrlen(m_path) > 3 && lstrlen(m_time) > 0)
		EnableWindow(MyDialog::ptr->hStart, TRUE);
	else
		EnableWindow(MyDialog::ptr->hStart, FALSE);

	//TEXT str[40];
	//SendMessage(hGroupCombo, CB_GETLBTEXT/*CB_GETLBTEXTLEN*/, nIndex, (LPARAM)s.c_str());

}
VOID MyDialog::createAllElements() {
	hsGroup = createStatic("Группа: ", 30, 25, 50, 16, &ptr->hDialog, NULL);
	hsTest = createStatic("Тест: ", 30, 65, 50, 16, &ptr->hDialog, NULL);
	hsTime = createStatic("Время: ", 30, 105, 50, 16, &ptr->hDialog, NULL);
	hsTest = createStatic("мин. ", 160, 105, 50, 22, &ptr->hDialog, NULL);

	hPath = createEdit(170, 66, 220, 22, &ptr->hDialog, IDC_PATH);//сделать -30 от ширины полной

	hBrowse = createButton("Обзор..", 100, 66, 62, 22, &ptr->hDialog, IDC_BROWSE, NULL);
	hStart = createButton("Start Quiz", 160, 390, 80, 22, &ptr->hDialog, IDC_START, WS_DISABLED);

	hGroupCombo = createCombo(100, 25, 290, 100, &ptr->hDialog, IDC_GROUPCOMBO);
	hTime = createCombo(100, 102, 50, 110, &ptr->hDialog, IDC_TIME);
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"1");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"15");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"30");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"45");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"60");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"120");
	SendMessage(hTime, CB_SETCURSEL, 0, NULL);

	//hUserList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), nullptr,
	//	WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_DISABLENOSCROLL, 30, 143, 355, 241, ptr->hDialog, (HMENU)IDC_USERLIST, NULL, NULL);//180 355
	//ShowWindow(hUserList, FALSE);
	hUserList = CreateListView(30, 143, 355, 241, &ptr->hDialog);
	InitListViewColumns(hUserList);
	//SetWindowTheme(hUserList, (LPCWSTR)"Explorer", NULL);
	//Полученные из базы пользователи (группа)(groupStudents - вектор строк) и в цикле добавляем их в hGroupCombo   
	/*for (int i = 0; i < groupStudents.size(); i++) {
	SendMessage(hGroupCombo, CB_ADDSTRING, 0, (LPARAM)groupStudents[i].c_str());
	}*/
	
	hCheckBox1 = createCheckBox("Randomize Questions", 205, 110, 140, 22, &ptr->hDialog, IDC_CHECKBOX1);

	EnumChildWindows(hDialog, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
}
BOOL MyDialog::loginCheck() {
	WaitForSingleObject(hDbconnect, INFINITE);//на всякий случай жду соединения с базой.Скорее всего уже буду подключен к ней на этот момент.
											  //////Узнать ещё какие группы есть у преподователя
	ssql << "select Name,Password,teacher.Group from teacher where name='";
	ssql << m_name << " " << m_surname << "' AND Password = '";
	ssql << m_password << "';";
	sql = ssql.str();
	ssql.str("");
	if (!mysql_query(db.getConnection(), sql.c_str())) {
		rset = mysql_use_result(db.getConnection());
		if ((row = mysql_fetch_row(rset)) == NULL) {
			MessageBox(hDialog, "Неверный логин или пароль!", "Ошибка", MB_ICONEXCLAMATION);
			EnableWindow(hLogin, FALSE);
			row = mysql_fetch_row(rset);
			return FALSE;
		}
		//while ((row = mysql_fetch_row(rset)) != NULL) {
		//	foundResult = TRUE;
		//}
		//if (!foundResult) {
		//	//Неправильный логин или пароль, дать ещё попытку все ввести
		//	
		//}
	}
	mysql_free_result(rset);
	return TRUE;
}


DWORD WINAPI DBconnect(LPVOID lpParam) {
	if (MyDialog::ptr->getDB().connect()) {
		mysql_query(MyDialog::ptr->getDB().getConnection(), "SET NAMES CP1251");
		//MessageBox(MyDialog::ptr->hDialog, "Подсоединился к БД", "YES", MB_OK);
		return 1;
	}
	else {
		MessageBox(MyDialog::ptr->gethDialogue(), "Нет соединения с БД", "Ошибка", MB_ICONERROR);
		//Исключение и выход
		exit(1);
		return 0;
	}
}

MyDialog::MyDialog(void)
{
	ptr = this;
	wsprintf(m_name, "");
	wsprintf(m_surname, "");
	wsprintf(m_password, "");

	hDbconnect = CreateThread(0, 0, DBconnect, nullptr, 0, 0);
	CloseHandle(hDbconnect);
}
MyDialog::~MyDialog(void)
{

}

VOID receiveUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo &clientinfo) {
	char szBuff[100];
	int result = recv(clientinfo.socket, szBuff, 100, 0);
	if (!strcmp(szBuff, "<CHECK_STUDENT>")) {
		strcpy_s(szBuff, "<OK>");
		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);

		result = recv(clientinfo.socket, szBuff, 100, 0);
		name = szBuff;
		strcpy_s(szBuff, "<OK>");
		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);

		result = recv(clientinfo.socket, szBuff, 100, 0);
		surname = szBuff;
		strcpy_s(szBuff, "<OK>");
		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);

		result = recv(clientinfo.socket, szBuff, 100, 0);
		fatherName = szBuff;
		strcpy_s(szBuff, "<OK>");
		send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);

		result = recv(clientinfo.socket, szBuff, 100, 0);
		group = szBuff;
	}
}
BOOL checkUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo & clientinfo) {
	char szBuff[100];
	MyDialog::ptr->ssql << "select Name,student.Group,student.Date from student where name='";
	MyDialog::ptr->ssql << surname << " " << name << " " << fatherName << "'";
	MyDialog::ptr->ssql << "AND student.Group='" << group << "';";
	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
	MyDialog::ptr->ssql.str("");
	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());

		if ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) == NULL) {
			strcpy(szBuff, "<DOESNT_EXIST>");
			send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
			shutdown(clientinfo.socket, SD_BOTH); // SD_BOTH запрещает как прием, так и отправку данных
			closesocket(clientinfo.socket);
		}
		else {
			//ПОДКЛЮЧИЛСЯ УЧЕНИК ВСЕ ХОРОШО 
			//Проверить сдавал ли он уже тест
			if (MyDialog::ptr->row[2] != NULL) {
				////Значит уже Сдавал тест
				strcpy(szBuff, "<ALREADY_PASSED>");
				send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
				shutdown(clientinfo.socket, SD_BOTH); // SD_BOTH запрещает как прием, так и отправку данных
				closesocket(clientinfo.socket);
			}
			else {
				strcpy_s(szBuff, "<OK>");
				send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
				MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
				mysql_free_result(MyDialog::ptr->rset);
				return TRUE;
			}
		}

	}
	MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
	mysql_free_result(MyDialog::ptr->rset);
	return FALSE;
}

DWORD WINAPI ThreadForReceive(LPVOID lpParam)
{
	ClientInfo* ptrinfo = (ClientInfo*)lpParam;
	char szBuff[4096];
	while (TRUE)
	{
		int result = recv(ptrinfo->socket, szBuff, 4096, 0);
		if (result == SOCKET_ERROR || result == 0)
		{
			MyDialog::ptr->MessageAboutError(WSAGetLastError());
			shutdown(ptrinfo->socket, SD_BOTH);
			closesocket(ptrinfo->socket);
			return 0;
		}
		if (!strcmp(szBuff, "<QUIZ_REQUEST>")) {
			send(ptrinfo->socket, MyDialog::ptr->getFileData().c_str(), strlen(MyDialog::ptr->getFileData().c_str()) + 1, 0);
			result = recv(ptrinfo->socket, szBuff, 4096, 0);
			if (!strcmp(szBuff, "<QUIZ_RECEIVED>")) {
				strcpy_s(szBuff, MyDialog::ptr->getTime());
				send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
				result = recv(ptrinfo->socket, szBuff, 4096, 0);
				if (!strcmp(szBuff, "<TIME_RECEIVED>")) {
					strcpy_s(szBuff, "<RANDOMIZE_QUESTIONS>");
					send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
					result = recv(ptrinfo->socket, szBuff, 4096, 0);
					if (!strcmp(szBuff, "<YES>")) {
						LRESULT lResult1 = SendMessage(MyDialog::ptr->getCheckBox1(), BM_GETCHECK, 0, 0);
						if (lResult1 == BST_CHECKED) {
							strcpy_s(szBuff, "<CHECKED>");
							send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
						}
						else {
							strcpy_s(szBuff, "<NOT_CHECKED>");
							send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
						}
						result = recv(ptrinfo->socket, szBuff, 4096, 0);
						if (!strcmp(szBuff, "<YES>")) {
							strcpy_s(szBuff, "<OK>");
							send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
							while (TRUE) {
								result = recv(ptrinfo->socket, szBuff, 4096, 0);
								if (!strcmp(szBuff, "<SUCCESSFUL_PERCENTS>")) {
									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);

									result = recv(ptrinfo->socket, szBuff, 4096, 0);
									//в double конвертирую а затем в таблицу необходимо поместить
									double percents = atof(szBuff);
									string percentsToTable = to_string(percents);
									percentsToTable += "%";
									///////////????
									//////////////////////////////////
									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
								}
								else if (!strcmp(szBuff, "<EXIT>")) {
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
									break;
								}
							}
						}
					}
				}
			}
		}
		break;
	}
	shutdown(ptrinfo->socket, SD_BOTH);
	closesocket(ptrinfo->socket);
	return 0;
}
DWORD WINAPI ThreadForAccept(LPVOID lpParam)
{
	SOCKET s = socket(AF_INET /*The Internet Protocol version 4 (IPv4) address family*/,
		SOCK_STREAM /*The type specification for the new socket*/,
		IPPROTO_TCP /*Protocol to be used with the socket that is specific to the indicated address family*/);
	// The socket function creates a socket that is bound to a specific transport service provider.
	if (s == SOCKET_ERROR)
	{
		MyDialog::ptr->MessageAboutError(WSAGetLastError());
		WSACleanup();
		EnableWindow(GetDlgItem(MyDialog::ptr->gethDialogue(), IDC_LOGIN), TRUE);
		return 0;
	}
	sockaddr_in local;
	local.sin_addr.S_un.S_addr = INADDR_ANY; // IP address.
	local.sin_family = AF_INET; // Address family (must be AF_INET).
	local.sin_port = htons(49152); // IP port (1 - 1023 - зарезервированы, HTTP - 80, FTP - 21) 
								   // The htons function returns the value in TCP/IP network byte order.

								   //После создания сокета определенного протокола следует связать его с IP-интерфейсом и портом.
	int res = bind(s /*сокет, на котором ожидаются соединения клиентов*/,
		(sockaddr*)&local /*адрес структуры sockaddr_in*/,
		sizeof(local) /*размер структуры sockaddr_in*/);
	//The bind function associates a local address with a socket.
	if (res == SOCKET_ERROR)
	{
		MyDialog::ptr->MessageAboutError(WSAGetLastError());
		closesocket(s); // The closesocket function closes an existing socket.
		WSACleanup();
		EnableWindow(GetDlgItem(MyDialog::ptr->gethDialogue(), IDC_LOGIN), TRUE);
		return 0;
	}
	//Для перевода сокета в состояние ожидания входящих соединений используется API-функция listen
	res = listen(s /*связанный сокет*/, 10 /*максимальную длину очереди соединений*/);
	// This function places a socket at a state where it is listening for an incoming connection.
	if (res == SOCKET_ERROR)
	{
		MyDialog::ptr->MessageAboutError(WSAGetLastError());
		closesocket(s); // The closesocket function closes an existing socket.
		WSACleanup();
		EnableWindow(GetDlgItem(MyDialog::ptr->gethDialogue(), IDC_LOGIN), TRUE);
		return 0;
	}

	while (TRUE)
	{
		int iAddrSize = sizeof(sockaddr_in);
		/*
		accept принимает соединения клиентов и возвращает новый дескриптор сокета, соответствующий принятому клиентскому соединению
		Для всех последующих операций с этим клиентом должен применяться новый сокет.
		Исходный прослушивающий сокет используется для приема других клиентских соединений и продолжает находиться в режиме прослушивания.
		*/
		ClientInfo clientinfo;
		clientinfo.socket = accept(
			s /*связанный сокет в состоянии прослушивания*/,
			(sockaddr*)&(clientinfo.addr) /* адрес структуры sockaddr_in, содержащей сведения об IP-адресе клиента,*/,
			&iAddrSize /* размер структуры sockaddr */);
		//This function permits an incoming connection attempt on a socket.

		if (clientinfo.socket == INVALID_SOCKET)
		{
			MyDialog::ptr->MessageAboutError(WSAGetLastError());
			break;
		}

		string name, surname, fatherName, group;
		receiveUserData(name, surname, fatherName, group, clientinfo);
		
		if (!checkUserData(name, surname, fatherName, group, clientinfo))
			continue;//Если не прошел валидацию в ThreadForReceive не заходишь
		HANDLE hThread = CreateThread(0, 0, ThreadForReceive, (LPVOID)&clientinfo, 0, 0);
		CloseHandle(hThread);
	}
	closesocket(s);
	WSACleanup();
	EnableWindow(MyDialog::ptr->gethStart(), TRUE);
	return 0;
}

VOID MyDialog::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_LOGIN)//Когда ввел группу и добавил тест файл и нажал логин
	{
		if (!loginCheck())
			return;
		//Удаляю все элементы в окне кроме самого окна
		EnumChildWindows(hDialog, DestoryChildCallback, NULL);
		windowWidth = 430;
		winowHeight = 475;
		MoveWindow(hwnd, (horizontal - (horizontal / 2) - windowWidth / 2), (vertical - (vertical / 2) - winowHeight / 2), windowWidth, winowHeight, TRUE);
		
		//тут создаю новые элементы для окна и присваиваю их к HWND
		createAllElements();

		ssql << "select distinct student.group from student;";
		sql = ssql.str();
		ssql.str("");
		if (!mysql_query(db.getConnection(), sql.c_str())) {
			rset = mysql_use_result(db.getConnection());
			while ((row = mysql_fetch_row(rset)) != NULL) {
				SendMessage(hGroupCombo, CB_ADDSTRING, 0, (LPARAM)row[0]);
			}
			row = mysql_fetch_row(rset);
		}
		mysql_free_result(rset);
	}
	else if (id == IDC_NAME) {
		GetWindowText(MyDialog::ptr->hName, MyDialog::ptr->m_name, 20);
		checkLogin();
	}
	else if (id == IDC_SURNAME) {
		GetWindowText(MyDialog::ptr->hSurname, MyDialog::ptr->m_surname, 20);
		checkLogin();
	}
	else if (id == IDC_PASSWORD) {
		GetWindowText(MyDialog::ptr->hPassword, MyDialog::ptr->m_password, 20);
		checkLogin();
	}
	else if (id == IDC_START) {
		//открывается сокет
		if (WSAStartup(WINSOCK_VERSION ,&getWSD()))
		{
			MessageAboutError(WSAGetLastError());
			return;
		}
		//Файл открываем и считываем
		ifs.open(m_path);
		if (ifs.is_open()) {
			ifs.seekg(0, std::ios::end);
			fileData.reserve(ifs.tellg());
			ifs.seekg(0, std::ios::beg);
			fileData.assign((std::istreambuf_iterator<char>(ifs)),
				std::istreambuf_iterator<char>());
			ifs.close();
		}
		else {
			MessageBox(hDialog, TEXT("Cannot open a file"), "Error", MB_ICONERROR);
		}
		HANDLE hThread = CreateThread(0, 0, ThreadForAccept, nullptr, 0, 0);
		CloseHandle(hThread);

		EnableWindow(hStart, FALSE);
		bStarted = TRUE;
	}
	else if (id == IDC_TIME) {
		if (codeNotify == CBN_SELCHANGE) {
			checkAllFields();
		}
	}
	else if (id == IDC_GROUPCOMBO) {
		if (codeNotify == CBN_SELCHANGE) {
			int ItemIndex = SendMessage((HWND)hGroupCombo, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			TCHAR  ListItem[256];
			(TCHAR)SendMessage((HWND)hGroupCombo, (UINT)CB_GETLBTEXT,
				(WPARAM)ItemIndex, (LPARAM)ListItem);
			checkAllFields();
										/////////////////////////////Научиться работать с ListView
			//ssql << "select name from student where student.group='";
			//ssql << ListItem << "';";
			//sql = ssql.str();
			//ssql.str("");
			//if (!mysql_query(db->getConnection(), sql.c_str())) {
			//	rset = mysql_use_result(db->getConnection());
			//	while ((row = mysql_fetch_row(rset)) != NULL) {
			//		SendMessage(hUserList, CB_ADDSTRING, 0, (LPARAM)row[0]);
			//	}
			//	row = mysql_fetch_row(rset);
			//}
			//mysql_free_result(rset);
			////Забираем имена всех учеников и бросаю их в UsersList
		}
	}
	else if (id == IDC_BROWSE) {
		try {
			ptr->OnBrowse();
		}
		catch (const std::exception& ex) {
			EnableWindow(MyDialog::ptr->hStart, FALSE);
			MessageBox(hDialog, TEXT(ex.what()), "Ошибка", MB_ICONERROR);
			return;
		}
		int len = lstrlen(MyDialog::ptr->m_path);
		if (len == 0)
			return;
		char dot[5];
		MyDialog::ptr->rightExtension(dot);
		if (strcmp(dot, ".txt")) {
			string l = "Неверное расширение файла: ";
			l += dot;
			MessageBox(hDialog, TEXT(l.c_str()), "Ошибка", MB_ICONERROR);
			EnableWindow(MyDialog::ptr->hStart, FALSE);
			SetWindowText(hPath, "");
		}
		else {
			SetWindowText(hPath, m_path);
			checkAllFields();
		}
	}
}

BOOL CALLBACK MyDialog::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	/*case WM_NOTIFY:
		if()
		break;*/
	}
	return FALSE;
}

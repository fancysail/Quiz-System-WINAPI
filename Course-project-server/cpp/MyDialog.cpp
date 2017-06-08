#include "../h/MyDialog.h"
#define IDC_BROWSE 0x8809
#define IDC_PATH 0x8810
#define IDC_GROUPCOMBO 0x8811
#define IDC_TIME 0x8812
#define IDC_START 0x8813
#define IDC_USERLIST 0x8814
#define IDC_LISTVIEW 0x8815
#define IDC_CHECKBOX1 0x8816
#define IDC_CHECKBOX2 0x8817
#define IDC_CREATE_QUIZ 0x8818

struct PARAMS{
	ClientInfo* clientInfo;
	char *name, *surname, *fathername, *group;
};

//Servicing of the next request will be performed in a separate thread
DWORD WINAPI ThreadForReceive(LPVOID lpParam);
//We will wait for connection requests in a separate thread
DWORD WINAPI ThreadForAccept(LPVOID lpParam);
//Getting data from the user
VOID receiveUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo &clientinfo);
/**
Checks database if user is valid

@param(clientInfo) Socket reference
@return Returns true if user is valid,FALSE if not.
*/
BOOL checkUserData(string &name, string &surname, string &fatherName, string &group, ClientInfo & clientinfo);


MyDialog* MyDialog::ptr = NULL;
void updateQuizCombo(HWND& h) {
	SendMessage(h, CB_RESETCONTENT, NULL, NULL);

	MyDialog::ptr->ssql << "select * from quizes;";
	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
	MyDialog::ptr->ssql.str("");
	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
		while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {
			SendMessage(h, CB_ADDSTRING, 0, (LPARAM)MyDialog::ptr->row[1]);
		}
		MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
	}
	mysql_free_result(MyDialog::ptr->rset);
}
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
	for (iCol = 0; iCol < /*C_COLUMNS*/4; iCol++)
	{
		lvc.iSubItem = iCol;
		lvc.pszText = szText;
		switch (iCol) {
		case 0:lvc.cx = 190; break;//190
		case 1:lvc.cx = 50; break;//50
		case 2:lvc.cx = 50; break;//50
		case 3:lvc.cx = 64; break;//50
		}


		if (iCol < 5)
			lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
		else
			lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

									// Load the names of the column headings from the string resources.
		LoadString(NULL,
			IDS_NAME + iCol,
			szText,
			sizeof(szText) / sizeof(szText[0]));

		// Insert the columns into the list view.
		if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
			return FALSE;
	}

	return TRUE;
}
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
void HandleWM_NOTIFY(LPARAM lParam)
{
	NMLVDISPINFO* plvdi;

	switch (((LPNMHDR)lParam)->code)
	{
	case LVN_GETDISPINFO:

		plvdi = (NMLVDISPINFO*)lParam;

		switch (plvdi->item.iSubItem)
		{
		case 0:
			StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, MyDialog::ptr->userListView[plvdi->item.iItem]->getFullName().c_str());
			break;
		case 1:
			switch (MyDialog::ptr->userListView[plvdi->item.iItem]->getStatus()) {
			case Status::CONNECTED:plvdi->item.pszText = "Connected"; break;
			case Status::SUBMITTED:plvdi->item.pszText = "Submitted"; break;
			}
			break;
		case 2: {
			stringstream ss;
			ss << fixed << setprecision(1) << MyDialog::ptr->userListView[plvdi->item.iItem]->getPercents();
			ss << "%";
			std::string message = ss.str();
			StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, message.c_str());
			break;
		}
		case 3:
			StringCchCopy(plvdi->item.pszText, plvdi->item.cchTextMax, MyDialog::ptr->userListView[plvdi->item.iItem]->getTimeSpent().c_str());
			break;

		default:
			break;
		}

		break;

	}
	return;
}

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
		MessageBox(hwnd, TEXT("The program is already running!"), TEXT("Admin Quiz"), MB_OK | MB_ICONINFORMATION);
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
		wsprintf(szBuf, "Error %d: %s", dwError, lpMsgBuf);
		MessageBox(hDialog, szBuf, "Error message", MB_OK | MB_ICONSTOP);
		if (strcmp(szBuf, "Error 0: Operation completed successfully.\r\n")) {//чтобы не мешало, когда пользователь отсоединяется.
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

HWND createListView(int x, int y, int width, int height, HWND *hwndParent)
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
		WS_EX_CLIENTEDGE | style , x, y, width, height, *parent, 0, nullptr, 0);
	return hStatic;
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
VOID rightExtension(TCHAR* m_path,char* dot) {
	int len = lstrlen(m_path);
	dot[0] = m_path[len - 4];
	dot[1] = m_path[len - 3];
	dot[2] = m_path[len - 2];
	dot[3] = m_path[len - 1];
	dot[4] = '\0';
}
//Enable disable login button
VOID toggleLogin() {
	if (lstrlen(MyDialog::ptr->getName()) > 2 && lstrlen(MyDialog::ptr->getSurname()) > 2 && lstrlen(MyDialog::ptr->getPassword()) > 3)
		EnableWindow(MyDialog::ptr->gethLogin(), TRUE);
	else
		EnableWindow(MyDialog::ptr->gethLogin(), FALSE);
}
//Enable disable Start button
VOID toggleStart() {
	if (MyDialog::ptr->isStarted())
		return;
	int nIndex = SendMessage(MyDialog::ptr->gethGroupCombo(), CB_GETCURSEL, 0, 0);
	if (nIndex == CB_ERR) {
		EnableWindow(MyDialog::ptr->gethStart(), FALSE);
		return;
	}
	nIndex = SendMessage(MyDialog::ptr->gethTime(), CB_GETCURSEL, 0, 0);
	if (nIndex == CB_ERR) {
		EnableWindow(MyDialog::ptr->gethStart(), FALSE);
		return;
	}
	nIndex = SendMessage(MyDialog::ptr->gethPath(), CB_GETCURSEL, 0, 0);
	if (nIndex == CB_ERR) {
		EnableWindow(MyDialog::ptr->gethStart(), FALSE);
		return;
	}

	GetWindowText(MyDialog::ptr->gethTime(), MyDialog::ptr->getTime(), 8);

	if (lstrlen(MyDialog::ptr->getTime()) > 0)
		EnableWindow(MyDialog::ptr->gethStart(), TRUE);
	else
		EnableWindow(MyDialog::ptr->gethStart(), FALSE);
}
VOID MyDialog::createAllElements() {
	hStart = createButton("Create Quiz..", windowWidth/2 - 60, 15, 120, 22, &ptr->hDialog, IDC_CREATE_QUIZ,NULL);

	hsGroup = createStatic("Group: ", 30, 25+25, 50, 16, &ptr->hDialog, NULL);
	hsTest = createStatic("Test: ", 30, 65 + 25, 50, 16, &ptr->hDialog, NULL);
	hsTime = createStatic("Time: ", 30, 105 + 25, 50, 16, &ptr->hDialog, NULL);
	hsTest = createStatic("min. ", 160, 105 + 25, 50, 22, &ptr->hDialog, NULL);

	hPath = createCombo(100, 66+25, 290, 100, &ptr->hDialog, IDC_PATH);
	updateQuizCombo(hPath);

	//hBrowse = createButton("Browse..", 100, 66+25, 62, 22, &ptr->hDialog, IDC_BROWSE, NULL);
	hStart = createButton("Start Quiz", 160, 390 + 35, 80, 22, &ptr->hDialog, IDC_START, WS_DISABLED);

	hGroupCombo = createCombo(100, 25+25, 290, 100, &ptr->hDialog, IDC_GROUPCOMBO);
	hTime = createCombo(100, 102 + 25, 50, 110, &ptr->hDialog, IDC_TIME);
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"1");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"15");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"30");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"45");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"60");
	SendMessage(hTime, CB_ADDSTRING, 0, (LPARAM)"120");
	SendMessage(hTime, CB_SETCURSEL, 0, NULL);

	hUserList = createListView(30, 143 + 25, 355, 241, &ptr->hDialog);
	InitListViewColumns(hUserList);
	
	hCheckBox1 = createCheckBox("Randomize Questions", 205, 110 + 25, 140, 22, &ptr->hDialog, IDC_CHECKBOX1);

	EnumChildWindows(hDialog, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
}
BOOL MyDialog::loginCheck() {
	WaitForSingleObject(hDbconnect, INFINITE);
	/*ssql << "select Name,Password,teacher.Group from teacher where name='";
	ssql << m_name << " " << m_surname << "' AND Password = '";
	ssql << m_password << "';";
	sql = ssql.str();
	ssql.str("");
	if (!mysql_query(db.getConnection(), sql.c_str())) {
		rset = mysql_use_result(db.getConnection());
		if ((row = mysql_fetch_row(rset)) == NULL) {
			MessageBox(hDialog, "Wrong input data!", "Error", MB_ICONEXCLAMATION);
			EnableWindow(hLogin, FALSE);
			row = mysql_fetch_row(rset);
			return FALSE;
		}
	}
	mysql_free_result(rset);*/
	return TRUE;
}

DWORD WINAPI DBconnect(LPVOID lpParam) {
	if (MyDialog::ptr->getDB().connect()) {
		mysql_query(MyDialog::ptr->getDB().getConnection(), "SET NAMES CP1251");
		return 1;
	}
	else {
		MessageBox(MyDialog::ptr->gethDialogue(), "No database connection!", "Error", MB_ICONERROR);
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
			//Student connected
			//If passed already
			

			if (MyDialog::ptr->row[2] != NULL) {
				////Значит уже Сдавал тест
				strcpy(szBuff, "<ALREADY_PASSED>");
				send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
				shutdown(clientinfo.socket, SD_BOTH); // SD_BOTH запрещает как прием, так и отправку данных
				closesocket(clientinfo.socket);
			}
			else {
				//If connected from another pc or erased date field in database
				string lookFor = surname;
				lookFor += " ";
				lookFor += name;
				lookFor += " ";
				lookFor += fatherName;
				LVFINDINFO item = { LVFI_STRING, (LPCTSTR)lookFor.c_str() };
				int indexUser = ListView_FindItem(MyDialog::ptr->getUserList(), -1, &item);
				if (MyDialog::ptr->userListView.at(indexUser)->getStatus() == Status::NOT_CONNECTED) {
					strcpy_s(szBuff, "<OK>");
					send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
					MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
					mysql_free_result(MyDialog::ptr->rset);
					return TRUE;
				}
				else {
					strcpy_s(szBuff, "<ALREADY_LOGGED>");
					send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
					MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
					mysql_free_result(MyDialog::ptr->rset);
					return FALSE;
				}
			}
		}

	}
	MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
	mysql_free_result(MyDialog::ptr->rset);
	return FALSE;
}

DWORD WINAPI ThreadForReceive(LPVOID lpParam)
{
	PARAMS *params = (PARAMS*)lpParam;
	ClientInfo* ptrinfo = params->clientInfo;
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
							
							string lookFor = params->surname;
							lookFor += " "; 
							lookFor += params->name;
							lookFor += " ";
							lookFor += params->fathername;
							LVFINDINFO item = { LVFI_STRING, (LPCTSTR)lookFor.c_str()};
							int indexUser = ListView_FindItem(MyDialog::ptr->getUserList(), -1, &item);
							MyDialog::ptr->userListView.at(indexUser)->setStatus(Status::CONNECTED);
							MyDialog::ptr->UpdateUserList(params->group);
							double percents;
							while (TRUE) {
								result = recv(ptrinfo->socket, szBuff, 4096, 0);
								if (!strcmp(szBuff, "<SUCCESSFUL_PERCENTS>")) {
									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);

									result = recv(ptrinfo->socket, szBuff, 4096, 0);
									percents = atof(szBuff);
									MyDialog::ptr->userListView.at(indexUser)->setPercents(percents);
									MyDialog::ptr->UpdateUserList(params->group);

									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
								}
								else if (!strcmp(szBuff, "<EXIT>")) {
									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
									result = recv(ptrinfo->socket, szBuff, 4096, 0);
									//принимаю время прохождения
									MyDialog::ptr->userListView.at(indexUser)->setTimeSpent(szBuff);
									MyDialog::ptr->userListView.at(indexUser)->setStatus(Status::SUBMITTED);

									MyDialog::ptr->ssql << "UPDATE student SET student.Result = '";
									MyDialog::ptr->ssql << percents << "'" << "where id = '";
									MyDialog::ptr->ssql << indexUser + 1 << "'";
									MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
									MyDialog::ptr->ssql.str("");
									mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

									MyDialog::ptr->ssql << "UPDATE student SET student.Date = curdate() where id = '";
									MyDialog::ptr->ssql << indexUser + 1 << "'";
									MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
									MyDialog::ptr->ssql.str("");
									mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

									MyDialog::ptr->ssql << "UPDATE student SET student.Time = '";
									MyDialog::ptr->ssql << szBuff << "'";
									MyDialog::ptr->ssql << "where id = '";
									MyDialog::ptr->ssql << indexUser + 1 << "'";
									MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
									MyDialog::ptr->ssql.str("");
									mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

									MyDialog::ptr->UpdateUserList(params->group);
									strcpy_s(szBuff, "<EXIT>");
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
			continue;//Not to get to the ThreadForRecieve with wrong input data

		PARAMS params;
		params.clientInfo = &clientinfo;
		int len = strlen(name.c_str());
		params.name = new char[len + 1];
		strcpy_s(params.name,len+1, name.c_str());
		len = strlen(surname.c_str());
		params.surname = new char[len + 1];
		strcpy_s(params.surname, len + 1, surname.c_str());
		len = strlen(fatherName.c_str());
		params.fathername = new char[len + 1];
		strcpy_s(params.fathername, len + 1, fatherName.c_str());
		len = strlen(group.c_str());
		params.group = new char[len + 1];
		strcpy_s(params.group, len + 1, group.c_str());

		HANDLE hThread = CreateThread(0, 0, ThreadForReceive, (LPVOID)&params, 0, 0);
		CloseHandle(hThread);
	}
	closesocket(s);
	WSACleanup();
	EnableWindow(MyDialog::ptr->gethStart(), TRUE);
	return 0;
}
VOID MyDialog::UpdateUserList(char* ListItem) {
	ListView_DeleteAllItems(hUserList);
	//userListView.clear();
	if (userListView.size() == 0) {
		ssql << "select name from student where student.group='";
		ssql << ListItem << "';";
		sql = ssql.str();
		ssql.str("");
		if (!mysql_query(MyDialog::ptr->getDB().getConnection(), sql.c_str())) {
			rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
			while ((row = mysql_fetch_row(rset)) != NULL) {
				userListView.push_back(new UserListView(row[0]));
				//SendMessage(hUserList, CB_ADDSTRING, 0, (LPARAM)row[0]);
			}
			row = mysql_fetch_row(rset);
		}
		mysql_free_result(rset);
	}
	InsertListViewItems(hUserList, userListView.size());
}
VOID MyDialog::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_LOGIN)
	{
		if (!loginCheck())
			return;
		//Remove all alements from the window
		EnumChildWindows(hDialog, DestoryChildCallback, NULL);
		windowWidth = 430;
		winowHeight = 500;
		MoveWindow(hwnd, (horizontal - (horizontal / 2) - windowWidth / 2), (vertical - (vertical / 2) - winowHeight / 2), windowWidth, winowHeight, TRUE);
		
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
		toggleLogin();
	}
	else if (id == IDC_SURNAME) {
		GetWindowText(MyDialog::ptr->hSurname, MyDialog::ptr->m_surname, 20);
		toggleLogin();
	}
	else if (id == IDC_PASSWORD) {
		GetWindowText(MyDialog::ptr->hPassword, MyDialog::ptr->m_password, 20);
		toggleLogin();
	}
	else if (id == IDC_START) {
		if (WSAStartup(WINSOCK_VERSION ,&getWSD()))
		{
			MessageAboutError(WSAGetLastError());
			return;
		}
		int ItemIndex = SendMessage((HWND)hPath, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		TCHAR quizName[60];
		SendMessage((HWND)hPath, (UINT)CB_GETLBTEXT,
			(WPARAM)ItemIndex, (LPARAM)quizName);
		ssql << "select question, wrongAnswer2, wrongAnswer3, wrongAnswer4, rightAnswer ";
		ssql << "from questions where quizId = (select id from quizes where name = '";
		ssql << quizName;
		ssql << "');";
		sql = ssql.str();
		ssql.str("");
		if (!mysql_query(MyDialog::ptr->getDB().getConnection(), sql.c_str())) {
			rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
			sql = "";
			while ((row = mysql_fetch_row(rset)) != NULL) {
				ssql << "~q" << row[0] << "q~";
				ssql << "~!" << row[1] << "!~";
				ssql << "~!" << row[2] << "!~";
				ssql << "~!" << row[3] << "!~";
				ssql << "~^" << row[4] << "^~";
				sql += ssql.str();
				ssql.str("");
			}
			row = mysql_fetch_row(rset);
		}
		mysql_free_result(rset);
		fileData = sql;
		HANDLE hThread = CreateThread(0, 0, ThreadForAccept, nullptr, 0, 0);
		CloseHandle(hThread);

		EnableWindow(hStart, FALSE);
		bStarted = TRUE;
		EnableWindow(hGroupCombo, FALSE);
		EnableWindow(hBrowse, FALSE);
		EnableWindow(hCheckBox1, FALSE);
		EnableWindow(hTime, FALSE);
		EnableWindow(hPath, FALSE);
	}
	else if (id == IDC_TIME) {
		if (codeNotify == CBN_SELCHANGE) {
			toggleStart();
		}
	}
	else if (id == IDC_GROUPCOMBO) {
		if (codeNotify == CBN_SELCHANGE) {
			int ItemIndex = SendMessage((HWND)hGroupCombo, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			TCHAR ListItem[256];
			SendMessage((HWND)hGroupCombo, (UINT)CB_GETLBTEXT,
				(WPARAM)ItemIndex, (LPARAM)ListItem);
			int len = 0;
			len = strlen(ListItem);
			char * listIt = new char[len + 1];
			sprintf(listIt, TEXT("%s"), ListItem);
			toggleStart();

			UpdateUserList(ListItem);
		}
	}
	else if (id == IDC_CREATE_QUIZ) {
		Quiz dlg(ptr);
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG3), hwnd, Quiz::DlgProc);
	}
	else if (id == IDC_PATH) {
		if (codeNotify == CBN_SELCHANGE) {
			toggleStart();
		}
	}
	/*else if (id == IDC_BROWSE) {
		try {
			ptr->OnBrowse();
		}
		catch (const std::exception& ex) {
			EnableWindow(MyDialog::ptr->hStart, FALSE);
			MessageBox(hDialog, TEXT(ex.what()), "Error", MB_ICONERROR);
			return;
		}
		int len = lstrlen(MyDialog::ptr->m_path);
		if (len == 0)
			return;
		char dot[5];
		rightExtension(MyDialog::ptr->m_path, dot);
		if (strcmp(dot, ".txt")) {
			string l = "Wrong file extension: ";
			l += dot;
			MessageBox(hDialog, TEXT(l.c_str()), "Error", MB_ICONERROR);
			EnableWindow(MyDialog::ptr->hStart, FALSE);
			SetWindowText(hPath, "");
		}
		else {
			SetWindowText(hPath, m_path);
			toggleStart();
		}
	}*/
}

INT_PTR CALLBACK MyDialog::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
		case WM_NOTIFY:
		HandleWM_NOTIFY(lParam);
		break;
	}
	return FALSE;
}









Quiz* Quiz::ptr = NULL;

void Quiz::Cls_OnClose(HWND hWnd) { EndDialog(hWnd, NULL); }
void Quiz::enableAdd() {
	int ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	int lQ = SendMessage(hQuestion, WM_GETTEXTLENGTH, 0, 0);
	int lA = SendMessage(hAnswer, WM_GETTEXTLENGTH, 0, 0);
	int lC2 = SendMessage(hChoice2, WM_GETTEXTLENGTH, 0, 0);
	int lC3 = SendMessage(hChoice3, WM_GETTEXTLENGTH, 0, 0);
	int lC4 = SendMessage(hChoice4, WM_GETTEXTLENGTH, 0, 0);
	if (ItemIndex != CB_ERR && lQ != 0 && lA != 0 && lC2 != 0 && lC3 != 0 && lC4 != 0)
		EnableWindow(hAdd, TRUE);
	else EnableWindow(hAdd, FALSE);
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
Quiz::Quiz(MyDialog*p) 
{ 
	ptr = this;
	Quiz::ptrMyDialog = p; 
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
	hCreateQuiz = GetDlgItem(hWnd, IDC_CREATEQUIZ);
	hChooseQuiz = GetDlgItem(hWnd, IDC_CHOOSEQUIZ);
	hQuestion = GetDlgItem(hWnd, IDC_QUESTION);
	hAnswer = GetDlgItem(hWnd, IDC_ANSWER);
	hChoice2 = GetDlgItem(hWnd, IDC_CHOICE2);
	hChoice3 = GetDlgItem(hWnd, IDC_CHOICE3);
	hChoice4 = GetDlgItem(hWnd, IDC_CHOICE4);
	hAdd = GetDlgItem(hWnd, IDC_ADD);
	hPlus = GetDlgItem(hWnd, IDC_PLUSBUTTON);


	updateQuizCombo(hChooseQuiz);

	/*HBITMAP hbit = LoadBitmap(GetModuleHandle(NULL), "C:\\Users\\mmari\\Desktop\\Course-work\\Quiz-System-WINAPI\\Course-project-server\\plusBMP.bmp");
	SendMessage(hPlus, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit);*/

	return TRUE;
}
void Quiz::addQuiz(char* quizName) {
	MyDialog::ptr->ssql << "INSERT INTO quizes(name) VALUES('";
	MyDialog::ptr->ssql << quizName << "')";
	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
	MyDialog::ptr->ssql.str("");
	mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());
}
void Quiz::Cls_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT CodeNotify)
{
	switch (id) {
	case IDC_CHOOSEQUIZ:
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
	case IDC_PLUSBUTTON:
		TCHAR quizName[40];
		GetWindowText(hCreateQuiz, quizName, 40);
		addQuiz(quizName);
		updateQuizCombo(MyDialog::ptr->gethPath());
		updateQuizCombo(hChooseQuiz);
		SendMessageA(hCreateQuiz, WM_SETTEXT, WPARAM(0), LPARAM(""));
		break;
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
void Quiz::AddQuestion() {
	int ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	TCHAR ListItem[256];
	SendMessage((HWND)hChooseQuiz, (UINT)CB_GETLBTEXT,
		(WPARAM)ItemIndex, (LPARAM)ListItem);

	char help[200];
	ptrMyDialog->ssql << "insert into questions ";
	GetWindowText(hQuestion, help, 200);
	ptrMyDialog->ssql << "SET questions.question = '" << help << "',";
	GetWindowText(hAnswer, help, 200);
	ptrMyDialog->ssql << "questions.rightAnswer = '" << help << "',";
	GetWindowText(hChoice2, help, 200);
	ptrMyDialog->ssql << "questions.wrongAnswer2 = '" << help << "',";
	GetWindowText(hChoice3, help, 200);
	ptrMyDialog->ssql << "questions.wrongAnswer3 = '" << help << "',";
	GetWindowText(hChoice4, help, 200);
	ptrMyDialog->ssql << "questions.wrongAnswer4 = '" << help << "',";
	ptrMyDialog->ssql << "questions.quizId = (select id from quizes WHERE quizes.name = '" << ListItem << "');";

	ptrMyDialog->sql = ptrMyDialog->ssql.str();
	ptrMyDialog->ssql.str("");
	mysql_query(ptrMyDialog->getDB().getConnection(), ptrMyDialog->sql.c_str());
	
	SendMessageA(hQuestion, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hAnswer, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice2, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice3, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice4, WM_SETTEXT, WPARAM(0), LPARAM(""));
}
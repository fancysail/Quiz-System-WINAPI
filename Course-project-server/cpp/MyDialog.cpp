#include "../h/MyDialog.h"
#define IDC_PATH 0x8810
#define IDC_GROUPCOMBO 0x8811
#define IDC_TIME 0x8812
#define IDC_START 0x8813
#define IDC_USERLIST 0x8814
#define IDC_LISTVIEW 0x8815
#define IDC_CHECKBOX1 0x8816
#define IDC_CHECKBOX2 0x8817
#define IDC_CREATE_QUIZ 0x8818
#define IDC_PLUSBUTTON 0x8819
#define IDC_MINUSBUTTON 0x8820
#define IDC_REMOVE 0x8821


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
/**
Reading whole file at once

@param(memblock) result string
*/
void getCode(char*& memblock, string fileName) {
	int size;
	std::ifstream input(fileName, ios::in | ios::binary | ios::ate);
	size = input.tellg();
	memblock = new char[size + 1];
	input.seekg(0, ios::beg);
	input.read(memblock, size);
	memblock[size] = '\0';
	input.close();
}


MyDialog* MyDialog::ptr = NULL;
void updateQuizCombo(HWND& h) {
	if (Quiz::ptr !=nullptr && Quiz::ptr->hChooseQuiz == h) {
		Quiz::ptr->m_quizRelation.clear();
	}
	SendMessage(h, CB_RESETCONTENT, NULL, NULL);

	MyDialog::ptr->ssql << "select * from quizes;";
	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
	MyDialog::ptr->ssql.str("");
	INT counter = 0;
	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
		while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {
			if (Quiz::ptr != nullptr && Quiz::ptr->hChooseQuiz == h) {
				Quiz::ptr->m_quizRelation.insert_or_assign(counter++, atoi(MyDialog::ptr->row[0]));
			}
			SendMessage(h, CB_ADDSTRING, 0, (LPARAM)MyDialog::ptr->row[1]);
		}
		MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
	}
	mysql_free_result(MyDialog::ptr->rset);
}
void updateQuestionsCombo(HWND& h,INT quizId) {
	Quiz::ptr->m_questionRelation.clear();
	SendMessage(h, CB_RESETCONTENT, NULL, NULL);

	MyDialog::ptr->ssql << "select * from questions where quizId = '"; 
	MyDialog::ptr->ssql << quizId << "';";
	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
	MyDialog::ptr->ssql.str("");
	INT counter = 0;
	string buff;
	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
		MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
		while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {
			Quiz::ptr->m_questionRelation.insert_or_assign(counter, atoi(MyDialog::ptr->row[0]));
			buff = to_string(++counter);
			buff += ".";
			buff += MyDialog::ptr->row[2];
			SendMessage(h, CB_ADDSTRING, 0, (LPARAM)buff.c_str());
			buff.clear();
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
			case Status::CONNECTED:plvdi->item.pszText = "Присоединился"; break;
			case Status::SUBMITTED:plvdi->item.pszText = "Сдал"; break;
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
	HICON hIcon1 = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));
	SendMessage(hwnd, WM_SETICON, 1, (LPARAM)hIcon1);

	TCHAR GUID[] = TEXT("{D99CD3E0-670D-4def-9B74-99FD7E793DFB}");
	HANDLE hMutex = CreateMutex(NULL, FALSE, GUID);
	DWORD dwAnswer = WaitForSingleObject(hMutex, 0);
	if (dwAnswer == WAIT_TIMEOUT)
	{
		MessageBox(hwnd, TEXT("Программа уже запущена!"), TEXT("Admin Quiz"), MB_OK | MB_ICONINFORMATION);
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
VOID MessageAboutError(DWORD dwError,HWND& hDialog)
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
	hStart = createButton("Создать тест..", windowWidth/2 - 60, 15, 120, 22, &ptr->hDialog, IDC_CREATE_QUIZ,NULL);

	createStatic("Группа: ", 30, 25+25, 50, 16, &ptr->hDialog, NULL);
	createStatic("Тест: ", 30, 65 + 25, 50, 16, &ptr->hDialog, NULL);
	createStatic("Время: ", 30, 105 + 25, 50, 16, &ptr->hDialog, NULL);
	createStatic("мин. ", 160, 105 + 25, 50, 22, &ptr->hDialog, NULL);

	hPath = createCombo(100, 66+25, 290, 100, &ptr->hDialog, IDC_PATH);
	updateQuizCombo(hPath);

	hStart = createButton("Старт", 160, 390 + 35, 80, 22, &ptr->hDialog, IDC_START, WS_DISABLED);

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
	
	hCheckBox1 = createCheckBox("Случайный порядок", 205, 110 + 25, 140, 22, &ptr->hDialog, IDC_CHECKBOX1);

	EnumChildWindows(hDialog, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
}
BOOL MyDialog::loginCheck() {
	WaitForSingleObject(hDbconnect, INFINITE);
	ssql << "select Name,Password from teacher where name='";
	ssql << m_name << " " << m_surname << "' AND Password = '";
	ssql << m_password << "';";
	sql = ssql.str();
	ssql.str("");
	if (!mysql_query(db.getConnection(), sql.c_str())) {
		rset = mysql_use_result(db.getConnection());
		if ((row = mysql_fetch_row(rset)) == NULL) {
			MessageBox(hDialog, "Неправильные данные!", "Ошибка", MB_ICONEXCLAMATION);
			EnableWindow(hLogin, FALSE);
			row = mysql_fetch_row(rset);
			return FALSE;
		}
	}
	mysql_free_result(rset);
	return TRUE;
}
DWORD WINAPI DBconnect(LPVOID lpParam) {
	if (MyDialog::ptr->getDB().connect()) {
		mysql_query(MyDialog::ptr->getDB().getConnection(), "SET NAMES CP1251");
		return 1;
	}
	else {
		MessageBox(MyDialog::ptr->gethDialogue(), "Нет соединения с БД!", "Error", MB_ICONERROR);
		exit(1);
	}
	return 0;
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
	INT studId = -1;
	INT groupId = -1;
	MyDialog::ptr->ssql << "select student.id,student.Name,student.groupId from student where name='";
	MyDialog::ptr->ssql << surname << " " << name << " " << fatherName << "' and student.groupId = (select groups.id from groups where groups.name='";
	MyDialog::ptr->ssql << group << "');";
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
			studId = atoi(MyDialog::ptr->row[0]);
			groupId = atoi(MyDialog::ptr->row[2]);
			mysql_free_result(MyDialog::ptr->rset);


			MyDialog::ptr->ssql << "select groups.name from groups where groups.id = '" << groupId << "';";
			MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
			MyDialog::ptr->ssql.str("");
			if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {//If a student from the group chosen by the teacher;
				MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
				if ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {

					if (!strcmp(MyDialog::ptr->getGroup(), MyDialog::ptr->row[0])) {
						MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
						mysql_free_result(MyDialog::ptr->rset);

						MyDialog::ptr->ssql << "select results.date from results where results.idStudent = '" << studId << "' ";
						MyDialog::ptr->ssql << "AND results.idQuiz = '" << MyDialog::ptr->quizID << "';";
						MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
						MyDialog::ptr->ssql.str("");

						if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
							MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());

							if ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) == NULL) {//Never passed no record in results table yet
								MyDialog::ptr->ssql << "INSERT INTO results(results.idQuiz,results.idStudent) VALUES('";
								MyDialog::ptr->ssql << MyDialog::ptr->quizID << "','" << studId << "');";
								MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
								MyDialog::ptr->ssql.str("");
								mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

								strcpy_s(szBuff, "<OK>");
								send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
								MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
								mysql_free_result(MyDialog::ptr->rset);
								return TRUE;
							}
							else if (MyDialog::ptr->row[0] != NULL) {//Record exists, and Date field is filled
								strcpy(szBuff, "<ALREADY_PASSED>");
								send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
								shutdown(clientinfo.socket, SD_BOTH); // SD_BOTH запрещает как прием, так и отправку данных
								closesocket(clientinfo.socket);
							}
							else {//Record exists, and Date field is empty
								  //If connected from another pc or erased date field in database
								string lookFor = surname;
								lookFor += " ";
								lookFor += name;
								lookFor += " ";
								lookFor += fatherName;
								LVFINDINFO item = { LVFI_STRING, (LPCTSTR)lookFor.c_str() };
								int indexUser = ListView_FindItem(MyDialog::ptr->getUserList(), -1, &item);

								if (MyDialog::ptr->userListView.at(indexUser)->getStatus() == Status::NOT_CONNECTED) {//Retake
									strcpy_s(szBuff, "<OK>");
									send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
									MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
									mysql_free_result(MyDialog::ptr->rset);
									return TRUE;
								}
								else {//Taking right now
									strcpy_s(szBuff, "<ALREADY_LOGGED>");
									send(clientinfo.socket, szBuff, strlen(szBuff) + 1, 0);
									MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
									mysql_free_result(MyDialog::ptr->rset);
									return FALSE;
								}
							}
						}

					}
					else {
						MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
						mysql_free_result(MyDialog::ptr->rset);
						return FALSE;
					}
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
			MessageAboutError(WSAGetLastError(), MyDialog::ptr->gethDialogue());
			shutdown(ptrinfo->socket, SD_BOTH);
			closesocket(ptrinfo->socket);
			return 0;
		}
		if (!strcmp(szBuff, "<QUIZ_REQUEST>")) {
			const char* file = MyDialog::ptr->getFileData().c_str();//8192
			send(ptrinfo->socket, file, strlen(file) + 1, 0);
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
									if (MyDialog::ptr->userListView.at(indexUser)->getPercents() != percents) {
										MyDialog::ptr->userListView.at(indexUser)->setPercents(percents);
										MyDialog::ptr->UpdateUserList(params->group);
									}
									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
								}
								else if (!strcmp(szBuff, "<EXIT>")) {
									strcpy_s(szBuff, "<OK>");
									send(ptrinfo->socket, szBuff, strlen(szBuff) + 1, 0);
									result = recv(ptrinfo->socket, szBuff, 4096, 0);
									//receiving spent time
									MyDialog::ptr->userListView.at(indexUser)->setTimeSpent(szBuff);
									MyDialog::ptr->userListView.at(indexUser)->setStatus(Status::SUBMITTED);

									MyDialog::ptr->ssql << "UPDATE results SET results.score = '";
									MyDialog::ptr->ssql << percents << "'" << "where results.idStudent = '";
									MyDialog::ptr->ssql << indexUser + 1 << "' ";
									MyDialog::ptr->ssql << "AND results.idQuiz = '";
									MyDialog::ptr->ssql << MyDialog::ptr->quizID << "';";
									MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
									MyDialog::ptr->ssql.str("");
									mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

									MyDialog::ptr->ssql << "UPDATE results SET results.date = curdate() ";
									MyDialog::ptr->ssql << "where results.idStudent = '";
									MyDialog::ptr->ssql << indexUser + 1 << "' ";
									MyDialog::ptr->ssql << "AND results.idQuiz = '";
									MyDialog::ptr->ssql << MyDialog::ptr->quizID << "';";
									MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
									MyDialog::ptr->ssql.str("");
									mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

									MyDialog::ptr->ssql << "UPDATE results SET results.Time = '";
									MyDialog::ptr->ssql << szBuff << "'";
									MyDialog::ptr->ssql << "where results.idStudent = '";
									MyDialog::ptr->ssql << indexUser + 1 << "' ";
									MyDialog::ptr->ssql << "AND results.idQuiz = '";
									MyDialog::ptr->ssql << MyDialog::ptr->quizID << "';";
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
		MessageAboutError(WSAGetLastError(), MyDialog::ptr->gethDialogue());
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
		MessageAboutError(WSAGetLastError(), MyDialog::ptr->gethDialogue());
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
		MessageAboutError(WSAGetLastError(), MyDialog::ptr->gethDialogue());
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
			MessageAboutError(WSAGetLastError(), MyDialog::ptr->gethDialogue());
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
	if (!MyDialog::ptr->isStarted()) {
		userListView.clear();
	}
	ssql << "select student.name from student where student.groupid=(select groups.id from groups where groups.name='";
	ssql << ListItem << "');";
	sql = ssql.str();
	ssql.str("");
	if (!mysql_query(MyDialog::ptr->getDB().getConnection(), sql.c_str())) {
		rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
		while ((row = mysql_fetch_row(rset)) != NULL) {
			if (!MyDialog::ptr->isStarted()) {
				userListView.push_back(new UserListView(row[0]));
			}
		}
		row = mysql_fetch_row(rset);
	}
	mysql_free_result(rset);
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

		ssql << "select distinct groups.name from groups where groups.teacherId = (";
		ssql<<"select teacher.id from teacher where teacher.name = '";
		ssql << m_name << " " << m_surname << "');";
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
			MessageAboutError(WSAGetLastError(), MyDialog::ptr->gethDialogue());
			return;
		}
		int ItemIndex = SendMessage((HWND)hPath, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		TCHAR quizName[60];
		SendMessage((HWND)hPath, (UINT)CB_GETLBTEXT,
			(WPARAM)ItemIndex, (LPARAM)quizName);
		ssql << "select question, wrongAnswer2, wrongAnswer3, wrongAnswer4, rightAnswer, code ";
		ssql << "from questions where quizId = (select id from quizes where name = '";
		ssql << quizName;
		ssql << "');";
		sql = ssql.str();
		ssql.str("");
		if (!mysql_query(MyDialog::ptr->getDB().getConnection(), sql.c_str())) {
			rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
			sql = "";
			while ((row = mysql_fetch_row(rset)) != NULL) {
				ssql << "'/~q/'" << row[0] << "'/q~/'";
				ssql << "'/~!/'" << row[1] << "'/!~/'";
				ssql << "'/~!/'" << row[2] << "'/!~/'";
				ssql << "'/~!/'" << row[3] << "'/!~/'";
				ssql << "'/~^/'" << row[4] << "'/^~/'";
				if (row[5] == NULL) {
					ssql << "'/~#/'" << "'/#~/'";
				}
				else {
					ssql << "'/~#/'" << row[5] << "'/#~/'";
				}
				sql += ssql.str();
				ssql.str("");
			}
			row = mysql_fetch_row(rset);
		}
		fileData = sql;
		mysql_free_result(rset);

		ssql << "select quizes.id from quizes where quizes.name = '";
		ssql << quizName << "'" << ";";
		sql = ssql.str();
		ssql.str("");
		if (!mysql_query(MyDialog::ptr->getDB().getConnection(), sql.c_str())) {
			rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
			while ((row = mysql_fetch_row(rset)) != NULL) {
				quizID = atoi(row[0]);
			}
			row = mysql_fetch_row(rset);
		}
		mysql_free_result(rset);

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
			wsprintf(m_group, ListItem);
		}
	}
	else if (id == IDC_CREATE_QUIZ) {
		Quiz dlg(ptr);
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG3), hwnd, Quiz::DlgProc);
		if (!MyDialog::ptr->isStarted()) {
			updateQuizCombo(MyDialog::ptr->hPath);
		}
		EnableWindow(ptr->hStart, FALSE);
	}
	else if (id == IDC_PATH) {
		if (codeNotify == CBN_SELCHANGE) {
			toggleStart();
		}
	}
}
bool Quiz::OnBrowse()
{
	BROWSEINFO binfo = { 0 };
	binfo.hwndOwner = hDialog;
	binfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_BROWSEINCLUDEFILES;
	binfo.lpszTitle = "";
	LPITEMIDLIST ptr = SHBrowseForFolder(&binfo);
	if (ptr)
	{
		char path[MAX_PATH];
		SHGetPathFromIDList(ptr, path);
		if (strlen(path) > 59) {
			Quiz::ptr->m_codeFileName.clear();
			throw std::runtime_error("");
		}
		if (strlen(path)>4) {
			Quiz::ptr->m_codeFileName = path;
			return true;
		}
	}
	else return false;
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
	if (ItemIndex != CB_ERR && lQ != 0 && lA != 0 && lC2 != 0 && lC3 != 0 && lC4 != 0) {
		EnableWindow(hAdd, TRUE);
		int ItemIndex = SendMessage((HWND)hDBquestions, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		if (ItemIndex != CB_ERR) {
			EnableWindow(hUpdate, TRUE);
		}
	}
	else {
		EnableWindow(hUpdate, FALSE);
		EnableWindow(hAdd, FALSE);
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
	hUpdate = GetDlgItem(hWnd, IDC_UPDATE);
	hDelete = GetDlgItem(hWnd, IDC_DELETE);
	hDBquestions = GetDlgItem(hWnd, IDC_DBQUESTIONS);
	hCodeFileName = GetDlgItem(hWnd, IDC_CODENAME);
	
	hPlus = CreateWindow("STATIC", NULL, SS_BITMAP | SS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP , 389, 13, 45, 45, hWnd, (HMENU)IDC_PLUSBUTTON, GetModuleHandle(NULL), NULL);
	HBITMAP hBmp1 = (HBITMAP)LoadImage(NULL, "PLUS.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp1 == NULL) {
		MessageBox(NULL, "Error while loading image", "Error", MB_OK | MB_ICONERROR);
	}
	SendMessage(Quiz::ptr->hPlus, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp1);

	hMinus = CreateWindow("STATIC", NULL, SS_BITMAP | SS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 389, 52, 45, 45, hWnd, (HMENU)IDC_MINUSBUTTON, GetModuleHandle(NULL), NULL);
	HBITMAP hBmp2 = (HBITMAP)LoadImage(NULL, "MINUS.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp2 == NULL) {
		MessageBox(NULL, "Error while loading image", "Error", MB_OK | MB_ICONERROR);
	}
	SendMessage(Quiz::ptr->hMinus, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp2);

	hRemove = CreateWindow("STATIC", NULL, SS_BITMAP | SS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 150, 352, 45, 45, hWnd, (HMENU)IDC_REMOVE, GetModuleHandle(NULL), NULL);
	HBITMAP hBmp3 = (HBITMAP)LoadImage(NULL, "removeICO.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp3 == NULL) {
		MessageBox(NULL, "Error while loading image", "Error", MB_OK | MB_ICONERROR);
	}
	SendMessage(Quiz::ptr->hRemove, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp3);
	ShowWindow(Quiz::ptr->hRemove, SW_HIDE);

	updateQuizCombo(hChooseQuiz);
	return TRUE;
}
void Quiz::addQuiz(const char* quizName) {
	MyDialog::ptr->ssql << "INSERT INTO quizes(name) VALUES('";
	MyDialog::ptr->ssql << quizName << "')";
	MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
	MyDialog::ptr->ssql.str("");
	mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());
}
string findFileName(string s) {
	string s1 = s;
	reverse(s1.begin(), s1.end());
	int i = 0;
	for (; i < s1.size(); i++) {
		if (s1[i] == '\\') {
			break;
		}
	}
	s1 = s1.substr(0, i);
	reverse(s1.begin(), s1.end());
	return s1;
}
void Quiz::Cls_OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT CodeNotify)
{
	switch (id) {
	case IDC_CHOOSEQUIZ:
	{
		if (CodeNotify == CBN_SELCHANGE) {
			int ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);

			if (Quiz::ptr->m_quizRelation.size() > 0) {
				updateQuestionsCombo(Quiz::ptr->hDBquestions, Quiz::ptr->m_quizRelation.at(ItemIndex));
			}
			EnableWindow(hUpdate, FALSE);
			EnableWindow(hDelete, FALSE);
		}
	}
	break;
	case IDC_DBQUESTIONS:
	{
		if (CodeNotify == CBN_SELCHANGE) {
			EnableWindow(Quiz::ptr->hDelete, TRUE);
			int ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);

			MyDialog::ptr->ssql << "select * from questions where quizId = '";
			MyDialog::ptr->ssql << Quiz::ptr->m_quizRelation.at(ItemIndex) << "' AND id = '";
			ItemIndex = SendMessage((HWND)hDBquestions, (UINT)CB_GETCURSEL,
				(WPARAM)0, (LPARAM)0);
			MyDialog::ptr->ssql << Quiz::ptr->m_questionRelation.at(ItemIndex) << "';";
			MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
			MyDialog::ptr->ssql.str("");
			INT counter = 0;
			string buff;
			if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
				MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
				while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {
					SetWindowText(Quiz::ptr->hQuestion, MyDialog::ptr->row[2]);
					SetWindowText(Quiz::ptr->hAnswer, MyDialog::ptr->row[3]);
					SetWindowText(Quiz::ptr->hChoice2, MyDialog::ptr->row[4]);
					SetWindowText(Quiz::ptr->hChoice3, MyDialog::ptr->row[5]);
					SetWindowText(Quiz::ptr->hChoice4, MyDialog::ptr->row[6]);
				}
				MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
			}
			mysql_free_result(MyDialog::ptr->rset);
			if (ItemIndex != CB_ERR) {
				EnableWindow(Quiz::ptr->hDelete, TRUE);
			}
			else {
				EnableWindow(Quiz::ptr->hDelete, FALSE);
			}
		}
	}
	break;
	case IDC_QUESTION:
	case IDC_ANSWER:
	case IDC_CHOICE2:
	case IDC_CHOICE3:
	case IDC_CHOICE4: {
		enableAdd();
	}
					  break;
	case IDC_DELETE:
	{
		int ItemIndex = SendMessage((HWND)hDBquestions, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		string buff;
		TCHAR buffHelp[120];
		SendMessage((HWND)hDBquestions, (UINT)CB_GETLBTEXT,
			(WPARAM)ItemIndex, (LPARAM)buffHelp);
		string trim = buffHelp;
		trim = trim.substr(trim.find(".") + 1, trim.length());
		buff += "\t\tУдалить этот вопрос?\n";
		buff += "Вопрос: ";
		buff += trim;
		buff += "\n";
		GetWindowText(hAnswer, buffHelp, 120);
		buff += "Ответ: ";
		buff += buffHelp;
		buff += "\n";
		GetWindowText(hChoice2, buffHelp, 120);
		buff += "Неверный ответ 1: ";
		buff += buffHelp;
		buff += "\n";
		GetWindowText(hChoice3, buffHelp, 120);
		buff += "Неверный ответ 2: ";
		buff += buffHelp;
		buff += "\n";
		GetWindowText(hChoice4, buffHelp, 120);
		buff += "Неверный ответ 3: ";
		buff += buffHelp;
		INT answer = MessageBox(Quiz::ptr->hDialog, buff.c_str(), "Quiz Creator", MB_YESNO);
		if (answer == IDYES) {
			if (Quiz::ptr->m_quizRelation.size() > 0) {
				MyDialog::ptr->ssql << "DELETE from questions where id = '";
				MyDialog::ptr->ssql << m_questionRelation.at(ItemIndex) << "'";
				ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
					(WPARAM)0, (LPARAM)0);
				MyDialog::ptr->ssql << "AND quizId ='" << m_quizRelation.at(ItemIndex) << "';";
				MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
				MyDialog::ptr->ssql.str("");
				mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());
				updateQuestionsCombo(Quiz::ptr->hDBquestions, Quiz::ptr->m_quizRelation.at(ItemIndex));
				SetWindowText(hQuestion, "");
				SetWindowText(hAnswer, "");
				SetWindowText(hChoice2, "");
				SetWindowText(hChoice3, "");
				SetWindowText(hChoice4, "");
				EnableWindow(hDelete, FALSE);
			}
		}
	}
	break;
	case IDC_UPDATE:
	{
		INT quizId = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		string buff = "\t\tИзменить этот вопрос\n";
		MyDialog::ptr->ssql << "select * from questions where quizId = '";
		MyDialog::ptr->ssql << Quiz::ptr->m_quizRelation.at(quizId) << "' AND id = '";
		INT questionId = SendMessage((HWND)hDBquestions, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		MyDialog::ptr->ssql << Quiz::ptr->m_questionRelation.at(questionId) << "';";
		MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
		MyDialog::ptr->ssql.str("");

		if (!mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str())) {
			MyDialog::ptr->rset = mysql_use_result(MyDialog::ptr->getDB().getConnection());
			while ((MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset)) != NULL) {
				buff += "Вопрос: ";
				buff += MyDialog::ptr->row[2];
				buff += "\n";
				buff += "Ответ: ";
				buff += MyDialog::ptr->row[3];
				buff += "\n";
				buff += "Неверный ответ 1: ";
				buff += MyDialog::ptr->row[4];
				buff += "\n";
				buff += "Неверный ответ 2: ";
				buff += MyDialog::ptr->row[5];
				buff += "\n";
				buff += "Неверный ответ 3: ";
				buff += MyDialog::ptr->row[6];
				buff += "\n";
				//Получаю здесь все варианты ответов 
			}
			MyDialog::ptr->row = mysql_fetch_row(MyDialog::ptr->rset);
		}
		mysql_free_result(MyDialog::ptr->rset);


		TCHAR hQ[120];
		TCHAR hA[80];
		TCHAR hC1[80];
		TCHAR hC2[80];
		TCHAR hC3[80];
		buff += "\n\t\tНа этот\n";
		GetWindowText(hQuestion, hQ, 120);
		buff += "Вопрос: ";
		buff += hQ;
		buff += "\n";
		GetWindowText(hAnswer, hA, 80);
		buff += "Ответ: ";
		buff += hA;
		buff += "\n";
		GetWindowText(hChoice2, hC1, 80);
		buff += "Неверный ответ 1: ";
		buff += hC1;
		buff += "\n";
		GetWindowText(hChoice3, hC2, 80);
		buff += "Неверный ответ 2: ";
		buff += hC2;
		buff += "\n";
		GetWindowText(hChoice4, hC3, 80);
		buff += "Неверный ответ 3: ";
		buff += hC3;
		if (!m_codeFileName.empty()) {
			buff += "\nКод: ";
			buff += m_codeFileName;
		}


		INT answer = MessageBox(Quiz::ptr->hDialog, buff.c_str(), "Quiz Creator", MB_YESNO);
		if (answer == IDYES) {
			if (Quiz::ptr->m_quizRelation.size() > 0) {
				MyDialog::ptr->ssql << "UPDATE questions SET question = '" << hQ << "', ";
				MyDialog::ptr->ssql << "rightAnswer = '" << hA << "', ";
				MyDialog::ptr->ssql << "wrongAnswer2 = '" << hC1 << "', ";
				MyDialog::ptr->ssql << "wrongAnswer3 = '" << hC2 << "', ";
				MyDialog::ptr->ssql << "wrongAnswer4 = '" << hC3 << "' ";
				if (!m_codeFileName.empty()) {
					MyDialog::ptr->ssql << ",";
					char * memblock;
					getCode(memblock, m_codeFileName);
					ptrMyDialog->ssql << "questions.code = '" << memblock << "'";
					delete[] memblock;
					SendMessageA(hCodeFileName, WM_SETTEXT, WPARAM(0), LPARAM(""));
					m_codeFileName.clear();
					ShowWindow(hRemove, SW_HIDE);
				}
				MyDialog::ptr->ssql << "where quizid = '" << m_quizRelation.at(quizId) << "' AND id = '" << m_questionRelation.at(questionId) << "';";
				MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
				MyDialog::ptr->ssql.str("");
				mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());
				updateQuestionsCombo(Quiz::ptr->hDBquestions, Quiz::ptr->m_quizRelation.at(quizId));
				SetWindowText(hQuestion, "");
				SetWindowText(hAnswer, "");
				SetWindowText(hChoice2, "");
				SetWindowText(hChoice3, "");
				SetWindowText(hChoice4, "");
				EnableWindow(hUpdate, FALSE);
			}
		}
	}
	break;
	case IDC_ADD:
		AddQuestion();
		break;
	case IDC_MINUSBUTTON:
	{
		int ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
			(WPARAM)0, (LPARAM)0);
		if (ItemIndex == CB_ERR) {
			break;
		}
		TCHAR ListItem[256];
		SendMessage((HWND)hChooseQuiz, (UINT)CB_GETLBTEXT,
			(WPARAM)ItemIndex, (LPARAM)ListItem);
		string buff = "Удалить тест: \"";
		buff += ListItem;
		buff += "\"?\nРезультаты студентов также будут удалены!";

		INT answer = MessageBox(Quiz::ptr->hDialog, buff.c_str(), "Quiz Creator", MB_YESNO);
		if (answer == IDYES) {
			if (Quiz::ptr->m_quizRelation.size() > 0) {
				MyDialog::ptr->ssql << "DELETE from questions where quizid = '";
				MyDialog::ptr->ssql << m_quizRelation.at(ItemIndex) << "';";
				MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
				MyDialog::ptr->ssql.str("");
				mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

				MyDialog::ptr->ssql << "DELETE from results where idQuiz = '";
				MyDialog::ptr->ssql << m_quizRelation.at(ItemIndex) << "';";
				MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
				MyDialog::ptr->ssql.str("");
				mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());

				MyDialog::ptr->ssql << "DELETE from quizes where id = '";
				MyDialog::ptr->ssql << m_quizRelation.at(ItemIndex) << "';";
				MyDialog::ptr->sql = MyDialog::ptr->ssql.str();
				MyDialog::ptr->ssql.str("");
				mysql_query(MyDialog::ptr->getDB().getConnection(), MyDialog::ptr->sql.c_str());
				SetWindowText(hQuestion, "");
				SetWindowText(hAnswer, "");
				SetWindowText(hChoice2, "");
				SetWindowText(hChoice3, "");
				SetWindowText(hChoice4, "");
				EnableWindow(hDelete, FALSE);
				EnableWindow(hUpdate, FALSE);
				EnableWindow(hAdd, FALSE);
				updateQuizCombo(Quiz::ptr->hChooseQuiz);
				SendMessage(Quiz::ptr->hDBquestions, CB_RESETCONTENT, NULL, NULL);
			}
		}
	}
	break;
	case IDC_PLUSBUTTON:
	{
		TCHAR quizName[40];
		GetWindowText(hCreateQuiz, quizName, 40);
		string check = quizName;

		check.erase(0, check.find_first_not_of(" \t"));
		check.erase(check.find_last_not_of(" \t") + 1);

		if (check.size() == 0) {
			break;
		}
		addQuiz(check.c_str());
		if (!MyDialog::ptr->isStarted()) {
			updateQuizCombo(MyDialog::ptr->gethPath());
		}
		updateQuizCombo(hChooseQuiz);
		SendMessageA(hCreateQuiz, WM_SETTEXT, WPARAM(0), LPARAM(""));
		break;
	}
	case IDC_BROWSE:
	{
		bool chose;
		try {
			chose = Quiz::ptr->OnBrowse();
		}
		catch (const std::exception& ex) {
			//SetWindowText(hPath, "");
			MessageBox(hDialog, TEXT(ex.what()), "", MB_ICONERROR);
			return;
		}
		if (!chose)
			break;
		int len = Quiz::ptr->m_codeFileName.size();
		char dot[5];
		TCHAR name[90];
		wsprintf(name,TEXT("%s"), Quiz::ptr->m_codeFileName.c_str());
		rightExtension(name, dot);
		string format;
		format += "";
		if (!strcmp(dot, ".txt")) {
			//SetWindowText(hPath, Quiz::ptr->m_codeFileName.c_str());
			string fileName = findFileName(Quiz::ptr->m_codeFileName);
			SetWindowText(hCodeFileName, fileName.c_str());
			ShowWindow(Quiz::ptr->hRemove, SW_SHOW);
		}
		else {
			format += dot;
			MessageBox(hDialog, TEXT(format.c_str()), "", MB_ICONERROR);
			//SetWindowText(hPath, "");
		}
		break;
	}
	case IDC_REMOVE:
	{
		SetWindowText(hCodeFileName, "");
		m_codeFileName.clear();
		ShowWindow(hRemove, SW_HIDE);
		break;
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
void Quiz::AddQuestion() {
	int ItemIndex = SendMessage((HWND)hChooseQuiz, (UINT)CB_GETCURSEL,
		(WPARAM)0, (LPARAM)0);
	TCHAR ListItem[256];
	SendMessage((HWND)hChooseQuiz, (UINT)CB_GETLBTEXT,
		(WPARAM)ItemIndex, (LPARAM)ListItem);

	char help[500];
	ptrMyDialog->ssql << "insert into questions ";
	GetWindowText(hQuestion, help, 500);
	ptrMyDialog->ssql << "SET questions.question = '" << help << "',";
	GetWindowText(hAnswer, help, 500);
	ptrMyDialog->ssql << "questions.rightAnswer = '" << help << "',";
	GetWindowText(hChoice2, help, 500);
	ptrMyDialog->ssql << "questions.wrongAnswer2 = '" << help << "',";
	GetWindowText(hChoice3, help, 500);
	ptrMyDialog->ssql << "questions.wrongAnswer3 = '" << help << "',";
	GetWindowText(hChoice4, help, 500);
	ptrMyDialog->ssql << "questions.wrongAnswer4 = '" << help << "',";

	if (!m_codeFileName.empty()) {
		char * memblock = nullptr;
		getCode(memblock,m_codeFileName);
		ptrMyDialog->ssql << "questions.code = '" << memblock << "',";
		delete[] memblock;
	}
	ptrMyDialog->ssql << "questions.quizId = (select id from quizes WHERE quizes.name = '" << ListItem << "');";
	ptrMyDialog->sql = ptrMyDialog->ssql.str();
	ptrMyDialog->ssql.str("");
	mysql_query(ptrMyDialog->getDB().getConnection(), ptrMyDialog->sql.c_str());
	
	SendMessageA(hQuestion, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hAnswer, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice2, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice3, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hChoice4, WM_SETTEXT, WPARAM(0), LPARAM(""));
	SendMessageA(hCodeFileName, WM_SETTEXT, WPARAM(0), LPARAM(""));
	m_codeFileName.clear();
	ShowWindow(hRemove, SW_HIDE);
	updateQuestionsCombo(Quiz::ptr->hDBquestions,m_quizRelation.at(ItemIndex));
}
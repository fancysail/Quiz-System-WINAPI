#include "../h/MyDialog.h"
#define ID_LISTBOX 0x8801
#define NOT_CHECKED 99
#define BUTTON_RESET 0x8809
#define BUTTON_REVIEW 0x8810
#define BUTTON_PREV 0x8811
#define BUTTON_NEXT 0x8812
#define BUTTON_SUBMIT 0x8813
#define ID_TIMER1 0x8820
#define ID_EDIT1 0x8820
#define ID_EDIT2 0x8821
#define ID_EDIT3 0x8822
#define ID_EDIT4 0x8823
#define ID_BUTTONCHOSEN 0x8824
using namespace std;

INT MyDialog::m_minutes = 0;
INT MyDialog::seconds = 0;
MyDialog* MyDialog::ptr = NULL;
/*Sending quantity of correct answers(in percents)
To the present moment with each change of the answer*/
DWORD WINAPI ThreadForStats(LPVOID lpParam);
//Receiving necessary for start data
DWORD WINAPI ThreadForReceive(LPVOID lpParam);
//Sending data on server for validation
DWORD WINAPI ThreadForConnect(LPVOID lpParam);
//Synchronized timer.
DWORD WINAPI ThreadForTimer(LPVOID lpParam);

MyDialog::MyDialog(VOID)
{
	ptr = this;
}
VOID MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;
	char szBuf[300];
	//Функция FormatMessage форматирует строку сообщения
	BOOL fOK = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM /* флаг сообщает функции, что нужна строка, соответствующая коду ошибки, определённому в системе */
		| FORMAT_MESSAGE_ALLOCATE_BUFFER, //нужно выделить соответствующий блок памяти для хранения текста
		NULL, //указатель на строку, содержащую текст сообщения
		dwError, //код ошибки
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//язык, на котором выводится описание ошибки (язык пользователя по умолчанию)
		(LPTSTR)&lpMsgBuf, //указатель на буфер, в который запишется текст сообщения
		0, //минимальный размер буфера для выделения памяти - память выделяет система
		NULL //список аргументов форматирования
	);
	if (lpMsgBuf != NULL)
	{
		wsprintf(szBuf, "Ошибка %d: %s", dwError, lpMsgBuf);
		MessageBox(NULL, szBuf, "Сообщение об ошибке", MB_OK | MB_ICONSTOP | MB_DEFAULT_DESKTOP_ONLY);
		LocalFree(lpMsgBuf); //освобождаем память, выделенную системой
	}
}
bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}
BOOL CALLBACK DestoryChildCallback(HWND hwnd, LPARAM lParam) {
	if (hwnd != NULL) {
		if (hwnd != MyDialog::ptr->gethDialogue())
		{
			DestroyWindow(hwnd);
		}
	}
	return TRUE;
}
VOID GetDesktopResolution(int& horizontal, INT& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

VOID toggleLogin() {
	if (lstrlen(MyDialog::ptr->getName()) > 2 && lstrlen(MyDialog::ptr->getGroup()) > 3 && lstrlen(MyDialog::ptr->getSurname()) > 3 && lstrlen(MyDialog::ptr->getFatherName()) > 2) {
		EnableWindow(MyDialog::ptr->gethLogin(), TRUE);
	}
	else {
		EnableWindow(MyDialog::ptr->gethLogin(), FALSE);
	}
}
VOID fillList() {
	for (INT i = 0; i < MyDialog::ptr->getQuiz().size(); i++) {
		stringstream ss;
		ss << to_string(i + 1);
		ss << ".";
		ss << MyDialog::ptr->getQuiz().at(i)->getQuestion();
		string ind = ss.str();
		SendMessage(MyDialog::ptr->getListBox(), LB_ADDSTRING, 0, (LPARAM)ind.c_str());
	}
}

HWND& createEditControl(INT x, INT y, INT width, INT height, HWND* parent, INT id) {
	HWND editControl = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", nullptr, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_WANTRETURN | WS_BORDER,
		x, y, width, height, *parent, (HMENU)id, nullptr, 0);
	return editControl;
}
HWND& createGroupBox(INT x, INT y, INT width, INT height, HWND* parent) {
	HWND groupBox = CreateWindow("Button", nullptr, WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		x, y, width, height, *parent, 0, nullptr, 0);
	return groupBox;
}
HWND& createStatic(const char* text, INT x, INT y, INT width, INT height, HWND*parent) {
	HWND hStatic = CreateWindowEx(0, TEXT("STATIC"), TEXT(text), WS_CHILD | WS_VISIBLE |
		WS_EX_CLIENTEDGE | SS_CENTER, x, y, width, height, *parent, 0, nullptr, 0);
	return hStatic;
}
HWND& createRadioButton(const char* text, INT x, INT y, INT width, INT height, HWND* parent) {
	HWND hRadio = CreateWindowEx(0, TEXT("BUTTON"), TEXT(text), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		x, y, width, height, *parent, 0, GetModuleHandle(NULL), 0);
	return hRadio;
}
HWND& createButton(const char* text, INT x, INT y, INT width, INT height, HWND* parent, INT id, int style = 0) {
	HWND hButton = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Button"), TEXT(text), WS_CHILD | WS_VISIBLE | WS_GROUP | style,
		x, y, width, height, *parent, (HMENU)id, GetModuleHandle(NULL), 0);
	return hButton;
}
VOID MyDialog::createAllElements(HWND hwnd) {
	hListBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), nullptr,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_DISABLENOSCROLL, 6, 0, 180, /*430*/393, hwnd, (HMENU)ID_LISTBOX, NULL, NULL);
	blistShown = TRUE;
	SendMessage(hListBox, LB_SETHORIZONTALEXTENT, 500, NULL);
	fillList();
	//Установка остальных элементов
	m_index = 0;
	hQuestionNumber = createStatic("Question: ", 210, 7, 100, 16, &ptr->hDialog);
	SetWindowText(hQuestionNumber, ("Question: " + std::to_string(1)).c_str());
	hPercents = createStatic("0%", 420, 7, 50, 16, &ptr->hDialog);
	SetWindowText(hPercents, "0%");

	groupBox = createGroupBox(210, 57, 495, 80, &hwnd);
	hQuestion = createStatic(MyDialog::ptr->quiz.at(0)->getQuestion().c_str(), 4, 10, 485, 65, &groupBox);
	hTimeLeft = createStatic(nullptr, 580, 7, 140, 16, &ptr->hDialog);
	SetWindowText(hTimeLeft, "");
	groupBox1 = createGroupBox(210, 137, 495, 210, &hwnd);
	//Установка радиобаттонов
	hRadios[0] = CreateWindowEx(0, TEXT("BUTTON"), nullptr, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		6, 16, 14, 50, groupBox1, 0, GetModuleHandle(NULL), 0);
	for (INT i = 1; i < 4; i++) {
		hRadios[i] = createRadioButton(nullptr, 6, 16 + i * 45, 14, 50, &groupBox1);
	}
	//Edits для Radio Чтобы текст был в несколько строк
	for (INT i = 0; i < 4; i++) {
		hEditForRadios[i] = createEditControl(210 + 30, 137 + (16 + i * 46), 660, 46, &hwnd, ID_EDIT1 + i);
		SetWindowText(hEditForRadios[i], MyDialog::ptr->quiz.at(0)->getChoiceAt(i).c_str());
	}

	//Установка кнопок
	buttons[0] = createButton("Reset", 210, 377, 80, 24, &ptr->hDialog, BUTTON_RESET);
	buttons[1] = createButton("Review Questions", 300, 377, 120, 24, &ptr->hDialog, BUTTON_REVIEW);
	buttons[2] = createButton("Previous", 430, 377, 100, 24, &ptr->hDialog, BUTTON_PREV);
	buttons[3] = createButton("Next", 540, 377, 80, 24, &ptr->hDialog, BUTTON_NEXT);
	buttons[4] = createButton("Submit", 630, 377, 75, 24, &ptr->hDialog, BUTTON_SUBMIT);

	EnableWindow(buttons[2], FALSE);

	std::pair<HWND, BOOL> pair;

	//Установка шрифта для всех элементов
	EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
	toggleListBox();
}

VOID MyDialog::disconnectMsg() {
	char SZbuff[100] = "<EXIT>";
	send(getClientInfo().socket, SZbuff, strlen(SZbuff) + 1, 0);
	int result = recv(ptr->getClientInfo().socket, SZbuff, 100, 0);
	if (!strcmp(SZbuff, "<OK>")) {
		//отправляю время
		string t;
		INT spentMinutes = getQuizTime() - getSpentTime()-1;
		INT spentSeconds = 60 - seconds;
		if (spentMinutes < 10) {
			t += "0";
		}
		t += to_string(spentMinutes);
		t += ":";
		if (spentSeconds < 10) {
			t += "0";
		}
		t += to_string(spentSeconds);
		wsprintf(SZbuff, TEXT("%s"), t.c_str());
		send(getClientInfo().socket, SZbuff, strlen(SZbuff) + 1, 0);
	}
}
VOID MyDialog::disconnect() {
	if (isConnected()) {
		shutdown(getClientInfo().socket, SD_BOTH); // SD_BOTH запрещает как прием, так и отправку данных
		closesocket(getClientInfo().socket); // The closesocket function closes an existing socket.
		WSACleanup(); // The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
		setConnected(FALSE);
	}
}

VOID MyDialog::countCurrentAnswer() {
	for (INT i = 0; i < 4; i++)
	{
		if ((Button_GetCheck(hRadios[i]) == BST_CHECKED))
		{
			if (quiz.at(m_index)->getUserAnswer() == NOT_CHECKED) {
				increaseCountUserAnswered();
			}
			quiz.at(m_index)->setUserAnswer(i);
			changePercents();
		}
	}
}
INT MyDialog::checkResult() {
	INT scored = 0;
	for (INT i = 0; i < quiz.size(); i++) {
		if (quiz.at(i)->getUserAnswer() == quiz.at(i)->getRightAnswer()) {
			scored++;
		}
	}
	return scored;
}
VOID MyDialog::changePercents() {
	double percents = (getCountUserAnswered() / (double)quiz.size()) * 100;
	stringstream ss;
	ss << fixed << setprecision(0) << percents;
	ss << "%";
	std::string resultstr = ss.str();
	const char* cstr2 = resultstr.c_str();
	//Signal for ThreadForStats to send percents
	SetEvent(hEvent);
	SetWindowText(hPercents, TEXT(cstr2));
}
VOID MyDialog::changedQuestion() {
	INT index = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
	//START Which radiobutton was chosen in a previous question
	INT checked = NOT_CHECKED;

	for (INT i = 0; i < 4; i++)
	{
		if ((Button_GetCheck(hRadios[i]) == BST_CHECKED))
		{
			checked = i;
		}
	}
	if (!isSubmitted()) {
		if (quiz.at(m_index)->getUserAnswer() == NOT_CHECKED  &&  checked != NOT_CHECKED) {//In prev question Checked when there was no check
			quiz.at(m_index)->setUserAnswer(checked);
			increaseCountUserAnswered();
			changePercents();
		}
		else if (quiz.at(m_index)->getUserAnswer() != NOT_CHECKED  &&  checked == NOT_CHECKED) {//In prev question Reset button when it was checked
			quiz.at(m_index)->setUserAnswer(checked);
			decreaseCountUserAnswered();
			changePercents();
		}
		else if (quiz.at(m_index)->getUserAnswer() != NOT_CHECKED  &&  checked != NOT_CHECKED) {//In prev question Rechecked
			quiz.at(m_index)->setUserAnswer(checked);
			changePercents();
		}
	}
	//END Which radiobutton was chosen in a previous question


	if (index == 0)  EnableWindow(buttons[2], FALSE);
	else EnableWindow(buttons[2], TRUE);

	if (index == quiz.size() - 1) EnableWindow(buttons[3], FALSE);
	else  EnableWindow(buttons[3], TRUE);

	m_index = index;

	if (checked != NOT_CHECKED) {
		SendMessage(hRadios[checked], BM_SETCHECK, BST_UNCHECKED, NULL);
	}
	if (quiz.at(m_index)->getUserAnswer() != NOT_CHECKED) {
		SendMessage(hRadios[quiz.at(m_index)->getUserAnswer()], BM_SETCHECK, BST_CHECKED, NULL);
	}

	SetWindowText(hQuestionNumber, ("Question: " + std::to_string(index + 1)).c_str());
	SetWindowText(hQuestion, quiz.at(index)->getQuestion().c_str());

	for (INT i = 0; i < 4; i++) {
		SetWindowText(hEditForRadios[i], quiz.at(index)->getChoiceAt(i).c_str());
	}
}
VOID MyDialog::toggleListBox() {
	if (blistShown) {
		ShowWindow(hListBox, FALSE);
		MoveWindow(hQuestionNumber, 10, 7, 100, 16, TRUE);
		MoveWindow(hPercents, 320, 7, 50, 16, TRUE);
		MoveWindow(groupBox, 14, 20, 695, 80, TRUE);//57 20
		MoveWindow(hQuestion, 4, 10, 685, 65, TRUE);
		MoveWindow(groupBox1, 14, 100, 695, 210, TRUE);//137 С радиобаттонами 100

		MoveWindow(buttons[0], 44, 340, 80, 24, TRUE);
		MoveWindow(buttons[1], 159, 340, 120, 24, TRUE);
		MoveWindow(buttons[2], 314, 340, 100, 24, TRUE);
		MoveWindow(buttons[3], 450, 340, 120, 24, TRUE);
		MoveWindow(buttons[4], 604, 340, 75, 24, TRUE);

		for (INT i = 0; i < 4; i++) {
			MoveWindow(hEditForRadios[i], 40, 100 + (16 + i * 45), 660, 46, TRUE);
		}
		blistShown = FALSE;
	}
	else {
		ShowWindow(hListBox, TRUE);
		MoveWindow(hQuestionNumber, 210, 7, 100, 16, TRUE);
		MoveWindow(hPercents, 420, 7, 50, 16, TRUE);
		MoveWindow(groupBox, 210, 20, 495, 80, TRUE);
		MoveWindow(hQuestion, 4, 10, 485, 65, TRUE);
		MoveWindow(groupBox1, 210, 100, 495, 210, TRUE);

		MoveWindow(buttons[0], 210, 340, 80, 24, TRUE);
		MoveWindow(buttons[1], 300, 340, 120, 24, TRUE);
		MoveWindow(buttons[2], 430, 340, 100, 24, TRUE);
		MoveWindow(buttons[3], 540, 340, 80, 24, TRUE);
		MoveWindow(buttons[4], 630, 340, 75, 24, TRUE);

		for (INT i = 0; i < 4; i++) {
			MoveWindow(hEditForRadios[i], 210 + 30, 100 + (16 + i * 45), 460, 46, TRUE);
		}

		if (IsWindowEnabled(buttons[2])) {
			EnableWindow(buttons[2], FALSE);
			EnableWindow(buttons[2], TRUE);
		}
		else {
			EnableWindow(buttons[2], TRUE);
			EnableWindow(buttons[2], FALSE);
		}

		blistShown = TRUE;
	}
}
VOID MyDialog::submitTest() {
	countCurrentAnswer();
	EnableWindow(buttons[4], FALSE);
	INT scored = checkResult();
	double scoredPercentage = (scored / (double)quiz.size()) * 100;
	stringstream ss;

	if (scoredPercentage > 90) { ss << "Congratulations!\n"; }
	else if (scoredPercentage > 75) { ss << "Very Good!\n"; }
	else if (scoredPercentage > 50) { ss << "Good!\n"; }
	else if (scoredPercentage > 25) { ss << "Not impressed..\n"; }
	else { ss << "Eh...\n"; }

	ss << fixed << setprecision(1) << scoredPercentage;
	ss << "%";
	ss << " right answers";
	std::string message = ss.str();
	const char* c_message = message.c_str();

	MessageBox(NULL, c_message, "Test Result", MB_OK | MB_ICONINFORMATION);
	RedrawWindow(hDialog, NULL, NULL, RDW_INVALIDATE);
}
VOID MyDialog::Cls_OnClose(HWND hwnd)
{
	if (isLogged()) {
		if (isSubmitted()) {
			EndDialog(hwnd, 0);
		}
		else {
			INT a = MessageBox(hDialog, TEXT("Завершить тест?"), "Quiz", MB_YESNO | MB_ICONQUESTION);
			if (a == IDYES) {
				disconnectMsg();
				disconnect();
				EndDialog(hwnd, 0);
			}
		}
	}
	else {
		EndDialog(hwnd, 0);
	}
}
BOOL MyDialog::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCHAR GUID[] = TEXT("{D93CD3E0-672D-4def-9B44-99FD7E796DFB}");
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
	INT widthRect = rect.right - rect.left;
	INT heightRect = rect.bottom - rect.top;
	MoveWindow(hwnd, (horizontal - (horizontal / 2) - widthRect / 2), (vertical - (vertical / 2) - heightRect / 2), widthRect, heightRect, TRUE);

	hDialog = hwnd;
	hName = GetDlgItem(hwnd, IDC_NAME);
	hSurname = GetDlgItem(hwnd, IDC_SURNAME);
	hGroup = GetDlgItem(hwnd, IDC_GROUP);
	hLogin = GetDlgItem(hwnd, IDC_LOGIN);
	hFatherName = GetDlgItem(hwnd, IDC_FATHERNAME);

	hEvent = CreateEvent(NULL, FALSE, FALSE, "{26E73077-4596-4321-9AA1-BC25CA222ABC}");
	return TRUE;
}
VOID MyDialog::Cls_OnCommand(HWND hwnd, INT id, HWND hwndCtl, UINT codeNotify)
{
	if (id == BUTTON_REVIEW) {
		toggleListBox();
	}
	else if (id == BUTTON_SUBMIT) {
		INT a = MessageBox(hDialog, TEXT("Завершить тест?"), "Тест", MB_YESNO);
		if (a == IDYES) {
			bsubmitted = TRUE;
			KillTimer(MyDialog::ptr->hDialog, ID_TIMER1);
			submitTest();
			EnableWindow(buttons[0], FALSE);
			for (INT i = 0; i < 4; i++) {
				EnableWindow(hRadios[i], FALSE);
			}
		}
	}
	else if (id == BUTTON_NEXT) {
		SendMessage(hListBox, LB_SETCURSEL, m_index + 1, NULL);
		changedQuestion();
	}
	else if (id == BUTTON_PREV) {
		SendMessage(hListBox, LB_SETCURSEL, m_index - 1, NULL);
		changedQuestion();
	}
	else if (id == BUTTON_RESET) {
		if (codeNotify == BN_CLICKED) {
			for (INT i = 0; i < 4; i++) {
				SendMessage(hRadios[i], BM_SETCHECK, BST_UNCHECKED, NULL);
			}
		}
	}
	else if (id == ID_LISTBOX) {
		if (codeNotify == LBN_DBLCLK) {
			INT index = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
			if (index != m_index) {
				changedQuestion();
			}
		}
	}
	else if (id == IDC_LOGIN) {
		hConnectThread = CreateThread(NULL, 0, ThreadForConnect, this, 0, nullptr);
		hReceiveThread = CreateThread(NULL, 0, ThreadForReceive, this, 0, nullptr);
		WaitForSingleObject(hConnectThread, INFINITE);
		WaitForSingleObject(hReceiveThread, INFINITE);
		if (ptr->bconnected == FALSE)
			return;
		hStatsThread = CreateThread(NULL, 0, ThreadForStats, this, 0, nullptr);

		EnumChildWindows(hDialog, DestoryChildCallback, NULL);
		windowMinWidth = 740;
		winowMinHeight = 420;//457 420
		MoveWindow(hwnd, (horizontal - (horizontal / 2) - windowMinWidth / 2), (vertical - (vertical / 2) - winowMinHeight / 2), windowMinWidth, winowMinHeight, TRUE);

		createAllElements(hwnd);

		blogged = TRUE;
	}
	else if (id == IDC_NAME) {
		GetWindowText(MyDialog::ptr->hName, MyDialog::ptr->m_name, 20);
		toggleLogin();
	}
	else if (id == IDC_SURNAME) {
		GetWindowText(MyDialog::ptr->hSurname, MyDialog::ptr->m_surname, 20);
		toggleLogin();
	}
	else if (id == IDC_GROUP) {
		GetWindowText(MyDialog::ptr->hGroup, MyDialog::ptr->m_group, 20);
		toggleLogin();
	}
	else if (id == IDC_FATHERNAME) {
		GetWindowText(MyDialog::ptr->hFatherName, MyDialog::ptr->m_fatherName, 20);
		toggleLogin();
	}
}

INT_PTR CALLBACK MyDialog::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	case WM_CTLCOLORSTATIC:
	{
		TCHAR senderClass[256];
		GetClassName((HWND)lParam, senderClass, 256);
		if (!strcmp(senderClass, "Edit"))
		{
			HDC hdcStatic = (HDC)wParam;

			if (MyDialog::ptr->isSubmitted()) {
				if (MyDialog::ptr->getQuiz().at(MyDialog::ptr->getIndex())->getUserAnswer() ==
					MyDialog::ptr->getQuiz().at(MyDialog::ptr->getIndex())->getRightAnswer()) {
					//green background
					if ((HWND)lParam == ptr->hEditForRadios[ptr->getQuiz().at(ptr->getIndex())->getUserAnswer()]) {
						SetBkColor(hdcStatic, RGB(102, 239, 97));
						return (INT_PTR)CreateSolidBrush(RGB(102, 239, 97));
					}
				}
				else if ((HWND)lParam == ptr->hEditForRadios[ptr->getQuiz().at(ptr->getIndex())->getUserAnswer()]) {
					//red background
					SetBkColor(hdcStatic, RGB(246, 83, 83));
					return (INT_PTR)CreateSolidBrush(RGB(246, 83, 83));
				}
			}
			SetBkColor(hdcStatic, RGB(230, 230, 230));
			return (INT_PTR)CreateSolidBrush(RGB(230, 230, 230));
		}
	}
	break;
	}
	return FALSE;
}

DWORD WINAPI ThreadForConnect(LPVOID lpParam)
{
	MyDialog* ptr = (MyDialog*)lpParam;
	if (WSAStartup(WINSOCK_VERSION, &ptr->getWSD()))
	{
		MessageAboutError(WSAGetLastError());
		return 0;
	}
	ptr->getClientInfo().socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ptr->getClientInfo().socket == SOCKET_ERROR)
	{
		MessageAboutError(WSAGetLastError());
		WSACleanup();
		return 0;
	}
	ptr->getClientInfo().addr.sin_family = AF_INET;
	ptr->getClientInfo().addr.sin_port = htons(49152);
	ptr->getClientInfo().addr.sin_addr.S_un.S_addr = inet_addr(MyDialog::ptr->getServerIp());

	if (connect(ptr->getClientInfo().socket, (sockaddr*)&(ptr->getClientInfo().addr), sizeof(ptr->getClientInfo().addr)) == SOCKET_ERROR)
	{
		MessageAboutError(WSAGetLastError());
		closesocket(ptr->getClientInfo().socket);
		WSACleanup();
		return 0;
	}

	char host[50];
	gethostname(host, 50); // The gethostname function retrieves the standard host name for the local computer.
	char buf[300];

	char szBuff[100] = "<CHECK_STUDENT>";
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
	INT result = recv(ptr->getClientInfo().socket, szBuff, 100, 0);

	sprintf_s(szBuff, "%s", ptr->getName());
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
	result = recv(ptr->getClientInfo().socket, szBuff, 100, 0);

	sprintf_s(szBuff, "%s", ptr->getSurname());
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
	result = recv(ptr->getClientInfo().socket, szBuff, 100, 0);

	sprintf_s(szBuff, "%s", ptr->getFatherName());
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
	result = recv(ptr->getClientInfo().socket, szBuff, 100, 0);

	sprintf_s(szBuff, "%s", ptr->getGroup());
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
	result = recv(ptr->getClientInfo().socket, szBuff, 100, 0);

	if (!strcmp(szBuff, "<OK>")) {
		//Student exists and hasn't took a quiz
		ptr->setConnected(TRUE);
	}
	else if (!strcmp(szBuff, "<ALREADY_PASSED>")) {
		shutdown(ptr->getClientInfo().socket, SD_BOTH);
		closesocket(ptr->getClientInfo().socket);
		WSACleanup();
		ptr->setConnected(FALSE);
		MessageBox(NULL, "Вы уже сдавали тест!", "Сообщение об ошибке", MB_OK | MB_ICONSTOP | MB_DEFAULT_DESKTOP_ONLY);
	}
	else if (!strcmp(szBuff, "<DOESNT_EXIST>")) {
		shutdown(ptr->getClientInfo().socket, SD_BOTH);
		closesocket(ptr->getClientInfo().socket);
		WSACleanup();
		ptr->setConnected(FALSE);
		MessageBox(NULL, "Такой пользователь не существует!", "Сообщение об ошибке", MB_OK | MB_ICONSTOP | MB_DEFAULT_DESKTOP_ONLY);
	}
	else if (!strcmp(szBuff, "<ALREADY_LOGGED>")) {
		shutdown(ptr->getClientInfo().socket, SD_BOTH);
		closesocket(ptr->getClientInfo().socket);
		WSACleanup();
		ptr->setConnected(FALSE);
		MessageBox(NULL, "Пользователь с такими данными уже сдаёт тест!", "Сообщение об ошибке", MB_OK | MB_ICONSTOP | MB_DEFAULT_DESKTOP_ONLY);
	}

	return 1;
}
DWORD WINAPI ThreadForReceive(LPVOID lpParam)
{
	MyDialog* ptr = (MyDialog*)lpParam;
	WaitForSingleObject(ptr->gethConnect(), INFINITE);
	if (!ptr->isConnected())
		return 0;
	char szBuff[4096] = "<QUIZ_REQUEST>";
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);

	INT result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
	ptr->setQuiz(Question::parseFile(szBuff, ptr->isRandQuestions()));
	strcpy_s(szBuff, "<QUIZ_RECEIVED>");
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);

	result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
	INT time = atoi(szBuff);
	ptr->setQuizTime(time);
	
	CreateThread(NULL, 0, ThreadForTimer, ptr, 0, nullptr);
	
	strcpy_s(szBuff, "<TIME_RECEIVED>");
	send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);

	result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
	if (!strcmp(szBuff, "<RANDOMIZE_QUESTIONS>")) {
		strcpy_s(szBuff, "<YES>");
		send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
		result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
		if (!strcmp(szBuff, "<CHECKED>")) {
			ptr->setRandQuestions(TRUE);
			strcpy_s(szBuff, "<YES>");
			send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
		}
		else if (!strcmp(szBuff, "<NOT_CHECKED>")) {
			ptr->setRandQuestions(FALSE);
			strcpy_s(szBuff, "<YES>");
			send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
		}
		else {
			//Smth gone wrong
		}
	}
	result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
	if (!strcmp(szBuff, "<OK>")) {
		return 0;
	}
	return 1;
}
DWORD WINAPI ThreadForStats(LPVOID lpParam)
{
	MyDialog* ptr = (MyDialog*)lpParam;
	char szBuff[4096];
	int result;
	while (!ptr->isSubmitted()) {
		WaitForSingleObject(ptr->hEvent, INFINITE);
		strcpy_s(szBuff, "<SUCCESSFUL_PERCENTS>");
		send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
		result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
		if (!strcmp(szBuff, "<OK>")) {
			INT scored = ptr->checkResult();
			double scoredPercentage = (scored / (double)ptr->getQuiz().size()) * 100;
			stringstream ss;
			ss << fixed << setprecision(1) << scoredPercentage;
			std::string message = ss.str();
			const char* c_message = message.c_str();
			strcpy_s(szBuff, c_message);
			send(ptr->getClientInfo().socket, szBuff, strlen(szBuff) + 1, 0);
			result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);

			if (ptr->isSubmitted()) {
				ptr->disconnectMsg();
				result = recv(ptr->getClientInfo().socket, szBuff, 4096, 0);
				if (!strcmp(szBuff, "<EXIT>")) {
					ptr->disconnect();
					return 0;
				}
			}
		}
	}
	return 0;
}
DWORD WINAPI ThreadForTimer(LPVOID lpParam) {
	MyDialog* ptr = (MyDialog*)lpParam;
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -10000000LL;
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	while (true) {
		SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
		if (ptr->isSubmitted())
			break;
		if (WaitForSingleObject(hTimer, INFINITE) == WAIT_OBJECT_0)
		{
			ptr->getSeconds()--;
			if (ptr->getSeconds() <= 0) {
				ptr->getm_minutes()--;
				ptr->getSeconds() = 59;
			}
			if (ptr->getm_minutes() < 0 && ptr->getSeconds() == 59) {
				SetWindowText(ptr->gethTimeLeft(), TEXT("Time left 00:00"));
				ptr->submitTest();
				EnableWindow(ptr->gethButtons()[0], FALSE);
				for (INT i = 0; i < 4; i++) {
					EnableWindow(ptr->gethRadios()[i], FALSE);
				}
				ptr->setSubmitted(TRUE);
				break;
			}
			ptr->setm_stime("Time left ");

			if (ptr->getm_minutes() > 9) ptr->getm_stime() += to_string(ptr->getm_minutes());
			else ptr->getm_stime() += "0" + to_string(ptr->getm_minutes());

			ptr->getm_stime() += ":";

			if (ptr->getSeconds() > 9) ptr->getm_stime() += to_string(ptr->getSeconds());
			else ptr->getm_stime() += "0" + to_string(ptr->getSeconds());

			SetWindowText(ptr->gethTimeLeft(), TEXT(ptr->getm_stime().c_str()));
			ptr->getm_stime().clear();
		}
	}
	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);
	return 0;
}
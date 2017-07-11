#pragma once
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#include <Winsock2.h>
#include<windows.h>
#include <WindowsX.h>
#include <iostream>
#include <string>
#include "../resource.h"
#include <shlobj.h>
#include <stdexcept>
#include <sstream>
#include <Strsafe.h>
#include <iomanip> 
#include <map>
#include <algorithm>

#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <Commctrl.h>


#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "comctl32.lib")

using namespace std;
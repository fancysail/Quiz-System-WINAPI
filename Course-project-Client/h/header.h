#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Winsock2.h"
#include <windows.h>
#include <WindowsX.h>
#include <string>
#include <sstream>
#include <iomanip> 
#include <vector>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <stdexcept>
#include "Question.h"
#include <tchar.h>
#include "../resource1.h"
#include <time.h>
#include <Commctrl.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
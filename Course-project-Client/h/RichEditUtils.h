#pragma once
#include <Windows.h>
#include <richedit.h>
// util function for rich edit
namespace rich_edit {
	CHARFORMAT get_char_fmt(HWND hwnd, DWORD range = SCF_SELECTION) {
		CHARFORMAT cf;
		SendMessage(hwnd, EM_GETCHARFORMAT, range, (LPARAM)&cf);
		return cf;
	}
	void set_char_fmt(HWND hwnd, const CHARFORMAT& cf, DWORD range = SCF_SELECTION) {
		SendMessage(hwnd, EM_SETCHARFORMAT, range, (LPARAM)&cf);
	}
	void replace_sel(HWND hwnd, const char* str) {
		SendMessage(hwnd, EM_REPLACESEL, 0, (LPARAM)str);
	}
	void cursor_to_bottom(HWND hwnd) {
		SendMessage(hwnd, EM_SETSEL, -2, -1);
	}
	void scroll_to(HWND hwnd, DWORD pos) {
		SendMessage(hwnd, WM_VSCROLL, pos, 0);
	}
	void scroll_to_bottom(HWND hwnd) {
		scroll_to(hwnd, SB_BOTTOM);
	}
	// this function is used to output text in different color
	void append(HWND hwnd, COLORREF clr, const char* str) {
		cursor_to_bottom(hwnd); // move cursor to bottom

		CHARFORMAT cf = get_char_fmt(hwnd); // get default char format
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_COLOR; // change color
		cf.dwEffects = 0; // add this line
		cf.crTextColor = clr;

		set_char_fmt(hwnd, cf); // set default char format

		replace_sel(hwnd, str); // code from google
		scroll_to_bottom(hwnd); // scroll to bottom
	}
}
#ifndef __GUI_CONTROL_H__
#define __GUI_CONTROL_H__

#include <winsock2.h>
#include <string>

namespace GUIControl {
	void AppendText(const HWND &hDlg, int resource, std::wstring newText);
}

#endif
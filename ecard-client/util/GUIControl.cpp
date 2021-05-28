#include "GUIControl.h"

void GUIControl::AppendText(const HWND& hDlg, int resource, std::wstring newText) {
    HWND hEdit = GetDlgItem(hDlg, resource);

    int idx = GetWindowTextLength(hEdit);
    SetFocus(hEdit);

    SendMessage(hEdit, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
    SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)newText.c_str());
}
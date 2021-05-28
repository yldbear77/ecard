// ecard-server.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

//#include <windows.h>
//#include <commctrl.h>
#include <winsock2.h>
#include <commctrl.h>

#include "framework.h"
#include "ecard-server.h"

#include "include/Server.h"
#include "include/Timer.h"

#include "net/SocketUtil.h"

#define MAX_LOADSTRING 100

// 전역 변수:
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HINSTANCE g_hInst;
HWND g_hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NotifyDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    g_hInst = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, MainDlgProc);

    return 0;
}

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
    case WM_INITDIALOG: {
        HWND hList1 = GetDlgItem(hDlg, IDC_LIST1);
        SendMessage(hList1, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        LVCOLUMN LvCol;
        memset(&LvCol, 0, sizeof(LvCol));
        LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        LvCol.pszText = (LPWSTR)(L"소켓 주소");
        LvCol.cx = 0x80;
        ListView_InsertColumn(hList1, 0, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"접속 ID");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList1, 1, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"상태");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList1, 2, (LPARAM)&LvCol);

        HWND hList2 = GetDlgItem(hDlg, IDC_LIST2);
        SendMessage(hList2, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        LvCol.pszText = (LPWSTR)(L"방 번호");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList2, 0, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"유저 1");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList2, 1, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"유저 2");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList2, 2, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"차례");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList2, 3, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"라운드");
        LvCol.cx = 0x42;
        ListView_InsertColumn(hList2, 4, (LPARAM)&LvCol);

        HWND hList3 = GetDlgItem(hDlg, IDC_LIST3);
        SendMessage(hList3, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        LvCol.pszText = (LPWSTR)(L"시간");
        LvCol.cx = 0x82;
        ListView_InsertColumn(hList3, 0, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"송신 소켓주소");
        LvCol.cx = 0x80;
        ListView_InsertColumn(hList3, 1, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"헤더");
        LvCol.cx = 0x61;
        ListView_InsertColumn(hList3, 2, (LPARAM)&LvCol);
        LvCol.pszText = (LPWSTR)(L"값");
        LvCol.cx = 0x119;
        ListView_InsertColumn(hList3, 3, (LPARAM)&LvCol);

        RECT rtDesk, rtWindow;
        GetWindowRect(GetDesktopWindow(), &rtDesk);
        GetWindowRect(hDlg, &rtWindow);

        int width = rtWindow.right - rtWindow.left;
        int height = rtWindow.bottom - rtWindow.top;
        int x = (rtDesk.right - width) / 2;
        int y = (rtDesk.bottom - height) / 2;
        MoveWindow(hDlg, x, y, width, height, TRUE);

        Timer::hMainDlg = hDlg;
        Server::hMainDlg = hDlg;
        Server::GetInstance()->hOnlineUserList = hList1;
        Server::GetInstance()->hRoomList = hList2;
        Server::GetInstance()->hPacketList = hList3;

        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1:
            EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), false);
            if (SocketUtil::Init()) {
                Server::GetInstance()->Run();
            }
            return TRUE;
        case IDC_BUTTON2:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), hDlg, NotifyDlgProc);
            return TRUE;
        case IDC_BUTTON3:
            Server::GetInstance()->SavePacketLog();
            return TRUE;
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK NotifyDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
    case WM_INITDIALOG: {
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            TCHAR msg[150];
            GetDlgItemText(hDlg, IDC_EDIT1, msg, 150);
            Server::GetInstance()->SendNotification(msg);
            EndDialog(hDlg, 0);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}
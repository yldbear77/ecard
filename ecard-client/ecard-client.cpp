// ecard-client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <winsock2.h>

#include "framework.h"
#include "ecard-client.h"

#include "net/SocketUtil.h"
#include "net/BitStream.h"

#include "include/Client.h"
#include "include/PacketHeader.h"
#include "include/Timer.h"

#include "util/Logger.h"

#define MAX_LOADSTRING 100

// 전역 변수:
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HINSTANCE g_hInst;
HWND g_hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SignInDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SignUpDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK RecordInquiryDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ModifyDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DeleteDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK RuleDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
void GetHandles(HWND hDlg);
void HideWindows();
void ShowWindows();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    g_hInst = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG3), HWND_DESKTOP, MainDlgProc);

    return 0;
}

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();

    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rtDesk, rtWindow;
        GetWindowRect(GetDesktopWindow(), &rtDesk);
        GetWindowRect(hDlg, &rtWindow);

        int width = rtWindow.right - rtWindow.left;
        int height = rtWindow.bottom - rtWindow.top;
        int x = (rtDesk.right - width) / 2;
        int y = (rtDesk.bottom - height) / 2;
        MoveWindow(hDlg, x, y, width, height, TRUE);

        g_hWnd = hDlg;
        //ShowWindow(hDlg, SW_SHOW);
        
        GetHandles(hDlg);
        HideWindows();

        if (SocketUtil::Init()) {
            clnt->Run();
        }

        clnt->hMainDlg = hDlg;
        Timer::hMainDlg = hDlg;

        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hDlg, SignInDlgProc);
        ShowWindow(hDlg, SW_SHOW);
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG4), hDlg, MenuDlgProc);
        return TRUE;
    }
    case WM_CTLCOLOREDIT: {
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_MYLV)) {
            SetTextColor((HDC)wParam, RGB(255, 0, 0));
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }
        else if ((HWND)lParam == GetDlgItem(hDlg, IDC_OPPONENTLV)) {
            SetTextColor((HDC)wParam, RGB(255, 0, 0));
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }
        return TRUE;
    }
    case WM_LBUTTONDOWN: {
        if (clnt->in_matching) return TRUE;
        if (clnt->was_played) {
            ShowWindows();
        }
        if (clnt->is_enabled) {
            ShowWindow(clnt->hWaitText, SW_HIDE);
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG4), hDlg, MenuDlgProc);
        }
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1:
            clnt->ProcessSelectES();
            return TRUE;
        case IDC_BUTTON2:
            clnt->ProcessSelectCz();
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK SignInDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();

    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(g_hWnd, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);

        HBITMAP hTitleBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
        SendMessage(GetDlgItem(hDlg, IDC_TITLE), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitleBitmap);

        Client::GetInstance()->hSignInDlg = hDlg;
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1: {     // 로그인
            clnt->ProcessSignIn(hDlg);
            return TRUE;
        }
        case IDC_BUTTON2: {     // 회원가입
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), hDlg, SignUpDlgProc);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, 0);
            DestroyWindow(g_hWnd);
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK SignUpDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();

    switch (iMessage) {
    case WM_INITDIALOG:
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(clnt->hSignInDlg, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);

        Client::GetInstance()->hSignUpDlg = hDlg;
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            clnt->ProcessSignUp(hDlg);
            return TRUE;
        }
        case IDCANCEL: {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK MenuDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();

    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(g_hWnd, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);
        clnt->is_enabled = TRUE;

        //RECT rtDesk, rtWindow;
        //GetWindowRect(GetDesktopWindow(), &rtDesk);
        //GetWindowRect(hDlg, &rtWindow);

        //int width = rtWindow.right - rtWindow.left;
        //int height = rtWindow.bottom - rtWindow.top;
        //int x = (rtDesk.right - width) / 2;
        //int y = (rtDesk.bottom - height) / 2;
        //MoveWindow(hDlg, x, y, width, height, TRUE);

        Client::GetInstance()->hMenuDlg = hDlg;
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1: {     /* 게임시작 */
            clnt->is_enabled = FALSE;
            clnt->was_played = TRUE;
            clnt->in_matching = TRUE;
            HideWindows();
            clnt->ProcessQueuing();
            EndDialog(hDlg, 0);
            SetDlgItemText(clnt->hMainDlg, IDC_WAITTEXT, L"매칭 중 ...");
            ShowWindow(clnt->hWaitText, SW_SHOW);
            return TRUE;
        }
        case IDC_BUTTON2: {     /* 전적조회 */
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG6), hDlg, RecordInquiryDlgProc);
            return TRUE;
        }
        case IDC_BUTTON3: {     /* 정보변경 */
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG5), hDlg, ModifyDlgProc);
            return TRUE;
        }
        case IDC_BUTTON4: {     /* 회원탈퇴 */
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG7), hDlg, DeleteDlgProc);
            return TRUE;
        }
        case IDC_BUTTON6: {     /* 게임규칙 */
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG8), hDlg, RuleDlgProc);
            return TRUE;
        }
        case IDC_BUTTON5: {     /* 게임종료 */
            EndDialog(hDlg, 0);
            DestroyWindow(g_hWnd);
            return TRUE;
        }
        case IDCANCEL: {
            clnt->is_enabled = TRUE;
            HideWindows();
            EndDialog(hDlg, 0);
            SetDlgItemText(clnt->hMainDlg, IDC_WAITTEXT, L"화면 좌클릭 시 메뉴가 다시 나타납니다.");
            ShowWindow(clnt->hWaitText, SW_SHOW);
            return TRUE;
        }
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK RecordInquiryDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();
    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(clnt->hMenuDlg, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);

        clnt->hRecordDlg = hDlg;
        clnt->ProcessRecordInquiry();
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDCANCEL: {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK ModifyDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();
    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(clnt->hMenuDlg, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);

        clnt->hModifyDlg = hDlg;
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            clnt->ProcessModify(hDlg);
            return TRUE;
        }
        case IDCANCEL: {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK DeleteDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();
    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(clnt->hMenuDlg, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);

        clnt->hDeleteDlg = hDlg;
        SetDlgItemTextA(hDlg, IDC_EDIT2, clnt->GetId().c_str());
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            clnt->ProcessDelete(hDlg);
            return TRUE;
        }
        case IDCANCEL: {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        }
        return FALSE;
    }
    return FALSE;
}

INT_PTR CALLBACK RuleDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    Client* clnt = Client::GetInstance();
    switch (iMessage) {
    case WM_INITDIALOG: {
        RECT rc, rcDlg, rcOwner;

        GetWindowRect(clnt->hMenuDlg, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        CopyRect(&rc, &rcOwner);

        OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
        OffsetRect(&rc, -rc.left, -rc.top);
        OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

        SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE);

        //GUIControl::AppendText(hDlg, IDC_EDIT1, captions[CAP_RULE]);
        HFONT hFontEdit = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, L"굴림");
        SendMessage(GetDlgItem(hDlg, IDC_EDIT1), WM_SETFONT, (WPARAM)hFontEdit, MAKELPARAM(FALSE, 0));

        SetDlgItemText(hDlg, IDC_EDIT1, captions[CAP_RULE].c_str());

        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        case IDCANCEL: {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        }
        return FALSE;
    }
    return FALSE;
}

void GetHandles(HWND hDlg) {
    Client* clnt = Client::GetInstance();

    clnt->hWaitText = GetDlgItem(hDlg, IDC_WAITTEXT);
    clnt->hNotify = GetDlgItem(hDlg, IDC_EDIT1);
    clnt->hBoard = GetDlgItem(hDlg, IDC_BOARD);
    clnt->hEsButton = GetDlgItem(hDlg, IDC_BUTTON1);
    clnt->hCzButton = GetDlgItem(hDlg, IDC_BUTTON2);
    clnt->hEsCount = GetDlgItem(hDlg, IDC_ESCOUNT);
    clnt->hCzCount = GetDlgItem(hDlg, IDC_CZCOUNT);

    clnt->hGameInfoBox = GetDlgItem(hDlg, IDC_GAMEINFOBOX);
    clnt->hMyInfoBox = GetDlgItem(hDlg, IDC_MYINFOBOX);
    clnt->hOpInfoBox = GetDlgItem(hDlg, IDC_OPINFOBOX);

    clnt->hMyIdRow = GetDlgItem(hDlg, IDC_MYIDROW);
    clnt->hMyLvRow = GetDlgItem(hDlg, IDC_MYLVROW);
    clnt->hOpIdRow = GetDlgItem(hDlg, IDC_OPIDROW);
    clnt->hOpLvRow = GetDlgItem(hDlg, IDC_OPLVROW);
    clnt->hTimerRow = GetDlgItem(hDlg, IDC_TIMERROW);
    clnt->hRoundRow = GetDlgItem(hDlg, IDC_ROUNDROW);
    clnt->hTurnRow = GetDlgItem(hDlg, IDC_TURNROW);
    clnt->hMyId = GetDlgItem(hDlg, IDC_MYID);
    clnt->hMyLv = GetDlgItem(hDlg, IDC_MYLV);
    clnt->hOpId = GetDlgItem(hDlg, IDC_OPPONENTID);
    clnt->hOpLv = GetDlgItem(hDlg, IDC_OPPONENTLV);
    clnt->hTimeCount = GetDlgItem(hDlg, IDC_TIMECOUNT);
    clnt->hRound = GetDlgItem(hDlg, IDC_ROUND);
    clnt->hTurn = GetDlgItem(hDlg, IDC_TURN);
}

void HideWindows() {
    Client* clnt = Client::GetInstance();

    ShowWindow(clnt->hWaitText, SW_HIDE);
    ShowWindow(clnt->hNotify, SW_HIDE);
    ShowWindow(clnt->hBoard, SW_HIDE);
    ShowWindow(clnt->hEsButton, SW_HIDE);
    ShowWindow(clnt->hCzButton, SW_HIDE);
    ShowWindow(clnt->hEsCount, SW_HIDE);
    ShowWindow(clnt->hCzCount, SW_HIDE);

    ShowWindow(clnt->hGameInfoBox, SW_HIDE);
    ShowWindow(clnt->hMyInfoBox, SW_HIDE);
    ShowWindow(clnt->hOpInfoBox, SW_HIDE);

    ShowWindow(clnt->hMyIdRow, SW_HIDE);
    ShowWindow(clnt->hMyLvRow, SW_HIDE);
    ShowWindow(clnt->hOpIdRow, SW_HIDE);
    ShowWindow(clnt->hOpLvRow, SW_HIDE);
    ShowWindow(clnt->hTimerRow, SW_HIDE);
    ShowWindow(clnt->hRoundRow, SW_HIDE);
    ShowWindow(clnt->hTurnRow, SW_HIDE);
    ShowWindow(clnt->hMyId, SW_HIDE);
    ShowWindow(clnt->hMyLv, SW_HIDE);
    ShowWindow(clnt->hOpId, SW_HIDE);
    ShowWindow(clnt->hOpLv, SW_HIDE);
    ShowWindow(clnt->hTimeCount, SW_HIDE);
    ShowWindow(clnt->hRound, SW_HIDE);
    ShowWindow(clnt->hTurn, SW_HIDE);
}

void ShowWindows() {
    Client* clnt = Client::GetInstance();

    ShowWindow(clnt->hNotify, SW_SHOW);
    ShowWindow(clnt->hBoard, SW_SHOW);
    ShowWindow(clnt->hEsButton, SW_SHOW);
    ShowWindow(clnt->hCzButton, SW_SHOW);
    ShowWindow(clnt->hEsCount, SW_SHOW);
    ShowWindow(clnt->hCzCount, SW_SHOW);

    ShowWindow(clnt->hGameInfoBox, SW_SHOW);
    ShowWindow(clnt->hMyInfoBox, SW_SHOW);
    ShowWindow(clnt->hOpInfoBox, SW_SHOW);

    ShowWindow(clnt->hMyIdRow, SW_SHOW);
    ShowWindow(clnt->hMyLvRow, SW_SHOW);
    ShowWindow(clnt->hOpIdRow, SW_SHOW);
    ShowWindow(clnt->hOpLvRow, SW_SHOW);
    ShowWindow(clnt->hTimerRow, SW_SHOW);
    ShowWindow(clnt->hRoundRow, SW_SHOW);
    ShowWindow(clnt->hTurnRow, SW_SHOW);
    ShowWindow(clnt->hMyId, SW_SHOW);
    ShowWindow(clnt->hMyLv, SW_SHOW);
    ShowWindow(clnt->hOpId, SW_SHOW);
    ShowWindow(clnt->hOpLv, SW_SHOW);
    ShowWindow(clnt->hTimeCount, SW_SHOW);
    ShowWindow(clnt->hRound, SW_SHOW);
    ShowWindow(clnt->hTurn, SW_SHOW);
}
#include "../include/Client.h"

Client* Client::mInstance = nullptr;

void Client::Run() {
    hCtzBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
    hEprBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
    hSlvBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));

    is_enabled = FALSE, was_played = FALSE, in_matching = FALSE;

    SendMessage(hEsButton, BM_SETIMAGE, 0, (LPARAM)hBckBitmap);
    
    InitNetworkManager();
}

void Client::InitNetworkManager() {
    mNm = NetworkManager::GetInstance();
    mNm->Init();
}

void Client::ProcessSignIn(HWND hDlg) {
    char id[30], pw[30];
    GetDlgItemTextA(hDlg, IDC_EDIT2, id, 20);
    GetDlgItemTextA(hDlg, IDC_EDIT1, pw, 20);

    currId = std::string(id);
    mNm->SendSignInPacket(id, pw);
}

void Client::ProcessSignUp(HWND hDlg) {
    char id[30], pw[30], repw[30];
    GetDlgItemTextA(hDlg, IDC_EDIT2, id, 20);
    GetDlgItemTextA(hDlg, IDC_EDIT1, pw, 20);
    GetDlgItemTextA(hDlg, IDC_EDIT3, repw, 20);

    mNm->SendSignUpPacket(id, pw, repw);
}

void Client::ProcessModify(HWND hDlg) {
    char pw[30], newPw[30], reNewPw[30];
    GetDlgItemTextA(hDlg, IDC_EDIT1, pw, 20);
    GetDlgItemTextA(hDlg, IDC_EDIT2, newPw, 20);
    GetDlgItemTextA(hDlg, IDC_EDIT4, reNewPw, 20);

    mNm->SendModifyPacket(currId, pw, newPw, reNewPw);
}

void Client::ProcessRecordInquiry() {
    mNm->SendRecordInquiryPacket(currId);
}

void Client::ProcessDelete(HWND hDlg) {
    char pw[30];
    GetDlgItemTextA(hDlg, IDC_EDIT1, pw, 20);

    mNm->SendDeletePacket(currId, pw);
}

void Client::ProcessSelectES() {
    if (gd.turn != gd.cards) {
        CAPTION(CAP_NOT_MYTURN)
        return;
    }
    Timer::KillTurnTimer(gd.turnTimerData);
    switch (gd.cards) {
    case CARD_EMPEROR:
        mNm->SendResPickCard(gd.roomNumber, CARD_EMPEROR);
        CAPTION(CAP_SUBMIT_EMPEROR)
        break;
    case CARD_SLAVE:
        mNm->SendResPickCard(gd.roomNumber, CARD_SLAVE);
        CAPTION(CAP_SUBMIT_SLAVE)
        break;
    }
    SetDlgItemTextA(hMainDlg, IDC_ESCOUNT, "0");
}

void Client::ProcessSelectCz() {
    if (gd.turn != gd.cards) {
        CAPTION(CAP_NOT_MYTURN)
        return;
    }
    if (gd.czCount == 0) {
        CAPTION(CAP_NO_CZCARD)
        return;
    }
    Timer::KillTurnTimer(gd.turnTimerData);
    --(gd.czCount);
    mNm->SendResPickCard(gd.roomNumber, CARD_CITIZEN);
    CAPTION(CAP_SUBMIT_CITIZEN)
    SetDlgItemTextA(hMainDlg, IDC_CZCOUNT, std::to_string(gd.czCount).c_str());
}

void Client::ProcessSignInResponse(int code) {
    switch (code) {
    case MSG_SUCCESS:
        EndDialog(hSignInDlg, 0);
        break;
    case MSG_FAILED:
        MessageBox(hSignInDlg, L"로그인 실패", L"알림", MB_OK);
        break;
    case MSG_ONLINE:
        MessageBox(hSignInDlg, L"이미 로그인된 사용자", L"알림", MB_OK);
        break;
    }
}

void Client::ProcessSignUpResponse(int code) {
    switch (code) {
    case MSG_REG_SUCCESS:
        MessageBox(hSignUpDlg, L"가입 성공", L"알림", MB_OK);
        EndDialog(hSignUpDlg, 0);
        break;
    case MSG_REG_PW_NOT_MATCHED:
        MessageBox(hSignUpDlg, L"비밀번호와 재확인 불일치", L"알림", MB_OK);
        break;
    case MSG_REG_ID_EXISTS:
        MessageBox(hSignUpDlg, L"ID 이미 존재", L"알림", MB_OK);
        break;
    case MSG_REG_ID_WRONGSZ:
        MessageBox(hSignUpDlg, L"ID 길이 문제", L"알림", MB_OK);
        break;
    case MSG_REG_PW_WRONGSZ:
        MessageBox(hSignUpDlg, L"비밀번호 길이 문제", L"알림", MB_OK);
        break;
    }
}

void Client::ProcessRecordInquiryResponse(uint16_t win, uint16_t lose, uint8_t winRatio) {
    SetDlgItemTextA(hRecordDlg, IDC_EDIT4, currId.c_str());
    SetDlgItemTextA(hRecordDlg, IDC_EDIT1, std::to_string(win).c_str());
    SetDlgItemTextA(hRecordDlg, IDC_EDIT2, std::to_string(lose).c_str());

    std::string myLv = "";
    if (winRatio < 10) myLv = "1";
    else if (10 <= winRatio && winRatio < 30) myLv = "2";
    else if (30 <= winRatio && winRatio < 60) myLv = "3";
    else if (60 <= winRatio && winRatio < 90) myLv = "4";
    else myLv = "777";
    SetDlgItemTextA(hRecordDlg, IDC_EDIT5, myLv.c_str());
}

void Client::ProcessModifyResponse(int code) {
    switch (code) {
    case MSG_SUCCESS:
        EndDialog(hModifyDlg, 0);
        break;
    case MSG_FAILED:
        MessageBox(HWND_DESKTOP, L"변경 실패", L"알림", MB_OK);
        break;
    }
}

void Client::ProcessDeleteResponse(int code) {
    switch (code) {
    case MSG_SUCCESS:
        MessageBox(hDeleteDlg, L"탈퇴 성공", L"알림", MB_OK);
        EndDialog(hDeleteDlg, 0);
        EndDialog(hMenuDlg, 0);
        EndDialog(hMainDlg, 0);
        break;
    case MSG_FAILED:
        MessageBox(hDeleteDlg, L"탈퇴 실패", L"알림", MB_OK);
        break;
    }
}

void Client::ProcessStartGame(
    uint16_t roomNumber,
    uint8_t cards,
    std::string opponentId,
    uint8_t beforeGameTimer,
    uint8_t turnTimer,
    uint8_t myRt,
    uint8_t opRt)
{
    InitGameData(roomNumber, cards, beforeGameTimer, turnTimer, opponentId, myRt, opRt);
    in_matching = FALSE;

    SetDlgItemTextA(hMainDlg, IDC_ROUND, std::to_string(gd.round).c_str());
    SetDlgItemText(hMainDlg, IDC_TURN, L"황제");

    CAPTION(CAP_GAMESTART)
    if(cards == CARD_EMPEROR) CAPTION(CAP_YOURCARD_EMPEROR)
    else CAPTION(CAP_YOURCARD_SLAVE)

    Timer::CreateBeforeGameTimer();
}

void Client::ProcessPickCard(uint8_t turn, uint8_t round) {
    Timer::KillTurnTimer(gd.turnTimerData);

    gd.turn = turn;
    gd.round = round;

    SetDlgItemTextA(hMainDlg, IDC_ROUND, std::to_string(gd.round).c_str());
    if (gd.turn == CARD_EMPEROR) {
        SetDlgItemText(hMainDlg, IDC_TURN, L"황제");
    }
    else {
        SetDlgItemText(hMainDlg, IDC_TURN, L"노예");
    }
    
    Timer::CreateTurnTimer();
    if(gd.turn != gd.cards) CAPTION(CAP_OPPONENT_TURN)
    else CAPTION(CAP_MY_TURN)
}

void Client::ProcessGameResult(uint8_t ePickedCard, uint8_t sPickedCard) {
    Timer::KillTurnTimer(gd.turnTimerData);
    int whoWin = (ePickedCard == CARD_EMPEROR) ?
        (sPickedCard == CARD_SLAVE ? 1 : 0) : (sPickedCard == CARD_CITIZEN ? -1 : 0);

    if (whoWin == -1) {
        CAPTION(CAP_DRAW)
    }
    else {
        EnableWindow(hEsButton, false);
        EnableWindow(hCzButton, false);
        if (whoWin == 0 && gd.cards == CARD_EMPEROR) CAPTION(CAP_WIN)
        else if (whoWin == 1 && gd.cards == CARD_SLAVE) CAPTION(CAP_WIN)
        else CAPTION(CAP_LOSE)
        Timer::CreateAfterGameTimer();
    }
}

void Client::ProcessOpponentDisconnected() {
    EnableWindow(hEsButton, false);
    EnableWindow(hCzButton, false);
    CAPTION(CAP_OPPONENT_DISCONNECTED)
    CAPTION(CAP_WIN)
    Timer::KillTurnTimer(gd.turnTimerData);
    Timer::CreateAfterGameTimer();
}

void Client::InitGameData(
    uint16_t roomNumber,
    uint8_t cards,
    uint8_t beforeGameTimer,
    uint8_t turnTimer,
    std::string opId,
    uint8_t myRt,
    uint8_t opRt)
{
    gd.roomNumber = roomNumber;
    gd.cards = cards;
    gd.esCount = 1;
    gd.czCount = 4;
    gd.round = 1;
    gd.turn = 1;
    gd.beforeGameTimer = beforeGameTimer;
    gd.turnTimer = turnTimer;
    gd.myId = currId;
    gd.opId = opId;
    gd.turnTimerData = nullptr;

    EnableWindow(hEsButton, false);
    EnableWindow(hCzButton, false);

    // ShowWindow(hWaitText, SW_HIDE);
    SetDlgItemTextA(hMainDlg, IDC_EDIT1, "");

    SetDlgItemTextA(hMainDlg, IDC_ESCOUNT, std::string("1").c_str());
    SetDlgItemTextA(hMainDlg, IDC_CZCOUNT, std::to_string(gd.czCount).c_str());

    SetDlgItemTextA(hMainDlg, IDC_OPPONENTID, opId.c_str());
    SetDlgItemTextA(hMainDlg, IDC_MYID, currId.c_str());

    // TODO: 승률에 따라 레벨 표시
    std::string myLv = "";
    if (myRt < 10) myLv = "1";
    else if (10 <= myRt && myRt < 30) myLv = "2";
    else if (30 <= myRt && myRt < 60) myLv = "3";
    else if (60 <= myRt && myRt < 90) myLv = "4";
    else myLv = "777";

    std::string opLv = "";
    if (opRt < 10) opLv = "1";
    else if (10 <= opRt && opRt < 30) opLv = "2";
    else if (30 <= opRt && opRt < 60) opLv = "3";
    else if (60 <= opRt && opRt < 90) opLv = "4";
    else opLv = "777";

    SetDlgItemTextA(hMainDlg, IDC_MYLV, myLv.c_str());
    SetDlgItemTextA(hMainDlg, IDC_OPPONENTLV, opLv.c_str());

    ShowWindow(hNotify, SW_SHOW);
    ShowWindow(hBoard, SW_SHOW);
    ShowWindow(hEsButton, SW_SHOW);
    ShowWindow(hCzButton, SW_SHOW);
    ShowWindow(hEsCount, SW_SHOW);
    ShowWindow(hCzCount, SW_SHOW);

    ShowWindow(hGameInfoBox, SW_SHOW);
    ShowWindow(hMyInfoBox, SW_SHOW);
    ShowWindow(hOpInfoBox, SW_SHOW);

    ShowWindow(hMyIdRow, SW_SHOW);
    ShowWindow(hMyLvRow, SW_SHOW);
    ShowWindow(hOpIdRow, SW_SHOW);
    ShowWindow(hOpLvRow, SW_SHOW);
    ShowWindow(hTimerRow, SW_SHOW);
    ShowWindow(hRoundRow, SW_SHOW);
    ShowWindow(hTurnRow, SW_SHOW);
    ShowWindow(hMyId, SW_SHOW);
    ShowWindow(hMyLv, SW_SHOW);
    ShowWindow(hOpId, SW_SHOW);
    ShowWindow(hOpLv, SW_SHOW);
    ShowWindow(hTimeCount, SW_SHOW);
    ShowWindow(hRound, SW_SHOW);
    ShowWindow(hTurn, SW_SHOW);

    SetTextColor(GetDC(hOpId), RGB(255, 0, 0));

    switch (cards) {
    case 0:
        SendMessage(hEsButton, BM_SETIMAGE, 0, (LPARAM)hEprBitmap);
        break;
    case 1:
        SendMessage(hEsButton, BM_SETIMAGE, 0, (LPARAM)hSlvBitmap);
        break;
    }
    SendMessage(hCzButton, BM_SETIMAGE, 0, (LPARAM)hCtzBitmap);
}
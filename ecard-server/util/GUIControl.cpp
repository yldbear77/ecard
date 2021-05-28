#include "GUIControl.h"

int GUIControl::onlineUserItemCount = 0;
int GUIControl::roomCount = 0;
int GUIControl::packetCount = 0;

void GUIControl::OnlineUserInsertItem(std::string sa) {
    HWND hList = Server::GetInstance()->hOnlineUserList;

    LVITEM LvItem;
    memset(&LvItem, 0, sizeof(LvItem));
    LvItem.mask = LVIF_TEXT;
    LvItem.cchTextMax = 256;

    std::wstring wsa; wsa.assign(sa.begin(), sa.end());

    int currentCount = onlineUserItemCount++;
    LvItem.iItem = currentCount;
    LvItem.iSubItem = 0;
    LvItem.pszText = (LPWSTR)wsa.c_str();
    ListView_InsertItem(hList, &LvItem);

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wsa.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);

    ListView_SetItemText(hList, idx, 2, (LPWSTR)L"접속 전");
}

void GUIControl::OnlineUserSetID(std::string sa, std::string id) {
    HWND hList = Server::GetInstance()->hOnlineUserList;
    std::wstring wid; wid.assign(id.begin(), id.end());
    std::wstring wsa; wsa.assign(sa.begin(), sa.end());

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wsa.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);
    ListView_SetItemText(hList, idx, 1, (LPWSTR)wid.c_str());
}

void GUIControl::OnlineUserSetStatus(std::string sa, char status) {
    HWND hList = Server::GetInstance()->hOnlineUserList;
    std::wstring wsa; wsa.assign(sa.begin(), sa.end());

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wsa.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);

    switch (status) {
    case 'M':
        ListView_SetItemText(hList, idx, 2, (LPWSTR)L"메인 메뉴");
        break;
    case 'Q':
        ListView_SetItemText(hList, idx, 2, (LPWSTR)L"게임 대기");
        break;
    case 'P':
        ListView_SetItemText(hList, idx, 2, (LPWSTR)L"게임 중");
        break;
    }
}

void GUIControl::OnlineUserDeleteItem(std::string sa) {
    HWND hList = Server::GetInstance()->hOnlineUserList;
    std::wstring wsa; wsa.assign(sa.begin(), sa.end());

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wsa.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);
    ListView_DeleteItem(hList, idx);
}

void GUIControl::RoomInsertItem(int roomNumber, std::string id1, std::string id2) {
    HWND hList = Server::GetInstance()->hRoomList;
    
    LVITEM LvItem;
    memset(&LvItem, 0, sizeof(LvItem));
    LvItem.mask = LVIF_TEXT;
    LvItem.cchTextMax = 256;

    std::wstring wrn = std::to_wstring(roomNumber);
    std::wstring wid1; wid1.assign(id1.begin(), id1.end());
    std::wstring wid2; wid2.assign(id2.begin(), id2.end());

    int currentCount = roomCount++;
    LvItem.iItem = currentCount;
    LvItem.iSubItem = 0;
    LvItem.pszText = (LPWSTR)wrn.c_str();
    ListView_InsertItem(hList, &LvItem);

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wrn.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);

    ListView_SetItemText(hList, idx, 1, (LPWSTR)wid1.c_str());
    ListView_SetItemText(hList, idx, 2, (LPWSTR)wid2.c_str());
    ListView_SetItemText(hList, idx, 3, (LPWSTR)L"황제");
    ListView_SetItemText(hList, idx, 4, (LPWSTR)L"1");
}

void GUIControl::RoomSetTurn(int roomNumber, int turn) {
    HWND hList = Server::GetInstance()->hRoomList;
    std::wstring wrn = std::to_wstring(roomNumber);

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wrn.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);

    if (turn == CARD_EMPEROR) {
        ListView_SetItemText(hList, idx, 3, (LPWSTR)L"황제");
    }
    else {
        ListView_SetItemText(hList, idx, 3, (LPWSTR)L"노예");
    }
}

void GUIControl::RoomSetRound(int roomNumber, int round) {
    HWND hList = Server::GetInstance()->hRoomList;
    std::wstring wrn = std::to_wstring(roomNumber);
    std::wstring wround = std::to_wstring(round);

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wrn.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);

    ListView_SetItemText(hList, idx, 4, (LPWSTR)wround.c_str());
}

void GUIControl::RoomDeleteItem(int roomNumber) {
    HWND hList = Server::GetInstance()->hRoomList;
    std::wstring wrn = std::to_wstring(roomNumber);

    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = wrn.c_str();
    int idx = ListView_FindItem(hList, -1, &fi);
    ListView_DeleteItem(hList, idx);
}

void GUIControl::PacketInsertItem(const int header, const std::string sa, const uint8_t* payload, const int size) {
    HWND hList = Server::GetInstance()->hPacketList;

    LVITEM LvItem;
    memset(&LvItem, 0, sizeof(LvItem));
    LvItem.mask = LVIF_TEXT;
    LvItem.cchTextMax = 256;

    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80] = { 0 };
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링

    std::wstring wtime(buf, &buf[80]);

    int currentCount = packetCount++;
    LvItem.iItem = currentCount;
    LvItem.iSubItem = 0;
    LvItem.pszText = (LPWSTR)wtime.c_str();
    ListView_InsertItem(hList, &LvItem);

    std::wstring wsa; wsa.assign(sa.begin(), sa.end());
    ListView_SetItemText(hList, currentCount, 1, (LPWSTR)wsa.c_str());

    const std::wstring headers[] = {
        L"REQ_SIGNIN", L"RES_SIGNIN", L"REQ_SIGNUP", L"RES_SIGNUP", L"REQ_MODIFY",
        L"RES_MODIFY", L"REQ_QUEUING", L"START_GAME", L"REQ_PICKCARD", L"RES_PICKCARD",
        L"GAME_RESULT", L"REQ_RECORD", L"RES_RECORD", L"REQ_DELETE", L"RES_DELETE"
    };
    ListView_SetItemText(hList, currentCount, 2, (LPWSTR)headers[header].c_str());

    const char hexBytes[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    std::wstring wpayload;
    for (int i = 0; i < size; ++i) {
        uint8_t data = payload[i];
        wpayload += hexBytes[(data & 0xF0) >> 4];
        wpayload += hexBytes[(data & 0x0F) >> 0];
    }
    ListView_SetItemText(hList, currentCount, 3, (LPWSTR)wpayload.c_str());
}
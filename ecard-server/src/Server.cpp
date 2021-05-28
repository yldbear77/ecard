#include "../include/Server.h"

Server* Server::mInstance = nullptr;
HWND Server::hMainDlg = 0;

void Server::Run() {
	InitNetworkManager();
	InitDBManager();
}

void Server::InitNetworkManager() {
	mNm = NetworkManager::GetInstance();
	mNm->Init();
}

void Server::InitDBManager() {
	mDb = DBManager::GetInstance();
	mDb->Init();
}

void Server::SavePacketLog() {
	OPENFILENAME ofn;
	TCHAR szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hMainDlg;
	ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = L"txt";

	if (GetSaveFileName(&ofn)) {
		HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			for (int i = 0; i < GUIControl::packetCount; ++i) {
				std::wstring output = L"";
				TCHAR tm[50] = { 0 }, addr[50] = { 0 }, hdr[50] = { 0 }, val[200] = { 0 };
				ListView_GetItemText(hPacketList, i, 0, tm, 50);
				output += tm; output += L"\t";
				ListView_GetItemText(hPacketList, i, 1, addr, 50);
				output += addr; output += L"\t\t";
				ListView_GetItemText(hPacketList, i, 2, hdr, 50);
				output += hdr; output += L"\t";
				ListView_GetItemText(hPacketList, i, 3, val, 200);
				output += val; output += L"\r\n";
				DWORD dwWritten;
				WriteFile(hFile, output.c_str(), output.size() * 2, &dwWritten, NULL);
			}
			CloseHandle(hFile);
		}
	}
}

void Server::SendNotification(TCHAR* msg) {
	mNm->SendNotification(msg);
}

int Server::GetWinRatio(int win, int lose) {
	int winRatio = 1;
	if (win + lose != 0) winRatio = ceil(win / (double)(win + lose) * 100);
	return winRatio;
}

void Server::SetOnlineUserInfo(const SocketAddress& sa) {
	GUIControl::OnlineUserInsertItem(sa.ToString());
}

void Server::SetOnlineUserId(const SocketAddress& sa, std::string id) {
	GUIControl::OnlineUserSetID(sa.ToString(), id);
}

void Server::SetOnlineUserStatus(const SocketAddress& sa, char status) {
	GUIControl::OnlineUserSetStatus(sa.ToString(), status);
}

void Server::DeleteOnlineUserInfo(const SocketAddress& sa) {
	GUIControl::OnlineUserDeleteItem(sa.ToString());
}

void Server::SetRoomInfo(const int rn, std::string id1, std::string id2) {
	GUIControl::RoomInsertItem(rn, id1, id2);
}

void Server::SetRoomTurn(const int rn, const int turn) {
	GUIControl::RoomSetTurn(rn, turn);
}

void Server::SetRoomRound(const int rn, const int round) {
	GUIControl::RoomSetRound(rn, round);
}

void Server::DeleteRoomInfo(const int rn) {
	GUIControl::RoomDeleteItem(rn);
}

void Server::SetPacketInfo(const int header, const SocketAddress &sa, const uint8_t* payload, const int size) {
	GUIControl::PacketInsertItem(header, sa.ToString(), payload, size);
}
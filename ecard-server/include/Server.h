#ifndef __SERVER_H__
#define __SERVER_H__

#include "../include/NetworkManager.h"
#include "../include/Config.h"
#include "../include/DBManager.h"

#include "../net/TCPSocket.h"
#include "../net/SocketAddress.h"

#include "../util/GUIControl.h"

class Server {
public:
	static Server* GetInstance() {
		if (mInstance == nullptr) {
			mInstance = new Server();
		}
		return mInstance;
	}

	static HWND hMainDlg;
	HWND hOnlineUserList, hRoomList, hPacketList;

	void Run();
	void InitNetworkManager();
	void InitDBManager();
	
	void SavePacketLog();

	void SendNotification(TCHAR* msg);

	int GetWinRatio(int win, int lose);

	void SetOnlineUserInfo(const SocketAddress& sa);
	void SetOnlineUserId(const SocketAddress& sa, std::string id);
	void SetOnlineUserStatus(const SocketAddress& sa, char status);
	void DeleteOnlineUserInfo(const SocketAddress& sa);

	void SetRoomInfo(const int rn, std::string id1, std::string id2);
	void SetRoomTurn(const int rn, const int turn);
	void SetRoomRound(const int rn, const int round);
	void DeleteRoomInfo(const int rn);

	void SetPacketInfo(const int header, const SocketAddress& sa, const uint8_t* payload, const int size);

private:
	static Server* mInstance;
	Server() {}

	NetworkManager* mNm;
	DBManager* mDb;
};

#endif
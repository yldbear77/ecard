#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include <WinSock2.h>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>

#include "PacketHeader.h"
#include "ClientData.h"
#include "DBManager.h"
#include "RoomManager.h"
#include "Server.h"

#include "../net/IOCP.h"
#include "../net/SocketUtil.h"
#include "../net/SocketAddress.h"
#include "../net/TCPSocket.h"
#include "../net/BitStream.h"

#include "../util/Logger.h"

class RoomManager;
class Server;

class NetworkManager {
public:
	static NetworkManager* GetInstance() {
		if (mInstance == nullptr) {
			mInstance = new NetworkManager();
		}
		return mInstance;
	}

	void Init();
	void CreateWorkers();

	void RegisterClientData(std::shared_ptr<TCPSocket> pClntSock, SocketAddress clntAddr);

	void ConstructPacket(std::string clntId, SocketAddress clntAddr, uint8_t* ioBuffer, int);
	void ProcessPacket(std::shared_ptr<ClientData> cd);

	void SendResQueuingPacket(short roomNumber, int cards, std::string id, short beforeGameTimer, short turnTimer, int myRt, int opRt, SocketAddress clntAddr);
	void SendReqPickCardPacket(uint16_t roomNumber, uint8_t turn, uint8_t round, SocketAddress clntAddr);
	void SendGameResultPacket(uint8_t ePickedCard, uint8_t sPickedCard, SocketAddress clntAddr);
	void SendNotification(TCHAR* msg);

	void SendOpponentDisconnected(SocketAddress clntAddr);

private:
	friend class TCPSocket;
	static NetworkManager* mInstance;
	NetworkManager() {}

	using IdToClientDataMap = std::unordered_map<std::string, std::shared_ptr<ClientData>>;
	using AddrToClientDataMap = std::unordered_map<SocketAddress, std::shared_ptr<ClientData>>;

	RoomManager* mRm;

	SYSTEM_INFO mSysInfo;

	std::shared_ptr<TCPSocket> mSock;
	uint16_t mPort;
	SocketAddress mAddr;

	IdToClientDataMap mIdToClientDataMap;
	AddrToClientDataMap mAddrToClientDataMap;

	std::unordered_map<std::string, bool> mOnlineMap;

	static void AcceptWorker(std::shared_ptr<TCPSocket> pSock, std::shared_ptr<IOCP> pIOCP);
	static void PacketWorker(std::shared_ptr<IOCP> pIOCP);

	void ProcessSignIn(InputBitStream& ibs, SocketAddress clntAddr);
	void ProcessSignUp(InputBitStream& ibs, SocketAddress clntAddr);
	void ProcessRecordInquiry(InputBitStream& ibs, SocketAddress clntAddr);
	void ProcessModify(InputBitStream& ibs, SocketAddress clntAddr);
	void ProcessDelete(InputBitStream& ibs, SocketAddress clntAddr);
	void ProcessQueuing(InputBitStream& ibs, std::shared_ptr<ClientData> cd);
	void ProcessPickCard(InputBitStream& ibs, ClientDataPtr cd);
};

#endif

#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include <string>
#include <thread>

#include "../include/Config.h"
#include "../include/PacketHeader.h"

#include "../net/BitStream.h"
#include "../net/SocketAddress.h"
#include "../net/SocketUtil.h"
#include "../net/TCPSocket.h"

class Client;

class NetworkManager {
public:
	static NetworkManager* GetInstance() {
		if (mInstance == nullptr) {
			mInstance = new NetworkManager();
		}
		return mInstance;
	}

	void Init();

	std::shared_ptr<TCPSocket> GetServSock() { return mServSock; }
	int GetReadBytes() { return mReadBytes; }
	int GetRequiredBytes() { return mPacketBuf[0]; }
	uint8_t* GetPacketBuffer() { return mPacketBuf; }
	void SetReadBytes(int n) { mReadBytes = n; }

	void ProcessPacket();

	void SendSignInPacket(std::string id, std::string pw);
	void SendSignUpPacket(std::string id, std::string pw, std::string repw);
	void SendRecordInquiryPacket(std::string id);
	void SendModifyPacket(std::string id, std::string pw, std::string newPw, std::string reNewPw);
	void SendDeletePacket(std::string id, std::string pw);
	void SendReqQueuingPacket();
	void SendResPickCard(uint16_t roomNumber, uint8_t picked);

	void ProcessSignIn(InputBitStream& ibs);
	void ProcessSignUp(InputBitStream& ibs);
	void ProcessRecordInquiry(InputBitStream& ibs);
	void ProcessModify(InputBitStream& ibs);
	void ProcessDelete(InputBitStream& ibs);
	void ProcessStartGame(InputBitStream& ibs);
	void ProcessPickCard(InputBitStream& ibs);
	void ProcessGameResult(InputBitStream& ibs);
	void ProcessNotification(InputBitStream& ibs);
	void ProcessOpponentDisconnected(InputBitStream& ibs);

private:
	static NetworkManager* mInstance;
	NetworkManager() {}

	Client* clnt;

	std::shared_ptr<TCPSocket> mServSock;
	uint16_t mPort;
	SocketAddress mServAddr;

	uint8_t mPacketBuf[MAX_BUF_SIZE];	// 패킷을 담을 버퍼
	int mReadBytes;						// 읽어들인 바이트 수

	static void ReceiveWorker();
};

#endif
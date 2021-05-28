#ifndef __CLIENT_DATA_H__
#define __CLIENT_DATA_H__

#include "../net/TCPSocket.h"
#include "../net/SocketAddress.h"

#include "Config.h"

class ClientData {
public:
	ClientData(std::shared_ptr<TCPSocket> sock, SocketAddress sockAddr) :
		mSock(sock),
		mSockAddr(sockAddr),
		mReadBytes(0),
		mId(""),
		mIsAuth(false),
		mInGame(false)
	{
		sock->pRecvExtOver = &mRecvExtOver;
	}
	int GetReadBytes() { return mReadBytes; }
	int GetRequiredBytes() { return mPacketBuf[0]; }
	uint8_t* GetPacketBuffer() { return mPacketBuf; }
	SocketAddress GetSocketAddress() { return mSockAddr; }
	std::string GetId() { return mId; }
	std::shared_ptr<TCPSocket> GetClntSock() { return mSock; }
	bool GetAuthenticated() { return mIsAuth; }
	bool GetInGame() { return mInGame; }

	void SetReadBytes(int n) { mReadBytes = n; }
	void SetId(std::string id) { mId = id; }
	void SetAuthenticated() { mIsAuth = true; }
	void UnsetAuthenticated() { mIsAuth = false; }
	void SetInGame() { mInGame = true; }
	void UnsetInGame() { mInGame = false; }

private:
	std::shared_ptr<TCPSocket> mSock;
	SocketAddress mSockAddr;
	std::string mId;

	bool mIsAuth;
	bool mInGame;

	EXT_OVERLAPPED mRecvExtOver;		// overlapped 구조체 확장
	uint8_t mPacketBuf[MAX_BUF_SIZE];	// 패킷을 담을 버퍼
	int mReadBytes;						// 읽어들인 바이트 수
};

typedef std::shared_ptr<ClientData> ClientDataPtr;

#endif
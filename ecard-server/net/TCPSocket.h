#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include <memory>

#include "SocketAddress.h"

#include "../include/Config.h"

class NetworkManager;
class Server;

typedef struct {
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[MAX_BUF_SIZE];
	int mode;
} EXT_OVERLAPPED, * LPEXT_OVERLAPPED;

class TCPSocket {
public:
	~TCPSocket() { closesocket(mSocket); }
	int Connect(const SocketAddress& inAddress);
	int Bind(const SocketAddress& inToAddress);
	int Listen(int inBackLog);
	std::shared_ptr<TCPSocket> Accept(SocketAddress& inFromAddress);
	int Send(const void* inData, int inLen);
	int Receive();
	void SetAddr(SocketAddress sa) { mAddr = sa; }

	SOCKET GetSocket() { return mSocket; }

private:
	static NetworkManager* mNm;

	friend class SocketUtil;
	friend class ClientData;
	TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET mSocket;
	SocketAddress mAddr;

	EXT_OVERLAPPED* pRecvExtOver;
};

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;

#endif

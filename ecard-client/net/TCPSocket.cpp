#include "TCPSocket.h"
#include "SocketUtil.h"

int TCPSocket::Connect(const SocketAddress& inAddress) {
	int err = connect(mSocket, &inAddress.mSockAddr, (int)inAddress.GetSize());
	if (err < 0) {
		SocketUtil::ReportError("TCPSocket::Connect");
		return -SocketUtil::GetLastError();
	}
	return 0;
}

int TCPSocket::Bind(const SocketAddress& inToAddress) {
	int err = bind(mSocket, &inToAddress.mSockAddr, (int)inToAddress.GetSize());
	if (err == SOCKET_ERROR) {
		SocketUtil::ReportError("TCPSocket::Bind");
		return -SocketUtil::GetLastError();
	}
	return 0;
}

int TCPSocket::Listen(int inBackLog) {
	int err = listen(mSocket, inBackLog);
	if (err == SOCKET_ERROR) {
		SocketUtil::ReportError("TCPSocket::Listen");
		return -SocketUtil::GetLastError();
	}
	return 0;
}

std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& inFromAddress) {
	socklen_t length = inFromAddress.GetSize();
	SOCKET sock = accept(mSocket, &inFromAddress.mSockAddr, &length);
	if (sock == INVALID_SOCKET) {
		SocketUtil::ReportError("TCPSocket::Accept");
		return nullptr;
	}
	return std::shared_ptr<TCPSocket>(new TCPSocket(sock));
}

int TCPSocket::Send(const void* inData, int inLen) {
	DWORD flags = 0;

	int sentBytes = send(mSocket, static_cast<const char*>(inData), inLen, flags);
	if (sentBytes < 0) {
		SocketUtil::ReportError("TCPSocket::Send");
		return -SocketUtil::GetLastError();
	}

	return sentBytes;
}

int TCPSocket::Receive(void* inData, int inLen) {
	DWORD flags = 0;

	int recvBytes = recv(mSocket, static_cast<char*>(inData), inLen, flags);
	if (recvBytes < 0) {
		SocketUtil::ReportError("TCPSocket::Receive");
		return -SocketUtil::GetLastError();
	}

	return recvBytes;
}
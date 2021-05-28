#include "TCPSocket.h"
#include "SocketUtil.h"

#include "../include/ClientData.h"
#include "../include/NetworkManager.h"
#include "../include/RoomManager.h"
#include "../include/Server.h"

NetworkManager* TCPSocket::mNm = NetworkManager::GetInstance();

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
	DWORD sentBytes = inLen, flags = 0;

	EXT_OVERLAPPED* pSendExtOver = new EXT_OVERLAPPED;
	memset(&(pSendExtOver->overlapped), 0, sizeof(OVERLAPPED));
	memcpy(static_cast<void*>(pSendExtOver->buffer), inData, inLen);
	pSendExtOver->wsaBuf.buf = pSendExtOver->buffer;
	pSendExtOver->wsaBuf.len = inLen;
	pSendExtOver->mode = MODE_SEND;

	int err = WSASend(mSocket, &(pSendExtOver->wsaBuf), 1, &sentBytes, flags, &(pSendExtOver->overlapped), NULL);
	if (err == SOCKET_ERROR) {
		int lastErr = SocketUtil::GetLastError();
		if (lastErr != WSA_IO_PENDING) {
			SocketUtil::ReportError("TCPSocket::Send");
			return -lastErr;
		}
	}

	return sentBytes;
}

int TCPSocket::Receive() {
	DWORD recvBytes = 0, flags = 0;

	memset(&(pRecvExtOver->overlapped), 0, sizeof(OVERLAPPED));
	pRecvExtOver->wsaBuf.buf = pRecvExtOver->buffer;
	pRecvExtOver->wsaBuf.len = sizeof(pRecvExtOver->buffer);
	pRecvExtOver->mode = MODE_READ;

	int err = WSARecv(mSocket, &(pRecvExtOver->wsaBuf), 1, &recvBytes, &flags, &(pRecvExtOver->overlapped), NULL);
	if (err == SOCKET_ERROR) {
		int lastErr = SocketUtil::GetLastError();
		if (lastErr != WSA_IO_PENDING) {
			SocketUtil::ReportError("TCPSocket::Receive");
			if (lastErr == WSAECONNRESET) {   // hard-close 상태
				mNm->mOnlineMap[mNm->mAddrToClientDataMap[mAddr]->GetId()] = false;  // 로그아웃 처리
				mNm->mAddrToClientDataMap[mAddr]->UnsetAuthenticated();  // 인증되지 않음
				if (mNm->mAddrToClientDataMap[mAddr]->GetInGame()) {
					// 남아 있는 상대방이 승리하도록 함
					RoomManager* rm = RoomManager::GetInstance();
					rm->HandleDisconnectedUser(mAddr);
				}
				mNm->mAddrToClientDataMap[mAddr]->UnsetInGame();  // 게임 중 아님
				mNm->mIdToClientDataMap.erase(mNm->mAddrToClientDataMap[mAddr]->GetId());
				mNm->mAddrToClientDataMap.erase(mAddr);

				// 서버측 클라이언트 정보에서 제거
				Server* sv = Server::GetInstance();
				sv->DeleteOnlineUserInfo(mAddr);
			}
			return -lastErr;
		}
	}

	return recvBytes;
}
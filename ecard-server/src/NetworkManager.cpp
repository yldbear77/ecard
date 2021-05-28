#include "../include/NetworkManager.h"
#include "../include/RoomManager.h"

NetworkManager* NetworkManager::mInstance = nullptr;

void NetworkManager::Init() {
	GetSystemInfo(&mSysInfo);
	mSock = SocketUtil::CreateTCPSocket();
	mPort = SERVER_PORT;
	mAddr = SocketAddress(SERVER_IP, SERVER_PORT);
	mSock->Bind(mAddr);
	mSock->Listen(15);
	mRm = RoomManager::GetInstance();
	CreateWorkers();
}

void NetworkManager::CreateWorkers() {
	std::shared_ptr<IOCP> pIOCP = SocketUtil::CreateIOCP();
	for (int i = 0; i < (int)mSysInfo.dwNumberOfProcessors; ++i) {
		std::thread(PacketWorker, pIOCP).detach();
	}
	std::thread(AcceptWorker, mSock, pIOCP).detach();
}

void NetworkManager::RegisterClientData(std::shared_ptr<TCPSocket> pClntSock, SocketAddress clntAddr) {
	std::shared_ptr<ClientData> newClient = std::make_shared<ClientData>(pClntSock, clntAddr);
	newClient->SetReadBytes(0);
	mAddrToClientDataMap[clntAddr] = newClient;
	LOG("Server Welcoming, new client %s", newClient->GetSocketAddress().ToString().c_str());
}

void NetworkManager::AcceptWorker(std::shared_ptr<TCPSocket> pServSock, std::shared_ptr<IOCP> pIOCP) {
	NetworkManager* nm = NetworkManager::GetInstance();
	Server* sv = Server::GetInstance();
	while (1) {
		SocketAddress clntAddr;
		std::shared_ptr<TCPSocket> pClntSock(pServSock->Accept(clntAddr));
		pClntSock->SetAddr(clntAddr);

		sv->SetOnlineUserInfo(clntAddr);

		pIOCP->ConnectSockToIOCP(pClntSock, clntAddr);
		nm->RegisterClientData(pClntSock, clntAddr);
		pClntSock->Receive();
	}
}

void NetworkManager::PacketWorker(std::shared_ptr<IOCP> pIOCP) {
	LPIOCP_KEY_DATA pKeyData;
	LPEXT_OVERLAPPED pExtOver;

	NetworkManager* nm = NetworkManager::GetInstance();

	while (1) {
		int32_t ioBytes = pIOCP->GetCompletion(pKeyData, pExtOver);
		switch (pExtOver->mode) {
		case MODE_READ: {
			if (ioBytes == 0) {
				// TODO: 클라이언트 연결 종료 처리, 현재 접속 중인 유저의 수 감소
				LOG("LOGGED OUT")
			}
			else {
				nm->ConstructPacket(pKeyData->id, pKeyData->clntAddr, (uint8_t*)pExtOver->buffer, ioBytes);
				pKeyData->pClntSock->Receive();
			}
			break;
		}
		case MODE_SEND: {
			delete pExtOver;
			// TODO: 전송 버퍼가 과도하게 찼을 경우, 해당 클라이언트와 연결 종료
			break;
		}
		}
	}
}

void NetworkManager::ConstructPacket(std::string clntId, SocketAddress clntAddr, uint8_t *ioBuffer, int remainingBytes) {
	std::shared_ptr<ClientData> cd = mAddrToClientDataMap[clntAddr];

	int readBytes = cd->GetReadBytes(), packetSize = 0;		// 패킷 전체의 크기: 패킷 크기(1-byte) + 헤더(1-byte) + 페이로드
	if (readBytes != 0) {
		packetSize = cd->GetRequiredBytes();
	}
	while (remainingBytes > 0) {
		if (packetSize == 0) {
			packetSize = ioBuffer[0];
		}
		/* 패킷을 완성하여 처리 가능 */
		if (packetSize <= cd->GetReadBytes() + remainingBytes) {
			memcpy(cd->GetPacketBuffer() + readBytes, ioBuffer, packetSize - readBytes);
			ProcessPacket(cd);
			ioBuffer += packetSize - readBytes;
			remainingBytes -= packetSize - readBytes;
			packetSize = 0;
			cd->SetReadBytes(0);
		}
		/* 패킷을 완성할 수 없으므로, 클라이언트 데이터에 백업 */
		else {
			memcpy(cd->GetPacketBuffer() + readBytes, ioBuffer, remainingBytes);
			cd->SetReadBytes(cd->GetReadBytes() + remainingBytes);
			remainingBytes = 0;
			// ioBuffer += remainingBytes;
		}
	}
}

void NetworkManager::ProcessPacket(std::shared_ptr<ClientData> cd) {
	uint8_t* payload = &(cd->GetPacketBuffer()[2]);
	uint8_t payloadSize = (cd->GetPacketBuffer()[0] - 2) * 8;
	InputBitStream ibs(payload, payloadSize);

	Server* sv = Server::GetInstance();
	sv->SetPacketInfo(cd->GetPacketBuffer()[1], cd->GetSocketAddress(), payload, payloadSize / 8);

	switch (cd->GetPacketBuffer()[1]) {
	/* 로그인 및 회원가입 */
	case REQ_SIGNIN: {
		ProcessSignIn(ibs, cd->GetSocketAddress());
		break;
	}
	case REQ_SIGNUP: {
		ProcessSignUp(ibs, cd->GetSocketAddress());
		break;
	}
	/* 전적 조회 */
	case REQ_RECORD: {
		ProcessRecordInquiry(ibs, cd->GetSocketAddress());
		break;
	}
	/* 회원정보 변경 */
	case REQ_MODIFY: {
		ProcessModify(ibs, cd->GetSocketAddress());
		break;
	}
	/* 회원정보 변경 */
	case REQ_DELETE: {
		ProcessDelete(ibs, cd->GetSocketAddress());
		break;
	}
	/* 게임 매칭 요청 및 진행 */
	case REQ_QUEUING: {
		ProcessQueuing(ibs, cd);
		break;
	}
	case RES_PICKCARD: {
		ProcessPickCard(ibs, cd);
		break;
	}
	}
}

void NetworkManager::ProcessSignIn(InputBitStream& ibs, SocketAddress clntAddr) {
	uint8_t idSize;			// 1-byte (A)
	char id[30] = { 0 };	// A-byte
	uint8_t pwSize;			// 1-byte (B)
	char pw[30] = { 0 };	// B-byte

	ibs.ReadBytes(static_cast<void*>(&idSize), 1);
	ibs.ReadBytes(static_cast<void*>(id), idSize);
	ibs.ReadBytes(static_cast<void*>(&pwSize), 1);
	ibs.ReadBytes(static_cast<void*>(pw), pwSize);

	DBManager* dm = DBManager::GetInstance();
	int result = dm->LoginCheck(id, pw);

	uint8_t packetSize = 0;
	packetSize += 1;           // 패킷 전체 크기 정보
	packetSize += 1;           // 패킷 헤더
	packetSize += 1;		   // 응답 코드
	const uint8_t packetHeader = RES_SIGNIN;
	OutputBitStream obs;

	if (idSize == 0 || pwSize == 0) {
		const uint8_t responseCode = MSG_FAILED;

		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);

		mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
		return;
	}

	if (mOnlineMap[std::string(id)]) {
		const uint8_t responseCode = MSG_ONLINE;

		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);

		mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
		return;
	}

	switch (result) {
	/* 로그인 성공 */
	case 0: {
		const uint8_t responseCode = MSG_SUCCESS;

		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);

		mOnlineMap[std::string(id)] = true;
		mAddrToClientDataMap[clntAddr]->SetId(std::string(id));
		mAddrToClientDataMap[clntAddr]->SetAuthenticated();
		mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
		mIdToClientDataMap[std::string(id)] = mAddrToClientDataMap[clntAddr];

		Server* sv = Server::GetInstance();
		sv->SetOnlineUserId(clntAddr, std::string(id));
		sv->SetOnlineUserStatus(clntAddr, 'M');
		break;
	}
	/* 로그인 실패 */
	case 1: {
		const uint8_t responseCode = MSG_FAILED;

		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);

		mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
		break;
	}
	}
}

void NetworkManager::ProcessSignUp(InputBitStream& ibs, SocketAddress clntAddr) {
	uint8_t idSize;
	char id[30] = { 0 };
	uint8_t pwSize;
	char pw[30] = { 0 };
	uint8_t rePwSize;
	char repw[30] = { 0 };

	ibs.ReadBytes(static_cast<void*>(&idSize), 1);
	ibs.ReadBytes(static_cast<void*>(id), idSize);
	ibs.ReadBytes(static_cast<void*>(&pwSize), 1);
	ibs.ReadBytes(static_cast<void*>(pw), pwSize);
	ibs.ReadBytes(static_cast<void*>(&rePwSize), 1);
	ibs.ReadBytes(static_cast<void*>(repw), rePwSize);

	DBManager* dm = DBManager::GetInstance();
	int result = dm->Register(id, pw, repw);

	uint8_t packetSize = 0;
	packetSize += 1;           // 패킷 전체 크기 정보
	packetSize += 1;           // 패킷 헤더
	packetSize += 1;		   // 응답 코드
	const uint8_t packetHeader = RES_SIGNUP;
	OutputBitStream obs;

	switch (result) {
	/* 가입 성공 */
	case 0: {
		const uint8_t responseCode = MSG_REG_SUCCESS;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	/* PW와 RePw 불일치 */
	case 1: {
		const uint8_t responseCode = MSG_REG_PW_NOT_MATCHED;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	/* 같은 ID가 이미 존재 */
	case 2: {
		const uint8_t responseCode = MSG_REG_ID_EXISTS;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	/* ID 길이 문제 */
	case 3: {
		const uint8_t responseCode = MSG_REG_ID_WRONGSZ;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	/* PW 길이 문제 */
	case 4: {
		const uint8_t responseCode = MSG_REG_PW_WRONGSZ;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	}
	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::ProcessRecordInquiry(InputBitStream& ibs, SocketAddress clntAddr) {
	uint8_t idSize;
	char id[30] = { 0 };

	ibs.ReadBytes(static_cast<void*>(&idSize), 1);
	ibs.ReadBytes(static_cast<void*>(id), idSize);

	int win = 0, lose = 0;
	DBManager* dm = DBManager::GetInstance();
	dm->RecordInquiry(id, &win, &lose);

	Server* sv = Server::GetInstance();
	uint8_t winRatio = sv->GetWinRatio(win, lose);

	uint8_t packetSize = 0;
	packetSize += 1;           // 패킷 전체 크기 정보
	packetSize += 1;           // 패킷 헤더
	packetSize += 2;		   // 승리 횟수
	packetSize += 2;		   // 패배 횟수
	packetSize += 1;		   // 승률
	const uint8_t packetHeader = RES_RECORD;
	OutputBitStream obs;

	obs.WriteBytes(&packetSize, 1);
	obs.WriteBytes(&packetHeader, 1);
	obs.WriteBytes(&win, 2);
	obs.WriteBytes(&lose, 2);
	obs.WriteBytes(&winRatio, 1);
	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::ProcessModify(InputBitStream& ibs, SocketAddress clntAddr) {
	uint8_t idSize;
	char id[30] = { 0 };
	uint8_t pwSize;
	char pw[30] = { 0 };
	uint8_t newPwSize;
	char newPw[30] = { 0 };
	uint8_t newRePwSize;
	char newRePw[30] = { 0 };

	ibs.ReadBytes(static_cast<void*>(&idSize), 1);
	ibs.ReadBytes(static_cast<void*>(id), idSize);
	ibs.ReadBytes(static_cast<void*>(&pwSize), 1);
	ibs.ReadBytes(static_cast<void*>(pw), pwSize);
	ibs.ReadBytes(static_cast<void*>(&newPwSize), 1);
	ibs.ReadBytes(static_cast<void*>(newPw), newPwSize);
	ibs.ReadBytes(static_cast<void*>(&newRePwSize), 1);
	ibs.ReadBytes(static_cast<void*>(newRePw), newRePwSize);

	DBManager* dm = DBManager::GetInstance();
	int result = dm->Modify(id, pw, newPw, newRePw);

	uint8_t packetSize = 0;
	packetSize += 1;           // 패킷 전체 크기 정보
	packetSize += 1;           // 패킷 헤더
	packetSize += 1;		   // 응답 코드
	const uint8_t packetHeader = RES_MODIFY;

	OutputBitStream obs;
	switch (result) {
	case 0: {
		const uint8_t responseCode = MSG_SUCCESS;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	case 1: {
		const uint8_t responseCode = MSG_FAILED;
		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);
		break;
	}
	}
	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::ProcessDelete(InputBitStream& ibs, SocketAddress clntAddr) {
	uint8_t idSize;			// 1-byte (A)
	char id[30] = { 0 };	// A-byte
	uint8_t pwSize;			// 1-byte (B)
	char pw[30] = { 0 };	// B-byte

	ibs.ReadBytes(static_cast<void*>(&idSize), 1);
	ibs.ReadBytes(static_cast<void*>(id), idSize);
	ibs.ReadBytes(static_cast<void*>(&pwSize), 1);
	ibs.ReadBytes(static_cast<void*>(pw), pwSize);

	DBManager* dm = DBManager::GetInstance();
	int result = dm->Delete(id, pw);

	uint8_t packetSize = 0;
	packetSize += 1;           // 패킷 전체 크기 정보
	packetSize += 1;           // 패킷 헤더
	packetSize += 1;		   // 응답 코드
	const uint8_t packetHeader = RES_DELETE;
	OutputBitStream obs;

	switch (result) {
	/* 회원탈퇴 성공 */
	case 0: {
		const uint8_t responseCode = MSG_SUCCESS;

		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);

		mAddrToClientDataMap[clntAddr]->SetId(std::string(id));
		mAddrToClientDataMap[clntAddr]->SetAuthenticated();
		mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
		mIdToClientDataMap[std::string(id)] = mAddrToClientDataMap[clntAddr];
		mOnlineMap.erase(std::string(id));
		break;
	}
	/* 회원탈퇴 실패 */
	case 1: {
		const uint8_t responseCode = MSG_FAILED;

		obs.WriteBits(&packetSize, 1 * 8);
		obs.WriteBits(&packetHeader, 1 * 8);
		obs.WriteBytes(&responseCode, 1);

		mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
		break;
	}
	}
}

void NetworkManager::ProcessQueuing(InputBitStream& ibs, std::shared_ptr<ClientData> cd) {
	Server* sv = Server::GetInstance();
	sv->SetOnlineUserStatus(cd->GetSocketAddress(), 'Q');
	mRm->EnqueueClient(cd);
}

void NetworkManager::ProcessPickCard(InputBitStream& ibs, ClientDataPtr cd) {
	uint16_t roomNumber;
	uint8_t picked;

	ibs.ReadBytes(static_cast<void*>(&roomNumber), 2);
	ibs.ReadBytes(static_cast<void*>(&picked), 1);

	mRm->UpdateStatus(roomNumber, picked, cd);
}

void NetworkManager::SendResQueuingPacket(
	short roomNumber,
	int cards,
	std::string id,
	short beforeGameTimer,
	short turnTimer,
	int myRt,
	int opRt,
	SocketAddress clntAddr)
{
	uint8_t packetSize = 0;
	packetSize += 1;			// 패킷 전체 크기 정보
	packetSize += 1;			// 패킷 헤더
	packetSize += 2;			// 방 번호
	packetSize += 1;			// 카드패
	packetSize += 1;			// 상대 ID 길이
	packetSize += id.size();	// 상대 ID
	packetSize += 1;			// 대기 타이머
	packetSize += 1;			// 턴 타이머
	packetSize += 1;			// 나의 승률
	packetSize += 1;			// 상대 승률
	const uint8_t packetHeader = START_GAME;

	uint8_t idSize = id.size();

	OutputBitStream obs;
	obs.WriteBytes(&packetSize, 1);
	obs.WriteBytes(&packetHeader, 1);
	obs.WriteBytes(&roomNumber, 2);
	obs.WriteBytes(&cards, 1);
	obs.WriteBytes(&idSize, 1);
	obs.WriteBytes(id.c_str(), idSize);
	obs.WriteBytes(&beforeGameTimer, 1);
	obs.WriteBytes(&turnTimer, 1);
	obs.WriteBytes(&myRt, 1);
	obs.WriteBytes(&opRt, 1);

	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendReqPickCardPacket(uint16_t roomNumber, uint8_t turn, uint8_t round, SocketAddress clntAddr) {
	uint8_t packetSize = 0;
	packetSize += 1;			// 패킷 전체 크기 정보
	packetSize += 1;			// 패킷 헤더
	packetSize += 2;			// 방 번호
	packetSize += 1;			// 차례
	packetSize += 1;			// 턴 번호
	const uint8_t packetHeader = REQ_PICKCARD;

	OutputBitStream obs;
	obs.WriteBytes(&packetSize, 1);
	obs.WriteBytes(&packetHeader, 1);
	obs.WriteBytes(&roomNumber, 2);
	obs.WriteBytes(&turn, 1);
	obs.WriteBytes(&round, 1);

	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendGameResultPacket(uint8_t ePickedCard, uint8_t sPickedCard, SocketAddress clntAddr) {
	uint8_t packetSize = 0;
	packetSize += 1;			// 패킷 전체 크기 정보
	packetSize += 1;			// 패킷 헤더
	packetSize += 1;			// 황제측 카드
	packetSize += 1;			// 노예측 카드
	const uint8_t packetHeader = GAME_RESULT;

	OutputBitStream obs;
	obs.WriteBytes(&packetSize, 1);
	obs.WriteBytes(&packetHeader, 1);
	obs.WriteBytes(&ePickedCard, 1);
	obs.WriteBytes(&sPickedCard, 1);

	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendNotification(TCHAR* msg) {
	std::wstring wmsg(msg);
	const uint8_t msgSize = wmsg.size() * 2;

	uint8_t packetSize = 0;
	packetSize += 1;		// 패킷 전체 크기 정보
	packetSize += 1;		// 패킷 헤더
	packetSize += 1;		// 메시지 크기 정보
	packetSize += msgSize;	// 메시지 크기
	const uint8_t packetHeader = NOTIFICATION;

	for (auto& clnt : mAddrToClientDataMap) {
		OutputBitStream obs;
		obs.WriteBytes(&packetSize, 1);
		obs.WriteBytes(&packetHeader, 1);
		obs.WriteBytes(&msgSize, 1);
		obs.WriteBytes(&(wmsg[0]), msgSize);
		clnt.second->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
	}
}

void NetworkManager::SendOpponentDisconnected(SocketAddress clntAddr) {
	uint8_t packetSize = 0;
	packetSize += 1;		// 패킷 전체 크기 정보
	packetSize += 1;		// 패킷 헤더
	const uint8_t packetHeader = OPPONENT_DISCONNECTED;

	OutputBitStream obs;
	obs.WriteBytes(&packetSize, 1);
	obs.WriteBytes(&packetHeader, 1);
	mAddrToClientDataMap[clntAddr]->GetClntSock()->Send(obs.GetBufferPtr(), obs.GetByteLength());
}
#include "../include/NetworkManager.h"
#include "../include/Client.h"

NetworkManager* NetworkManager::mInstance = nullptr;

void NetworkManager::Init() {
    mServSock = SocketUtil::CreateTCPSocket();
    SocketAddress servAddr(SERVER_IP, SERVER_PORT);
    if (mServSock->Connect(servAddr) != 0) {
        MessageBox(Client::GetInstance()->hSignInDlg, L"서버와의 연결에 실패", L"알림", MB_OK);
        exit(0);
    }
    mReadBytes = 0;
    clnt = Client::GetInstance();
    std::thread(ReceiveWorker).detach();
}

void NetworkManager::ProcessPacket() {
    uint8_t* payload = &(mPacketBuf[2]);
    uint8_t payloadSize = (mPacketBuf[0] - 2) * 8;
    InputBitStream ibs(payload, payloadSize);
    switch (mPacketBuf[1]) {
    case RES_SIGNIN: {
        ProcessSignIn(ibs);
        break;
    }
    case RES_SIGNUP: {
        ProcessSignUp(ibs);
        break;
    }
    case RES_RECORD: {
        ProcessRecordInquiry(ibs);
        break;
    }
    case RES_MODIFY: {
        ProcessModify(ibs);
        break;
    }
    case RES_DELETE: {
        ProcessDelete(ibs);
        break;
    }
    case START_GAME: {
        ProcessStartGame(ibs);
        break;
    }
    case REQ_PICKCARD: {
        ProcessPickCard(ibs);
        break;
    }
    case GAME_RESULT: {
        ProcessGameResult(ibs);
        break;
    }
    case NOTIFICATION: {
        ProcessNotification(ibs);
        break;
    }
    case OPPONENT_DISCONNECTED: {
        ProcessOpponentDisconnected(ibs);
        break;
    }
    }
}

void NetworkManager::SendSignInPacket(std::string id, std::string pw) {
    uint8_t packetSize = 0;
    packetSize += 1;            // 패킷 전체 크기 정보
    packetSize += 1;            // 패킷 헤더
    packetSize += 1;            // id 크기 정보
    packetSize += id.size();    // id 크기
    packetSize += 1;            // pw 크기 정보
    packetSize += pw.size();    // pw
    uint8_t packetHeader = REQ_SIGNIN;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);
    obs.WriteBits(static_cast<uint8_t>(id.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&id[0]), id.size());
    obs.WriteBits(static_cast<uint8_t>(pw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&pw[0]), pw.size());

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendSignUpPacket(std::string id, std::string pw, std::string repw) {
    uint8_t packetSize = 0;
    packetSize += 1;           // 패킷 전체 크기 정보
    packetSize += 1;           // 패킷 헤더
    packetSize += 1;           // id 크기 정보
    packetSize += id.size();   // id 크기
    packetSize += 1;           // pw 크기 정보
    packetSize += pw.size();   // pw 크기
    packetSize += 1;           // repw 크기 정보
    packetSize += repw.size();   // re-pw
    uint8_t packetHeader = REQ_SIGNUP;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);
    obs.WriteBits(static_cast<uint8_t>(id.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&id[0]), id.size());
    obs.WriteBits(static_cast<uint8_t>(pw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&pw[0]), pw.size());
    obs.WriteBits(static_cast<uint8_t>(repw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&repw[0]), repw.size());

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendRecordInquiryPacket(std::string currId) {
    uint8_t packetSize = 0;
    packetSize += 1;              // 패킷 전체 크기 정보
    packetSize += 1;              // 패킷 헤더
    packetSize += 1;              // id 크기 정보
    packetSize += currId.size();  // id 크기
    const uint8_t packetHeader = REQ_RECORD;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);
    obs.WriteBits(static_cast<uint8_t>(currId.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&currId[0]), currId.size());

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendModifyPacket(std::string id, std::string pw, std::string newPw, std::string reNewPw) {
    uint8_t packetSize = 0;
    packetSize += 1;           // 패킷 전체 크기 정보
    packetSize += 1;           // 패킷 헤더
    packetSize += 1;           // id 크기 정보
    packetSize += id.size();   // id 크기
    packetSize += 1;           // pw 크기 정보
    packetSize += pw.size();   // pw 크기
    packetSize += 1;            // newPw 크기 정보
    packetSize += newPw.size(); // newpw 크기
    packetSize += 1;            // reNewPw 크기 정보
    packetSize += reNewPw.size(); // reNewPw 크기
    uint8_t packetHeader = REQ_MODIFY;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);
    obs.WriteBits(static_cast<uint8_t>(id.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&id[0]), id.size());
    obs.WriteBits(static_cast<uint8_t>(pw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&pw[0]), pw.size());
    obs.WriteBits(static_cast<uint8_t>(newPw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&newPw[0]), newPw.size());
    obs.WriteBits(static_cast<uint8_t>(reNewPw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&reNewPw[0]), reNewPw.size());

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendDeletePacket(std::string id, std::string pw) {
    uint8_t packetSize = 0;
    packetSize += 1;           // 패킷 전체 크기 정보
    packetSize += 1;           // 패킷 헤더
    packetSize += 1;           // id 크기 정보
    packetSize += id.size();   // id 크기
    packetSize += 1;           // pw 크기 정보
    packetSize += pw.size();   // pw 크기
    uint8_t packetHeader = REQ_DELETE;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);
    obs.WriteBits(static_cast<uint8_t>(id.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&id[0]), id.size());
    obs.WriteBits(static_cast<uint8_t>(pw.size()), 1 * 8);
    obs.WriteBytes(static_cast<void*>(&pw[0]), pw.size());

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendReqQueuingPacket() {
    uint8_t packetSize = 0;
    packetSize += 1;
    packetSize += 1;
    uint8_t packetHeader = REQ_QUEUING;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::SendResPickCard(uint16_t roomNumber, uint8_t picked) {
    uint8_t packetSize = 0;
    packetSize += 1;           // 패킷 전체 크기 정보
    packetSize += 1;           // 패킷 헤더
    packetSize += 2;           // 방 번호
    packetSize += 1;           // 선택한 카드
    uint8_t packetHeader = RES_PICKCARD;

    OutputBitStream obs;
    obs.WriteBits(&packetSize, 1 * 8);
    obs.WriteBits(&packetHeader, 1 * 8);
    obs.WriteBytes(static_cast<void*>(&roomNumber), 2);
    obs.WriteBytes(static_cast<void*>(&picked), 1);

    mServSock->Send(obs.GetBufferPtr(), obs.GetByteLength());
}

void NetworkManager::ProcessSignIn(InputBitStream& ibs) {
    uint8_t code = -1;
    ibs.ReadBytes(static_cast<void*>(&code), 1);
    clnt->ProcessSignInResponse(code);
}

void NetworkManager::ProcessSignUp(InputBitStream& ibs) {
    uint8_t code = -1;
    ibs.ReadBytes(static_cast<void*>(&code), 1);
    clnt->ProcessSignUpResponse(code);
}

void NetworkManager::ProcessRecordInquiry(InputBitStream& ibs) {
    uint16_t win, lose, winRatio;
    ibs.ReadBytes(static_cast<void*>(&win), 2);
    ibs.ReadBytes(static_cast<void*>(&lose), 2);
    ibs.ReadBytes(static_cast<void*>(&winRatio), 1);
    clnt->ProcessRecordInquiryResponse(win, lose, winRatio);
}

void NetworkManager::ProcessModify(InputBitStream& ibs) {
    uint8_t code = -1;
    ibs.ReadBytes(static_cast<void*>(&code), 1);
    clnt->ProcessModifyResponse(code);
}

void NetworkManager::ProcessDelete(InputBitStream& ibs) {
    uint8_t code = -1;
    ibs.ReadBytes(static_cast<void*>(&code), 1);
    clnt->ProcessDeleteResponse(code);
}

void NetworkManager::ProcessStartGame(InputBitStream& ibs) {
    uint16_t roomNumber;
    uint8_t cards, idSize, beforeGameTimer, turnTimer, myRt, opRt;
    char opponentId[30] = { 0 };

    ibs.ReadBytes(static_cast<void*>(&roomNumber), 2);
    ibs.ReadBytes(static_cast<void*>(&cards), 1);
    ibs.ReadBytes(static_cast<void*>(&idSize), 1);
    ibs.ReadBytes(static_cast<void*>(opponentId), idSize);
    ibs.ReadBytes(static_cast<void*>(&beforeGameTimer), 1);
    ibs.ReadBytes(static_cast<void*>(&turnTimer), 1);
    ibs.ReadBytes(static_cast<void*>(&myRt), 1);
    ibs.ReadBytes(static_cast<void*>(&opRt), 1);

    clnt->ProcessStartGame(roomNumber, cards, opponentId, beforeGameTimer, turnTimer, myRt, opRt);
}

void NetworkManager::ProcessPickCard(InputBitStream& ibs) {
    uint16_t roomNumber;
    uint8_t turn;
    uint8_t round;

    ibs.ReadBytes(static_cast<void*>(&roomNumber), 2);
    ibs.ReadBytes(static_cast<void*>(&turn), 1);
    ibs.ReadBytes(static_cast<void*>(&round), 1);

    clnt->ProcessPickCard(turn, round);
}

void NetworkManager::ProcessGameResult(InputBitStream& ibs) {
    uint8_t ePickedCard;
    uint8_t sPickedCard;

    ibs.ReadBytes(static_cast<void*>(&ePickedCard), 1);
    ibs.ReadBytes(static_cast<void*>(&sPickedCard), 1);

    clnt->ProcessGameResult(ePickedCard, sPickedCard);
}

void NetworkManager::ProcessNotification(InputBitStream& ibs) {
    uint8_t msgSize;
    TCHAR msg[150] = { 0 };

    ibs.ReadBytes(static_cast<void*>(&msgSize), 1);
    ibs.ReadBytes(static_cast<void*>(msg), msgSize);

    MessageBox(HWND_DESKTOP, msg, L"서버 메시지", MB_OK);
}

void NetworkManager::ProcessOpponentDisconnected(InputBitStream& ibs) {
    clnt->ProcessOpponentDisconnected();
}

void NetworkManager::ReceiveWorker() {
    NetworkManager* nm = NetworkManager::GetInstance();

    uint8_t ioBuffer[MAX_BUF_SIZE];
    uint8_t* bufPtr;
    while (1) {
        int remainingBytes = nm->GetServSock()->Receive(static_cast<void*>(ioBuffer), MAX_BUF_SIZE);
        int readBytes = nm->GetReadBytes(), packetSize = 0;		// 패킷 전체의 크기: 패킷 크기(1-byte) + 헤더(1-byte) + 페이로드
        bufPtr = ioBuffer;
        if (readBytes != 0) {
            packetSize = nm->GetRequiredBytes();
        }
        while (remainingBytes > 0) {
            if (packetSize == 0) {
                packetSize = bufPtr[0];
            }
            /* 패킷을 완성하여 처리 가능 */
            if (packetSize <= nm->GetReadBytes() + remainingBytes) {
                memcpy(nm->GetPacketBuffer() + readBytes, bufPtr, packetSize - readBytes);
                nm->ProcessPacket();
                bufPtr += packetSize - readBytes;
                remainingBytes -= packetSize - readBytes;
                packetSize = 0;
                nm->SetReadBytes(0);
            }
            /* 패킷을 완성할 수 없으므로, 클라이언트 데이터에 백업 */
            else {
                memcpy(nm->GetPacketBuffer() + readBytes, bufPtr, remainingBytes);
                nm->SetReadBytes(nm->GetReadBytes() + remainingBytes);
                remainingBytes = 0;
                // ioBuffer += remainingBytes;
            }
        }
    }
}
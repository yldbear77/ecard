#include "../include/RoomManager.h"

RoomManager* RoomManager::mInstance = nullptr;
std::unordered_map<int, RoomPtr> RoomManager::mRoomMap = std::unordered_map<int, RoomPtr>();

RoomManager::RoomManager() :
	mRoomCount(0)
{
	mNm = NetworkManager::GetInstance();
}

void RoomManager::EnqueueClient(std::shared_ptr<ClientData> cd) {
	mMtx.lock();
	mWaitingQueue.push(cd);
	if (mWaitingQueue.size() >= 2) {
		ClientDataPtr cd1 = mWaitingQueue.front(); mWaitingQueue.pop();
		ClientDataPtr cd2 = mWaitingQueue.front(); mWaitingQueue.pop();

		bool isAuth1 = cd1->GetAuthenticated(), isAuth2 = cd2->GetAuthenticated();
		if (isAuth1 && isAuth2) {
			Server* sv = Server::GetInstance();
			sv->SetOnlineUserStatus(cd1->GetSocketAddress(), 'P');
			sv->SetOnlineUserStatus(cd2->GetSocketAddress(), 'P');
			CreateRoom(cd1, cd2);
		}
		else if(isAuth1 || isAuth2) {
			if (isAuth1) mWaitingQueue.push(cd1);
			else mWaitingQueue.push(cd2);
		}
	}
	mMtx.unlock();
}

void RoomManager::CreateRoom(ClientDataPtr cd1, ClientDataPtr cd2) {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, 1);

	int cards1 = dist(mt), cards2;
	if (cards1 == CARD_EMPEROR) cards2 = CARD_SLAVE;
	else cards2 = CARD_EMPEROR;

	mRoomMap[++mRoomCount] = std::make_shared<Room>(mRoomCount, cd1, cd2, cards1, cards2);

	DBManager* dm = DBManager::GetInstance();

	int id1Win = 0, id1Lose = 0;
	dm->RecordInquiry(cd1->GetId().c_str(), &id1Win, &id1Lose);

	int id2Win = 0, id2Lose = 0;
	dm->RecordInquiry(cd2->GetId().c_str(), &id2Win, &id2Lose);

	int id1Ratio = 1;
	if (id1Win + id1Lose != 0) id1Ratio = ceil(id1Win / (double)(id1Win + id1Lose) * 100);
	int id2Ratio = 1;
	if (id2Win + id2Lose != 0) id2Ratio = ceil(id2Win / (double)(id2Win + id2Lose) * 100);

	mNm->SendResQueuingPacket(
		mRoomCount,
		cards1,
		cd2->GetId(),
		SEC_BEFOREGAME,
		SEC_EACHTURN,
		id1Ratio,
		id2Ratio,
		cd1->GetSocketAddress()
	);
	mNm->SendResQueuingPacket(
		mRoomCount,
		cards2,
		cd1->GetId(),
		SEC_BEFOREGAME,
		SEC_EACHTURN,
		id2Ratio,
		id1Ratio,
		cd2->GetSocketAddress()
	);

	mAddrToRoomNumberMap[cd1->GetSocketAddress()] = mRoomCount;
	mAddrToRoomNumberMap[cd2->GetSocketAddress()] = mRoomCount;

	cd1->SetInGame();
	cd2->SetInGame();

	Timer::CreateBeforeGameTimer(mRoomCount);

	Server* sv = Server::GetInstance();
	sv->SetRoomInfo(mRoomCount, cd1->GetId(), cd2->GetId());
}

bool RoomManager::VerifyUser(uint16_t roomNumber, uint8_t picked, ClientDataPtr cd) {
	RoomPtr r = mRoomMap[roomNumber];

	if (r->mCd[0] != cd && r->mCd[1] != cd) return false;
	uint8_t idx;
	if (r->mCd[0] == cd) idx = 0;
	else idx = 1;
	if (r->mTurn != r->mCards[idx]) return false;
	if (picked == CARD_CITIZEN && r->mCzCount[idx] == 0) return false;
	else if (picked == CARD_EMPEROR || picked == CARD_SLAVE) {
		if (r->mCards[idx] != picked) return false;
	}
	return true;
}

void RoomManager::UpdateStatus(uint16_t roomNumber, uint8_t picked, ClientDataPtr cd) {
	RoomPtr r = mRoomMap[roomNumber];

	if (!VerifyUser(roomNumber, picked, cd)) return;

	if (r->mCd[0] == cd) {
		r->mSubmitted[0] = picked;
		if (r->mSubmitted[0] == CARD_CITIZEN) --(r->mCzCount[0]);
	}
	else {
		r->mSubmitted[1] = picked;
		if (r->mSubmitted[1] == CARD_CITIZEN) --(r->mCzCount[1]);
	}

	SocketAddress clntAddr1 = RoomManager::mRoomMap[roomNumber]->mCd[0]->GetSocketAddress();
	SocketAddress clntAddr2 = RoomManager::mRoomMap[roomNumber]->mCd[1]->GetSocketAddress();
	Server* sv = Server::GetInstance();

	Timer::KillTurnTimer(r->mTurnTimerData);
	if (r->mSubmitted[0] != NOT_SUBMITTED && r->mSubmitted[1] != NOT_SUBMITTED) {
		++(r->mRound);
		if (r->mRound % 2 == 0) r->mTurn = CARD_SLAVE;
		else r->mTurn = CARD_EMPEROR;

		int whoWin = -1;
		if (r->mCards[0] == CARD_EMPEROR) {
			whoWin = (r->mSubmitted[0] == CARD_EMPEROR) ?
				(r->mSubmitted[1] == CARD_SLAVE ? 1 : 0) : (r->mSubmitted[1] == CARD_CITIZEN ? -1 : 0);
		}
		else {
			whoWin = (r->mSubmitted[0] == CARD_SLAVE) ?
				(r->mSubmitted[1] == CARD_EMPEROR ? 1 : 0) : (r->mSubmitted[1] == CARD_CITIZEN ? -1 : 0);
		}

		if (r->mCards[0] == CARD_EMPEROR) {
			mNm->SendGameResultPacket(r->mSubmitted[0], r->mSubmitted[1], clntAddr1);
			mNm->SendGameResultPacket(r->mSubmitted[0], r->mSubmitted[1], clntAddr2);
		}
		else {
			mNm->SendGameResultPacket(r->mSubmitted[1], r->mSubmitted[0], clntAddr1);
			mNm->SendGameResultPacket(r->mSubmitted[1], r->mSubmitted[0], clntAddr2);
		}
		r->mSubmitted[0] = r->mSubmitted[1] = NOT_SUBMITTED;

		if (whoWin == -1) {
			mNm->SendReqPickCardPacket(roomNumber, r->mTurn, r->mRound, clntAddr1);
			mNm->SendReqPickCardPacket(roomNumber, r->mTurn, r->mRound, clntAddr2);

			Timer::CreateTurnTimer(r->mRoomNumber);

			sv->SetRoomTurn(roomNumber, r->mTurn);
			sv->SetRoomRound(roomNumber, r->mRound);
		}
		else {
			DBManager* dm = DBManager::GetInstance();

			if (whoWin == 0 && r->mCards[0] == CARD_EMPEROR) {
				dm->WriteRecord(r->mCd[0]->GetId().c_str(), r->mCd[1]->GetId().c_str());
			}
			else if (whoWin == 0 && r->mCards[0] == CARD_SLAVE) {
				dm->WriteRecord(r->mCd[1]->GetId().c_str(), r->mCd[0]->GetId().c_str());
			}
			else if (whoWin == 1 && r->mCards[0] == CARD_EMPEROR) {
				dm->WriteRecord(r->mCd[1]->GetId().c_str(), r->mCd[0]->GetId().c_str());
			}
			else if(whoWin == 1 && r->mCards[0] == CARD_SLAVE) {
				dm->WriteRecord(r->mCd[0]->GetId().c_str(), r->mCd[1]->GetId().c_str());
			}

			r->mCd[0]->UnsetInGame();
			r->mCd[1]->UnsetInGame();

			sv->SetOnlineUserStatus(clntAddr1, 'M');
			sv->SetOnlineUserStatus(clntAddr2, 'M');
			sv->DeleteRoomInfo(roomNumber);
		}
	}
	else {
		if (r->mTurn == CARD_EMPEROR) r->mTurn = CARD_SLAVE;
		else r->mTurn = CARD_EMPEROR;
	
		mNm->SendReqPickCardPacket(roomNumber, r->mTurn, r->mRound, clntAddr1);
		mNm->SendReqPickCardPacket(roomNumber, r->mTurn, r->mRound, clntAddr2);
		Timer::CreateTurnTimer(r->mRoomNumber);
		sv->SetRoomTurn(roomNumber, r->mTurn);
	}
}

void RoomManager::HandleDisconnectedUser(SocketAddress sa) {
	int roomNumber = mAddrToRoomNumberMap[sa];

	DBManager* dm = DBManager::GetInstance();
	RoomPtr r = mRoomMap[roomNumber];

	if (r->mCd[0]->GetSocketAddress() == sa) {  // cd1ÀÌ ½Â¸®		
		mNm->SendOpponentDisconnected(r->mCd[1]->GetSocketAddress());
		dm->WriteRecord(r->mCd[1]->GetId().c_str(), r->mCd[0]->GetId().c_str());
	}
	else {  // cd0ÀÌ ½Â¸®
		mNm->SendOpponentDisconnected(r->mCd[0]->GetSocketAddress());
		dm->WriteRecord(r->mCd[0]->GetId().c_str(), r->mCd[1]->GetId().c_str());
	}

	r->mCd[0]->UnsetInGame();
	r->mCd[1]->UnsetInGame();

	mAddrToRoomNumberMap.erase(r->mCd[0]->GetSocketAddress());
	mAddrToRoomNumberMap.erase(r->mCd[1]->GetSocketAddress());

	Server* sv = Server::GetInstance();
	sv->DeleteRoomInfo(roomNumber);
}
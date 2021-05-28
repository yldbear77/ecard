#include "../include/Timer.h"

HWND Timer::hMainDlg = 0;

void Timer::CreateBeforeGameTimer(uint16_t roomNumber) {
	TimerData* d = new TimerData;

	d->mRoomNumber = roomNumber;
	d->mCount = SEC_BEFOREGAME;
	SetTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d), 1000, BeforeGameTimerProc);
}

void Timer::CreateTurnTimer(uint16_t roomNumber) {
	TimerData* d = new TimerData;

	d->mRoomNumber = roomNumber;
	d->mCount = SEC_EACHTURN;
	RoomManager::mRoomMap[roomNumber]->mTurnTimerData = d;
	SetTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d), 1000, TurnTimerProc);
}

void Timer::KillBeforeGameTimer(TimerData* d) {
	if (d != nullptr) {
		::KillTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d));
		delete d;
	}
}

void Timer::KillTurnTimer(TimerData *d) {
	if (d != nullptr) {
		::KillTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d));
		RoomManager::mRoomMap[d->mRoomNumber]->mTurnTimerData = nullptr;
		delete d;
	}
}

VOID CALLBACK Timer::BeforeGameTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	TimerData* d = reinterpret_cast<TimerData*>(idEvent);
	--(d->mCount);
	if (d->mCount == 0) {
		NetworkManager* nm = NetworkManager::GetInstance();
		uint8_t turn = RoomManager::mRoomMap[d->mRoomNumber]->mTurn;
		uint8_t round = RoomManager::mRoomMap[d->mRoomNumber]->mRound;

		SocketAddress clntAddr1 = RoomManager::mRoomMap[d->mRoomNumber]->mCd[0]->GetSocketAddress();
		SocketAddress clntAddr2 = RoomManager::mRoomMap[d->mRoomNumber]->mCd[1]->GetSocketAddress();
		
		nm->SendReqPickCardPacket(d->mRoomNumber, turn, round, clntAddr1);
		nm->SendReqPickCardPacket(d->mRoomNumber, turn, round, clntAddr2);
		CreateTurnTimer(d->mRoomNumber);

		KillBeforeGameTimer(d);
	}
}

VOID CALLBACK Timer::TurnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	TimerData* d = reinterpret_cast<TimerData*>(idEvent);
	--(d->mCount);
	if (d->mCount == 0) {
		uint8_t pickedCard = NOT_SUBMITTED;
		std::shared_ptr<ClientData> cd;

		if (RoomManager::mRoomMap[d->mRoomNumber]->mTurn == CARD_EMPEROR) {
			pickedCard = CARD_EMPEROR;
			if (RoomManager::mRoomMap[d->mRoomNumber]->mCards[0] == CARD_EMPEROR) {
				cd = RoomManager::mRoomMap[d->mRoomNumber]->mCd[0];
			}
			else cd = RoomManager::mRoomMap[d->mRoomNumber]->mCd[1];
		}
		else {
			pickedCard = CARD_SLAVE;
			if (RoomManager::mRoomMap[d->mRoomNumber]->mCards[0] == CARD_SLAVE) {
				cd = RoomManager::mRoomMap[d->mRoomNumber]->mCd[0];
			}
			else cd = RoomManager::mRoomMap[d->mRoomNumber]->mCd[1];
		}
		RoomManager::GetInstance()->UpdateStatus(d->mRoomNumber, pickedCard, cd);
		KillTurnTimer(d);
	}
}
#ifndef __ROOM_MANAGER_H__
#define __ROOM_MANAGER_H__

#include <queue>
#include <random>
#include <unordered_map>
#include <atomic>
#include <mutex>

#include "Config.h"
#include "Server.h"
#include "ClientData.h"
#include "NetworkManager.h"
#include "Timer.h"
#include "DBManager.h"

#include "../net/SocketAddress.h"

class TimerData;

class Room {
public:
	Room() {}
	Room(uint16_t roomNumber, ClientDataPtr cd1, ClientDataPtr cd2, uint8_t cards1, uint8_t cards2) :
		mRoomNumber(roomNumber),
		mTurn(0),
		mRound(1)
	{
		mCd[0] = cd1;
		mCd[1] = cd2;
		mCards[0] = cards1;
		mCards[1] = cards2;
		mSubmitted[0] = NOT_SUBMITTED;
		mSubmitted[1] = NOT_SUBMITTED;
	}

	uint16_t mRoomNumber;
	ClientDataPtr mCd[2];
	uint8_t mCards[2];
	uint8_t mCzCount[2];
	uint8_t mTurn;
	uint8_t mRound;
	TimerData* mTurnTimerData;

	uint8_t mSubmitted[2];
};

typedef std::shared_ptr<Room> RoomPtr;

class RoomManager {
public:
	static RoomManager* GetInstance() {
		if (mInstance == nullptr) {
			mInstance = new RoomManager;
		}
		return mInstance;
	}

	static std::unordered_map<int, RoomPtr> mRoomMap;

	void EnqueueClient(std::shared_ptr<ClientData> cd);
	void CreateRoom(ClientDataPtr cd1, ClientDataPtr cd2);

	bool VerifyUser(uint16_t roomNumber, uint8_t picked, ClientDataPtr cd);
	void UpdateStatus(uint16_t roomNumber, uint8_t picked, ClientDataPtr cd);
	void HandleDisconnectedUser(SocketAddress sa);

private:
	friend class NetworkManager;

	static RoomManager* mInstance;
	RoomManager();

	NetworkManager* mNm;
	
	std::mutex mMtx;
	std::queue<std::shared_ptr<ClientData>> mWaitingQueue;
	std::unordered_map<SocketAddress, int> mAddrToRoomNumberMap;

	unsigned short mRoomCount;
};

#endif
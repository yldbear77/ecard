#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <thread>
#include <string>

#include "../resource.h"

#include "PacketHeader.h"
#include "Config.h"
#include "NetworkManager.h"
#include "Timer.h"
#include "Caption.h"

#include "../net/TCPSocket.h"
#include "../net/SocketUtil.h"
#include "../net/SocketAddress.h"
#include "../net/BitStream.h"

#include "../util/StringUtil.h"
#include "../util/GUIControl.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

class TimerData;

class Client {
private:
	class GameData {
	public:
		uint16_t roomNumber;
		uint8_t cards;
		uint8_t esCount;
		uint8_t czCount;
		uint8_t turn;
		uint8_t round;
		uint8_t beforeGameTimer;
		uint8_t turnTimer;
		std::string myId;
		std::string opId;
		TimerData* turnTimerData;
	};

public:
	static Client* GetInstance() {
		if (mInstance == nullptr) {
			mInstance = new Client();
		}
		return mInstance;
	}

	HWND hMainDlg, hSignInDlg, hSignUpDlg, hMenuDlg, hRecordDlg, hModifyDlg, hDeleteDlg;
	HWND hWaitText;
	HWND hBoard, hNotify, hEsButton, hCzButton, hEsCount, hCzCount;
	HWND hGameInfoBox, hMyInfoBox, hOpInfoBox;
	HWND hMyIdRow, hMyLvRow, hOpIdRow, hOpLvRow, hTimerRow, hRoundRow, hTurnRow, hMyId, hMyLv, hOpId, hOpLv, hTimeCount, hRound, hTurn;

	HBITMAP hEprBitmap, hSlvBitmap, hCtzBitmap, hBckBitmap;
	BOOL is_enabled, was_played, in_matching;

	void Run();
	void InitNetworkManager();

	void ProcessSignIn(HWND hDlg);
	void ProcessSignUp(HWND hDlg);
	void ProcessModify(HWND hDlg);
	void ProcessRecordInquiry();
	void ProcessDelete(HWND hDlg);

	/* button handler */
	void ProcessQueuing() { mNm->SendReqQueuingPacket(); }
	void ProcessSelectES();
	void ProcessSelectCz();

	void ProcessSignInResponse(int code);
	void ProcessSignUpResponse(int code);
	void ProcessRecordInquiryResponse(uint16_t win, uint16_t lose, uint8_t winRatio);
	void ProcessModifyResponse(int code);
	void ProcessDeleteResponse(int code);
	void ProcessStartGame(uint16_t, uint8_t, std::string, uint8_t, uint8_t, uint8_t, uint8_t);
	void ProcessPickCard(uint8_t turn, uint8_t round);
	void ProcessGameResult(uint8_t ePickedCard, uint8_t sPickedCard);
	void ProcessOpponentDisconnected();

	uint8_t GetTurn() { return gd.turn; }
	uint8_t GetMyCards() { return gd.cards; }
	uint8_t GetBeforeGameTimer() { return gd.beforeGameTimer; }
	uint8_t GetTurnTimer() { return gd.turnTimer; }
	TimerData* GetTurnTimerData() { return gd.turnTimerData; }
	std::string GetId() { return currId; }
	void SetTurnTimerData(TimerData* td) { gd.turnTimerData = td; }

	void InitGameData(uint16_t, uint8_t, uint8_t, uint8_t, std::string, uint8_t, uint8_t);

private:
	static Client* mInstance;
	Client() {}

	NetworkManager* mNm;
	GameData gd;

	std::string currId;
};

#endif
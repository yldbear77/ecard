#ifndef __TIMER_H__
#define __TIMER_H__

#include <winsock2.h>
#include <string>
#include <unordered_map>

#include "../include/Config.h"
#include "../include/NetworkManager.h"
#include "../include/RoomManager.h"

#include "../util/Logger.h"

class TimerData {
public:
	uint16_t mRoomNumber;
	uint8_t mCount;
};

class Timer {
public:
	static HWND hMainDlg;

	static void CreateBeforeGameTimer(uint16_t roomNumber);
	static void CreateTurnTimer(uint16_t roomNumber);

	static void KillBeforeGameTimer(TimerData* d);
	static void KillTurnTimer(TimerData *d);

private:
	static VOID CALLBACK BeforeGameTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static VOID CALLBACK TurnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
};

#endif
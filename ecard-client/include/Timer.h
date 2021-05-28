#ifndef __TIMER_H__
#define __TIMER_H__

#include <winsock2.h>
#include <string>

#include "../resource.h"

#include "../include/Client.h"

extern INT_PTR CALLBACK MenuDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

class TimerData {
public:
	uint8_t mCount;
};

class Timer {
public:
	static HWND hMainDlg;

	static void CreateBeforeGameTimer();
	static void CreateTurnTimer();
	static void CreateAfterGameTimer();

	static void KillBeforeGameTimer(TimerData* d);
	static void KillTurnTimer(TimerData *d);
	static void KillAfterGameTimer(TimerData* d);

private:
	static VOID CALLBACK BeforeGameTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static VOID CALLBACK TurnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static VOID CALLBACK AfterGameTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	static Client* clnt;
};

#endif
#include "../include/Timer.h"

HWND Timer::hMainDlg = 0;
Client* Timer::clnt = Client::GetInstance();

void Timer::CreateBeforeGameTimer() {
	ShowWindow(clnt->hWaitText, SW_HIDE);
	SetDlgItemTextA(hMainDlg, IDC_TIMECOUNT, std::to_string(Client::GetInstance()->GetBeforeGameTimer()).c_str());
	TimerData* d = new TimerData;
	d->mCount = clnt->GetBeforeGameTimer();
	SetTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d), 1000, BeforeGameTimerProc);
}

void Timer::CreateTurnTimer() {
	SetDlgItemTextA(hMainDlg, IDC_TIMECOUNT, std::to_string(Client::GetInstance()->GetTurnTimer()).c_str());
	TimerData* d = new TimerData;
	d->mCount = clnt->GetTurnTimer();
	Client::GetInstance()->SetTurnTimerData(d);
	SetTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d), 1000, TurnTimerProc);
}

void Timer::CreateAfterGameTimer() {
	SetDlgItemTextA(hMainDlg, IDC_TIMECOUNT, std::to_string(SEC_AFTERGAME).c_str());
	TimerData* d = new TimerData;
	d->mCount = SEC_AFTERGAME;
	SetTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d), 1000, AfterGameTimerProc);
}

void Timer::KillBeforeGameTimer(TimerData* d) {
	if (d != nullptr) {
		::KillTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d));
		EnableWindow(clnt->hEsButton, true);
		EnableWindow(clnt->hCzButton, true);
		delete d;
	}
}

void Timer::KillTurnTimer(TimerData *d) {
	if (d != nullptr) {
		::KillTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d));
		if (clnt->GetTurn() == clnt->GetMyCards()) {
			//SetDlgItemTextA(hMainDlg, IDC_ESCOUNT, "0");
			//if (clnt->GetMyCards() == CARD_SLAVE) CAPTION(CAP_SUBMIT_SLAVE)
			//else CAPTION(CAP_SUBMIT_EMPEROR)
		}
		Client::GetInstance()->SetTurnTimerData(nullptr);
		delete d;
	}
}

void Timer::KillAfterGameTimer(TimerData* d) {
	if (d != nullptr) {
		::KillTimer(hMainDlg, reinterpret_cast<UINT_PTR>(d));
		delete d;
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG4), clnt->hMainDlg, MenuDlgProc);
	}
}

VOID CALLBACK Timer::BeforeGameTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	TimerData* d = reinterpret_cast<TimerData*>(idEvent);
	--(d->mCount);
	SetDlgItemTextA(hWnd, IDC_TIMECOUNT, std::to_string(d->mCount).c_str());
	if (d->mCount == 0) {
		KillBeforeGameTimer(d);
	}
}

VOID CALLBACK Timer::TurnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	TimerData* d = reinterpret_cast<TimerData*>(idEvent);
	--(d->mCount);
	SetDlgItemTextA(hWnd, IDC_TIMECOUNT, std::to_string(d->mCount).c_str());
	if (d->mCount == 0) {
		KillTurnTimer(d);
	}
}

VOID CALLBACK Timer::AfterGameTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	TimerData* d = reinterpret_cast<TimerData*>(idEvent);
	--(d->mCount);
	SetDlgItemTextA(hWnd, IDC_TIMECOUNT, std::to_string(d->mCount).c_str());
	if (d->mCount == 0) {
		KillAfterGameTimer(d);
	}
}
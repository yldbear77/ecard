#ifndef __CAPTION_H__
#define __CAPTION_H__

#include <string>

enum {
	CAP_GAMESTART,
	CAP_NO_CZCARD,
	CAP_NOT_MYTURN,
	CAP_YOURCARD_EMPEROR,
	CAP_YOURCARD_SLAVE,
	CAP_OPPONENT_TURN,
	CAP_MY_TURN,
	CAP_DRAW,
	CAP_WIN,
	CAP_LOSE,
	CAP_SUBMIT_EMPEROR,
	CAP_SUBMIT_SLAVE,
	CAP_SUBMIT_CITIZEN,
	CAP_RULE,
	CAP_OPPONENT_DISCONNECTED
};

const std::wstring captions[] = {
	L"[*] 게임이 곧 시작합니다.\n",
	L"[*] 시민 카드가 없습니다.\n",
	L"[*] 나의 차례가 아닙니다.\n",
	L"[*] 당신의 카드패는 \"황제\" 입니다.\n",
	L"[*] 당신의 카드패는 \"노예\" 입니다.\n",
	L"[*] 상대방 차례입니다.\n",
	L"[*] 나의 차례입니다.\n",
	L"[*] 비겼습니다. 다음 라운드를 계속합니다.\n",
	L"[*] 이겼습니다. 게임이 곧 종료됩니다.\n",
	L"[*] 졌습니다. 게임이 곧 종료됩니다.\n",
	L"[*] \"황제\" 카드를 제출했습니다.\n",
	L"[*] \"노예\" 카드를 제출했습니다.\n",
	L"[*] \"시민\" 카드를 제출했습니다.\n",
	L"[*] 황제패와 노예패가 있으며, 각 패는 황제 1장, 시민 4장 그리고 노예 1장, 시민 4장으로 구성된다.\r\n" \
	"[*] 1, 3번째 턴은 황제패, 2, 4번째 턴은 노예패를 보유한 유저가 먼저 카드를 낸다.\r\n" \
	"[*] 두 사용자가 카드를 모두 내면 아래의 항목에 따라 승패가 결정된다.\r\n" \
	"[*] 황제 > 시민 > 노예 > 황제\r\n"\
	"[*] 타임 아웃 시에 황제패는 황제 카드, 노예패는 노예 카드가 강제로 제출됨\r\n\r\n"\
	"[*] 승률에 따른 등급 (소수점 승률은 반올림)\r\n"\
	"  - 승률 < 10% ---> 1\r\n"\
	"  - 10% <= 승률 < 30% ---> 2\r\n"\
	"  - 30% <= 승률 < 60% ---> 3\r\n"\
	"  - 60% <= 승률 < 90% ---> 4\r\n"\
	"  - 90% <= 승률 ---> 777\r\n",
	L"[*] 상대가 게임에서 나갔습니다.\n"
};

#define CAPTION(C) GUIControl::AppendText(hMainDlg, IDC_EDIT1, captions[C]);

#endif
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
	L"[*] ������ �� �����մϴ�.\n",
	L"[*] �ù� ī�尡 �����ϴ�.\n",
	L"[*] ���� ���ʰ� �ƴմϴ�.\n",
	L"[*] ����� ī���д� \"Ȳ��\" �Դϴ�.\n",
	L"[*] ����� ī���д� \"�뿹\" �Դϴ�.\n",
	L"[*] ���� �����Դϴ�.\n",
	L"[*] ���� �����Դϴ�.\n",
	L"[*] �����ϴ�. ���� ���带 ����մϴ�.\n",
	L"[*] �̰���ϴ�. ������ �� ����˴ϴ�.\n",
	L"[*] �����ϴ�. ������ �� ����˴ϴ�.\n",
	L"[*] \"Ȳ��\" ī�带 �����߽��ϴ�.\n",
	L"[*] \"�뿹\" ī�带 �����߽��ϴ�.\n",
	L"[*] \"�ù�\" ī�带 �����߽��ϴ�.\n",
	L"[*] Ȳ���п� �뿹�а� ������, �� �д� Ȳ�� 1��, �ù� 4�� �׸��� �뿹 1��, �ù� 4������ �����ȴ�.\r\n" \
	"[*] 1, 3��° ���� Ȳ����, 2, 4��° ���� �뿹�и� ������ ������ ���� ī�带 ����.\r\n" \
	"[*] �� ����ڰ� ī�带 ��� ���� �Ʒ��� �׸� ���� ���а� �����ȴ�.\r\n" \
	"[*] Ȳ�� > �ù� > �뿹 > Ȳ��\r\n"\
	"[*] Ÿ�� �ƿ� �ÿ� Ȳ���д� Ȳ�� ī��, �뿹�д� �뿹 ī�尡 ������ �����\r\n\r\n"\
	"[*] �·��� ���� ��� (�Ҽ��� �·��� �ݿø�)\r\n"\
	"  - �·� < 10% ---> 1\r\n"\
	"  - 10% <= �·� < 30% ---> 2\r\n"\
	"  - 30% <= �·� < 60% ---> 3\r\n"\
	"  - 60% <= �·� < 90% ---> 4\r\n"\
	"  - 90% <= �·� ---> 777\r\n",
	L"[*] ��밡 ���ӿ��� �������ϴ�.\n"
};

#define CAPTION(C) GUIControl::AppendText(hMainDlg, IDC_EDIT1, captions[C]);

#endif
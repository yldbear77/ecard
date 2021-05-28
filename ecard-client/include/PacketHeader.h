#ifndef __PACKET_HEADER_H__
#define __PACKET_HEADER_H__

#define REQ_SIGNIN				0
#define RES_SIGNIN				1
#define REQ_SIGNUP				2
#define RES_SIGNUP				3
#define REQ_RECORD				11
#define RES_RECORD				12
#define REQ_MODIFY				4
#define RES_MODIFY				5
#define REQ_DELETE				13
#define RES_DELETE				14

#define REQ_QUEUING				6
#define START_GAME				7

#define REQ_PICKCARD			8
#define RES_PICKCARD			9
#define GAME_RESULT				10
#define OPPONENT_DISCONNECTED	16

#define NOTIFICATION			15

#define MSG_SUCCESS				0
#define MSG_FAILED				1
#define MSG_ONLINE				3

/* register */
#define MSG_REG_SUCCESS			0
#define MSG_REG_PW_NOT_MATCHED	1
#define MSG_REG_ID_EXISTS		2
#define MSG_REG_ID_WRONGSZ		3
#define MSG_REG_PW_WRONGSZ		4

/* card */
#define CARD_EMPEROR			0
#define CARD_SLAVE				1
#define CARD_CITIZEN			2
#define NOT_SUBMITTED			3

#endif
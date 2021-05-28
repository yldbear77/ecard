#ifndef __CONFIG_H__
#define __CONFIG_H__

/* network configuration */
#define SERVER_IP		"172.20.13.34"	// È¸»ç IP
#define SERVER_PORT		9001
#define MAX_BUF_SIZE	1024
#define MAX_CLIENTS		50			// not used now

/* IOCP configuration */
#define MODE_READ		0
#define MODE_SEND		1

/* game */
#define CARD_EMPEROR	0
#define CARD_SLAVE		1
#define CARD_CITIZEN	2

#define SEC_BEFOREGAME	5
#define SEC_EACHTURN	30

#endif
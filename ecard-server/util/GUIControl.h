#ifndef __GUI_CONTROL_H__
#define __GUI_CONTROL_H__

#include <winsock2.h>
#include <commctrl.h>
#include <string>
#include <unordered_map>

#include "../include/Server.h"

class GUIControl {
public:
	static void OnlineUserInsertItem(std::string sa);
	static void OnlineUserSetID(std::string sa, std::string id);
	static void OnlineUserSetStatus(std::string sa, char status);
	static void OnlineUserDeleteItem(std::string sa);

	static int onlineUserItemCount;

	static void RoomInsertItem(int roomNumber, std::string id1, std::string id2);
	static void RoomSetTurn(int roomNumber, int turn);
	static void RoomSetRound(int roomNumber, int round);
	static void RoomDeleteItem(int roomNumber);

	static int roomCount;

	static void PacketInsertItem(const int header, const std::string sa, const uint8_t* payload, const int size);

	static int packetCount;
};

#endif
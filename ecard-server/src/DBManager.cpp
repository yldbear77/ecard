#include "../include/DBManager.h"

DBManager* DBManager::mInstance = nullptr;

void DBManager::Init() {
	otl_connect::otl_initialize();
	db.rlogon(
		"DRIVER={SQL Server};\
					SERVER=C1QADB-DEV;\
					PORT=1433;UID=EstGameDev;\
					PWD=tprP^dudrnr95;\
					DATABASE=PilotProject;\
					DSN=PilotProject"
	);
}

int DBManager::LoginCheck(const char* id, const char* pw) {
	try {
		int result = -1;

		otl_stream o(1,
			"{call PilotProject.DM_LoginCheck("
			" :iId<char[30],in>, "
			" :iPw<char[30],in>, "
			" :iResult<int,out> "
			")};",
			db
		);

		o << id << pw;
		o >> result;

		return result;
	}
	catch (otl_exception& p) {
		LOG("DBManager::LoginCheck")
		LOG(std::to_string(p.code).c_str());
		LOG((char*)p.sqlstate);
	}
}

int DBManager::Register(const char* id, const char* pw, const char* repw) {
	try {
		int result = -1;

		otl_stream o(1,
			"{call PilotProject.DM_Register("
			" :iId<char[30],in>, "
			" :iPw<char[30],in>, "
			" :iRePw<char[30],in>, "
			" :iResult<int,out> "
			")};",
			db
		);

		o << id << pw << repw;
		o >> result;

		return result;
	}
	catch (otl_exception& p) {
		LOG("DBManager::Register")
		LOG(std::to_string(p.code).c_str());
		LOG((char*)p.sqlstate);
	}
}

void DBManager::WriteRecord(const char* winId, const char* loseId) {
	try {
		otl_stream o(1,
			"{call PilotProject.DM_WriteRecord("
			" :iWinId<char[30],in>, "
			" :iLoseId<char[30],in> "
			")};",
			db
		);

		o << winId << loseId;
	}
	catch (otl_exception& p) {
		LOG("DBManager::WriteRecord")
		LOG(std::to_string(p.code).c_str());
		LOG((char*)p.sqlstate);
	}
}

void DBManager::RecordInquiry(const char* id, int *win, int *lose) {
	try {
		otl_stream o(1,
			"{call PilotProject.DM_RecordInquiry("
			" :iId<char[30],in>, "
			" :iWin<int,out>, "
			" :iLose<int,out> "
			")};",
			db
		);

		o << id;
		o >> *win >> *lose;
	}
	catch (otl_exception& p) {
		LOG("DBManager::RecordInquiry")
		LOG(std::to_string(p.code).c_str());
		LOG((char*)p.sqlstate);
	}
}

int DBManager::Modify(const char* id, const char* pw, const char* newPw, const char* newRePw) {
	try {
		int result = -1;

		otl_stream o(1,
			"{call PilotProject.DM_Modify("
			" :iId<char[30],in>, "
			" :iPw<char[30],in>, "
			" :iNewPw<char[30],in>, "
			" :iNewRePw<char[30],in>, "
			" :iResult<int,output> "
			")};",
			db
		);

		o << id << pw << newPw << newRePw;
		o >> result;

		return result;
	}
	catch (otl_exception& p) {
		LOG("DBManager::Modify")
			LOG(std::to_string(p.code).c_str());
		LOG((char*)p.sqlstate);
	}
}

int DBManager::Delete(const char* id, const char* pw) {
	try {
		int result = -1;

		otl_stream o(1,
			"{call PilotProject.DM_Delete("
			" :iId<char[30],in>, "
			" :iPw<char[30],in>, "
			" :iResult<int,output> "
			")};",
			db
		);

		o << id << pw;
		o >> result;

		return result;
	}
	catch (otl_exception& p) {
		LOG("DBManager::Delete")
			LOG(std::to_string(p.code).c_str());
		LOG((char*)p.sqlstate);
	}
}
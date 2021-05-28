#ifndef __DATABASE_H__
#define __DATABASE_H__

#define OTL_ODBC_MSSQL_2008

#include <string>

#include "../util/otlv4.h"
#include "../util/Logger.h"

class DBManager {
public:
	static DBManager* GetInstance() {
		if (mInstance == nullptr) {
			mInstance = new DBManager();
		}
		return mInstance;
	}

	void Init();
	int LoginCheck(const char* id, const char* pw);
	int Register(const char* id, const char* pw, const char* repw);
	void WriteRecord(const char* winId, const char* loseId);
	void RecordInquiry(const char* id, int *win, int *lose);
	int Modify(const char* id, const char* pw, const char* newPw, const char* newRePw);
	int Delete(const char* id, const char* pw);

private:
	static DBManager* mInstance;
	DBManager() {}

	otl_connect db;
};

#endif
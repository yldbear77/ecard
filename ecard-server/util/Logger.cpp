#include "Logger.h"

void Logger::Log(const char* inFormat, ...) {
	static char temp[4096]; // not thread safe...
	static wchar_t msg[6000];

	va_list args;
	va_start(args, inFormat);

	_vsnprintf_s(temp, 4096, 4096, inFormat, args);

	size_t len = strlen(temp) + 1;
	mbstowcs_s(&len, msg, temp, 6000);

	OutputDebugString(msg);
}

void Logger::LogW(const TCHAR* inFormat, ...) {
	static TCHAR buffer[6000] = { TEXT('\0') };

	va_list arg; // 가변인수 벡터

	va_start(arg, inFormat); // 가변인수 벡터 리셋
	_vsntprintf_s(buffer, 6000, inFormat, arg);
	va_end(arg);

	OutputDebugString(buffer);
}

std::string Logger::Sprintf(const char* inFormat, ...) {
	static char temp[4096]; //not thread safe...

	va_list args;
	va_start(args, inFormat);

	_vsnprintf_s(temp, 4096, 4096, inFormat, args);
	return std::string(temp);
}
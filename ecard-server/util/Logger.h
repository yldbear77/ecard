#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <windows.h>
#include <iostream>
#include <cstdarg>
#include <tchar.h>

namespace Logger {
	void Log(const char* inFormat, ...);
	void LogW(const TCHAR* inFormat, ...);
	std::string Sprintf(const char* inFormat, ...);
}

#define LOG(...) Logger::Log(__VA_ARGS__);
#define LOGW(...) Logger::LogW(__VA_ARGS__);

#endif
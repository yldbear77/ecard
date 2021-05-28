#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <winsock2.h>
#include <string>

namespace StringUtil {
	std::string TCHARToString(const TCHAR* tstr);
}

#endif
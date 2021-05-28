#include "StringUtil.h"

std::string StringUtil::TCHARToString(const TCHAR* tstr) {
	std::wstring original(tstr);
	std::string ret(original.begin(), original.end());
	return ret;
}
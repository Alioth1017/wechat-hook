#pragma once
#include <string>

wchar_t* AnsiToUnicode(const char* szStr);
inline char* UnicodeToAnsi(const wchar_t* szStr);
std::string string_To_UTF8(const std::string & str);
std::string Wchar_tToString(wchar_t *wchar);
wchar_t * StringToWchar_t(const std::string & str);
void WriteLog(const std::string & level, const std::string & content);
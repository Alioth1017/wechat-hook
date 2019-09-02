#include "stdafx.h"
#include <corecrt_malloc.h>
#include "utils.h"

// »ñÈ¡WechatWin.dll»ùÖ·
DWORD getWechatWinAdd() {
	HMODULE winAdd = LoadLibrary(L"WechatWin.dll");
	return DWORD(winAdd);
}

wchar_t * UTF8ToUnicode(const char* str)
{
	int    textlen = 0;
	wchar_t * result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1) * sizeof(wchar_t));
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return    result;
}

#pragma once
#include "stdafx.h"
#define INJECT_PROCESS_NAME "WeChat.exe"
#define INJECT_DLL_NAME "wechat-inject-helper.dll"
LPSTR GetDllPath(LPCSTR dllName);
DWORD ProcessNameToPID(LPCSTR processName);
VOID injectDll(char * dllPath);
VOID readMemory();
VOID setWindow(HWND thisWindow);
VOID runWechat(TCHAR * dllPath, TCHAR * wechatPath);
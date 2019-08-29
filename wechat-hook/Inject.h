#pragma once
#include "stdafx.h"
#define INJECT_PROCESS_NAME "WeChat.exe"
LPCSTR GetDllPath(LPCSTR dllName);
DWORD ProcessNameToPID(LPCSTR processName);
VOID injectDll(char * dllPath);
VOID readMemory();
VOID setWindow(HWND thisWindow);
VOID runWechat(TCHAR * dllPath, TCHAR * wechatPath);
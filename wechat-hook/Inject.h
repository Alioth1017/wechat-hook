#pragma once
#include "stdafx.h"

char* GetDllPath(const char* dllName);
DWORD ProcessNameToPID(const char* ProcessName);
BOOL CheckIsInject(DWORD dwProcessid);
BOOL InjectDll(HANDLE& wxPid);
void UnloadDll();
void ReadMemory();
void RunWechat(char* wechatPath);
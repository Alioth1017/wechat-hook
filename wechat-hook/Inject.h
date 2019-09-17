#pragma once
#include "stdafx.h"

DWORD ProcessNameToPID(const char* ProcessName);
BOOL CheckIsNotInject(DWORD dwProcessid);
BOOL InjectDll(HANDLE& wxPid);
void UnloadDll();
void ReadMemory();
void RunWechat(char* wechatPath);
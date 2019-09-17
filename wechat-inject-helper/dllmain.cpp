// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "resource.h"
#include "WndMsgLoop.h"
#include <stdio.h>
#include "test.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadTest, hModule, NULL, 0);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitWindow, hModule, NULL, 0);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
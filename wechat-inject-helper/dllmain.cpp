// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Windows.h>
#include "resource.h"
#include "wechat-inject-helper.h"

VOID ThreadProcess(HMODULE hModule);
INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcess, hModule, NULL, 0);
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

VOID ThreadProcess(HMODULE hModule)
{
	DialogBox(hModule, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
}

INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_CLOSE:
		EndDialog(hDlg, wParam);
		break;
		// 按钮事件
	case WM_COMMAND:
		// 读内存
		if (wParam == READ_DATA) {
			readWechatData(hDlg);
		}
		break;
	default:
		break;
	}
	return FALSE;
}

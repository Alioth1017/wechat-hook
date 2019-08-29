// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Windows.h>
#include "resource.h"
#include "wechat-inject-helper.h"

INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, L"加载中", L"DLL消息", 0);
		//DialogBox(hModule, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, L"卸载中", L"DLL消息", 0);
		break;
	}
	return TRUE;
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

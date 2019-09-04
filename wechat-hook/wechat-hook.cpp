// wechat-hook.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "wechat-hook.h"
#include <Windows.h> // 系统窗体库
#include "resource.h" // 窗体资源信息
#include <stdio.h>
#include "Inject.h"

//************************************************************
// 函数名称: wWinMain
// 函数说明: 程序入口函数
//***********************************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	// 查看文档按F1
	DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
	return 0;
}

//************************************************************
// 函数名称: Dlgproc
// 函数说明: DialogBox弹框事件处理
//***********************************************************
INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		//防多开
		RunSingle();
		break;
	case WM_CLOSE:
		//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)setWindowWechat, hwndDlg, 0, NULL);
		EndDialog(hDlg, wParam);
		break;
		// 按钮事件
	case WM_COMMAND:
		handleWmCommand(hDlg, wParam);
		break;
	default:
		break;
	}
	return FALSE;
}

//************************************************************
// 函数名称: RunSingle
// 函数说明: 防多开
//***********************************************************
void RunSingle()
{
	HANDLE hMutex = NULL;
	hMutex = CreateMutexA(NULL, FALSE, "wechat-hook");
	if (hMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			ExitProcess(-1);
		}
	}
}

//************************************************************
// 函数名称: handleWmCommand
// 函数说明: 界面事件处理函数
//***********************************************************
VOID handleWmCommand(HWND hwndDlg, WPARAM wParam) {
	TCHAR wechat_path[0x100] = { "D:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe" };
	LPSTR paths = GetDllPath(INJECT_DLL_NAME);
	switch (wParam)
	{
	case START_HELPER:
		runWechat(paths, wechat_path);
		break;
	default:
		break;
	}
}
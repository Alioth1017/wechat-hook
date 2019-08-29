// wechat-hook.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "wechat-hook.h"
#include <Windows.h> // 系统窗体库
#include "resource.h" // 窗体资源信息
#include <TlHelp32.h>
#include <stdio.h>
#include "Inject.h"

//------------------------------------函数申明区-------------------------------------------------
INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID handleWmCommand(HWND hwndDlg, WPARAM wParam);
VOID setWindowWechat(HWND hwndDlg);
//------------------------------------函数申明区-------------------------------------------------

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	// 查看文档按F1
	DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
	return 0;
}

INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		//MessageBox(NULL, "首次加载", "提示", 0);
		break;
	case WM_CLOSE:
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

//界面事件处理函数
VOID handleWmCommand(HWND hwndDlg, WPARAM wParam) {
	TCHAR wechat_path[0x100] = { "D:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe" };
	LPSTR paths = GetDllPath(INJECT_DLL_NAME);
	switch (wParam)
	{
	case LOAD_DLL:
		runWechat(paths, wechat_path);
		break;
	case UNLOAD_DLL:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)setWindowWechat, hwndDlg, 0, NULL);
		break;
	default:
		break;
	}
}

VOID setWindowWechat(HWND hwndDlg)
{
	while (true)
	{
		setWindow(hwndDlg);
	}
}
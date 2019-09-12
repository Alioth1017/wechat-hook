// wechat-hook.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "wechat-hook.h"
#include "resource.h" // 窗体资源信息
#include <stdio.h>
#include "Inject.h"

HANDLE wxPid = NULL;		//微信的PID
CListCtrl m_ChatRecord;

//***********************************************************
// 函数名称: wWinMain
// 函数说明: 程序入口函数
//***********************************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
	m_ChatRecord.SetDlgCtrlID(ID_MESSAGES);
	m_ChatRecord.InsertColumn(0, "消息类型", 0, 100);
	m_ChatRecord.InsertColumn(1, "消息来源", 0, 100);
	m_ChatRecord.InsertColumn(2, "微信ID/群ID", 0, 150);
	m_ChatRecord.InsertColumn(3, "群发送者", 0, 150);
	m_ChatRecord.InsertColumn(4, "消息内容", 0, 310);
	m_ChatRecord.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	return 0;
}

//***********************************************************
// 函数名称: Dlgproc
// 函数说明: DialogBox弹框事件处理
//***********************************************************
INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		// 防多开
		RunSingle();
		// 注入
		if (InjectDll(wxPid) == FALSE)
		{
			ExitProcess(-1);
		}
		break;
	case WM_CLOSE:
		// 卸载dll
		UnloadDll();
		// 关闭弹框
		EndDialog(hDlg, wParam);
		break;
		// 按钮事件
	case WM_COMMAND:
		handleWmCommand(hDlg, wParam);
		break;
	case WM_COPYDATA:
		OnCopyData(hDlg, (COPYDATASTRUCT*)lParam);
		break;
	default:
		break;
	}
	return FALSE;
}

//***********************************************************
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

//***********************************************************
// 函数名称: handleWmCommand
// 函数说明: 界面事件处理函数
//***********************************************************
void handleWmCommand(HWND hwndDlg, WPARAM wParam) {
	switch (wParam)
	{
	case START_HELPER: {
		break;
	}
	case ID_TEST: {
		//查找注入dll生成的窗口句柄
		HWND hWxInjectHelper = FindWindow(NULL, "wechat-inject-helper");
		if (hWxInjectHelper == NULL)
		{
			MessageBoxA(NULL, "未查找到wechat-inject-helper窗口", "错误", MB_OK);
			return;
		}
		//const wchar_t* sz = L"这是一条测试消息";
		const wchar_t* sz = L"发送关闭消息";
		COPYDATASTRUCT msg;
		msg.cbData = wcslen(sz) * 2 + 1; // 宽字符需要*2,数组完整需要+1
		msg.dwData = WM_Logout;
		msg.lpData = (LPVOID)sz;
		//发送消息给控制端
		SendMessage(hWxInjectHelper, WM_COPYDATA, (WPARAM)hWxInjectHelper, (LPARAM)&msg);
		break;
	}
	default:
		break;
	}
}

void OnCopyData(HWND hDlg, COPYDATASTRUCT* pCopyDataStruct) {
	if (pCopyDataStruct->dwData == WM_Login) {
		MessageBoxW(hDlg, L"微信登录", L"Message", 0);
	}
	else if (pCopyDataStruct->dwData == WM_AlreadyLogin) {
		MessageBoxW(hDlg, L"已经登陆微信", L"Message", 0);
	}
	else {
		wchar_t buff[0x1000] = { 0 };
		swprintf_s(buff, L"%s", pCopyDataStruct->lpData);
		MessageBoxW(hDlg, buff, L"Message", 0);
	}
}
// wechat-hook.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "wechat-hook.h"
#include "resource.h" // 窗体资源信息
#include <stdio.h>
#include "Inject.h"

HANDLE wxPid = NULL;		//微信的PID
BOOL bAutoChat = FALSE;     //自动聊天

//好友信息
struct UserInfo
{
	wchar_t UserId[80];
	wchar_t UserNumber[80];
	wchar_t UserRemark[80];
	wchar_t UserNickName[80];
};


//消息结构体
struct Message
{
	wchar_t type[10];		//消息类型
	wchar_t source[20];		//消息来源
	wchar_t wxid[40];		//微信ID/群ID
	wchar_t msgSender[40];	//消息发送者
	wchar_t content[200];	//消息内容
};

//***********************************************************
// 函数名称: wWinMain
// 函数说明: 程序入口函数
//***********************************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, &Dlgproc);
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
	case ID_AUTOCHAT: {
		if (bAutoChat == FALSE)
		{
			HWND hWxInjectHelper = FindWindow(NULL, "wechat-inject-helper");
			if (hWxInjectHelper == NULL)
			{
				MessageBoxA(NULL, "未查找到wechat-inject-helper窗口", "错误", MB_OK);
				return;
			}
			COPYDATASTRUCT autochat;
			autochat.dwData = WM_AutoChat;
			autochat.cbData = 0;
			autochat.lpData = NULL;
			//发送消息
			SendMessage(hWxInjectHelper, WM_COPYDATA, (WPARAM)hWxInjectHelper, (LPARAM)&autochat);
			bAutoChat = TRUE;
			MessageBoxW(hwndDlg, L"自动聊天已开启", L"Message", 0);
		}
		else
		{
			HWND hWxInjectHelper = FindWindow(NULL, "wechat-inject-helper");
			if (hWxInjectHelper == NULL)
			{
				MessageBoxA(NULL, "未查找到wechat-inject-helper窗口", "错误", MB_OK);
				return;
			}
			COPYDATASTRUCT autochat;
			autochat.dwData = WM_CancleAutoChat;
			autochat.cbData = 0;
			autochat.lpData = NULL;
			//发送消息
			SendMessage(hWxInjectHelper, WM_COPYDATA, (WPARAM)hWxInjectHelper, (LPARAM)&autochat);
			bAutoChat = FALSE;
			MessageBoxW(hwndDlg, L"自动聊天已关闭", L"Message", 0);
		}
		break;
	}
	case ID_LOGINOUT: {
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
		OutputDebugString("\n****微信登录成功****\n");
	}
	else if (pCopyDataStruct->dwData == WM_AlreadyLogin) {
		OutputDebugString("\n****已经登陆微信****\n");
	}
	else if (pCopyDataStruct->dwData == WM_ShowChatRecord) {
		Message *msg = new Message;
		msg = (Message*)pCopyDataStruct->lpData;

		wchar_t buff[0x1000] = { 0 };
		swprintf_s(buff, L"{\"wxid\":\"%s\",\"content\":\"%s\",\"msgSender\":\"%s\",\"source\":\"%s\",\"type\":\"%s\"}", msg->wxid, msg->content, msg->msgSender, msg->source, msg->type);
		OutputDebugStringW(buff);
		OutputDebugString("\n");
		std::string logContent = Wchar_tToString(buff);
		WriteLog("INFO", logContent);
	}
	else if (pCopyDataStruct->dwData == WM_Test) {
		wchar_t buff[0x1000] = { 0 };
		swprintf_s(buff, L"\n****收到[测试]消息****\n%s\n", (wchar_t*)pCopyDataStruct->lpData);
		OutputDebugStringW(buff);
	}
	else {
		OutputDebugStringW(L"\n****收到[未处理]消息****\n");
	}
}
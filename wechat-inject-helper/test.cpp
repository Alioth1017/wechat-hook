#include "stdafx.h"
#include "resource.h"
#include "test.h"
#include "wechat-inject-helper.h"

void ThreadTest(HMODULE hModule)
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
	{
		// 读内存
		if (wParam == READ_DATA) {
			readWechatData(hDlg);
		}
	}
	break;
	case WM_COPYDATA:
	{
		COPYDATASTRUCT *pCopyData = (COPYDATASTRUCT*)lParam;
		wchar_t buff[0x1000] = { 0 };
		swprintf_s(buff, L"%s", (wchar_t*)pCopyData->lpData);
		MessageBoxW(hDlg, buff, L"Message", 0);
	}
	break;
	default:
		break;
	}
	return FALSE;
}
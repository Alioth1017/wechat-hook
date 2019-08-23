// wechat-inject-helper.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>
#include "resource.h"

/*
WechatWin.dll 基址
昵称 1131C64
wxid 1131BEC
wxid2 1131B78
微信账号 1131B90
手机号码 1131C98
登录设备 1132030
头像地址 1131F2C
*/


// 获取WechatWin.dll基址
DWORD getWechatWinAdd() {
	HMODULE winAdd = LoadLibrary(L"WechatWin.dll");
	return DWORD(winAdd);
}

// 读取内存数据
VOID readWechatData(HWND hDlg) {
	// 拿到模块基址
	DWORD wechatWin = getWechatWinAdd();

	// 装数据的容器
	CHAR wxid[0x100] = { 0 };
	sprintf_s(wxid, "%s", wechatWin + 0x1131BEC);
	OutputDebugString((LPCWSTR)wxid);
	//SetDlgItemText(hDlg, SHOW_DATA, wxid);

	//CHAR headPic[0x100] = { 0 };
	//DWORD pPic = wechatWin + 0x1131F2C;
	//0x1212112 -> 0x3343434
	//*((DWORD *)pPic)
	//sprintf_s(headPic, "%s", *((DWORD *)pPic));
	//SetDlgItemText(hDlg, SHOW_DATA, (LPCWSTR)headPic);
	//OutputDebugString(headPic);
}
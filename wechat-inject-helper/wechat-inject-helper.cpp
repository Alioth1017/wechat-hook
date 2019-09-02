// wechat-inject-helper.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>
#include "resource.h"
#include "wechat-info-get.h"

// 读取内存数据
VOID readWechatData(HWND hDlg) {
	wxMyUserInfo userInfo = getUserInfo();
	wchar_t buff[0x1000] = { 0 };
	swprintf_s(buff, L"微信号：%s \r\n昵称：%s \r\n头像：%s \r\n手机号：%s \r\n设备：%s", userInfo.wxid, userInfo.wxNick, userInfo.wxHeadPic, userInfo.wxBindPhone, userInfo.wxDevice);
	SetDlgItemText(hDlg, SHOW_DATA, buff);
}
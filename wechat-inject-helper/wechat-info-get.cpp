#include "stdafx.h"
#include "utils.h"
#include "wechat-info-get.h"

/*
WechatWin.dll 基址
昵称 + 0x126D91C
wxid + 0x126D8A4
微信账号 + 0x
手机号码 + 0x126D950
登录设备 + 0x126DD48
头像地址 + 0x126DBFC
*/

wxMyUserInfo getUserInfo()
{
	// 拿到模块基址
	DWORD WechatWin = getWechatWinAdd();
	// 取数据
	wxMyUserInfo userInfo = { 0 };
	userInfo.wxid = UTF8ToUnicode((const char *)WechatWin + 0x126D8A4);
	if (wcslen(userInfo.wxid) < 0x6) {
		DWORD pWxid = WechatWin + 0x126D8A4;
		pWxid = *((DWORD *)pWxid);
		userInfo.wxid = UTF8ToUnicode((const char *)pWxid);
	}
	userInfo.wxUsername = UTF8ToUnicode((const char *)WechatWin + 0x126D950);
	userInfo.wxNick = UTF8ToUnicode((const char *)WechatWin + 0x126D91C);
	userInfo.wxBindPhone = UTF8ToUnicode((const char *)WechatWin + 0x126D950);
	userInfo.wxDevice = UTF8ToUnicode((const char *)WechatWin + 0x126DD48);
	DWORD pHeadPic = WechatWin + 0x126DBFC;
	pHeadPic = *((DWORD *)pHeadPic);
	userInfo.wxHeadPic = UTF8ToUnicode((const char *)pHeadPic);
	return userInfo;
}
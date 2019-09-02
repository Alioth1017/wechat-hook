#pragma once
struct wxMyUserInfo
{
	wchar_t * wxid;
	wchar_t * wxUsername;
	wchar_t * wxNick;
	wchar_t * wxHeadPic;
	wchar_t * wxBindPhone;
	wchar_t * wxProvince;
	wchar_t * wxCity;
	wchar_t * wxDevice;
};
wxMyUserInfo getUserInfo();
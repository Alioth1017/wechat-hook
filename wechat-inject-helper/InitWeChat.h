#pragma once
#include "stdafx.h"
#include <string>
using namespace std;

string GetWxVersion(); //获取当前微信版本
BOOL IsWxVersionValid(string wxVersoin);	//检查当前微信版本
void CheckIsLogin();	//检测微信是否登陆

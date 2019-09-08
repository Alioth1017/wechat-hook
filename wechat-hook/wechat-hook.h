#pragma once

#include "resource.h"

//------------------------------------函数申明区-------------------------------------------------
INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void RunSingle();
void handleWmCommand(HWND hwndDlg, WPARAM wParam);
//------------------------------------函数申明区-------------------------------------------------
// wechat-hook.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "wechat-hook.h"
#include <Windows.h> // 系统窗体库
#include "resource.h" // 窗体资源信息
#include <TlHelp32.h>
#include <stdio.h>
#define WECHAT_PROCESS_NAME "WeChat.exe"
HANDLE WECHAT_PROCESS;
LPVOID dllAdd;
HANDLE hThread;
CHAR pathStr[0x100] = { "D://Code//Study//wechat-hook//Debug//wechat-inject-helper.dll" };

INT_PTR CALLBACK Dlgproc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DWORD ProcessNameFindPID(LPCSTR ProcessName);
//LPCSTR GetInjectDllPath();
VOID LoadDll();
VOID UnLoadDll();

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
		// 加载
		if (wParam == LOAD_DLL) {
			LoadDll();
		}
		// 卸载
		if (wParam == UNLOAD_DLL) {
			UnLoadDll();
		}
		break;
	default:
		break;
	}
	return FALSE;
}

// 1.拿到微信句柄 (通过句柄操作微信内存)
DWORD ProcessNameFindPID(LPCSTR ProcessName)
{
	// 1.获取进程快照
	// #include <TlHelp32.h>
	HANDLE processAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);// 获取系统进程快照
	// 2.快照里对比ProcessName进程名称
	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		// WeChat.exe
		// th32ProcessID 进程ID
		// szExeFile 进程名称
		if (strcmp(ProcessName, processInfo.szExeFile) == 0) {
			return processInfo.th32ProcessID;
		}
	} while (Process32Next(processAll, &processInfo));

	return 0;
}

// 获取注入DLL全路径
//LPCSTR GetInjectDllPath()
//{
//	char szPath[MAX_PATH] = { 0 };
//	GetModuleFileNameA(NULL, szPath, MAX_PATH);
//	(strrchr(szPath, '\\'))[0] = 0; // 删除文件名，只获得路径字串
//	return szPath;
//}

// 2.微信内部申请内存 存放dll路径
// 微信进程找到微信PID，通过PID打开微信获取到句柄
VOID LoadDll()
{
	// 1.获取到微信句柄
	DWORD PID = ProcessNameFindPID(WECHAT_PROCESS_NAME);
	if (PID == 0) {
		MessageBox(NULL, "未找到微信进程或微信未启动", "错误", 0);
		return;
	}
	// 2.用找到的PID打开获取到的句柄
	WECHAT_PROCESS = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (WECHAT_PROCESS == NULL) {
		MessageBox(NULL, "进程打开失败，可能权限不足或者关闭了应用", "错误", 0);
		return;
	}
	// 3.申请内存
	dllAdd = VirtualAllocEx(WECHAT_PROCESS, NULL, sizeof(pathStr), MEM_COMMIT, PAGE_READWRITE);
	if (dllAdd == NULL)
	{
		MessageBox(NULL, "内存分配失败", "错误", 0);
		return;
	}
	// 4.dll路径写入到申请的内存中
	if (WriteProcessMemory(WECHAT_PROCESS, dllAdd, pathStr, strlen(pathStr), NULL) == 0)
	{
		MessageBox(NULL, "路径写入失败", "错误", 0);
		return;
	}
	// #include <stdio.h>
	//CHAR test[0x100] = { 0 };
	//sprintf_s(test, "写入的地址为 %p", dllAdd);
	//OutputDebugString(test);

	// 5.写入dll路径 通过远程线程执行 loadLibrary 来写入dll
	HMODULE k32 = GetModuleHandle("Kernel32.dll");
	LPVOID loadAdd = GetProcAddress(k32, "LoadLibraryA");
	//LoadLibraryW("test.dll");
	HANDLE hThread = CreateRemoteThread(WECHAT_PROCESS, NULL, 0, (LPTHREAD_START_ROUTINE)loadAdd, dllAdd, 0, NULL);
	if (hThread == NULL) {
		MessageBox(NULL, "远程注入失败", "错误", 0);
		return;
	}
}

VOID UnLoadDll() {
	DWORD dwHandle;
	// 等待GetModuleHandle运行完毕
	WaitForSingleObject(hThread, INFINITE);
	// 获得GetModuleHandle的返回值
	GetExitCodeThread(hThread, &dwHandle);
	// 释放目标进程中申请的空间
	VirtualFreeEx(WECHAT_PROCESS, dllAdd, sizeof(pathStr), MEM_DECOMMIT);
	CloseHandle(hThread);
	// 使目标进程调用FreeLibrary，卸载DLL
	LPVOID pFunc = FreeLibrary;
	hThread = CreateRemoteThread(WECHAT_PROCESS, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, (LPVOID)dwHandle, 0, NULL);
	// 等待FreeLibrary卸载完毕
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(WECHAT_PROCESS);
}

#include "stdafx.h"
#include "Inject.h"
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <AtlConv.h>
#include <direct.h>

#define INJECT_PROCESS_NAME "WeChat.exe"
#define INJECT_DLL_NAME "wechat-inject-helper.dll"

//***********************************************************
// 函数名称: GetDllPath
// 函数说明: 获取注入DLL全路径
//***********************************************************
char* GetDllPath(const char* dllName)
{
	char szPath[MAX_PATH] = { 0 };
	char buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	(strrchr(buffer, '\\'))[0] = 0; // 删除文件名，只获得路径字串
	sprintf_s(szPath, "%s\\%s", buffer, dllName);
	return szPath;

	//获取当前工作目录下的dll
	//char szPath[MAX_PATH] = { 0 };
	//char* buffer = NULL;
	//if ((buffer = _getcwd(NULL, 0)) != NULL)
	//{
	//	sprintf_s(szPath, "%s\\%s", buffer, dllName);
	//}
	//return szPath;
}

//***********************************************************
// 函数名称: ProcessNameToPID
// 函数说明: 通过进程名称查找进程ID
//***********************************************************
DWORD ProcessNameToPID(const char* ProcessName)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(hProcess, &pe32) == TRUE)
	{
		do
		{
			USES_CONVERSION;
			if (strcmp(ProcessName, pe32.szExeFile) == 0)
			{
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hProcess, &pe32));
	}
	return 0;
}

//************************************************************
// 函数名称: CheckIsInject
// 函数说明: 检测是否已经注入dll
//************************************************************
BOOL CheckIsInject(DWORD dwProcessid)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	//初始化模块信息结构体
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	//创建模块快照 1 快照类型 2 进程ID
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessid);
	//如果句柄无效就返回FALSE
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		//MessageBox(NULL, "创建模块快照失败", "错误", MB_OK);
		return FALSE;
	}
	//通过模块快照句柄获取第一个模块的信息
	if (!Module32First(hModuleSnap, &me32))
	{
		//MessageBox(NULL, "获取第一个模块的信息失败", "错误", MB_OK);
		//获取失败则关闭句柄
		CloseHandle(hModuleSnap);
		return FALSE;
	}
	do
	{
		if (strcmp(me32.szModule, INJECT_DLL_NAME) == 0)
		{
			return FALSE;
		}

	} while (Module32Next(hModuleSnap, &me32));

	return TRUE;
}

//***********************************************************
// 函数名称: InjectDll
// 函数说明: 注入dll
//***********************************************************
BOOL InjectDll()
{
	// 1.获取到微信句柄
	DWORD dwPid = ProcessNameToPID(INJECT_PROCESS_NAME);
	if (dwPid == 0) {
		//MessageBox(NULL, "未找到微信进程或微信未启动", "错误", MB_OK);
		return FALSE;
	}
	//检测dll是否已经注入
	if (!CheckIsInject(dwPid)) {
		//MessageBox(NULL, "重复注入", "错误", MB_OK);
		return FALSE;
	}
	// 2.用找到的PID打开获取到的句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwPid);
	if (NULL == hProcess) {
		//MessageBox(NULL, "进程打开失败，可能权限不足或者关闭了应用", "错误", MB_OK);
		return FALSE;
	}
	// 3.在进程中申请内存
	char* dllPath = GetDllPath(INJECT_DLL_NAME);
	DWORD strSize = strlen(dllPath) * 2;
	// 进程打开后我们把我们的dll路径存进去
	// 首先申请一片内存用于储存dll路径
	LPVOID pAddress = VirtualAllocEx(hProcess, NULL, strSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (NULL == pAddress) {
		//MessageBox(NULL, "内存申请失败", "错误", MB_OK);
		return FALSE;
	}

	// 4.写入dll路径到进程
	DWORD dwWrite = 0;
	if (WriteProcessMemory(hProcess, pAddress, dllPath, strSize, &dwWrite) == 0) {
		//MessageBox(NULL, "DLL路径写入失败", "错误", MB_OK);
		return FALSE;
	}
	// 获取LoadLibraryA函数地址		LoadLibraryA在Kernel32.dll里面 所以我们先获取这个dll的基址
	LPVOID pLoadLibraryAddress = GetProcAddress(GetModuleHandle("Kernel32.dll"), "LoadLibraryA");
	if (pLoadLibraryAddress == NULL)
	{
		//MessageBox(NULL, "获取LoadLibraryA函数地址失败", "错误", 0);
		return FALSE;
	}
	// 通过远程线程执行这个函数 参数传入 我们dll的地址
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);
	if (NULL == hRemoteThread) {
		//MessageBox(NULL, "远程线程注入失败", "错误", MB_OK);
		return FALSE;
	}

	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	return TRUE;
}

//************************************************************
// 函数名称: UnloadDll
// 函数说明: 卸载DLL
//************************************************************
BOOL UnloadDll()
{
	//获取微信Pid
	DWORD dwPid = ProcessNameToPID(INJECT_PROCESS_NAME);
	if (dwPid == 0)
	{
		//MessageBox(NULL, "没有找到微信进程或者微信没有启动", "错误", 0);
		return FALSE;
	}

	//遍历模块
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	MODULEENTRY32 ME32 = { 0 };
	ME32.dwSize = sizeof(MODULEENTRY32);
	BOOL isNext = Module32First(hSnap, &ME32);
	BOOL flag = FALSE;
	while (isNext)
	{
		USES_CONVERSION;
		if (strcmp(ME32.szModule, INJECT_DLL_NAME) == 0)
		{
			flag = TRUE;
			break;
		}
		isNext = Module32Next(hSnap, &ME32);
	}
	if (flag == FALSE)
	{
		//MessageBox(NULL, "找不到目标模块", "错误", 0);
		return FALSE;
	}

	//打开目标进程
	HANDLE hPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	//获取FreeLibrary函数地址
	FARPROC pFun = GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");

	//创建远程线程执行FreeLibrary
	HANDLE hThread = CreateRemoteThread(hPro, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, ME32.modBaseAddr, 0, NULL);
	if (!hThread)
	{
		MessageBox(NULL, "创建远程线程失败", "错误", 0);
		return FALSE;
	}

	CloseHandle(hSnap);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hPro);
	MessageBox(NULL, "dll卸载成功", "Tip", 0);
	return TRUE;
}

//***********************************************************
// 函数名称: ReadMemory
// 函数说明: 读取内存dll
//***********************************************************
void ReadMemory()
{
	DWORD PID = ProcessNameToPID(INJECT_PROCESS_NAME);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);
	LPCVOID phoneAdd = (LPCVOID)0x10611E80;
	DWORD reSize = 0xB;
	char buff[0x100] = { 0 };
	char buffTest[0x100] = { 0 };
	ReadProcessMemory(hProcess, phoneAdd, buff, reSize, NULL);
	sprintf_s(buffTest, "add=%p %s ", buff, buff);
	OutputDebugString(buffTest);
}

//***********************************************************
// 函数名称: RunWechat
// 函数说明: 启动微信
//			 CreateProcess 创建目标进程 创建时即挂起该进程.
//			 然后注入
//			 然后再ResumeThread 让目标进程运行
//***********************************************************
void RunWechat(char* wechatPath)
{
	// 微信如果已经启动 直接注入 否则 启动并 注入
	if (InjectDll()) {
		return;
	}
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	CreateProcess(NULL, wechatPath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	LPVOID Param = VirtualAllocEx(pi.hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	char add[0x100] = { 0 };

	char* dllPath = GetDllPath(INJECT_DLL_NAME);
	WriteProcessMemory(pi.hProcess, Param, dllPath, strlen(dllPath) * 2 + sizeof(char), NULL);

	char buff[0x100] = { 0 };
	HMODULE hModule = GetModuleHandle("Kernel32.dll");
	LPVOID address = GetProcAddress(hModule, "LoadLibraryA");
	//通过远程线程执行这个函数 参数传入 我们dll的地址
	//开始注入dll
	HANDLE hRemote = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, Param, 0, NULL);
	if (NULL == hRemote) {
		MessageBox(NULL, "远程执行失败", "错误", MB_OK);
		return;
	}

	DWORD rThread = ResumeThread(pi.hThread);
	if (-1 == rThread) {
		MessageBox(NULL, "微信进程唤醒失败", "错误", MB_OK);
		return;
	}
}
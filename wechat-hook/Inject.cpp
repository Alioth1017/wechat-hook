#include "stdafx.h"
#include "Inject.h"
#include <stdio.h>
#include <TlHelp32.h>
#include <direct.h>
#include <atlstr.h>

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

#pragma comment(lib,"advapi32")
CString GetAppRegeditPath(CString strAppName)
{
	//定义相关变量
	HKEY hKey;
	CString strAppRegeditPath("");
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//下面是打开注册表,只有打开后才能做其他操作
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
		LPCTSTR(strAppName), //要打开的子子键
		0, //这个一定为0
		KEY_QUERY_VALUE, //指定打开方式,此为读
		&hKey); //用来返回句柄

	if (lRet != ERROR_SUCCESS) //判断是否打开成功
	{
		return strAppRegeditPath;
	}
	else
	{
		//下面开始查询
		lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
			TEXT("Wechat"), //要查询的名称,查询的软件安装目录在这里
			NULL, //一定为NULL或者0
			NULL,
			(LPBYTE)szProductType, //我们要的东西放在这里
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //判断是否查询成功
		{
			return strAppRegeditPath;
		}
		else
		{
			RegCloseKey(hKey);

			strAppRegeditPath = szProductType;

			int nPos = strAppRegeditPath.Find('-');

			if (nPos >= 0)
			{
				CString sSubStr = strAppRegeditPath.Left(nPos - 1);//包含$,不想包含时nPos+1
				strAppRegeditPath = sSubStr;
			}
		}
	}
	return strAppRegeditPath;
}
CString GetAppRegeditPath2(CString strAppName)
{
	//定义相关变量
	HKEY hKey;
	CString strAppRegeditPath("");
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//下面是打开注册表,只有打开后才能做其他操作
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
		LPCTSTR(strAppName), //要打开的子子键
		0, //这个一定为0
		KEY_QUERY_VALUE, //指定打开方式,此为读
		&hKey); //用来返回句柄

	if (lRet != ERROR_SUCCESS) //判断是否打开成功
	{
		return strAppRegeditPath;
	}
	else
	{
		//下面开始查询
		lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
			TEXT("InstallPath"), //要查询的名称,查询的软件安装目录在这里
			NULL, //一定为NULL或者0
			NULL,
			(LPBYTE)szProductType, //我们要的东西放在这里
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //判断是否查询成功
		{
			return strAppRegeditPath;
		}
		else
		{
			RegCloseKey(hKey);
			strAppRegeditPath = szProductType;

		}
	}
	return strAppRegeditPath;
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
BOOL InjectDll(HANDLE& wxPid)
{
	// 待注入的dll路径
	char* dllPath = GetDllPath(INJECT_DLL_NAME);
	// 1.获取到微信句柄
	DWORD dwPid = ProcessNameToPID(INJECT_PROCESS_NAME);
	if (dwPid == 0) {
		// 启动微信
		CString wxStrAppName = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		CString szProductType = GetAppRegeditPath(wxStrAppName);
		if (szProductType.GetLength() < 5)
		{
			wxStrAppName = TEXT("Software\\Tencent\\WeChat");
			szProductType = GetAppRegeditPath2(wxStrAppName);
			szProductType.Append("\\WeChat.exe");
		}
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		si.dwFlags = STARTF_USESHOWWINDOW;// 指定wShowWindow成员有效
		si.wShowWindow = TRUE;          // 此成员设为TRUE的话则显示新建进程的主窗口，
									   // 为FALSE的话则不显示

		CreateProcess(szProductType, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

		HWND  hWechatMainForm = NULL;
		//WeChatLoginWndForPC
		while (NULL == hWechatMainForm)
		{
			hWechatMainForm = FindWindow(TEXT("WeChatLoginWndForPC"), NULL);
			Sleep(500);
		}
		if (NULL == hWechatMainForm)
		{
			return FALSE;
		}
		dwPid = pi.dwProcessId;
		wxPid = pi.hProcess;
	}
	//检测dll是否已经注入
	if (!CheckIsInject(dwPid)) {
		//MessageBox(NULL, "重复注入", "错误", MB_OK);
		return FALSE;
	}
	// 2.用找到的PID打开获取到的句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL) {
		//MessageBox(NULL, "进程打开失败，可能权限不足或者关闭了应用", "错误", MB_OK);
		return FALSE;
	}
	// 3.在进程中申请内存
	LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pAddress == NULL) {
		//MessageBox(NULL, "内存申请失败", "错误", MB_OK);
		return FALSE;
	}
	// 4.写入dll路径到进程
	if (WriteProcessMemory(hProcess, pAddress, dllPath, MAX_PATH, NULL) == 0) {
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
	if (hRemoteThread == NULL) {
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
void UnloadDll()
{
	//获取微信Pid
	DWORD dwPid = ProcessNameToPID(INJECT_PROCESS_NAME);
	if (dwPid == 0)
	{
		//MessageBox(NULL, "没有找到微信进程或者微信没有启动", "错误", 0);
		return;
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
		return;
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
		return;
	}

	CloseHandle(hSnap);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hPro);
	//MessageBox(NULL, "卸载成功", "Tip", 0);
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
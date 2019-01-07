#include "stdafx.h"
#include "Injecter.h"
#include "Common.h"

Injecter::Injecter()
{
}


Injecter::~Injecter()
{
}

BOOL Injecter::EnablePrivilege(BOOL enable)
{
	// �õ����ƾ��
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_READ, &hToken))
		return FALSE;

	// �õ���Ȩֵ
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
		return FALSE;

	// �������ƾ��Ȩ��
	TOKEN_PRIVILEGES tp = {};
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL))
		return FALSE;

	// �ر����ƾ��
	CloseHandle(hToken);

	return TRUE;
}


HMODULE Injecter::InjectDll(LPCTSTR commandLine, LPCTSTR dllPath/*, DWORD* pid, HANDLE* process*/)
{
	TCHAR* commandLineCopy = new TCHAR[32768]; // CreateProcess�����޸����
	_tcscpy_s(commandLineCopy, 32768, commandLine);
	int cdSize = _tcsrchr(commandLine, _T('\\')) - commandLine + 1;
	TCHAR* cd = new TCHAR[cdSize];
	_tcsnccpy_s(cd, cdSize, commandLine, cdSize - 1);
	// �������̲���ͣ
	STARTUPINFO startInfo = {};
	PROCESS_INFORMATION processInfo = {};
	if (!CreateProcess(NULL, commandLineCopy, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, cd, &startInfo, &processInfo))
	{
		setlog(L"CreateProcess %s False,code=%d",commandLineCopy, GetLastError());
		delete commandLineCopy;
		delete cd;
		return 0;
	}
	delete commandLineCopy;
	delete cd;

	/**pid = processInfo.dwProcessId;
	*process = processInfo.hProcess;*/

	DWORD dllPathSize = ((DWORD)_tcslen(dllPath) + 1) * sizeof(TCHAR);

	// �����ڴ��������DLL·��
	void* remoteMemory = VirtualAllocEx(processInfo.hProcess, NULL, dllPathSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (remoteMemory == NULL)
	{
		setlog(L"�����ڴ�ʧ�ܣ�������룺%u\n", GetLastError());
		return 0;
	}

	// д��DLL·��
	if (!WriteProcessMemory(processInfo.hProcess, remoteMemory, dllPath, dllPathSize, NULL))
	{
		setlog(L"д���ڴ�ʧ�ܣ�������룺%u\n", GetLastError());
		return 0;
	}

	// ����Զ�̵߳���LoadLibrary
	HANDLE remoteThread = CreateRemoteThread(processInfo.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, remoteMemory, 0, NULL);
	if (remoteThread == NULL)
	{
		setlog(L"����Զ�߳�ʧ�ܣ�������룺%u\n", GetLastError());
		return NULL;
	}
	// �ȴ�Զ�߳̽���
	WaitForSingleObject(remoteThread, INFINITE);
	// ȡDLL��Ŀ����̵ľ��
	DWORD remoteModule;
	GetExitCodeThread(remoteThread, &remoteModule);

	// �ָ��߳�
	ResumeThread(processInfo.hThread);

	// �ͷ�
	CloseHandle(remoteThread);
	VirtualFreeEx(processInfo.hProcess, remoteMemory, dllPathSize, MEM_DECOMMIT);

	return (HMODULE)remoteModule;
}
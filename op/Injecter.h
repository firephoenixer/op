#pragma once
class Injecter
{
public:
	Injecter();
	~Injecter();
	static BOOL EnablePrivilege(BOOL enable);
	// ��������ʱע��DLL������ģ������64λ����ֻ�ܷ��ص�32λ��
	static HMODULE InjectDll(LPCTSTR commandLine, LPCTSTR dllPath/*, DWORD* pid, HANDLE* process*/);
};


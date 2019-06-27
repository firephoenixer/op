
#include "stdafx.h"
#include <BlackBone/Process/Process.h>
#include <BlackBone/Process/RPC/RemoteFunction.hpp>
#include "bkdo.h"
#include "Tool.h"
#include <exception>
//#include "3rd_party/include/BlackBone/Process/Process.h"
//#include "3rd_party/include/BlackBone/Process/RPC/RemoteFunction.hpp"

#ifdef _M_X64
#pragma comment(lib,"E:/git_pro/Blackbone/build/x64/Release/BlackBone.lib")
#else
#pragma comment(lib,"E:/git_pro/Blackbone/build/Win32/Release/BlackBone.lib")
#endif

bkdo::bkdo()
{
	_render_type = 0;
}


bkdo::~bkdo()
{

}


long bkdo::Bind(HWND hwnd, long render_type) {
	//
	if (render_type == RDT_GL_NOX)
		return BindNox(hwnd, render_type);
	_render_type = render_type;
	_hwnd = hwnd;
	RECT rc;
	//��ȡ�ͻ�����С
	::GetClientRect(hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;
	bind_init();
	if (render_type == RDT_GL_NOX) {
	}
	DWORD id;
	::GetWindowThreadProcessId(_hwnd, &id);

	

	//attach ����
	blackbone::Process proc;
	NTSTATUS hr;

	hr = proc.Attach(id);

	long bind_ret = 0;
	if (NT_SUCCESS(hr)) {
		wstring dllname=g_op_name;
		//����Ƿ�������ͬ��32/64λ,�����ͬ����ʹ����һ��dll
		BOOL is64 = proc.modules().GetMainModule()->type == blackbone::eModType::mt_mod64;
		if (is64 != OP64) {
			dllname = is64 ? L"op_x64.dll" : L"op_x86.dll";
		}

		bool injected = false;
		//�ж��Ƿ��Ѿ�ע��
		auto _dllptr = proc.modules().GetModule(dllname);
		auto mods = proc.modules().GetAllModules();
		if (_dllptr) {
			injected = true;
		}
		else {
			auto iret = proc.modules().Inject(g_op_path + L"\\" + dllname);
			injected = (iret ? true : false);
		}
		if (injected) {
			using my_func_t = long(__stdcall*)(HWND, int);
			auto pSetXHook = blackbone::MakeRemoteFunction<my_func_t>(proc, dllname, "SetXHook");
			if (pSetXHook) {
				auto cret = pSetXHook(hwnd, render_type);
				bind_ret = cret.result();
			}
			else {
				setlog(L"remote function not found.");
			}
		}
		else {
			setlog(L"Inject false.");
		}




	}
	else {
		setlog(L"attach false.");
	}
	proc.Detach();
	if (bind_ret) {
		_bind_state = 1;

	}
	else {
		bind_release();
		_bind_state = 0;
	}

	return bind_ret;
}

long bkdo::UnBind() {

	if (_bind_state) {
		if (_render_type == RDT_GL_NOX)
			return UnBindNox();
		DWORD id;
		::GetWindowThreadProcessId(_hwnd, &id);

		//attach ����
		blackbone::Process proc;
		NTSTATUS hr;

		hr = proc.Attach(id);

		if (NT_SUCCESS(hr)) {
			wstring dllname = g_op_name;
			//����Ƿ�������ͬ��32/64λ,�����ͬ����ʹ����һ��dll
			BOOL is64 = proc.modules().GetMainModule()->type == blackbone::eModType::mt_mod64;
			if (is64 != OP64) {
				dllname = is64 ? L"op_x64.dll" : L"op_x86.dll";
			}
			using my_func_t = long(__stdcall*)(void);
			auto pUnXHook = blackbone::MakeRemoteFunction<my_func_t>(proc, dllname, "UnXHook");
			if (pUnXHook) {
				pUnXHook();
			}
			else {
				setlog(L"get unhook ptr false.");
			}
		}
		else {
			setlog("blackbone::MakeRemoteFunction false,errcode:%X,pid=%d,hwnd=%d", hr, id, _hwnd);
		}

		proc.Detach();
	}
	bind_release();
	return 1;
}

long bkdo::capture(const std::wstring& file_name) {
	//setlog(L"Bkdx::capture")
	std::fstream file;
	file.open(file_name, std::ios::out | std::ios::binary);
	if (!file.is_open())return 0;
	//_mutex.lock();
	BITMAPFILEHEADER bfh = { 0 };
	BITMAPINFOHEADER bih = { 0 };//λͼ��Ϣͷ
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bfh.bfSize = bfh.bfOffBits + _width * _height * 4;
	bfh.bfType = static_cast<WORD>(0x4d42);

	bih.biBitCount = 32;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = -_height;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = _width * _height * 4;//ͼ�����ݴ�С
	bih.biWidth = _width;//���
	file.write((char*)&bfh, sizeof(BITMAPFILEHEADER));
	file.write((char*)&bih, sizeof(BITMAPINFOHEADER));

	_pmutex->lock();
	file.write(_shmem->data<char>(), bih.biSizeImage);
	_pmutex->unlock();

	file.close();
	return 1;
}

long bkdo::BindNox(HWND hwnd, long render_type) {
	_render_type = render_type;
	_hwnd = hwnd;
	RECT rc;
	//��ȡ�ͻ�����С
	::GetClientRect(hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;
	bind_init();

	

	//attach ����
	blackbone::Process proc;
	NTSTATUS hr;


	wstring dllname = L"op_x64.dll";
	

	hr = proc.Attach(L"NoxVMHandle.exe");

	long bind_ret = 0;

	if (NT_SUCCESS(hr)) {
		/*_process.Resume();*/
		bool injected = false;
		//�ж��Ƿ��Ѿ�ע��
		auto _dllptr = proc.modules().GetModule(dllname);
		auto mods = proc.modules().GetAllModules();
		if (_dllptr) {
			injected = true;
		}
		else {
			auto iret = proc.modules().Inject(g_op_path + L"\\" + dllname);
			injected = (iret ? true : false);
		}
		if (injected) {
			using my_func_t = long(__stdcall*)(HWND, int);
			auto pSetXHook = blackbone::MakeRemoteFunction<my_func_t>(proc, dllname, "SetXHook");
			if (pSetXHook) {
				auto cret = pSetXHook(hwnd, render_type);
				bind_ret = cret.result();
			}
			else {
				setlog(L"remote function not found.");
			}
		}
		else {
			setlog(L"Inject false.");
		}



	}
	else {
		setlog(L"attach false.");
	}
	proc.Detach();
	if (bind_ret) {
		_bind_state = 1;

	}
	else {
		bind_release();
		_bind_state = 0;
	}

	return bind_ret;
}

long bkdo::UnBindNox() {
	if (_bind_state) {
		//attach ����
		blackbone::Process proc;
		NTSTATUS hr;

		hr = proc.Attach(L"NoxVMHandle.exe");
		wstring dllname = L"op_x64.dll";
		

		if (NT_SUCCESS(hr)) {

			using my_func_t = long(__stdcall*)(void);
			auto pUnXHook = blackbone::MakeRemoteFunction<my_func_t>(proc, dllname, "UnXHook");
			if (pUnXHook) {
				pUnXHook();
			}
			else {
				setlog(L"get unhook ptr false.");
			}
		}
		else {
			setlog("blackbone::MakeRemoteFunction false,errcode:%Xhwnd=%d", hr, _hwnd);
		}

		proc.Detach();
	}
	bind_release();
	return 1;
}


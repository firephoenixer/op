#include "stdafx.h"
#include "Bkdx.h"
#include <d3dx9.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/windows_shared_memory.hpp> 
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <exception>
#pragma pack(push)
#pragma pack(1)
#ifndef _WIN64
struct JmpCode
{
private:
	const BYTE jmp;
	DWORD address;

public:
	JmpCode(DWORD srcAddr, DWORD dstAddr)
		: jmp(0xE9)
	{
		setAddress(srcAddr, dstAddr);
	}

	void setAddress(DWORD srcAddr, DWORD dstAddr)
	{
		address = dstAddr - srcAddr - sizeof(JmpCode);
	}
};
#else
struct JmpCode
{
private:
	BYTE jmp[6];
	uintptr_t address;

public:
	JmpCode(uintptr_t srcAddr, uintptr_t dstAddr)
	{
		static const BYTE JMP[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
		memcpy(jmp, JMP, sizeof(jmp));
		setAddress(srcAddr, dstAddr);
	}

	void setAddress(uintptr_t srcAddr, uintptr_t dstAddr)
	{
		address = dstAddr;
	}
};
#endif
#pragma pack(pop)

long CreateSharedMemory(const std::string& name) {
	using namespace boost::interprocess;
	try {
		shared_memory_object obj(create_only, name.c_str(), read_write);
		obj.truncate(SHARED_MEMORY_SIZE);
	}
	catch (std::exception& err) {
		setlog("CreateSharedMemor exception:%s", err.what());
	}

	return 0;

}

long ReleaseSharedMemory(const std::string& name) {
	using namespace boost::interprocess;
	setlog(L"ReleaseSharedMemory");
	bool removed = boost::interprocess::shared_memory_object::remove(name.c_str());
	setlog(L"ReleaseSharedMemory? %d", removed ? 1 : 0);
	return 0;
}

void hook(void* originalFunction, void* hookFunction, BYTE* oldCode)
{
	JmpCode code((uintptr_t)originalFunction, (uintptr_t)hookFunction);
	DWORD oldProtect, oldProtect2;
	VirtualProtect(originalFunction, sizeof(code), PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(oldCode, originalFunction, sizeof(code));
	memcpy(originalFunction, &code, sizeof(code));
	VirtualProtect(originalFunction, sizeof(code), oldProtect, &oldProtect2);
}

void unhook(void* originalFunction, BYTE* oldCode)
{
	DWORD oldProtect, oldProtect2;
	VirtualProtect(originalFunction, sizeof(JmpCode), PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(originalFunction, oldCode, sizeof(JmpCode));
	VirtualProtect(originalFunction, sizeof(JmpCode), oldProtect, &oldProtect2);
}
//variable for host process
void* g_endSceneAddr = NULL;
BYTE g_endSceneOldCode[sizeof(JmpCode)];

ID3DXFont* g_font = NULL;
char g_shared_memory_name[256];
int g_need_hook = 0;
boost::interprocess::named_mutex *g_pmutex=nullptr;

HRESULT ScreenShot(LPDIRECT3DDEVICE9 pDevice) {
	//save bmp
	HRESULT hr = NULL;
	IDirect3DSurface9 *pSurface;
	hr = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	if (FAILED(hr)) return hr;

	D3DSURFACE_DESC surface_Desc;
	hr = pSurface->GetDesc(&surface_Desc);
	if (FAILED(hr)) return 0;

	IDirect3DTexture9 *pTex = NULL;
	IDirect3DSurface9 *pTexSurface = NULL;
	pDevice->CreateTexture(surface_Desc.Width,
		surface_Desc.Height,
		1,
		0,
		surface_Desc.Format,
		D3DPOOL_SYSTEMMEM, //����Ϊ���
		&pTex, NULL);
	if (pTex)
		hr = pTex->GetSurfaceLevel(0, &pTexSurface);
	if (pTexSurface)
		hr = pDevice->GetRenderTargetData(pSurface, pTexSurface);

	D3DLOCKED_RECT lockedRect;

	pTex->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY);
	/*
		ȡ����
	*/
	//
	//setlog("step 3. open shared mem name=\"%s\"", shared_memory_name);
	//Open already created shared memory object.
	try {
		boost::interprocess::shared_memory_object shm(
			boost::interprocess::open_only,
			g_shared_memory_name,
			boost::interprocess::read_write);
		//Map the whole shared memory in this process
		g_pmutex->lock();
		boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
		auto *p = static_cast<char*>(region.get_address());
		//setlog(L"step 4.try memcpy(p, lockedRect.pBits,size=%d,%d,%d", surface_Desc.Width, surface_Desc.Height, lockedRect.Pitch);
		memcpy(p, (byte*)lockedRect.pBits, lockedRect.Pitch*surface_Desc.Height);
		g_pmutex->unlock();
	}
	catch (std::exception& e) {
		setlog("catch exception:%s", e.what());

	}

	//end
	pTex->UnlockRect(0);

	//D3DXSaveTextureToFile(file, D3DXIFF_BMP, pTex, NULL);
	pSurface->Release();
	//setlog(L"memcpy end.");
	return hr;
}


HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* thiz)
{
	if (g_font == NULL)
	{
		// ��ʼ��
		D3DXFONT_DESC d3dFont = {};
		d3dFont.Height = 25;
		d3dFont.Width = 12;
		d3dFont.Weight = 500;
		d3dFont.Italic = FALSE;
		d3dFont.CharSet = DEFAULT_CHARSET;
		wcscpy_s(d3dFont.FaceName, L"Times New Roman");
		D3DXCreateFontIndirect(thiz, &d3dFont, &g_font);
	}

	static RECT rect = { 0, 0, 200, 200 };
	g_font->DrawText(NULL, L"Hello World", -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(255, 0, 0));

	unhook(g_endSceneAddr, g_endSceneOldCode);
	HRESULT hr = thiz->EndScene();
	//
	static DWORD t = 0;
	if (::GetTickCount() - t > 500) {
		t = ::GetTickCount();
		ScreenShot(thiz);

	}
	if (g_need_hook)
		hook(g_endSceneAddr, MyEndScene, g_endSceneOldCode);
	return hr;
}

//�˺������ڻ�ȡendScene address
DWORD WINAPI initHook(HWND target_hwnd)
{
	// �ȴ�DllMain��LoadLibrary�̣߳�����
	//WaitForSingleObject(dllMainThread, INFINITE);
	//CloseHandle(dllMainThread);

	// ����һ���������ڳ�ʼ��D3D

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = L"DummyWindow";
	if (RegisterClassEx(&wc) == 0)
	{
		MessageBox(NULL, L"ע�ᴰ����ʧ��", L"", MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"��������ʧ��", L"", MB_OK);
		return 0;
	}

	// ��ʼ��D3D

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == NULL)
	{
		MessageBox(NULL, L"����D3Dʧ��", L"", MB_OK);
		DestroyWindow(hwnd);
		return 0;
	}

	D3DPRESENT_PARAMETERS pp = {};
	pp.Windowed = TRUE;
	pp.SwapEffect = D3DSWAPEFFECT_COPY;

	IDirect3DDevice9* device;
	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &device)))
	{
		MessageBox(NULL, L"�����豸ʧ��", L"", MB_OK);
		d3d9->Release();
		DestroyWindow(hwnd);
		return 0;
	}

	// hook EndScene
	g_endSceneAddr = (*(void***)device)[42]; // EndScene��IDirect3DDevice9��43������

	//�����ﲻ����hook
	//hook(endSceneAddr, MyEndScene, endSceneOldCode);

	// �ͷ�
	d3d9->Release();
	device->Release();
	DestroyWindow(hwnd);
	sprintf(g_shared_memory_name, "DX_SHARED_MEMORY_OF_%p", target_hwnd);
	setlog("step 2.create shared name=%s", g_shared_memory_name);
	CreateSharedMemory(g_shared_memory_name);
	return 0;
}


//export function
long SetDX9Hook(HWND hwnd) {

	setlog(L"step 1. init hook.");
	g_need_hook = 1;
	initHook(hwnd);
	char name[128];
	sprintf(name, "mutex_%p", hwnd);
	g_pmutex = new boost::interprocess::named_mutex(boost::interprocess::open_or_create,name);
	if (g_endSceneAddr)
		hook(g_endSceneAddr, MyEndScene, g_endSceneOldCode);
	else
		return 0;


	return 1;
}

long UnDX9Hook() {
	g_need_hook = 0;
	if (g_endSceneAddr)
		unhook(g_endSceneAddr, g_endSceneOldCode);
	SAFE_DELETE(g_pmutex);
	setlog("un hook");
	ReleaseSharedMemory(g_shared_memory_name);
	return 0;
}



Bkdx::Bkdx() :_hwnd(NULL),_pmutex(nullptr)
{

}


Bkdx::~Bkdx()
{
	UnBind();
	
}


long Bkdx::Bind(HWND hwnd) {
	DWORD id;
	if (!::IsWindow(hwnd))
		return 0;
	::GetWindowThreadProcessId(hwnd, &id);
	RECT rc;
	::GetClientRect(hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;
	setlog(L"Bkdx::Bind,width=%d,height=%d", _width, _height);
	auto hr = _process.Attach(id);
	long bind_ret = 0;
	if (NT_SUCCESS(hr)) {
		wchar_t buff[256];
		::GetModuleFileName(gInstance, buff, 256);
		setlog(buff);
		_process.Resume();
		setlog(L"inject..");
		auto reg_ret = _process.modules().Inject(buff);
		setlog(L"inject finish...");
		_process.Resume();
		if (NT_SUCCESS(reg_ret.status)) {
			//wait some time 
			::Sleep(200);
			using my_func_t = long(__stdcall*)(HWND);
			_dllname = buff;
			_dllname = _dllname.substr(_dllname.rfind(L"\\") + 1);
			setlog(_dllname.c_str());
			auto SetDX9HookPtr = blackbone::MakeRemoteFunction<my_func_t>(_process, _dllname, "SetDX9Hook");
			//auto UnDX9HookPtr = blackbone::MakeRemoteFunction<my_func_t>(_process, L"dll_test.dll", "UnDX9Hook");
			if (SetDX9HookPtr) {
				SetDX9HookPtr(hwnd);
				bind_ret = 1;
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
	_process.Detach();
	_hwnd = bind_ret ? hwnd : NULL;
	if (bind_ret) {
		char mname[128];
		sprintf(mname, "mutex_%p", hwnd);
		_pmutex = new boost::interprocess::named_mutex(boost::interprocess::open_or_create, mname);
	}
		
	return bind_ret;
}

long Bkdx::UnBind() {
	DWORD id;
	if (!::IsWindow(_hwnd))
		return 0;
	::GetWindowThreadProcessId(_hwnd, &id);
	auto hr = _process.Attach(id);
	long bind_ret = 0;
	if (NT_SUCCESS(hr)) {
		//wait some time 
		::Sleep(200);
		using my_func_t = long(__stdcall*)(void);
		auto UnDX9HookPtr = blackbone::MakeRemoteFunction<my_func_t>(_process, _dllname, "UnDX9Hook");
		//auto UnDX9HookPtr = blackbone::MakeRemoteFunction<my_func_t>(_process, L"dll_test.dll", "UnDX9Hook");
		if (UnDX9HookPtr) {
			UnDX9HookPtr();
			bind_ret = 1;
		}
		else {
			setlog(L"get unhook ptr false.");
		}
#ifndef _WIN64
		_process.modules().RemoveManualModule(_dllname, blackbone::eModType::mt_mod32);
#else
		_process.modules().RemoveManualModule(_dllname, blackbone::eModType::mt_mod64);
#endif
	}
	else {
		setlog("attach false.");
	}
	_process.Detach();
	_hwnd = NULL;
	SAFE_DELETE(_pmutex);
	return bind_ret;
}

long Bkdx::capture(const std::wstring& file_name) {
	setlog(L"Bkdx::capture");
	if (!::IsWindow(_hwnd))
		return 0;
	char mem_name[256];
	sprintf(mem_name, "DX_SHARED_MEMORY_OF_%p", _hwnd);
	setlog(L"DX_SHARED_MEMORY_OF_%p", _hwnd);
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
	try {
		boost::interprocess::shared_memory_object shm(
			boost::interprocess::open_only,
			mem_name,
			boost::interprocess::read_only);

		//Map the whole shared memory in this process
		_pmutex->lock();
		boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);
		file.write((char*)region.get_address(), bih.biSizeImage);
		_pmutex->unlock();
	}
	catch (std::exception&e) {
		setlog("Bkdx::capture:%s", e.what());
	}
	file.close();
	return 1;
}

long Bkdx::FindPic(long x1, long y1, long x2, long y2, const std::wstring& files, double sim, long& x, long &y) {
	long ret = 0;
	x = y = -1;
	setlog("Bkdx::FindPic");
	if (!::IsWindow(_hwnd)) {

	}
	else {
		char mem_name[256];
		sprintf(mem_name, "DX_SHARED_MEMORY_OF_%p", _hwnd);
		setlog(mem_name);
		try {
			
			boost::interprocess::shared_memory_object shm(
				boost::interprocess::open_only,
				mem_name,
				boost::interprocess::read_only);

			//Map the whole shared memory in this process
			setlog("_pmutex->lock()");
			_pmutex->lock();
			setlog("Map the whole shared memory in this process");
			boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);
			_imageloc.input_image(static_cast<byte*>(region.get_address()), _width, _height, 0);
			_pmutex->unlock();
			setlog("_pmutex->unlock()");
			std::vector<std::wstring> images;
			split(files, images, L"|");
			ret = _imageloc.imageloc(images, sim, x, y);
		}
		catch (std::exception&e) {
			setlog("Bkdx::FindPic:exception:%s", e.what());
		}
		

		
	}
	return ret;
}

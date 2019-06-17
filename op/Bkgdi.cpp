#include "stdafx.h"
#include "Bkgdi.h"
#include "Common.h"
#include <fstream>
#include "Tool.h"
bkgdi::bkgdi() :_is_cap(0), _pthread(nullptr)
{
	_mode = 0;
	_hdc = _hmdc = NULL;
	_hbmpscreen = _holdbmp = NULL;
	//4*2^22=16*2^20=16MB
	//_image_data = new byte[MAX_IMAGE_WIDTH*MAX_IMAGE_WIDTH * 4];
}

bkgdi::~bkgdi()
{
	//SAFE_DELETE_ARRAY(_image_data);
}

long bkgdi::Bind(HWND hwnd, long mode) {
	if (!::IsWindow(hwnd))
		return 0;
	_hwnd = hwnd; _mode = mode;
	long ret = 0;
	
	_pthread = new std::thread(&bkgdi::cap_thread, this);
	ret = 1;
	return ret;
}

long bkgdi::UnBind() {
	_is_cap = 0;
	if (_pthread) {
		_pthread->join();
		SAFE_DELETE(_pthread);
	}
	//setlog(" bkgdi::UnBind()");
	return 1;
}

int bkgdi::cap_thread() {
	
	_is_cap = cap_init();
	while (_is_cap) {
		
		//do cap
		cap_image();
		
		//sleep some time to free cpu
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	cap_release();
	return 0;
}

long bkgdi::cap_init() {
	if (_mode == RENDER_TYPE::NORMAL)
		_hdc = ::GetWindowDC(_hwnd);
	else {
		_hdc = ::GetDC(_hwnd);
	}
	RECT rc;
	if (_hdc == NULL) {
		setlog("hdc == NULL", _hdc);
		return 0;
	}
	
	::GetWindowRect(_hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;
	//::MoveWindow(_hwnd, 0, 0, _width, _height, 1);
	POINT pt;
	pt.x = rc.left; pt.y = rc.top;
	::ScreenToClient(_hwnd, &pt);

	//::GetWindowRect(_hwnd, &rc);
	//::GetClientRect(_hwnd, &rcc);
	//����ƫ��
	_client_x = -pt.x;
	_client_y = -pt.y;
	//setlog("dx,dy=%d,%d", _client_x, _client_y);
	//setlog("pt[%d,%d]", pt.x, pt.y);
	//setlog("WindowRect:%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom);
	//setlog("ClientRect:%d,%d,%d,%d", rcc.left,rcc.top,rcc.right,rcc.bottom);

	_hmdc = CreateCompatibleDC(_hdc); //����һ����ָ���豸���ݵ��ڴ��豸�����Ļ���	
	if (_hmdc == NULL) {
		setlog("CreateCompatibleDC false");
		return -2;
	}
	_hbmpscreen = CreateCompatibleBitmap(_hdc, _width, _height); //������ָ�����豸������ص��豸���ݵ�λͼ

	_holdbmp = (HBITMAP)SelectObject(_hmdc, _hbmpscreen); //ѡ��һ����ָ�����豸�����Ļ�����


	GetObject(_hbmpscreen, sizeof(_bm), (LPSTR)&_bm); //�õ�ָ��ͼ�ζ������Ϣ	

	_bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	_bfh.bfSize = _bfh.bfOffBits + _bm.bmWidthBytes*_bm.bmHeight;
	_bfh.bfType = static_cast<WORD>(0x4d42);

	_bih.biBitCount = _bm.bmBitsPixel;//ÿ�������ֽڴ�С
	_bih.biCompression = BI_RGB;
	_bih.biHeight = _bm.bmHeight;//�߶�
	_bih.biPlanes = 1;
	_bih.biSize = sizeof(BITMAPINFOHEADER);
	_bih.biSizeImage = _bm.bmWidthBytes * _bm.bmHeight;//ͼ�����ݴ�С
	_bih.biWidth = _bm.bmWidth;//���

	_hbmpscreen = (HBITMAP)SelectObject(_hmdc, _holdbmp);
	bind_init();
	return 1;
}

long bkgdi::cap_release() {
	
	//delete[dwLen_2]hDib;
	if (_hdc)DeleteDC(_hdc); _hdc = NULL;
	if (_hmdc)DeleteDC(_hmdc); _hmdc = NULL;

	if (_hbmpscreen)DeleteObject(_hbmpscreen); _hbmpscreen = NULL;
	if (_holdbmp)DeleteObject(_holdbmp); _holdbmp = NULL;
	//Tool::setlog(L"cap_release");
	bind_release();
	return 0;
}

long bkgdi::cap_image() {
	
	
	if (!IsWindow(_hwnd)) { _is_cap = 0; return 0; }

	_holdbmp = (HBITMAP)SelectObject(_hmdc, _hbmpscreen);
	//��ָ����Դ�豸���������е����ؽ���λ�飨bit_block��ת��
	
	if (_mode == RENDER_TYPE::GDI)
		::PrintWindow(_hwnd, _hmdc, 0);

	BitBlt(_hmdc, 0, 0, _width, _height, _hdc, 0, 0, SRCCOPY);
	_holdbmp = (HBITMAP)SelectObject(_hmdc, _hbmpscreen);
	//������ȡָ������λͼ��λ��Ȼ������һ��DIB���豸�޹�λͼ��Device-Independent Bitmap��ʹ�õ�ָ����ʽ���Ƶ�һ����������
	_pmutex->lock();
	GetDIBits(_hmdc, _hbmpscreen, 0L, (DWORD)_height, _shmem->data<byte>(), (LPBITMAPINFO)&_bih, (DWORD)DIB_RGB_COLORS);
	_pmutex->unlock();

	_hbmpscreen = (HBITMAP)SelectObject(_hmdc, _holdbmp);
	return 1;

}

long bkgdi::capture(const std::wstring& file_name) {
	//setlog(L"bkgdi::capture");
	std::fstream file;
	file.open(file_name, std::ios::out | std::ios::binary);
	if (!file.is_open())return 0;
	_pmutex->lock();
	file.write((char*)&_bfh, sizeof(BITMAPFILEHEADER));
	file.write((char*)&_bih, sizeof(BITMAPINFOHEADER));
	file.write(_shmem->data<char>(), _bih.biSizeImage);
	_pmutex->unlock();
	file.close();
	return 1;
}




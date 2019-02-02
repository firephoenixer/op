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
	Tool::setlog(" bkgdi::UnBind()");
	return 1;
}

int bkgdi::cap_thread() {
	_is_cap = 1;
	cap_init();
	while (_is_cap) {
		_pmutex->lock();
		//do cap
		cap_image();
		_pmutex->unlock();
		//sleep some time to free cpu
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	cap_release();
	return 0;
}

long bkgdi::cap_init() {
	if (!IsWindow(_hwnd)) { _is_cap = 0; return 0; }
	_hdc = ::GetWindowDC(_hwnd);
	RECT rc;
	
	::GetWindowRect(_hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;
	::MoveWindow(_hwnd, 0, 0, _width, _height, 1);
	POINT pt;
	::GetWindowRect(_hwnd, &rc);
	pt.x = rc.left; pt.y = rc.top;
	::ScreenToClient(_hwnd, &pt);
	//setlog("WindowRect:%d,%d", pt.x,pt.y);
	//setlog("Window:%d,%d", _width, _height);
	//����ƫ��
	::GetClientRect(_hwnd, &rc);
	_client_x = -pt.x;
	_client_y = -pt.y;
	//setlog("ClientRect:%d,%d,%d,%d", rc.left,rc.top,rc.right,rc.bottom);

	_hmdc = CreateCompatibleDC(_hdc); //����һ����ָ���豸���ݵ��ڴ��豸�����Ļ���	
	if (_hmdc == NULL) {
		Tool::setlog("CreateCompatibleDC false");
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
	bind_init();
	return 1;
}

long bkgdi::cap_release() {
	if (_holdbmp&&_hmdc)
		_hbmpscreen = (HBITMAP)SelectObject(_hmdc, _holdbmp);
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
	
	//Tool::setlog("bkgdi::cap_image()");
	if (!IsWindow(_hwnd)) { _is_cap = 0; return 0; }
	//��ָ����Դ�豸���������е����ؽ���λ�飨bit_block��ת��
	if (_mode == BACKTYPE::NORMAL)
		BitBlt(_hmdc, 0, 0, _width, _height, _hdc, 0, 0, SRCCOPY);
	else if (_mode == BACKTYPE::GDI)
		::PrintWindow(_hwnd, _hmdc, 0);

	//������ȡָ������λͼ��λ��Ȼ������һ��DIB���豸�޹�λͼ��Device-Independent Bitmap��ʹ�õ�ָ����ʽ���Ƶ�һ����������
	GetDIBits(_hmdc, _hbmpscreen, 0L, (DWORD)_height, (LPBYTE)_region->get_address(), (LPBITMAPINFO)&_bih, (DWORD)DIB_RGB_COLORS);
	return 1;

}

long bkgdi::capture(const std::wstring& file_name) {
	Tool::setlog(L"bkgdi::capture");
	std::fstream file;
	file.open(file_name, std::ios::out | std::ios::binary);
	if (!file.is_open())return 0;
	_pmutex->lock();
	file.write((char*)&_bfh, sizeof(BITMAPFILEHEADER));
	file.write((char*)&_bih, sizeof(BITMAPINFOHEADER));
	file.write((char*)_region->get_address(), _bih.biSizeImage);
	_pmutex->unlock();
	file.close();
	return 1;
}




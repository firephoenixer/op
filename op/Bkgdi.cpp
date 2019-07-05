#include "stdafx.h"
#include "Bkgdi.h"
#include "Common.h"
#include <fstream>
#include "Tool.h"
bkgdi::bkgdi() :_is_cap(0), _pthread(nullptr)
{
	_render_type = 0;
	_hdc = _hmdc = NULL;
	_hbmpscreen = _hbmp_old = NULL;
	//4*2^22=16*2^20=16MB
	//_image_data = new byte[MAX_IMAGE_WIDTH*MAX_IMAGE_WIDTH * 4];
}

bkgdi::~bkgdi()
{
	//SAFE_DELETE_ARRAY(_image_data);
}

long bkgdi::Bind(HWND hwnd, long render_type) {
	if (!::IsWindow(hwnd))
		return 0;
	_hwnd = hwnd; _render_type = render_type;
	_is_cap = 1;
	bind_init();
	_pthread = new std::thread(&bkgdi::cap_thread, this);
	return 1;
}

long bkgdi::UnBind() {
	_is_cap = 0;
	if (_pthread) {
		_pthread->join();
		SAFE_DELETE(_pthread);
	}
	bkdisplay::bind_release();
	return 1;
}

int bkgdi::cap_thread() {
	
	
	while (_is_cap) {
		
		//do cap
		cap_image();
		//sleep some time to free cpu
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	
	return 0;
}

long bkgdi::cap_init() {
	if (!::IsWindow(_hwnd))
		return 0;
	_hdc = NULL;
	if (_render_type == RDT_NORMAL)
		_hdc = ::GetWindowDC(_hwnd);
	else if(_render_type==RDT_GDI){
		_hdc = ::GetDC(_hwnd);
	}
	else if (_render_type == RDT_GDI2||_render_type==RDT_GDI_DX2) {
		_hdc = ::GetDCEx(_hwnd, NULL, DCX_PARENTCLIP);
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

	_hbmp_old = (HBITMAP)SelectObject(_hmdc, _hbmpscreen); //ѡ��һ����ָ�����豸�����Ļ�����


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

	_hbmpscreen = (HBITMAP)SelectObject(_hmdc, _hbmp_old);
	
	return 1;
}

long bkgdi::cap_release() {
	
	//delete[dwLen_2]hDib;
	if (_hdc)DeleteDC(_hdc); _hdc = NULL;
	if (_hmdc)DeleteDC(_hmdc); _hmdc = NULL;

	if (_hbmpscreen)DeleteObject(_hbmpscreen); _hbmpscreen = NULL;
	if (_hbmp_old)DeleteObject(_hbmp_old); _hbmp_old = NULL;
	return 0;
}

long bkgdi::cap_image() {
	
	if (!cap_init())return 0;

	_hbmp_old = (HBITMAP)SelectObject(_hmdc, _hbmpscreen);
	//��ָ����Դ�豸���������е����ؽ���λ�飨bit_block��ת��
	
	if (_render_type == RDT_GDI)
		::PrintWindow(_hwnd, _hmdc, 0);
	else if (_render_type == RDT_GDI2) {
		::UpdateWindow(_hwnd);
		::RedrawWindow(_hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
		::PrintWindow(_hwnd, _hmdc, 0);
	}

	BitBlt(_hmdc, 0, 0, _width, _height, _hdc, 0, 0, CAPTUREBLT|SRCCOPY);

	//_hbmp_old = (HBITMAP)SelectObject(_hmdc, _hbmpscreen);

	_hbmpscreen = (HBITMAP)SelectObject(_hmdc, _hbmp_old);

	//������ȡָ������λͼ��λ��Ȼ������һ��DIB���豸�޹�λͼ��Device-Independent Bitmap��ʹ�õ�ָ����ʽ���Ƶ�һ����������
	_pmutex->lock();
	GetDIBits(_hmdc, _hbmpscreen, 0L, (DWORD)_height, _shmem->data<byte>(), (LPBITMAPINFO)&_bih, (DWORD)DIB_RGB_COLORS);
	_pmutex->unlock();

	cap_release();
	return 1;

}






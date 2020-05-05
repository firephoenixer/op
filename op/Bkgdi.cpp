#include "stdafx.h"
#include "Bkgdi.h"
#include "globalVar.h"
#include "helpfunc.h"
#include <fstream>

bkgdi::bkgdi()
{
	_render_type = 0;

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
	bind_init();
	RECT rc;
	//step 3.��ȡ��ͼ���򣬴�С
	::GetWindowRect(_hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;
	//step 2.��� �豸���
	_hdc = NULL;
	if (_render_type == RDT_NORMAL) {
		_hdc = ::GetDC(::GetDesktopWindow());
		//hdc = ::CreateDCW(L"DISPLAY", nullptr, nullptr, nullptr);
	}
	else if (_render_type == RDT_GDI) {
		_hdc = ::GetDC(_hwnd);
	}
	else if (_render_type == RDT_GDI2 || _render_type == RDT_GDI_DX2) {
		_hdc = ::GetDCEx(_hwnd, NULL, DCX_PARENTCLIP);
	}
	
	if (_hdc == NULL) {
		setlog("hdc == NULL", _hdc);
		return 0;
	}
	
	
	//����һ����ָ���豸���ݵ��ڴ��豸�����Ļ���	
	_hmdc = CreateCompatibleDC(_hdc);
	if (_hmdc == NULL) {
		setlog("CreateCompatibleDC false");
		return -2;
	}
	
	updata_screen();
	return 1;
}

long bkgdi::UnBind(HWND hwnd) {
	_hwnd = hwnd;
	return UnBind();
}

long bkgdi::UnBind() {
	_hbmpscreen = (HBITMAP)SelectObject(_hmdc, _hbmp_old);
	//delete[dwLen_2]hDib;
	if (_hdc)DeleteDC(_hdc); _hdc = NULL;
	if (_hmdc)DeleteDC(_hmdc); _hmdc = NULL;

	if (_hbmpscreen)DeleteObject(_hbmpscreen); _hbmpscreen = NULL;
	//if (_hbmp_old)DeleteObject(_hbmp_old); _hbmp_old = NULL;
	bkdisplay::bind_release();
	return 1;
}







byte* bkgdi::get_data() {
	//����  ˢ������
	updata_screen();

	return _shmem->data<byte>();
}

long bkgdi::updata_screen() {
	//step 1.�ж� �����Ƿ����
	if (!::IsWindow(_hwnd))
		return 0;
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	_hbmpscreen = CreateCompatibleBitmap(_hdc, w, h); //������ָ�����豸������ص��豸���ݵ�λͼ
	_hbmp_old = (HBITMAP)SelectObject(_hmdc, _hbmpscreen); //ѡ��һ����ָ�����豸�����Ļ�����
	//RECT rc;
	//step 3.��ȡ��ͼ���򣬴�С
	/*::GetWindowRect(_hwnd, &rc);
	_width = rc.right - rc.left;
	_height = rc.bottom - rc.top;*/
	//::MoveWindow(_hwnd, 0, 0, _width, _height, 1);
	//POINT pt;
	//pt.x = rc.left; pt.y = rc.top;

	//
	//if (_render_type != RDT_NORMAL) {
	//	::ScreenToClient(_hwnd, &pt);
	//	//����ƫ��
	//	_client_x = -pt.x;
	//	_client_y = -pt.y;
	//}
	//else {
	//	_client_x = _client_y = 0;
	//}
	

	_bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	_bfh.bfSize = _bfh.bfOffBits +w* h * 4;
	_bfh.bfType = static_cast<WORD>(0x4d42);

	_bih.biBitCount = 32;//ÿ�������ֽڴ�С
	_bih.biCompression = BI_RGB;
	_bih.biHeight = h;//�߶�
	_bih.biPlanes = 1;
	_bih.biSize = sizeof(BITMAPINFOHEADER);
	_bih.biSizeImage = w * h * 4;//ͼ�����ݴ�С
	_bih.biWidth = w;//���


	

	//��ָ����Դ�豸���������е����ؽ���λ�飨bit_block��ת��

	if (_render_type == RDT_GDI)
		::PrintWindow(_hwnd, _hdc, 0);
	else if (_render_type == RDT_GDI2) {
		::UpdateWindow(_hwnd);
		//::RedrawWindow(_hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
		::PrintWindow(_hwnd, _hdc, 0);
	}
	/*int x1 = 0, y1 = 0;
	if (_render_type == RDT_NORMAL) {
		x1 = rc.left; y1 = rc.top;
	}*/
	//StretchBlt(_hmdc, 0, 0, w, h, _hdc, rect.left, rect.top, w, h, SRCCOPY);
	if (BitBlt(_hmdc, 0, 0, w, h, _hdc, rect.left, rect.top, SRCCOPY)) {
		//ok
	}
	else {
		setlog("error in bitbit");
	}

	//_hbmp_old = (HBITMAP)SelectObject(_hmdc, _hbmpscreen);

	

	//������ȡָ������λͼ��λ��Ȼ������һ��DIB���豸�޹�λͼ��Device-Independent Bitmap��ʹ�õ�ָ����ʽ���Ƶ�һ����������
	_pmutex->lock();
	GetDIBits(_hmdc, _hbmpscreen, 0L, (DWORD)h, _shmem->data<byte>(), (LPBITMAPINFO)&_bih, (DWORD)DIB_RGB_COLORS);
	_pmutex->unlock();

	if (_hbmpscreen)DeleteObject(_hbmpscreen); _hbmpscreen = NULL;
	return 1;

}






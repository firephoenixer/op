#pragma once
#ifndef __BKDISPLAY_H_
#define __BKDISPLAY_H_
#include <thread>
#include "optype.h"
#include "bkdisplay.h"
struct Image;
class bkgdi:public bkdisplay
{
public:
	bkgdi();
	~bkgdi();
	//��
	long Bind(HWND _hwnd, long render_type) override;
	long UnBind(HWND hwnd);
	//���
	long UnBind() override;
	
	
	//long updata_screen();

	//byte* get_data() override;

	virtual bool requestCapture(int x1, int y1, int w, int h, Image& img)override;
	
private:
	//�豸���
	HDC _hdc = NULL;

	HDC _hmdc = NULL;
	//λͼ���
	HBITMAP _hbmpscreen = NULL;
	HBITMAP _hbmp_old = NULL;
	//bmp �ļ�ͷ
	BITMAPFILEHEADER _bfh = { 0 };
	BITMAPINFOHEADER _bih = { 0 };//λͼ��Ϣͷ
	int dx_, dy_;//ȥ��������
	//bytearray temp_src;
};

#endif


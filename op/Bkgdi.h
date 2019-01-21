#pragma once
#ifndef __BKDISPLAY_H_
#define __BKDISPLAY_H_
#include <thread>
#include "bkdisplay.h"
class Bkgdi:public bkdisplay
{
public:
	Bkgdi();
	~Bkgdi();
	//��
	long Bind(HWND _hwnd, long flag);
	//���
	long UnBind();
	//��ͼ��ʼ��
	long cap_init();
	//��ͼ�ͷ�
	long cap_release();
	//��ͼ
	long cap_image();
	//��ͼ���ļ�
	long capture(const std::wstring& file_name);
	
	
private:
	//��ͼģʽ
	int _mode;
	//�豸���
	HDC _hdc;

	HDC _hmdc;
	//λͼ���
	HBITMAP _hbmpscreen;
	HBITMAP _holdbmp;
	//λͼ��Ϣ
	BITMAP           _bm;
	//bmp �ļ�ͷ
	BITMAPFILEHEADER _bfh = { 0 };
	BITMAPINFOHEADER _bih = { 0 };//λͼ��Ϣͷ
	//��ͼ�߳�ָ��
	std::thread* _pthread;
	//��ͼ��ʶ
	int _is_cap;
	
	//��ͼ�̺߳���
	int cap_thread();
};

#endif


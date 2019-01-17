#pragma once
#ifndef __BKDISPLAY_H_
#define __BKDISPLAY_H_
#include <thread>
#include <mutex>
class Bkgdi
{
public:
	Bkgdi();
	~Bkgdi();
	//��
	long Bind(HWND _hwnd, int mode);
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
	byte* get_data() {
		return _image_data;
	}
	std::mutex& get_mutex() {
		return _mutex;
	}
	long get_width() {
		return _width;
	}
	long get_height() {
		return _height;
	}
	//�ͻ���ƫ��
	int _client_x, _client_y;
private:
	//��ͼ���ھ��
	HWND _hwnd;
	//��ͼģʽ
	int _mode;
	//��Ļ����ָ��
	byte* _image_data;
	//ͼ����
	long _width, _height;
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
	//������
	std::mutex _mutex;
	//��ͼ��ʶ
	int _is_cap;
	
	//��ͼ�̺߳���
	int cap_thread();
};

#endif


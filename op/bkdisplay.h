#pragma once
//#include <boost/interprocess/sync/named_mutex.hpp> 
//#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/windows_shared_memory.hpp> 
//#include <boost/interprocess/mapped_region.hpp>
//#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <exception>
#include "./include/promutex.h"
#include "./include/sharedmem.h"
struct Image;
class bkdisplay
{
public:
	bkdisplay();
	~bkdisplay();
	//bind window
	virtual long Bind(HWND hwnd, long flag) = 0;
	//unbind window
	virtual long UnBind() = 0;
	//unbind window
	virtual long UnBind(HWND hwnd) = 0;
	virtual bool requestCapture(int x1, int y1, int w, int h, Image& img) = 0;
	//��Ϊ���ֽ�ͼ��ʽ�Ĳ��죬�Ƿ�ɹ��жϽϸ��ӣ����ڴ�ʵ����Դ��������ͷţ��������
	//��Դ����
	long bind_init();
	//��Դ�ͷ�
	long bind_release();
	//virtual byte* get_data();

	promutex* get_mutex() {
		return _pmutex;
	}

	long get_height() {
		return _height;
	}

	long get_width() {
		return _width;
	}
public:
	//���ھ��
	HWND _hwnd;
	//�����ڴ�
	sharedmem* _shmem;
	//���̻�����
	promutex* _pmutex;

	wchar_t _shared_res_name[256];

	wchar_t _mutex_name[256];
	//��״̬
	long _bind_state;
	//
	int _render_type;
	//���
	long _width;
	long _height;
	//�ͻ���ƫ��
	int _client_x, _client_y;
	//need capture rect
	//RECT rect;
	
};


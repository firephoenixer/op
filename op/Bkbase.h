#pragma once
#ifndef __BACKBASE_H_
#define __BACKBASE_H_
#include <string>
#include "bkgdi.h"
#include "Bkmouse.h"
#include "Bkkeypad.h"
#include "bkdx_gl.h"
#include "include/Image.hpp"
using std::wstring;

/*
��̨�����࣬�������¹���:
1.���ڰ�
2.��̨��ͼ
3.�����̲���
*/
class bkbase
{
public:
	
	bkbase();
	~bkbase();
public:
	virtual long BindWindow(long hwnd, const wstring& sdisplay, const wstring& smouse, const wstring& skeypad, long mode);
	virtual long UnBindWindow();
	virtual long GetBindWindow();
	virtual long IsBind();
	virtual long GetCursorPos(int& x, int& y);
	
	long GetDisplay();
	/*byte* GetScreenData();*/
	void lock_data();
	void unlock_data();
	long get_height();
	long get_width();
	long RectConvert(long&x1, long&y1, long&x2, long&y2);
	//0:normal;-1 reserve 1 need cut
	long get_image_type();
	//����Ƿ�󶨻�������ǰ̨
	bool check_bind();
	const std::pair<wstring, wstring>& get_display_method()const;
	long set_display_method(const wstring& method);

	bool requestCapture(int x1,int y1,int w,int h,Image& img);
private:
	HWND _hwnd;
	int _is_bind;
	int _display;
	int _mode;
	std::pair<wstring,wstring> _display_method;
	Image _pic;
public:
	bkdisplay* _pbkdisplay;
	bkmouse _bkmouse;
	bkkeypad _keypad;
	wstring _curr_path;
	
};
#endif



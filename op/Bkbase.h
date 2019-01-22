#pragma once
#ifndef __BACKBASE_H_
#define __BACKBASE_H_
#include "Bkmouse.h"
#include "bkdisplay.h"
using std::wstring;
/*
��̨�����࣬�������¹���:
1.���ڰ�
2.��̨��ͼ
3.�����̲���
*/
class Bkbase
{
public:
	
	Bkbase();
	~Bkbase();
public:
	virtual long BindWindow(long hwnd, const wstring& sdisplay, const wstring& smouse, const wstring& skeypad, long mode);
	virtual long UnBindWindow();
	virtual long GetBindWindow();
	virtual long IsBind();
	virtual long GetCursorPos(int& x, int& y);
	virtual long GetKeyState(int vk_code);
	virtual long KeyDown(int vk_code);
	virtual long KeyUp(int vk_code);
	virtual long LeftClick();
	virtual long RightClick();
	virtual long MoveTo(long x, long y);
	virtual long Capture(const std::wstring& file_name);
	long GetDisplay();
	byte* GetScreenData();
	void lock_data();
	void unlock_data();
	long get_height();
	long get_widht();
	long RectConvert(long&x1, long&y1, long&x2, long&y2);
	long get_image_type() {
		switch (_display)
		{
		case BACKTYPE::NORMAL:
			return 0;
		case BACKTYPE::GDI:
			return -1;
		case BACKTYPE::DX:
			return 0;
		case BACKTYPE::OPENGL:
			return -1;
		default:
			return 0;
		}
		
	}
private:
	HWND _hwnd;
	int _is_bind;
	int _display;
	int _mode;
public:
	bkdisplay* _pbkdisplay;
	Bkmouse _bkmouse;
	
};
#endif



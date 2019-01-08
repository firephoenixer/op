#pragma once
#ifndef __DXBACKGROUND_H_
#define __DXBACKGROUND_H_
#include <BlackBone/Process/Process.h>
//#include <BlackBone/Patterns/PatternSearch.h>
#include <BlackBone/Process/RPC/RemoteFunction.hpp>
//#include <BlackBone/Syscalls/Syscall.h>
#include "Common.h"
#include "ImageLoc.h"
#include <boost/interprocess/sync/named_mutex.hpp> 
long CreateSharedMemory(const std::string& name);

long ReleaseSharedMemory(const std::string& name);

//�˺��������¹���
/*
1.hook��غ���
2.���ù����ڴ�
3.��ͼ(hook)�������ڴ�
*/
DLL_API long SetDX9Hook(HWND hwnd);
//�ָ�ԭ״̬���ͷŹ����ڴ�
DLL_API long UnDX9Hook();




using std::wstring;
class Bkdx
{
public:
	Bkdx();
	~Bkdx();
	long Bind(HWND hwnd);
	long UnBind();
	//��ͼ���ļ�
	long capture(const std::wstring& file_name);
	//ͼ�ζ�λ
	long FindPic(long x1, long y1, long x2, long y2, const std::wstring& files, double sim, long& x, long &y);
private:
	HWND _hwnd;
	wstring _dllname;
	long _width, _height;
	blackbone::Process _process;
	ImageLoc _imageloc;
	//user mutex
	boost::interprocess::named_mutex* _pmutex;
};

#endif


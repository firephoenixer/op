#pragma once
#ifndef __OPTEST_H_
#define __OPTEST_H_
#include "../include/libop.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <time.h>
#ifndef _CMAKE_BUILD
#ifndef _DEBUG
#pragma comment(lib, "../bin/x86/op_x86.lib")
#else
#pragma comment(lib,"../bin/x86/op_x86d.lib")
#endif
#endif

#define _TEST(func,...) \
t1=clock();\
op.##func(__VA_ARGS__);\
t2 = clock();\
wcout<<"test method:op."<<#func<<L" time:"<<t2-t1<<L"ms.\n"
#define _PRINT(var)wcout<<#var<<L":"<<var<<endl;

class optest {
public:
	optest();
};

optest::optest() {
	libop op;
	using namespace std;
	wstring str;
	long  lret;
	long t1, t2;
	long x, y;
	//
	auto path = L"C:\\Users\\wall\\Desktop";
	locale loc("chs");
	wcout.imbue(loc);
	//1.�汾��Version
	op.Ver(str);
	wcout << L"1.�汾��Version:" << str << endl;

	//����Ŀ¼
	op.SetPath(path,&lret);
	cout << "2.����Ŀ¼:" << lret << endl;

	//��ȡĿ¼
	op.GetPath(str);
	wcout << L"3.��ȡĿ¼:" << str << endl;

	//��ȡ���Ŀ¼
	op.GetBasePath(str);
	wcout << L"4.��ȡopĿ¼:" << str << endl;

	//�����Ƿ񵯳�������Ϣ,Ĭ���Ǵ� 0Ϊ�رգ�1Ϊ��ʾΪ��Ϣ��2Ϊ���浽�ļ�
	op.SetShowErrorMsg(1,&lret);

	//sleep
	//op.Sleep(1,&lret);
	//ETEST(Sleep, 1, &lret);
	
	//�����Ƿ������߹رղ���ڲ���ͼƬ�������
	op.EnablePicCache(1, &lret);
	//ȡ�ϴβ�����ͼɫ���򣬱���Ϊfile(24λλͼ)

	_TEST(FindPic, 0, 0, 2000, 2000, L"test.bmp", L"050505", 0.95, 0, &x, &y, &lret);
	_TEST(FindPic,0, 0, 2000, 2000, L"test.bmp", L"000000", 0.95, 0, &x, &y, &lret);
	_TEST(FindPic, 0, 0, 2000, 2000, L"test.bmp", L"000000", 0.95, 0, &x, &y, &lret);
	printf("x=%d,y=%d\n", x, y); 
	_TEST(SetDict, 0, L"xx.txt", &lret);
	
	_TEST(OcrAutoFromFile,L"t2.png", 1., str);

	_PRINT(str);

	_TEST(Ocr, 0, 0, 500, 500,L"000000-050505", 1.0, str);

	_PRINT(str);
	LoadLibraryW(L"D:\\tool\\dm.dll");
	

	op.CapturePre(L"pre.png",&lret);
}

#endif
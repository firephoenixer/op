#pragma once
#ifndef __DX9HOOK_H_
#define __DX9HOOK_H_
#include "Common.h"
//���º�������HOOK DX9
//�˺��������¹���
/*
1.hook��غ���
2.���ù����ڴ�,������
3.��ͼ(hook)�������ڴ�
*/
//dx9
DLL_API long SetDX9Hook(HWND hwnd);
//�ָ�ԭ״̬���ͷŹ����ڴ�
DLL_API long UnDX9Hook();
//dx10
DLL_API long SetDX10Hook(HWND hwnd);
DLL_API long UnDX10Hook();
//dx11
DLL_API long SetDX11Hook(HWND hwnd);
DLL_API long UnDX11Hook();
//opengl
DLL_API long SetOpenglHook(HWND hwnd);
DLL_API long UnOpenglHook();

#endif // !__DX9HOOK_H_

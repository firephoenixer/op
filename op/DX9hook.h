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
DLL_API long SetDX9Hook(HWND hwnd);
//�ָ�ԭ״̬���ͷŹ����ڴ�
DLL_API long UnDX9Hook();


#endif // !__DX9HOOK_H_
